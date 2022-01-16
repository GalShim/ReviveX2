/* ===================================================================================================================
File Name:   Revive_Vsup.c
Description: Supply voltage for Revive
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
#include "Revive_Vsup_Pub.h"
#include "Revive_Vsup_Loc.h"

#define REVIVE_VSUP_V_MIN       1
#define REVIVE_VSUP_V_MAX       200
#define REVIVE_VSUP_CMND_MIN    600
#define REVIVE_VSUP_CMND_MAX    4800

#define REVIVE_VSUP_CMND_LEN    2
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
    uint8                   Timer;          //Timer
    REVIVE_VSUP_STATE_e     State;          //Task state
    struct{                                 //Message to the DAC
        uint8   Msg[REVIVE_VSUP_CMND_LEN];
        uint8   Index;
    }Cmnd;    
} Revive_Vsup_Obj;


//=============================================================================
//=============================================================================
//Hardware functions
//=============================================================================
//=============================================================================
#define REVIVE_VSUP_RUN_PORT    LATD //PORTD
#define REVIVE_VSUP_RUN_BIT     0x04
#define REVIVE_VSUP_RUN_ON      REVIVE_VSUP_RUN_PORT|=REVIVE_VSUP_RUN_BIT
#define REVIVE_VSUP_RUN_OFF     REVIVE_VSUP_RUN_PORT&=~REVIVE_VSUP_RUN_BIT
#define REVIVE_VSUP_CS_PORT     LATD //PORTD
#define REVIVE_VSUP_CS_BIT      0x08
#define REVIVE_VSUP_CS_OFF      REVIVE_VSUP_CS_PORT|=REVIVE_VSUP_CS_BIT
#define REVIVE_VSUP_CS_ON       REVIVE_VSUP_CS_PORT&=~REVIVE_VSUP_CS_BIT
#define REVIVE_VSUP_SPI_READY   SSP1STATbits.BF
#define REVIVE_VSUP_SPI_BUF     SSP1BUF
#define REVIVE_VSUP_CS_TIME     10  //1mS units

void Revive_Vsup_SpiInit (void)
{
                                    //Ports
    TRISDbits.TRISD2 = 0;           //Run output
    TRISDbits.TRISD3 = 0;           //CS output
    TRISCbits.TRISC3 = 0;           //Clock output
    TRISCbits.TRISC5 = 0;           //SDO output 
    REVIVE_VSUP_CS_OFF;
                                    //Config SPI
    SSP1CON1bits.SSPEN = 0;         //Disable, before config
    SSP1STATbits.CKE = 1; //0;           //Transmit on clock idle to active
    SSP1CON1bits.CKP = 0;           //0 is Idle
    SSP1CON1bits.SSPM = 0x01;       //SPI master mode. Clock = Fosc/16 (1MHz). DAC works up to 20MHz.
    SSP1CON1bits.SSPEN = 1;         //Enable, after config
}
void Revive_Vsup_SpiSend (uint8 Data)
{
    REVIVE_VSUP_SPI_BUF = Data;
}        
//=============================================================================
//=============================================================================

/****************************************************************
*  Function name: Revive_Vsup_Cmnd
*  Main tasks:
*    Set V
*  Parameters:
*    V command
*    Value (0.1V units)
*  Returned values:
*    None
*  Remarks:
*    None
****************************************************************/
void Revive_Vsup_Cmnd (REVIVE_VSUP_CMND_e Cmnd, uint8 Val)
{
  uint16 DacVal = 0x3000;
  uint16 TempW;
  uint32 TempLW;
    switch (Cmnd)
    {
      case REVIVE_VSUP_CMND_OFF:                            //Turn V off
        REVIVE_VSUP_RUN_OFF;
        break;
      case REVIVE_VSUP_CMND_ON:                             //Set V on
        REVIVE_VSUP_RUN_ON;
        #ifdef SW_PROD_ReviveUs
          DacVal |= 0x0c6c; //0x0e10; //0x0a64;                                 //12V
        #endif
        #ifdef SW_PROD_ReviveRf
          //TempLW = 23482;
          TempLW = 29630;
          TempLW *= Val;
          TempLW /= 100;
          //TempLW = 45084 - TempLW;
          TempLW = 41090 - TempLW;
          TempLW /= 10;
          TempW = (uint16)TempLW;
          DacVal |= TempW;
        #endif
        break;
    }    
    Revive_Vsup_Obj.Cmnd.Msg[0] = (uint8)(DacVal>>8);
    Revive_Vsup_Obj.Cmnd.Msg[1] = (uint8)DacVal;
    Revive_Vsup_Obj.Cmnd.Index = 1;
    REVIVE_VSUP_CS_ON;
    Revive_Vsup_SpiSend(Revive_Vsup_Obj.Cmnd.Msg[0]);       //Send first byte
    Revive_Vsup_Obj.State = REVIVE_VSUP_STATE_SEND;
}

/****************************************************************
*  Function name: Revive_Vsup_TaskInit
*  Main tasks:
*    Init function of Revive_Vsup task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called on Startup
****************************************************************/
void Revive_Vsup_TaskInit (void)
{
    Revive_Vsup_SpiInit();
    Revive_Vsup_Obj.Timer = 0;
    Revive_Vsup_Obj.Flags.Byte = 0;
    Revive_Vsup_Obj.State = REVIVE_VSUP_STATE_INIT;
}

/****************************************************************
*  Function name: Revive_Vsup_main
*  Main tasks:
*    Main function of Revive_Vsup task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called periodically
****************************************************************/
void Revive_Vsup_TaskMain (void)
{
    switch(Revive_Vsup_Obj.State)
    {
      case REVIVE_VSUP_STATE_INIT:
        //Revive_Vsup_SpiInit();
        Revive_Vsup_Obj.State = REVIVE_VSUP_STATE_START;
        break;
      case REVIVE_VSUP_STATE_START:
        Revive_Vsup_Obj.State = REVIVE_VSUP_STATE_IDLE;
        break;
      case REVIVE_VSUP_STATE_IDLE:
        break;
      case REVIVE_VSUP_STATE_SEND:
        if (REVIVE_VSUP_SPI_READY)
        {
          uint8 TempB = REVIVE_VSUP_SPI_BUF;                //Dummy read to clear BF
            if (Revive_Vsup_Obj.Cmnd.Index < REVIVE_VSUP_CMND_LEN)
            {                                               //Data to send to DAC
                Revive_Vsup_SpiSend(Revive_Vsup_Obj.Cmnd.Msg[Revive_Vsup_Obj.Cmnd.Index]);
                Revive_Vsup_Obj.Cmnd.Index++;
            }
            else
            {                                               //No data to send
                //Revive_Vsup_Obj.Timer = REVIVE_VSUP_CS_TIME; //Wait till last byte is sent, before deactivating CS
                Revive_Vsup_Obj.State = REVIVE_VSUP_STATE_DONE;
            }
        }
        break;
      case REVIVE_VSUP_STATE_DONE:
        REVIVE_VSUP_CS_OFF;
        Revive_Vsup_Obj.State = REVIVE_VSUP_STATE_IDLE;
        break;
    }    
}

/****************************************************************
*  Function name: Revive_Vsup_1mS_tick
*  Main tasks:
*    1mS tick
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
****************************************************************/
void Revive_Vsup_1mS_tick(void)
{
    if (Revive_Vsup_Obj.Timer)
    {
        Revive_Vsup_Obj.Timer--;
        if (Revive_Vsup_Obj.Timer == 0)
            {Revive_Vsup_Obj.Flags.Bits.TimerFlag = 1;}
    }
}
