/* ========================================
 *
 * Handle LEDs
 *
 * ========================================
*/

#include "General.h"
#ifdef SW_UC_PIC16F
    #include <htc.h>
#endif
#ifdef SW_UC_PIC18F
    #ifdef SW_UC_DEVBOARD
        #include <p18f46k22.h>
    #else
        #ifdef SW_UC_PIC18F45K22
//            #include <p18f45k22.h>
        #else
            #include <p18f25k22.h>
        #endif
    #endif
#endif
#include <stdlib.h>
#include <string.h>
#include "Types.h"
#include "Comm_Pub.h"
#include "Eeprom_Pub.h"
#include "Led_Pub.h"
#include "Led_Loc.h"
#include "..\..\Revive\V2-0\ReviveX2.X\mcc_generated_files\pin_manager.h"

//RGB LEDs intensity
#ifdef SW_LED_RGB_VAR
    const int Led_RgbIntensity[256]={
        0,  2,  4,  6,  8,  10, 12, 14, 16, 18, 20, 21, 23, 25, 27, 29,
        31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61,
        63, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92,
        94, 96, 98, 100,102,104,105,107,109,111,113,115,117,119,121,123,
        125,127,129,131,133,135,137,139,141,143,145,146,148,150,152,154,
        156,158,160,162,164,166,168,170,172,174,176,178,180,182,184,186,
        188,189,191,193,195,197,199,201,203,205,207,209,211,213,215,217,
        219,221,223,225,227,229,230,232,234,236,238,240,242,244,246,248,
        250,252,254,256,258,260,262,264,266,268,270,271,273,275,277,279,
        281,283,285,287,289,291,293,295,297,299,301,303,305,307,309,311,
        313,314,316,318,320,322,324,326,328,330,332,334,336,338,340,342,
        344,346,348,350,352,354,355,357,359,361,363,365,367,369,371,373,
        375,377,379,381,383,385,387,389,391,393,395,396,398,400,402,404,
        406,408,410,412,414,416,418,420,422,424,426,428,430,432,434,436,
        438,439,441,443,445,447,449,451,453,455,457,459,461,463,465,467,
        469,471,473,475,477,479,480,482,484,486,488,490,492,494,496,500
    };
#endif

//RGB LEDs colors
#define LED_NUM_OF_COLORS   6
const uint8 Led_RgbColor[LED_NUM_OF_COLORS][LED_NUM_OF_RGBLEDS] = {
    {  0,   0,   0},    //0     //% power (scale 0-255) of R, G, B
    {255, 255,   0},    //1
    {255,   0,   0},    //2
    {  0, 255,   0},    //3
    {  0,   0, 255},    //4
    {255,   0, 255},    //5
};    

volatile struct{
    struct{
        union{
            uint8       Byte;
            struct {
                uint8       Tick100mS;      //1mSec tick
                uint8       ColorBlue:1;    //0-green, 1-blue
                uint8       b2:1;
                uint8       b3:1;
                uint8       b4:1;
                uint8       b5:1;
                uint8       b6:1;
                uint8       b7:1;
            } Bits;
        } Flags;
    }Task;    
} Led_IntObj;    

struct{
    struct{
        LED_TASK_STATE_e    State;          //State of LED task
    } Task;
    struct{
        struct{                             //PWM time (100mS units)
            uint8           Total;          //Total PWM time
            uint8           On;             //On (active) time
            uint8           Current;        //Current time (count up)
        } PwmTime;    
        struct{                             //Total time (100mS units)
            uint16          Total;          //Total time
            uint16          Current;        //Current time (count up)
        } TotalTime;
        struct{                             //Motor state
            LED_GREEN_MODE_e    Led;        //Off/Continous/Pulses
            LED_PULSE_STATE_e   Pulse;
            LED_TASK_STATE_e    NextLedState;//State after pulses time is over
            #ifdef SW_LED_RGB_FIXED
                LED_GREEN_COLOR_e Color;    //Color of Green LED
            #endif
        } State;
    } LedGreen;    
    struct{
        LED_RGB_MODE_e  Mode;           //Off/Continous/Pulses
        uint8           State;          //On/Off
        uint8           Total;          //Total PWM time
        uint8           On;             //On (active) time
        uint8           Current;        //Current time (count up)
        uint8           Freq;           //Frequency (Hz)
        uint8           Color;          //Color code
        uint8           Intensity;      //0-100 (%)
        uint8           TotalTime;      //Total time
    } RgbLed;    
} Led_Obj;    

LED_CMD_SET_STATE_e Led_CmdState = LED_CMD_SET_STATE_AUTO;

