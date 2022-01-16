/* ===================================================================================================================
File Name:   IO.c
Description: IO handler
====================================================================================================================*/

#include "General.h"
#ifdef SW_UC_PIC16F
    #include <htc.h>
#endif
#ifdef SW_UC_PIC18F
    #ifdef SW_UC_DEVBOARD
        #include <p18f46k22.h>
    #else
        #include <p18f25k22.h>
    #endif
#endif
#include <stdlib.h>
#include <string.h>
#include "Types.h"
#include "Keys_Pub.h"
#include "Keys_Loc.h"

volatile struct{
    union{
        struct{
            uint8           TimerFlag:1;
            uint8           bit1:1;
            uint8           bit2:1;
            uint8           bit3:1;
            uint8           bit4:1;
            uint8           bit5:1;
            uint8           bit6:1;
            uint8           bit7:1;
        }Bits;
        uint8               Byte;    
    }Flags;
} Keys_IntObj;    

struct {
    uint8   StateCurrent;       //current keys state (active low. only this!)
    uint8   StateStable;        //stable keys state
    uint16  StateNewPress;      //new key pressed (clear on read).
                                // 0-3 short press (and released.
                                // 4-7 continuous press.
                                // 8-11 long press.
                                // 12-15 stable state
    uint16  Timer[KEYS_NUM];    //anti bouncing timer. Note: these timers count up!!
} Keys_Obj;


/****************************************************************
*  Hardware functions
****************************************************************/
uint8 Keys_HW_read_keys(void)   //Read keys
{
    return(KEY_READ_PORT);
}    


/****************************************************************
*  Function name: Keys_read_keys_state
*  Main tasks:
*    Read a keys stable state
*  Parameters:
*    Mask of the keys to check
*  Returned values:
*    Keys state
*  Remarks:
*    None
****************************************************************/
uint16 Keys_read_keys_state (uint16 Mask)
{
  uint16 Keys;
    Keys = Keys_Obj.StateStable & Mask;
    return(Keys);
}

/****************************************************************
*  Function name: Keys_read_keys_new_event
*  Main tasks:
*    Read new keys event
*  Parameters:
*    Mask of the keys to check
*  Returned values:
*    OK - Event was read
*    Fail
*  Remarks:
*    Event state - bit map. 1 - pressed.
*    Clear on read
****************************************************************/
uint16 Keys_read_keys_new_event (uint16 Mask)
{
  uint16 Keys;
    Keys = Keys_Obj.StateNewPress & Mask;
    if (Keys)
    {
        Keys_Obj.StateNewPress &= ~Mask;
    }
    return(Keys);
}

/****************************************************************
*  Function name: Keys_init
*  Main tasks:
*    Init function of Keys task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
****************************************************************/
void Keys_TaskInit (void)
{
uint8 i = 0;

    //Set ports
    KEY_INIT_PORT;

    Keys_Obj.StateCurrent = Keys_HW_read_keys();
    Keys_Obj.StateStable = 0;
    Keys_Obj.StateNewPress = 0;
    Keys_IntObj.Flags.Byte = 0;
    for (i=0; i< KEYS_NUM; i++)
    {
        Keys_Obj.Timer[i] = KEY_TIMER_STABLE;
    }
}

/****************************************************************
*  Function name: Keys_main
*  Main tasks:
*    Main function of Keys task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
****************************************************************/
void Keys_TaskMain (void)
{
  uint8 i = 0;
  uint8 mask = 0x01;
  uint8 keys = Keys_HW_read_keys();
	
	//Handle timers
	if (Keys_IntObj.Flags.Bits.TimerFlag)
	{
    	Keys_IntObj.Flags.Bits.TimerFlag = 0;
        for (i=0; i<KEYS_NUM; i++)                                  //Decrement keys counters
        {
            if (Keys_Obj.Timer[i])
            {
                Keys_Obj.Timer[i]--;
            }
        }
	
        for (i=0; i< KEYS_NUM; i++)                                 //Scan the keys
        {
            if ((keys & mask) != (Keys_Obj.StateCurrent & mask))
            {                                                       //New state
                Keys_Obj.StateCurrent &= ~mask;
                Keys_Obj.StateCurrent |= (keys & mask);             //save new key state
                Keys_Obj.Timer[i] = KEY_TIMER_STABLE;               //restart stable timer
            }
            else
            {                                                       //Not new state
                if (Keys_Obj.Timer[i] == 1)
                {                                                   //Stable time - update stable state
                    Keys_Obj.StateStable &= ~mask;
                    Keys_Obj.StateStable |= (~Keys_Obj.StateCurrent) & mask;        //Save key state (in active high)
                    if (Keys_Obj.StateStable & mask)
                    {                                               //Key is pressed
                        Keys_Obj.StateNewPress |= ((uint16)mask << KEYS_SHIFT_STABLE);  //Update stable flag
                    }
                }
            }
            mask = mask<<1;
        }
    }   	
}

/****************************************************************
*  Function name: Keys_1mS_tick
*  Main tasks:
*    1mS tick
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called every 1mS
****************************************************************/
void Keys_1mS_tick(void)
{
    Keys_IntObj.Flags.Bits.TimerFlag = 1;
}
