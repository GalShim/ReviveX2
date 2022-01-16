/* ===================================================================================================================
File Name:   IO.c
Description: IO handler
====================================================================================================================*/

#include "..\..\GeneoIIbPIC1939bl\V1_2\General.h"
#ifdef SW_UC_PIC16F
    #include <htc.h>
#endif
#ifdef SW_UC_PIC18F
    #ifdef SW_UC_DEVBOARD
        #include <p18f46k22.h>
    #else
        #ifdef SW_UC_PIC18F45K22
            #include <p18f45k22.h>
        #else
            #include <p18f25k22.h>
        #endif
    #endif
#endif
#include <stdlib.h>
#include <string.h>
#include "..\..\GeneoIIbPIC1939bl\V1_2\Types.h"
#include "..\..\RfApplicator\V1-0\IO_Pub.h"
#include "..\..\RfApplicator\V1-0\IO_Loc.h"

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
    uint16      BuzTimer;       //Buzzer beep timer
} IO_Obj;


//=============================================================================
//=============================================================================
//Hardware functions
//=============================================================================
//=============================================================================
    #ifdef SW_UC_PIC16F
    #endif
    #ifdef SW_UC_PIC18F
        #ifdef SW_JUMPER_PROD_TEST
            #define IO_GET_PROD_JUMPER ((~PORTA)&0x20)
        #endif
    #endif
    #ifdef SW_UC_PIC18F
        #ifdef SW_PLL_ON
            #define BUZ_PWM_PR2 255
        #else
            #define BUZ_PWM_PR2 92
        #endif
        #define BUZ_PWM_INTENSE (BUZ_PWM_PR2/2)
        //-----------------------------------
        void IO_SetIntensityBuz(uint8 Intensity)
        {
            CCPR2L = Intensity;
            CCP2CONbits.DC2B0  = 0;
            CCP2CONbits.DC2B1  = 0;
        }
        //----------------------------------------
        void IO_ConfigBuz (void)
        {
            TRISCbits.TRISC1 = 0; //CCP1              //Output pins
            PR2 = BUZ_PWM_PR2;              //PWM period 2.7KHz
            CCP2CON = 0b00001100;
            IO_SetIntensityBuz(0);          //Set initial values to 0
            CCPTMRS0bits.C2TSEL = 0;        //Base for PWM is timer2 (for R,G and B)
            T2CON = 123; //0b01111011;      //Config timer2
            //APFCONbits.CCP2SEL = 1;         //PWM
            T2CONbits.TMR2ON = 1;           //Enable timer2
        }    
        //----------------------------------------
        void IO_ConfigJumper (void)
        {
            TRISAbits.TRISA5 = 1;
            ANSELAbits.ANSA5 = 0;
        }    
        //----------------------------------------
        //Buzzer functions
        void IO_Hw_BuzOn(void)
        {
            IO_SetIntensityBuz(BUZ_PWM_INTENSE);
        }    
        void IO_Hw_BuzOff(void)
        {
            IO_SetIntensityBuz(0);
        }    
    #endif

//=============================================================================
//=============================================================================

/****************************************************************
*  Function name: IO_GetProdJumper
*  Main tasks:
*    return state of production jumper
*  Parameters:
*    Command
*  Returned values:
*    None
*  Remarks:
*    None
****************************************************************/
uint8 IO_GetProdJumper (void)
{
    return(IO_GET_PROD_JUMPER);
}    

/****************************************************************
*  Function name: IO_BuzCmnd
*  Main tasks:
*    Buzzer command
*  Parameters:
*    Command
*  Returned values:
*    None
*  Remarks:
*    None
****************************************************************/
void IO_BuzCmnd (IO_BUZ_CMND_e Cmnd)
{
    switch (Cmnd)
    {
      case IO_BUZ_CMND_OFF:
        IO_Hw_BuzOff();
        IO_Obj.BuzTimer = 0;
        break;
      case IO_BUZ_CMND_ON:
        IO_Hw_BuzOn();
        IO_Obj.BuzTimer = 0;
        break;
      case IO_BUZ_CMND_BEEP:
        IO_Hw_BuzOn();
        IO_Obj.BuzTimer = IO_BUZ_BEEP_TIME;
        break;
      case IO_BUZ_CMND_BEEP_LONG:
        IO_Hw_BuzOn();
        IO_Obj.BuzTimer = IO_BUZ_BEEP_LONG_TIME;
        break;
    }    
}

/****************************************************************
*  Function name: IO_TaskInit
*  Main tasks:
*    Init function of IO task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
****************************************************************/
void IO_TaskInit (void)
{
    IO_ConfigJumper();
    IO_ConfigBuz();
    IO_BuzCmnd(IO_BUZ_CMND_OFF);
    IO_Obj.Flags.Byte = 0;
}

/****************************************************************
*  Function name: IO_main
*  Main tasks:
*    Main function of IO task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
****************************************************************/
void IO_TaskMain (void)
{
    if (IO_Obj.Flags.Bits.TimerFlag)
    {
        IO_Obj.Flags.Bits.TimerFlag = 0;
        if (IO_Obj.BuzTimer)
        {                                           //Timer beep is on
            IO_Obj.BuzTimer--;
            if (IO_Obj.BuzTimer == 0)
            {                                       //timer beep time over
                IO_BuzCmnd(IO_BUZ_CMND_OFF);
            }    
        }
        if (IO_Obj.BuzTimer == 0) {IO_BuzCmnd(IO_BUZ_CMND_OFF);}    //Protection. No buz forever. Only beep.
    }
}

/****************************************************************
*  Function name: IO_1mS_tick
*  Main tasks:
*    1mS tick
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
****************************************************************/
void IO_1mS_tick(void)
{
    IO_Obj.Flags.Bits.TimerFlag = 1;
}