//=============================================================================
//=============================================================================
//Hardware functions
//=============================================================================
//=============================================================================
//Registers
    #ifdef SW_UC_PIC16F
        #define LED_PORT_GREEN          PORTA
        #define LED_PORT_BIT_GREEN      0x40
        #define LED_PORT_RGB_R          PORTC
        #define LED_PORT_BIT_RGB_R      0x04
        #define LED_PORT_RGB_G          PORTB
        #define LED_PORT_BIT_RGB_G      0x08
        #define LED_PORT_RGB_B          PORTB
        #define LED_PORT_BIT_RGB_B      0x20
    #endif
    #ifdef SW_UC_PIC18F
        #define LED_PORT_RED            LATA //PORTA
        #define LED_PORT_BIT_RED        0x40
        #define LED_PORT_GREEN          LATA //PORTA
        #define LED_PORT_BIT_GREEN      0x80
        #ifdef SW_REVIVE_BOARD
            #define LED_PORT_BLUE           LATE //PORTE
            #define LED_PORT_BIT_BLUE       0x01
        #else
            #define LED_PORT_BLUE           LATC //PORTC
            #define LED_PORT_BIT_BLUE       0x08
        #endif
        #define LED_PORT_RGB_R          LATC //PORTC
        #define LED_PORT_BIT_RGB_R      0x04        //Used as Orange
        #define LED_PORT_RGB_G          LATA //PORTA
        #define LED_PORT_BIT_RGB_G      0x00
        #define LED_PORT_RGB_B          LATA //PORTA
        #define LED_PORT_BIT_RGB_B      0x00
        #define LED_ORANGE_BLINK_ON     5
        #define LED_ORANGE_BLINK_OFF    5
    #endif

//Set RGB intensity
    #ifdef SW_UC_PIC16F
        //-----------------------------------
        void Led_SetIntensityR(unsigned char IntensityR)
        {
            CCPR1L = (Led_RgbIntensity[IntensityR] >> 2);
            DC1B0  = Led_RgbIntensity[IntensityR] & 0x01;
            DC1B1  = (Led_RgbIntensity[IntensityR] >> 1) & 0x01;
        }
        //-----------------------------------
        void Led_SetIntensityG(unsigned char IntensityG)
        {
            CCPR2L = (Led_RgbIntensity[IntensityG] >> 2);
            DC2B0  = Led_RgbIntensity[IntensityG] & 0x01;
            DC2B1  = (Led_RgbIntensity[IntensityG] >> 1) & 0x01;
        }
        //-----------------------------------
        void Led_SetIntensityB(unsigned char IntensityB)
        {
            CCPR3L = (Led_RgbIntensity[IntensityB] >> 2);
            DC3B0  = Led_RgbIntensity[IntensityB] & 0x01;
            DC3B1  = (Led_RgbIntensity[IntensityB] >> 1) & 0x01;
        }
        //-----------------------------------
    #endif
    #ifdef SW_UC_PIC18F
        //-----------------------------------
        void Led_SetIntensityR(unsigned char IntensityR)
        {
            //Intensity not in use
        }
        //-----------------------------------
        void Led_SetIntensityG(unsigned char IntensityG)
        {
            //Intensity not in use
        }
        //-----------------------------------
        void Led_SetIntensityB(unsigned char IntensityB)
        {
            //Intensity not in use
        }
        //-----------------------------------
        #ifdef SW_DEBUG_PROD_RF_CUR_TEST
         void LED_Test_ActBlue(void)
         {
            LED_PORT_BLUE |= LED_PORT_BIT_BLUE;
         }
         void LED_Test_ActGreen(void)
         {
            LED_PORT_GREEN |= LED_PORT_BIT_GREEN;
         }
        #endif
    #endif

//Check RGB LEDs
    /******************************************************************************
    *  Function name: Led_CheckRgbSet
    *  Main tasks:
    *    Set to check RGB LEDs
    *  Parameters:
    *    Single LED ID
    *  Returned values:
    *    None
    *  Remarks:
    *    None
    ******************************************************************************/
    #ifdef SW_UC_PIC16F
        void Led_CheckRgbSet (LED_RGB_ID_e LedId)
        {
            switch (LedId)
            {
              case LED_RGB_ID_R:
                LED_PORT_RGB_R |= LED_PORT_BIT_RGB_R;
                LED_PORT_RGB_G &= ~LED_PORT_BIT_RGB_G;
                LED_PORT_RGB_B &= ~LED_PORT_BIT_RGB_B;
                break;
              case LED_RGB_ID_G:
                LED_PORT_RGB_R &= ~LED_PORT_BIT_RGB_R;
                LED_PORT_RGB_G |= LED_PORT_BIT_RGB_G;
                LED_PORT_RGB_B &= ~LED_PORT_BIT_RGB_B;
                break;
              case LED_RGB_ID_B:
                LED_PORT_RGB_R &= ~LED_PORT_BIT_RGB_R;
                LED_PORT_RGB_G &= ~LED_PORT_BIT_RGB_G;
                LED_PORT_RGB_B |= LED_PORT_BIT_RGB_B;
                break;
            }    
        }    
    #endif
    #ifdef SW_UC_PIC18F
        void Led_CheckRgbSet (LED_RGB_ID_e LedId)
        {
            switch (LedId)
            {
              case LED_RGB_ID_R:
                LED_PORT_RGB_R |= LED_PORT_BIT_RGB_R;
                LED_PORT_RGB_G &= ~LED_PORT_BIT_RGB_G;
                LED_PORT_RGB_B &= ~LED_PORT_BIT_RGB_B;
                break;
              case LED_RGB_ID_G:
                LED_PORT_RGB_R &= ~LED_PORT_BIT_RGB_R;
                LED_PORT_RGB_G |= LED_PORT_BIT_RGB_G;
                LED_PORT_RGB_B &= ~LED_PORT_BIT_RGB_B;
                break;
              case LED_RGB_ID_B:
                LED_PORT_RGB_R &= ~LED_PORT_BIT_RGB_R;
                LED_PORT_RGB_G &= ~LED_PORT_BIT_RGB_G;
                LED_PORT_RGB_B |= LED_PORT_BIT_RGB_B;
                break;
              case LED_RGB_ID_ALL:
                LED_PORT_RGB_R |= LED_PORT_BIT_RGB_R;
                LED_PORT_RGB_G |= LED_PORT_BIT_RGB_G;
                LED_PORT_RGB_B |= LED_PORT_BIT_RGB_B;
                break;
            }    
        }    
    #endif

//Config/unconfig
    #ifdef SW_UC_PIC16F
        //----------------------------------------
        void Led_ConfigRgbled (void)
        {
            TRISC2 = 0; //CCP1              //Output pins
            TRISB3 = 0; //CCP2
            TRISB5 = 0; //CCP3
            PR2 = 255;                      //PWM period 0x65 - 51us
            CCP1CON = 0b00001100;           //Set to PWM mode
            CCP2CON = 0b00001100;
            CCP3CON = 0b00001100;
            Led_SetIntensityR(0); //R               //Set initial values to 0
            Led_SetIntensityG(0); //G
            Led_SetIntensityB(0); //B
            CCPTMRS0bits.C1TSEL = 0;        //Base for PWM is timer2 (for R,G and B)
            CCPTMRS0bits.C2TSEL = 0;
            CCPTMRS0bits.C3TSEL = 0;
            T2CON = 123; //0b01111011;      //Config timer2
            APFCONbits.CCP2SEL = 1;         //PWM
            APFCONbits.CCP3SEL = 1;
            ANSELBbits.ANSB5 = 0;           //Disable ADC functions
            ANSELBbits.ANSB3 = 0;
            T2CONbits.TMR2ON = 1;           //Enable timer2
        }    
        //----------------------------------------
        void Led_UnConfigRgbled (void)
        {
            CCP1CON = 0;
            CCP2CON = 0;
            CCP3CON = 0;
            T2CONbits.TMR2ON = 0;
        }    
        //----------------------------------------
        void Led_InitPorts (void)
        {
            TRISA &= ~LED_PORT_BIT_GREEN;
            ANSELA &= ~LED_PORT_BIT_GREEN;
        }    
        //----------------------------------------
    #endif
    #ifdef SW_UC_PIC18F
        //----------------------------------------
        void Led_ConfigRgbled (void)
        {
            TRISAbits.TRISA6 = 0;
            TRISAbits.TRISA7 = 0;
            #ifdef SW_REVIVE_BOARD
                TRISEbits.TRISE0 = 0;
                ANSELEbits.ANSE0 = 0;
            #else
                TRISCbits.TRISC3 = 0;
                ANSELCbits.ANSC3 = 0;
            #endif
        }    
        //----------------------------------------
        void Led_UnConfigRgbled (void)
        {
            //???<<<<
        }    
        //----------------------------------------
        void Led_InitPorts (void)
        {
            Led_ConfigRgbled();
            TRISCbits.TRISC2 = 0;
            ANSELCbits.ANSC2 = 0;
        }    
        //----------------------------------------
        #ifdef SW_LED_RGB_FIXED
            void Led_ActRgbled (LED_RGB_ID_e LedId, uint8 Action)
            {
                switch (LedId)
                {
                  case LED_RGB_ID_R:
                    if (Action)
                    {
                        LED_PORT_RGB_R |= LED_PORT_BIT_RGB_R;
                    }
                    else
                    {
                        LED_PORT_RGB_R &= ~LED_PORT_BIT_RGB_R;
                    }        
                    break;
                  case LED_RGB_ID_G:
                    if (Action)
                    {
                        LED_PORT_RGB_G |= LED_PORT_BIT_RGB_G;
                    }
                    else
                    {
                        LED_PORT_RGB_G &= ~LED_PORT_BIT_RGB_G;
                    }        
                    break;
                  case LED_RGB_ID_B:
                    if (Action)
                    {
                        LED_PORT_RGB_B |= LED_PORT_BIT_RGB_B;
                    }
                    else
                    {
                        LED_PORT_RGB_B &= ~LED_PORT_BIT_RGB_B;
                    }        
                    break;
                  case LED_RGB_ID_ALL:
                    break;
                }    
                #ifdef SW_COMM_LED_ON_EVENT
                    if (Comm_GetEventState (APP_EVENT_FAULT_OverVolt))
                    {
                        //LED_PORT_RED = LED_PORT_RED | LED_PORT_BIT_RED;
                        LED_PORT_RED |= LED_PORT_BIT_RED;
                    }
                    else
                    {
                        //LED_PORT_RED = LED_PORT_RED & ~LED_PORT_BIT_RED;
                        LED_PORT_RED &= ~LED_PORT_BIT_RED;
                    }        
                #endif
            }    
        #endif
    #endif

/******************************************************************************
*  Function name: Led_SetGreen
*  Main tasks:
*    Set green LED on/off
*  Parameters:
*    TRUE/FALSE
*  Returned values:
*    None
*  Remarks:
*    In fixed mode: Any action in Green deactivates Blue, and vice versa
******************************************************************************/
void Led_SetGreen(uint8 State)
{
    #ifdef SW_LED_RGB_FIXED
        switch (Led_Obj.LedGreen.State.Color)
        {
          case LED_GREEN_COLOR_BLUE:
            if (State)
            {
                LED_PORT_BLUE |= LED_PORT_BIT_BLUE;
            }
            else
            {
                LED_PORT_BLUE &= ~LED_PORT_BIT_BLUE;
            }
            LED_PORT_GREEN &= ~LED_PORT_BIT_GREEN;
            break;
          case LED_GREEN_COLOR_RED:
            if (State)
            {
                LED_PORT_RED |= LED_PORT_BIT_RED;
            }
            else
            {
                LED_PORT_RED &= ~LED_PORT_BIT_RED;
            }
            break;
          case LED_GREEN_COLOR_ALL:
            if (State)
            {
                LED_PORT_RED |= LED_PORT_BIT_RED;
                LED_PORT_BLUE |= LED_PORT_BIT_BLUE;
                LED_PORT_GREEN |= LED_PORT_BIT_GREEN;
            }
            else
            {
                LED_PORT_RED &= ~LED_PORT_BIT_RED;
                LED_PORT_BLUE &= ~LED_PORT_BIT_BLUE;
                LED_PORT_GREEN &= ~LED_PORT_BIT_GREEN;
            }
            break;
          case LED_GREEN_COLOR_GREENnBLUE:
            if (State)
            {
                LED_PORT_BLUE |= LED_PORT_BIT_BLUE;
                LED_PORT_GREEN |= LED_PORT_BIT_GREEN;
            }
            else
            {
                LED_PORT_BLUE &= ~LED_PORT_BIT_BLUE;
                LED_PORT_GREEN &= ~LED_PORT_BIT_GREEN;
            }
            break;
          case LED_GREEN_COLOR_BLUEnRED:
            if (State)
            {
                LED_PORT_RED |= LED_PORT_BIT_RED;
                LED_PORT_BLUE |= LED_PORT_BIT_BLUE;
            }
            else
            {
                LED_PORT_RED &= ~LED_PORT_BIT_RED;
                LED_PORT_BLUE &= ~LED_PORT_BIT_BLUE;
            }
            break;
          case LED_GREEN_COLOR_REDnGREEN:
            if (State)
            {
                LED_PORT_RED |= LED_PORT_BIT_RED;
                LED_PORT_GREEN |= LED_PORT_BIT_GREEN;
            }
            else
            {
                LED_PORT_RED &= ~LED_PORT_BIT_RED;
                LED_PORT_GREEN &= ~LED_PORT_BIT_GREEN;
            }
            break;
          //default: //Green
          case LED_GREEN_COLOR_GREEN:
            if (State)
            {
                LED_PORT_GREEN |= LED_PORT_BIT_GREEN;
            }
            else
            {
                LED_PORT_GREEN &= ~LED_PORT_BIT_GREEN;
            }
            LED_PORT_BLUE &= ~LED_PORT_BIT_BLUE;
            break;
        }
    #endif

    #ifdef SW_LED_RGB_VAR
        if (State)
        {
            LED_PORT_GREEN &= ~LED_PORT_BIT_GREEN;
        }
        else
        {
            LED_PORT_GREEN |= LED_PORT_BIT_GREEN;
        }
    #endif
}    


//=============================================================================
//=============================================================================


/******************************************************************************
*  Function name: Led_SetRgb
*  Main tasks:
*    Set RGB color LEDs on/off
*  Parameters:
*    TRUE/FALSE
*  Returned values:
*    None
*  Remarks:
*    Parameters taken from EEPROM
******************************************************************************/
#ifdef SW_LED_RGB_VAR
    //-----------------------------------
    void Led_SetIntensityR1(uint8 IntensityR)
    {
        CCPR1L = (Led_RgbIntensity[IntensityR] >> 2);
        DC1B0  = Led_RgbIntensity[IntensityR] & 0x01;
        DC1B1  = (Led_RgbIntensity[IntensityR] >> 1) & 0x01;
    }
    //-----------------------------------
    void Led_SetIntensityG1(uint8 IntensityG)
    {
        CCPR2L = (Led_RgbIntensity[IntensityG] >> 2);
        DC2B0  = Led_RgbIntensity[IntensityG] & 0x01;
        DC2B1  = (Led_RgbIntensity[IntensityG] >> 1) & 0x01;
    }
    //-----------------------------------
    void Led_SetIntensityB1(uint8 IntensityB)
    {
        CCPR3L = (Led_RgbIntensity[IntensityB] >> 2);
        DC3B0  = Led_RgbIntensity[IntensityB] & 0x01;
        DC3B1  = (Led_RgbIntensity[IntensityB] >> 1) & 0x01;
    }
    //-----------------------------------
#endif

void Led_SetRgb(uint8 State)
{
#ifdef SW_LED_RGB_VAR
  uint16 TempIntensity;
    if (State)
    {                                       //On - set RGB LEDs intensity
        //Red intensity
        TempIntensity = Led_Obj.RgbLed.Intensity;
        TempIntensity *= Led_RgbColor[Led_Obj.RgbLed.Color][LED_RGB_ID_R];
        TempIntensity /= 255;
        TempIntensity *= 10;                //Convert from 1-10 to 10-100
        Led_SetIntensityR1(TempIntensity);   //Set PWM
        
        //Green intensity
        TempIntensity = Led_Obj.RgbLed.Intensity;
        TempIntensity *= Led_RgbColor[Led_Obj.RgbLed.Color][LED_RGB_ID_G];
        TempIntensity /= 255;
        TempIntensity *= 10;                //Convert from 1-10 to 10-100
        Led_SetIntensityG1(TempIntensity);   //Set PWM
        
        //Blue intensity
        TempIntensity = Led_Obj.RgbLed.Intensity;
        TempIntensity *= Led_RgbColor[Led_Obj.RgbLed.Color][LED_RGB_ID_B];
        TempIntensity /= 255;
        TempIntensity *= 10;                //Convert from 1-10 to 10-100
        Led_SetIntensityB1(TempIntensity);   //Set PWM
    }
    else
    {                                   //Off
        Led_SetIntensityR1(0); //R  
        Led_SetIntensityG1(0); //G
        Led_SetIntensityB1(0); //B
    }
#endif

#ifdef SW_LED_RGB_FIXED
    Led_ActRgbled(LED_RGB_ID_ORANGE, State);
#endif
}    

/******************************************************************************
*  Function name: Led_CheckRgb
*  Main tasks:
*    Check RGB LEDs
*  Parameters:
*    Single LED ID
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void Led_CheckRgb (LED_RGB_ID_e LedId)
{
    Led_UnConfigRgbled();
    Led_CheckRgbSet(LedId);
}    

/******************************************************************************
*  Function name: Led_ActRgb
*  Main tasks:
*    Activate RGB color LEDs
*  Parameters:
*    TRUE/FALSE
*  Returned values:
*    None
*  Remarks:
*    Parameters taken from EEPROM
******************************************************************************/
void Led_ActRgb (uint8 Act)
{
    #ifdef SW_LED_RGB_VAR
        if (Act)
        {
            Led_Obj.RgbLed.Mode = Eeprom_ReadParam(EEPROM_PARAM_TASK_LED, LED_EE_PARAM_RGB_MODE);
            Led_Obj.RgbLed.Color = Eeprom_ReadParam(EEPROM_PARAM_TASK_LED, LED_EE_PARAM_RGB_COLOR);
            Led_Obj.RgbLed.Intensity = Eeprom_ReadParam(EEPROM_PARAM_TASK_LED, LED_EE_PARAM_RGB_POWER);
            Led_Obj.RgbLed.On = Eeprom_ReadParam(EEPROM_PARAM_TASK_LED, LED_EE_PARAM_RGB_ON);
            Led_Obj.RgbLed.Total = Eeprom_ReadParam(EEPROM_PARAM_TASK_LED, LED_EE_PARAM_RGB_OFF) + Led_Obj.RgbLed.On;
            
            Led_Obj.RgbLed.Current = 0;
        }
        else
        {
            Led_Obj.RgbLed.Mode = LED_RGB_MODE_OFF;
        }
    #endif
}    

/******************************************************************************
*  Function name: Led_ActOrange
*  Main tasks:
*    Activate orange LED
*  Parameters:
*    OFF/CONT_PULSE
*  Returned values:
*    None
*  Remarks:
*    Uses mechanism of RGB
******************************************************************************/
#ifdef SW_LED_RGB_FIXED
    void Led_ActOrange (LED_RGB_MODE_e Mode, uint8 TotalTime, uint8 PwmTime)
    {
        Led_Obj.RgbLed.Mode = Mode;

        Led_Obj.RgbLed.TotalTime = TotalTime;
        Led_Obj.RgbLed.Total = PwmTime;
        Led_Obj.RgbLed.On = Led_Obj.LedGreen.PwmTime.Total / 2;   //50% duty cycle blinking
        Led_Obj.RgbLed.Current = 0;
    }    
#endif

/******************************************************************************
*  Function name: Led_ActGreen
*  Main tasks:
*    Activate green LED
*  Parameters:
*    Mode (Off/Continuous/Blink)
*    Blink cycle time (100mS units)
*    Blink time (100mS units)
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void Led_ActGreen (LED_GREEN_MODE_e Mode, uint8 TotalTime, uint8 PwmTime)
{
                                                //Handle mode
   #ifndef SW_LED_RGB_FIXED
    if (Mode == LED_RGB_MODE_PULSEnCONT)
    {                                           //Pulse+Continuous
        Mode = LED_RGB_MODE_PULSE;
        Led_Obj.LedGreen.State.NextLedState = LED_RGB_MODE_CONT;
    }
    else
    {
        if (Mode == LED_RGB_MODE_PULSEnOFF)
        {                                       //Pulse+Off
            Mode = LED_RGB_MODE_PULSE;
        }    
        Led_Obj.LedGreen.State.NextLedState = LED_RGB_MODE_OFF;
    }
   #endif
    
                                                //Save color
    #ifdef SW_LED_RGB_FIXED                                                
        switch (Mode)
        {
          case LED_BLUE_MODE_CONT:
            Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_BLUE;
            Mode = LED_GREEN_MODE_CONT;
            break;
          case LED_BLUE_MODE_PULSE:
            Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_BLUE;
            Mode = LED_GREEN_MODE_PULSE;
            break;
          case LED_RED_MODE_CONT:
            Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_RED;
            Mode = LED_GREEN_MODE_CONT;
            break;
          case LED_ALL_MODE_CONT:
            Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_ALL;
            Mode = LED_GREEN_MODE_CONT;
            break;
          case LED_GREENnBLUE_MODE_CONT:
            Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_GREENnBLUE;
            Mode = LED_GREEN_MODE_CONT;
            break;
          case LED_BLUEnRED_MODE_CONT:
            Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_BLUEnRED;
            Mode = LED_GREEN_MODE_CONT;
            break;
          case LED_REDnGREEN_MODE_CONT:
            Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_REDnGREEN;
            Mode = LED_GREEN_MODE_CONT;
            break;
          default:
            //Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_GREEN;
            break;
        }
    #else
        if (Mode == LED_BLUE_MODE_CONT)
        {
            Led_IntObj.Task.Flags.Bits.ColorBlue = 1;
            Mode = LED_GREEN_MODE_CONT;
        }
        else if (Mode == LED_BLUE_MODE_PULSE)
        {
            Led_IntObj.Task.Flags.Bits.ColorBlue = 1;
            Mode = LED_GREEN_MODE_PULSE;
        }    
        else
        {
            Led_IntObj.Task.Flags.Bits.ColorBlue = 0;
        }            
    #endif
    
                                                //Save mode
    Led_Obj.LedGreen.State.Led = Mode;
    Led_Obj.LedGreen.TotalTime.Total = TotalTime;
    Led_Obj.LedGreen.PwmTime.Total = PwmTime;
    Led_Obj.LedGreen.PwmTime.On = Led_Obj.LedGreen.PwmTime.Total / 2;   //50% duty cycle blinking
    
    Led_Obj.LedGreen.TotalTime.Current = 0;
    Led_Obj.LedGreen.PwmTime.Current = 0;
}    

/******************************************************************************
*  Function name: Led_Cmd_SetState
*  Main tasks:
*    Set LED command from communication
*  Parameters:
*    State
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void Led_Cmd_SetState (LED_CMD_SET_STATE_e State)
{
	Led_CmdState = State;
}

/******************************************************************************
*  Function name: Led_TaskInit
*  Main tasks:
*    Initialize Leds task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called on start up
******************************************************************************/
void Led_TaskInit (void)
{
    //Config green LED
    Led_InitPorts();
    Led_SetGreen(FALSE);
    
    //Config RGB LEDs
    Led_ConfigRgbled();
    Led_Obj.RgbLed.TotalTime = 0;
    
    #ifdef SW_LED_RGB_FIXED
        Led_ActRgbled(LED_RGB_ID_R, FALSE);
        Led_ActRgbled(LED_RGB_ID_G, FALSE);
        Led_ActRgbled(LED_RGB_ID_B, FALSE);
        Led_ActOrange(LED_ORANGE_MODE_OFF,0 ,0);
        Led_Obj.RgbLed.Mode =       LED_RGB_MODE_OFF;
        Led_Obj.RgbLed.On =         LED_ORANGE_BLINK_ON;
        Led_Obj.RgbLed.Total =      LED_ORANGE_BLINK_ON + LED_ORANGE_BLINK_OFF;
        Led_ActGreen (LED_GREEN_MODE_OFF, 0, 0);
        Led_ActOrange(LED_ORANGE_MODE_OFF,0 ,0);
    #else
        Led_Obj.RgbLed.Mode =       Eeprom_ReadParam(EEPROM_PARAM_TASK_LED, LED_EE_PARAM_RGB_MODE);
        Led_Obj.RgbLed.Color =      Eeprom_ReadParam(EEPROM_PARAM_TASK_LED, LED_EE_PARAM_RGB_COLOR);
        Led_Obj.RgbLed.Intensity =  Eeprom_ReadParam(EEPROM_PARAM_TASK_LED, LED_EE_PARAM_RGB_POWER);
        Led_Obj.RgbLed.On =         Eeprom_ReadParam(EEPROM_PARAM_TASK_LED, LED_EE_PARAM_RGB_ON);
        Led_Obj.RgbLed.Total =      Eeprom_ReadParam(EEPROM_PARAM_TASK_LED, LED_EE_PARAM_RGB_OFF) + Led_Obj.RgbLed.On;
        Led_Obj.RgbLed.Freq =       122;
    #endif
}

/******************************************************************************
*  Function name: Led_TaskMain
*  Main tasks:
*    Handle Leds task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called periodically
******************************************************************************/
void Led_TaskMain (void)
{
  uint8 i;
    if (Led_IntObj.Task.Flags.Bits.Tick100mS)
    {                                                   //Tick - handle Green LED

		//Handle LED according to COMM_TEXT_Cmnd_setled
	    struct{
	        struct{                             //PWM time (100mS units)
	            uint8           Total;          //Total PWM time
	            uint8           On;             //On (active) time
	        } PwmTime;    
	        struct{                             //Total time (100mS units)
	            uint16          Total;          //Total time
	        } TotalTime;
	        struct{                             //Motor state
	            LED_GREEN_MODE_e    Led;        //Off/Continous/Pulses
	            #ifdef SW_LED_RGB_FIXED
	                LED_GREEN_COLOR_e Color;    //Color of Green LED
	            #endif
	        } State;
	    } LedGreenSav;
	    struct{
	        LED_RGB_MODE_e  Mode;           //Off/Continous/Pulses
	        uint8           State;          //On/Off
	        uint8           Total;          //Total PWM time
        	uint8           On;             //On (active) time
	        uint8           TotalTime;      //Total time
	    } RgbLedSav;    
		if (Led_CmdState != LED_CMD_SET_STATE_AUTO)
		{
	        LedGreenSav.State.Color = Led_Obj.LedGreen.State.Color;
		    LedGreenSav.State.Led = Led_Obj.LedGreen.State.Led;
		    LedGreenSav.TotalTime.Total = Led_Obj.LedGreen.TotalTime.Total;
		    LedGreenSav.PwmTime.Total = Led_Obj.LedGreen.PwmTime.Total;
		    LedGreenSav.PwmTime.On = Led_Obj.LedGreen.PwmTime.On;
			RgbLedSav.Mode = Led_Obj.RgbLed.Mode;
			RgbLedSav.State = Led_Obj.RgbLed.State;
			RgbLedSav.Total = Led_Obj.RgbLed.Total;
			RgbLedSav.On = Led_Obj.RgbLed.On;
			RgbLedSav.TotalTime = Led_Obj.RgbLed.TotalTime;
			
			switch (Led_CmdState)
			{
              case LED_CMD_SET_STATE_AUTO:
                break;
			  case LED_CMD_SET_STATE_OFF:
		        Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_GREEN;
			    Led_Obj.LedGreen.State.Led = LED_GREEN_MODE_OFF;
			    Led_Obj.LedGreen.PwmTime.Total = 0;
				Led_Obj.RgbLed.Mode = LED_RGB_MODE_OFF;
				break;
			  case LED_CMD_SET_STATE_ON_GREEN:
		        Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_GREEN;
			    Led_Obj.LedGreen.State.Led = LED_GREEN_MODE_CONT;
			    Led_Obj.LedGreen.PwmTime.Total = 0;
				Led_Obj.RgbLed.Mode = LED_RGB_MODE_OFF;
				break;
			  case LED_CMD_SET_STATE_BLINK_GREEN:
		        Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_GREEN;
			    Led_Obj.LedGreen.State.Led = LED_GREEN_MODE_PULSE;
			    Led_Obj.LedGreen.PwmTime.Total = 20;
				Led_Obj.RgbLed.Mode = LED_RGB_MODE_OFF;
				break;
			  case LED_CMD_SET_STATE_ON_BLUE:
		        Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_BLUE;
			    Led_Obj.LedGreen.State.Led = LED_GREEN_MODE_CONT; //LED_BLUE_MODE_CONT;
			    Led_Obj.LedGreen.PwmTime.Total = 0;
				Led_Obj.RgbLed.Mode = LED_RGB_MODE_OFF;
				break;
			  case LED_CMD_SET_STATE_BLINK_BLUE:
		        Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_BLUE;
			    Led_Obj.LedGreen.State.Led = LED_GREEN_MODE_PULSE; //LED_BLUE_MODE_PULSE;
			    Led_Obj.LedGreen.PwmTime.Total = 20;
				Led_Obj.RgbLed.Mode = LED_RGB_MODE_OFF;
				break;
			  case LED_CMD_SET_STATE_ON_ORANGE:
		        Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_GREEN;
			    Led_Obj.LedGreen.State.Led = LED_GREEN_MODE_OFF;
			    Led_Obj.LedGreen.PwmTime.Total = 0;
				Led_Obj.RgbLed.Mode = LED_RGB_MODE_CONT;
				RgbLedSav.Total = 0;
				break;
			  case LED_CMD_SET_STATE_BLINK_ORANGE:
		        Led_Obj.LedGreen.State.Color = LED_GREEN_COLOR_GREEN;
			    Led_Obj.LedGreen.State.Led = LED_GREEN_MODE_OFF;
			    Led_Obj.LedGreen.PwmTime.Total = 0;
				Led_Obj.RgbLed.Mode = LED_RGB_MODE_PULSE;
				RgbLedSav.Total = 20;
				break;
			}
			Led_Obj.LedGreen.TotalTime.Total = 0;
			Led_Obj.LedGreen.PwmTime.On = Led_Obj.LedGreen.PwmTime.Total / 2;
			Led_Obj.RgbLed.TotalTime = 0;
			Led_Obj.RgbLed.On = RgbLedSav.Total / 2;
		}

        Led_IntObj.Task.Flags.Bits.Tick100mS = 0;
        Led_Obj.LedGreen.TotalTime.Current++;
        
                                                        //Handle the green LED total time / on / off states
        if (Led_Obj.LedGreen.TotalTime.Current > Led_Obj.LedGreen.TotalTime.Total)
        {
            Led_Obj.LedGreen.TotalTime.Current = 0;
        }
        switch (Led_Obj.LedGreen.State.Led)
        {
          case LED_GREEN_MODE_OFF:                      //Off - motor not active
            Led_Obj.LedGreen.State.Pulse = LED_PULSE_STATE_OFF;
            break;
          case LED_GREEN_MODE_CONT:                     //Continous - motor active
            Led_Obj.LedGreen.State.Pulse = LED_PULSE_STATE_ON;
            break;
          case LED_GREEN_MODE_PULSE:                    //Blinking
            if (Led_Obj.LedGreen.TotalTime.Total == 0)
            {                                           //Endless blinking
                Led_Obj.LedGreen.State.Pulse = LED_PULSE_STATE_PULSE;
            }
            else if (Led_Obj.LedGreen.TotalTime.Current < Led_Obj.LedGreen.TotalTime.Total)
            {                                           //Total blinking time is not over
                Led_Obj.LedGreen.State.Pulse = LED_PULSE_STATE_PULSE;
            }
            else
            {                                           //Blink time is over
                if (Led_Obj.LedGreen.State.NextLedState == LED_RGB_MODE_CONT)
                {                                       //Stay in On
                    Led_Obj.LedGreen.State.Pulse = LED_PULSE_STATE_ON;
                     Led_Obj.LedGreen.State.Led = LED_GREEN_MODE_CONT;
                }
                else
                {                                       //LED Off
                    Led_Obj.LedGreen.State.Pulse = LED_PULSE_STATE_OFF;
                    Led_Obj.LedGreen.State.Led = LED_GREEN_MODE_OFF;
                }
            }    
            break;
          case LED_BLUE_MODE_OFF:
          case LED_BLUE_MODE_CONT:
          case LED_BLUE_MODE_PULSE:
          case LED_RED_MODE_CONT:
          case LED_ALL_MODE_CONT:
          case LED_GREENnBLUE_MODE_CONT:
          case LED_BLUEnRED_MODE_CONT:
          case LED_REDnGREEN_MODE_CONT:
            break;
        }
        
                                                        //Handle Green LED PWM
        Led_Obj.LedGreen.PwmTime.Current++;
        if (Led_Obj.LedGreen.PwmTime.Current > Led_Obj.LedGreen.PwmTime.Total)
        {
            Led_Obj.LedGreen.PwmTime.Current = 0;
        }
        switch (Led_Obj.LedGreen.State.Pulse)
        {
          case LED_PULSE_STATE_OFF:                     //LED off
            Led_SetGreen(FALSE);
            break;
          case LED_PULSE_STATE_ON:                      //LED on
            Led_SetGreen(TRUE);
            break;
          case LED_PULSE_STATE_PULSE:                   //LED blinking
            if (Led_Obj.LedGreen.PwmTime.Current <= Led_Obj.LedGreen.PwmTime.On)
            {
                Led_SetGreen(TRUE);
            }
            else
            {
                Led_SetGreen(FALSE);
            }        
            break;
        }    
        
                                                        //Handle RGB LEDs
        Led_Obj.RgbLed.Current++;
        if (Led_Obj.RgbLed.Current > Led_Obj.RgbLed.Total)
        {
            Led_Obj.RgbLed.Current = 0;
        }
        if (Led_Obj.RgbLed.TotalTime)
        {                                               //Count down total time
            Led_Obj.RgbLed.TotalTime--;
            if (Led_Obj.RgbLed.TotalTime == 0)
            {                                           //Action total time is over
                Led_Obj.RgbLed.Mode = LED_RGB_MODE_OFF;
            }    
        }    
        switch (Led_Obj.RgbLed.Mode)
        {
          case LED_RGB_MODE_OFF:                        //Off - motor not active
            Led_Obj.RgbLed.State = LED_PULSE_STATE_OFF;
            break;
          case LED_RGB_MODE_CONT:                       //Continous - motor active
            Led_Obj.RgbLed.State = LED_PULSE_STATE_ON;
            break;
          case LED_RGB_MODE_PULSE:                      //Pulses - active/not active
            if (Led_Obj.RgbLed.Current <= Led_Obj.RgbLed.On)
            {
                Led_Obj.RgbLed.State = LED_PULSE_STATE_ON;
            }
            else
            {
                Led_Obj.RgbLed.State = LED_PULSE_STATE_OFF;
            }    
            break;
          case LED_RGB_MODE_PULSEnOFF:
          case LED_RGB_MODE_PULSEnCONT:
            break;
        }
        Led_SetRgb(Led_Obj.RgbLed.State);

		//Recover LED parameters from COMM_TEXT_Cmnd_setled
		if (Led_CmdState != LED_CMD_SET_STATE_AUTO)
		{
	        Led_Obj.LedGreen.State.Color = LedGreenSav.State.Color;
		    Led_Obj.LedGreen.State.Led = LedGreenSav.State.Led;
		    Led_Obj.LedGreen.TotalTime.Total = LedGreenSav.TotalTime.Total;
		    Led_Obj.LedGreen.PwmTime.Total = LedGreenSav.PwmTime.Total;
		    Led_Obj.LedGreen.PwmTime.On = LedGreenSav.PwmTime.On;
			Led_Obj.RgbLed.Mode = RgbLedSav.Mode;
			Led_Obj.RgbLed.State = RgbLedSav.State;
			Led_Obj.RgbLed.Total = RgbLedSav.Total;
			Led_Obj.RgbLed.On = RgbLedSav.On;
			Led_Obj.RgbLed.TotalTime = RgbLedSav.TotalTime;
		}

    }    
}    

/******************************************************************************
*  Function name: Led_TaskTick100mS
*  Main tasks:
*    Timers of Leds task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called every 100mS, from an interrupt
******************************************************************************/
void Led_TaskTick100mS (void)
{
    Led_IntObj.Task.Flags.Bits.Tick100mS = 1;
}


