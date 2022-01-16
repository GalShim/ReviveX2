/* ========================================
 *
 * Handle ADC
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
            #include <p18f45k22.h>
        #else
            #include <p18f25k22.h>
        #endif
    #endif
#endif
#include <stdlib.h>
#include <string.h>
#include "Types.h"
#include "Motor_Pub.h"
#include "Adc_Pub.h"
#include "Adc_Loc.h"

extern uint8 main_DisFlag_Get (void);
extern void main_DisFlag_Clr (void);

volatile struct{
    struct{
        uint8               Timer;
    }Task;    
} Adc_IntObj;
    
struct{
    struct{
        ADC_TASK_STATE_e    State;              //State of ADC task
        uint8               Channel;            //Channel sampled now
        uint8               SampleCounter;      //Counter to indicate that all channels were sampled enough samples
        union{
            uint8       Byte;
            struct {
                uint8       Tick1mS:1;          //1mSec tick
                uint8       Ready:1;            //All channels were sampled
                uint8       OnlyOneChannel:1;   //Measure only one channel (Adc_Obj.Task.Channel)
                uint8       b3:1;
                uint8       b4:1;
                uint8       b5:1;
                uint8       b6:1;
                uint8       b7:1;
            } Bits;
        } Flags;
    } Task;
    uint16      Adc[ADC_IN_num];                //ADC reading
    uint16      AdcAve[ADC_IN_num][ADC_SAMPLES_NUM]; //Samples to calc average
    uint16      Input[ADC_IN_num];              //Phisycal units
    uint16      Acurate_5V;                     //5V in 0.01V units
    #ifdef SW_UC_PIC18F
        struct{
            uint16      RefTimer;                   //Timer of ref resistor
            uint16      ThermTime;                  //Timer of thermistor
            uint16      Temperature;                //Temperature in 0.1C units
        } Therm;
    #endif
} Adc_Obj;    

#ifdef SW_TEMP_ADD_RES_RATIO
    uint16 Adc_SavResRatio;
    uint16 Adc_GetResRatio(void)
    {
        return(Adc_SavResRatio);
    }    
#endif

//Test
uint16 StartTimer = 500; //10000; //3000;

#ifdef SW_DEBUG_TEMP_EMUL
    uint16 Adc_Debug_Temp = SW_DEBUG_TEMP_EMUL_VAL;
#endif
//=============================================================================
//=============================================================================
//Hardware functions
//=============================================================================
//=============================================================================
//--------------------------------------------------------------
// Registers
//--------------------------------------------------------------
#ifdef SW_UC_PIC16F
    #define ADC_REG_CONVERT_nDONE   ADCON0bits.GO_nDONE
    #define ADC_REG_ON              ADCON0bits.ADON
#endif
#ifdef SW_UC_PIC18F
    #define ADC_REG_CONVERT_nDONE   ADCON0bits.GO_NOT_DONE
    #define ADC_REG_ON              ADCON0bits.ADON
#endif
//--------------------------------------------------------------
//Convert code to ADC channel #
//--------------------------------------------------------------
#ifdef SW_UC_PIC16F
    const uint8 Adc_ConvertCodeToChannel[ADC_IN_num] = {
         0, //ADC_IN_MOT_X_I
         1, //ADC_IN_MOT_Y_I
         2, //ADC_IN_RGB_LED_I
         3, //ADC_IN_MOT_X_V
         4, //ADC_IN_MOT_Y_V
         5, //ADC_IN_RGB_LED_V
         6, //ADC_IN_12V_V
        12, //ADC_IN_5V_V
         8, //ADC_IN_0V_V
        10  //ADC_IN_DAC_SAMP_V
    };
#endif
#ifdef SW_UC_PIC18F
    rom const uint8 Adc_ConvertCodeToChannel[ADC_IN_num] = {
        13, //ADC_IN_12V_V
        31, //ADC_IN_5V_V           //Internal
        11, //ADC_IN_SupCur_I
        0   //ADC_IN_Temp_C         //Dummy. Not analog in
        #ifdef SW_PROD_ReviveRf
        ,9  //ADC_IN_OUT_V
        #endif
    };
#endif
//--------------------------------------------------------------
// Thermistor
//--------------------------------------------------------------
#define ADC_TEMPTABLE_LEN ((50-5)+1+2)
#ifdef SW_UC_PIC18F
 const rom struct{                  
#else
 const struct{                  
#endif
    uint16  ResRatio;       //Resistance ratio (*100)
    uint16  Temp;           //Temperature in 0.1C units
 } ADC_TEMP_TABLE [ADC_TEMPTABLE_LEN+1] = {
    {999,  00},
    {718,  50},
    {708,  60},
    {697,  70},
    {687,  80},
    {676,  90},
    {666, 100},
    {655, 110},
    {644, 120},
    {633, 130},
    {622, 140},
    {611, 150},
    {600, 160},
    {589, 170},
    {578, 180},
    {567, 190},
    {555, 200},
    {544, 210},
    {533, 220},
    {522, 230},
    {511, 240},
    {500, 250},
    {489, 260},
    {478, 270},
    {467, 280},
    {457, 290},
    {446, 200},
    {436, 310},
    {425, 320},
    {415, 330},
    {405, 340},
    {395, 350},
    {385, 360},
    {376, 370},
    {366, 380},
    {357, 390},
    {348, 400},
    {338, 410},
    {330, 420},
    {321, 430},
    {312, 440},
    {304, 450},
    {296, 460},
    {288, 470},
    {280, 480},
    {272, 490},
    {265, 500},
    {000, 990},
    {000, 990}
};

//--------------------------------------------------------------
// Utilities
//--------------------------------------------------------------
#ifdef SW_UC_PIC16F
    void Adc_StartConvert (void)                    //Start ADC standard conversion
    {
        ADCON0 &= ~0b1111100;
        ADCON0 |= (Adc_ConvertCodeToChannel[Adc_Obj.Task.Channel] << 2);  //Select channel
        ADCON0bits.GO_nDONE = 1;                    //Start conversion
    }
    void Adc_StartConvert5v (void)                  //start ADC 5V conversion
    {
        ADCON1 = 0b11100000;                        //Vdd ref voltage
    }    
    void Adc_EndConvert (void)                      //End conversion
    {
      uint16 TempW;
        TempW = ADRESH & 0x03;                      //Save reading
        TempW = TempW << 8;
        TempW += ADRESL;
        Adc_Obj.Adc[Adc_Obj.Task.Channel] = TempW;
    }
    void Adc_HwInit (void)                          //Init hardware
    {
        FVRCON = 0b10000011;                        //4.096V ref
        #ifndef SW_MOT_BLDC                         //DC applicator
            ANSELA |= 0b00101111;
            ANSELB |= 0b00000111;
            ANSELE |= 0x00000011;
        #else                                       //BLDC applicator
            ANSELA |= 0b00000100;
            ANSELB |= 0b00000001;
            ANSELE |= 0x00000011;
        #endif
    
        ADCON0 = 0b00000000;
        ADCON1 = 0b11101000;                        //FVR BUF2 ref voltage
    }
#endif
#ifdef SW_UC_PIC18F
    void Adc_StartConvert (void)                    //Start ADC standard conversion
    {
        ADCON0 &= ~0b1111100;
        ADCON0 |= (Adc_ConvertCodeToChannel[Adc_Obj.Task.Channel] << 2);  //Select channel
        ADCON0bits.GO_NOT_DONE = 1;                 //Start conversion
    }
    void Adc_StartConvert5v (void)                  //Start ADC 5V conversion
    {
        ADCON1 = 0b11100000;                        //Vdd ref voltage
    }    
    void Adc_StartConvert12v (void)                 //Start ADC 12V conversion
    {
        ADCON1 = 0b11100000;                        //Vdd ref voltage
    }    
    void Adc_EndConvert (void)                      //End conversion
    {
      uint16 TempW;
        TempW = ADRESH & 0x03;                      //Save reading
        TempW = TempW << 8;
        TempW += ADRESL;
        Adc_Obj.Adc[Adc_Obj.Task.Channel] = TempW;
    }
    void Adc_HwInit (void)                          //Init hardware
    {
        VREFCON0 = 0b10110000;                      //4.096V ref
        ANSELA &= ~0b00000111;
        #ifdef SW_PROD_ReviveRf
          ANSELB |= 0b00111000;
        #else
          ANSELB |= 0b00110000;
        #endif
        ADCON0 = 0b00000000;
        ADCON1 = 0b11101000;                        //FVR BUF2 ref voltage
        ADCON2 = 0b10101101;
    }    
    void Adc_Hw_SetLoadCap(void)                    //Set to load capacitor
    {
        TRISAbits.TRISA1 = 1;                       //RA1 input
        TRISAbits.TRISA0 = 0;                       //RA0 output
        TRISAbits.TRISA2 = 0;                       //RA2 output
        PORTA |= 0b00000101;                        //Load via RA0 and RA2
    }
    void Adc_Hw_SetDischargeMeasure(void)           //Set to discharge measurement
    {
        CM1CON0 = 0b10011101;                       //Set comparator
        CM2CON1 = 0b00001000;
        VREFCON1 = 0b11000000;                      //Set DAC
        VREFCON2 = 3;                               //~10% Vdd
        T5CON = 0b00000100;                         //Set timer mode
                                                    //T5CON bit3 (TxSOSCEN) must be 0, to enable use of RF_ON
        T5GCON = 0b00000010;
        TMR5H = 0;                                  //Clear timer
        TMR5L = 0;
    }
    void Adc_Hw_SetDischargeRef(void)               //Set to discharge capacitor via ref resistor
    {
        TRISAbits.TRISA2 = 1;                       //Leave thermistor floating
        PORTA &= ~0b00000001;                       //Start discharge via ref resistor
        T5CONbits.TMR5ON = 1;                       //Enable counter
    }
    void Adc_Hw_SetDischargeTherm(void)             //Set to discharge capacitor via thermistor
    {
        TRISAbits.TRISA0 = 1;                       //Leave ref resistor floating
        PORTA &= ~0b00000100;                       //Start discharge via thermistor
        T5CONbits.TMR5ON = 1;                       //Enable counter
    }
    void Adc_Hw_StopThermMeasure(void)              //Stop thermistor measurement
    {
        CM1CON0 = 0b10010101;
        CM2CON1 = 0b00001000;
        T5CON = 0b00110100;
        T5GCON = 0b00000010;
    }    
    uint16 Adc_Hw_GetTime(void)                     //Get timer value
    {
      uint16 TimerVal = TMR5H;
        TimerVal = TimerVal << 8;
        TimerVal += TMR5L;
        return(TimerVal);
    }
#endif
//=============================================================================
//=============================================================================

/******************************************************************************
*  Function name: Adc_CheckReady
*  Main tasks:
*    Check if ADC ready (all channels were sampled
*  Parameters:
*    None
*  Returned values:
*    True/false
*  Remarks:
*    None
******************************************************************************/
uint8 Adc_CheckReady (void)
{
  uint8 Result = FALSE;
    if (Adc_Obj.Task.SampleCounter == 0)
    {
        Result = TRUE;
    }
    return(Result);
}    

/******************************************************************************
*  Function name: Adc_SetOneChannel
*  Main tasks:
*    Set one channel to measure
*  Parameters:
*    TRUE/FALSE
*    Channel
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void Adc_SetOneChannel (uint8 Cmnd, ADC_IN_e Channel)
{
    if (Cmnd)
    {
        //Adc_Obj.Task.SampleCounter = ADC_SAMPLES_NUM + 1;
        Adc_Obj.Task.Flags.Bits.OnlyOneChannel = 1;
        Adc_Obj.Task.Channel = Channel;
    }
    else
    {
        Adc_Obj.Task.Flags.Bits.OnlyOneChannel = 0;
    }    
}    

/******************************************************************************
*  Function name: Adc_CalcAverage
*  Main tasks:
*    Calc average of samples
*  Parameters:
*    Channel
*    New sample
*  Return
*    Average sample
*  Remarks:
*    None
******************************************************************************/
uint16 Adc_CalcAverage (ADC_IN_e Channel, uint16 NewSample)
{
  uint8 i;
  uint16 Total = 0;
    for (i=0; i<(ADC_SAMPLES_NUM-1); i++)
    {                                               //Move old samples
        Total += Adc_Obj.AdcAve[Channel][i];
        Adc_Obj.AdcAve[Channel][i+1] = Adc_Obj.AdcAve[Channel][i];
    }
    Adc_Obj.AdcAve[Channel][0] = NewSample;         //Enter new sample
    Total += NewSample;
    Total /= ADC_SAMPLES_NUM;
    return(Total);
}    

/******************************************************************************
*  Function name: Adc_GetInput
*  Main tasks:
*    Get ADC input
*  Parameters:
*    Input code
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
uint16 Adc_GetInput(ADC_IN_e Channel)
{
    return (Adc_Obj.Input[Channel]);
}    

/******************************************************************************
*  Function name: Adc_SampleRequest
*  Main tasks:
*    Sample one input
*  Parameters:
*    Channel code
*    Use/not average
*  Returned values:
*    Input
*  Remarks:
*    Also restarts periodic ADC sampling
******************************************************************************/
uint16 Adc_SampleRequest (ADC_IN_e Channel, uint8 UseAve)
{
  uint32 Temp32;
  uint16 LoopLimit = ADC_CONVERT_nDONE_LOOP_LIMIT;
  uint16 ReturnVal = 0;
  ADC_IN_e SaveChannel = Adc_Obj.Task.Channel;
    if (Channel <= ADC_IN_ChannelMax)
    {                                                   //ADC channel
        ADC_REG_ON = 0;                                 //Stop ADC (if was active)
        Adc_Obj.Task.Channel = Channel;
        ADC_REG_ON = 1;                                 //Start ADC
        if (Channel == ADC_IN_5V_V)
        {                                               //5V is measured internally
            Adc_StartConvert5v();
        }
        #ifdef SW_UC_PIC18F
         if (Channel == ADC_IN_12V_V)
         {                                               //5V ref for 12V measure, to reach up to ~15V
             Adc_StartConvert12v();
         }
        #endif
        Adc_StartConvert();                             //Start conversion
        while ((ADC_REG_CONVERT_nDONE) && (LoopLimit)) {LoopLimit--;} //Wait till conversion end
        if (LoopLimit)
        {
            Adc_EndConvert();                               //End conversion, and save reading
            if (UseAve)
            {
                Adc_Obj.Adc[Adc_Obj.Task.Channel] =  Adc_CalcAverage (Adc_Obj.Task.Channel, Adc_Obj.Adc[Adc_Obj.Task.Channel]); //Save average
            }
            else
            {
                Adc_Obj.Input[Adc_Obj.Task.Channel] = Adc_Obj.Adc[Adc_Obj.Task.Channel];
            }    
            ADC_REG_ON = 0;                                 //Stop ADC
            #ifdef SW_UC_PIC16F
             switch (Channel)                               //Convert to phisical units
             {
              case ADC_IN_12V_V:                            //Convert 12V sample to 0.1V units
                Temp32 = Adc_Obj.Adc[Adc_Obj.Task.Channel];
                Temp32 *= 100;
                Temp32 /= 362; //384; //368; //399; //798; //4.7K/10K resistors
                Adc_Obj.Input[Adc_Obj.Task.Channel] = (uint16)Temp32;
                break;
              case ADC_IN_5V_V:                             //Convert 5V sample to 0.1V units
                Temp32 = 0x003ff000; //4096 * 1023;         
                Temp32 /= Adc_Obj.Adc[Adc_Obj.Task.Channel];
                Temp32 /= 200; //100;
                Adc_Obj.Input[Adc_Obj.Task.Channel] = (uint16)Temp32;
                Adc_HwInit();                               //Re-config to other ADCs
                break;
             }
            #endif
            #ifdef SW_UC_PIC18F
             switch (Channel)                               //Convert to phisical units
             {
              case ADC_IN_12V_V:                            //Convert 12V sample to 0.1V units
                Temp32 = Adc_Obj.Adc[Adc_Obj.Task.Channel];
                Temp32 *= 100;
                Temp32 /= 654;                              //5V ref
                Temp32 *= Adc_Obj.Acurate_5V;
                #ifdef SW_HW_SUP_24V
                  Temp32 /= 250; //203;                            //1.91K/10K resistors instead of 4.7K/10K
                #else
                  Temp32 /= 500;                            //Adjust to measured value of 5V
                #endif
                Adc_Obj.Input[Adc_Obj.Task.Channel] = (uint16)Temp32;
                Adc_HwInit();                               //Re-config to other ADCs
                break;
              case ADC_IN_5V_V:                             //Convert 5V sample to 0.1V units
                Temp32 = 0x003ff000;                        //4096 * 1023;         
                Temp32 /= Adc_Obj.Adc[Adc_Obj.Task.Channel];
                Temp32 /= 10;
                Adc_Obj.Acurate_5V = (uint16)Temp32;        //Save 5V in 0.01V units
                Temp32 /= 10;
                Adc_Obj.Input[Adc_Obj.Task.Channel] = (uint16)Temp32;
                Adc_HwInit();                               //Re-config to other ADCs
                break;
              case ADC_IN_SupCur_I:                         //Convert to 1mA units
                Temp32 = Adc_Obj.Adc[Adc_Obj.Task.Channel] * 100;
                Temp32 /= 25;
                Adc_Obj.Input[Adc_Obj.Task.Channel] = (uint16)Temp32;
                break;
              case ADC_IN_Temp_C:
                break;
              #ifdef SW_PROD_ReviveRf
               case ADC_IN_OUT_V:
                 Adc_Obj.Input[Adc_Obj.Task.Channel] = Adc_Obj.Adc[Adc_Obj.Task.Channel];
                 break;
              #endif
             }
            #endif
            ReturnVal = Adc_Obj.Input[Adc_Obj.Task.Channel];
        }
    }
    else
    {                                                   //Special channel
    }
    Adc_Obj.Task.Channel = SaveChannel;
    return (ReturnVal);
}

/******************************************************************************
*  Function name: Adc_GetVal
*  Main tasks:
*    Get input value (in V,A,C, etc.)
*  Parameters:
*    Channel code
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
uint16 Adc_GetVal(ADC_IN_e Channel)
{
  uint16 Result = 0;
    switch (Channel)
    {
      case ADC_IN_12V_V:        //0.1V units
        Result = Adc_Obj.Input[ADC_IN_12V_V];
        
        break;
      case ADC_IN_5V_V:         //0.1V units
        Result = Adc_Obj.Input[ADC_IN_5V_V];
        break;
      #ifdef SW_UC_PIC18F
       case ADC_IN_SupCur_I:    //1mA units
         Result = Adc_Obj.Input[ADC_IN_SupCur_I];
         break;
       case ADC_IN_Temp_C:      //0.1C units
         Result = Adc_Obj.Input[ADC_IN_Temp_C];
         break;
          #ifdef SW_UC_PIC18F
           case ADC_IN_OUT_V:    //?? units
             Result = Adc_Obj.Input[ADC_IN_OUT_V];
             break;
          #endif
      #endif
    }
    return(Result);
}  

/******************************************************************************
*  Function name: Adc_TaskInit
*  Main tasks:
*    Initialize ADC task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called on start up
******************************************************************************/
void Adc_TaskInit (void)
{
  uint8 i, j;
    Adc_HwInit();
    Adc_Obj.Task.Flags.Byte = 0;
    #ifdef SW_UC_PIC18F
      IPR2bits.C1IP = 1;
    #endif

    for (i=0; i<ADC_IN_num; i++)
    {
        for (j=0; j<ADC_SAMPLES_NUM; j++)
        {
            Adc_Obj.AdcAve[i][j] = 0;
        }
        Adc_Obj.Input[i] = 0;
    }    
    Adc_Obj.Task.State = ADC_TASK_STATE_INIT;
    Adc_IntObj.Task.Timer = ADC_SAMPLE_TIME;
    //Adc_IntObj.Task.Timer = ADC_STARTUP_DELAY;
    Adc_Obj.Task.SampleCounter = (ADC_IN_num + 1) * ADC_SAMPLES_NUM;
}


/******************************************************************************
*  Function name: Adc_TaskMain
*  Main tasks:
*    Handle ADC task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called periodically
******************************************************************************/
void Adc_TaskMain (void)
{
    switch (Adc_Obj.Task.State)
    {
      case ADC_TASK_STATE_INIT:                     //Init state
        //if (Adc_Obj.Task.Flags.Bits.Tick1mS)        //Startup delay
        if (StartTimer == 0)                        //Startup delay
        {
            Adc_Obj.Task.Flags.Bits.Tick1mS = 0;
            Adc_Obj.Task.Channel = ADC_IN_num - 1;
            Adc_Obj.Task.Flags.Byte = 0;
            #ifndef SW_ADC_NO_PERIODIC_SAMPLES
                Adc_Obj.Task.State = ADC_TASK_STATE_START;
            #endif
        }
        break;
        
      case ADC_TASK_STATE_START:                    //Start ADC samples
        if ((Adc_Obj.Task.Flags.Bits.Tick1mS) ||
            (!Adc_CheckReady()))                    //If waiting to finish scans, ignore timeout between samples
        {                                           //1mS tick. Start sampling all channels
            Adc_Obj.Task.Flags.Bits.Tick1mS = 0;
            if (Adc_Obj.Task.Flags.Bits.OnlyOneChannel == 0)
            {                                       //Not one channel - continue to next channel
                Adc_Obj.Task.Channel++;             //Next channel
                if (Adc_Obj.Task.Channel >= ADC_IN_num)
                {
                    Adc_Obj.Task.Channel = 0;
                }
            }
            if (Adc_Obj.Task.SampleCounter)         //Check that all shannels were sampled
            {
                Adc_Obj.Task.SampleCounter--;
            }
            //else
            //{
            //    Adc_Obj.Task.Flags.Bits.OnlyOneChannel = 0;
            //}    
            Adc_Obj.Task.State = ADC_TASK_STATE_CHANNEL_SAMPLE;
        }    
        break;
        
      case ADC_TASK_STATE_CHANNEL_SAMPLE:           //Sample ADC channel
        #ifdef SW_UC_PIC18F
         if (Adc_Obj.Task.Channel == ADC_IN_Temp_C)
         {                                          //Measure temperature - special handling
             Adc_Obj.Task.State = ADC_TASK_STATE_THERM_LOAD_CAP1;
         }
         else if (Adc_Obj.Task.Channel == ADC_IN_SupCur_I)
         {                                          //Measure currnet - special handling
             if (Motor_GetMotor(MOT_ID_RF) == MOTOR_PARAM_MODE_OFF)
             {                                      //RF not active
                #ifdef SW_ADC_CHECK_I_ON_RF         //Skip
                    Adc_Obj.Task.State = ADC_TASK_STATE_START;
                #else                               //Check current
                    Adc_SampleRequest(Adc_Obj.Task.Channel, TRUE);
                    Adc_Obj.Task.State = ADC_TASK_STATE_START;
                #endif
             }    
             else
             {
                 if ((Motor_Rf_EnState_Get() == MOTOR_RF_EN_STATE_ON1_EN1) &&
                     (Motor_Rf_Sample_Time_Act()))
                {                                       //RF in active state - measure
                    Adc_SampleRequest(Adc_Obj.Task.Channel, TRUE);
                    Adc_Obj.Task.State = ADC_TASK_STATE_START;
                }   
             } 
         }
         else
        #endif
        {
            Adc_SampleRequest(Adc_Obj.Task.Channel, TRUE);
            Adc_Obj.Task.State = ADC_TASK_STATE_START;
        }
        break;

    #ifdef SW_UC_PIC18F
      case ADC_TASK_STATE_THERM_LOAD_CAP1:          //Charge capacitor 1
        Adc_Hw_SetLoadCap();                        //Set to load capacitor
        Adc_IntObj.Task.Timer = ADC_LOAD_CAP_TIME;  //time to load
        Adc_Obj.Task.Flags.Bits.Tick1mS = 0;
        Adc_Obj.Task.State = ADC_TASK_STATE_THERM_DISC_CAP1;
        break;
        
      case ADC_TASK_STATE_THERM_DISC_CAP1:          //Discharge capacitor 1
        if (Adc_Obj.Task.Flags.Bits.Tick1mS)
        {                                           //Timeout over - capacitor is charged
          uint16 LoopLimit = ADC_C1IF_LOOP_LIMIT;
            Adc_IntObj.Task.Timer = ADC_DISC_CAP_MAX_TIME;
            Adc_Obj.Task.Flags.Bits.Tick1mS = 0;
            main_DisFlag_Clr();
            Adc_Hw_SetDischargeMeasure();
            PIR2bits.C1IF = 0;
            PIE2bits.C1IE = 1;
            Adc_Hw_SetDischargeRef();               //Start discharge via ref resistor
            Adc_Obj.Task.State = ADC_TASK_STATE_THERM_LOAD_CAP2;
        }    
        break;
        
      case ADC_TASK_STATE_THERM_LOAD_CAP2:          //Charge capacitor 2
        if (main_DisFlag_Get())
        {                                           //Capacitor was discharged
            Adc_Obj.Therm.RefTimer = Adc_Hw_GetTime();  //Save the time
            Adc_Hw_SetLoadCap();                    //Set to load capacitor
            Adc_IntObj.Task.Timer = ADC_LOAD_CAP_TIME;  //time to load
            Adc_Obj.Task.Flags.Bits.Tick1mS = 0;
            Adc_Obj.Task.State = ADC_TASK_STATE_THERM_DISC_CAP2;
        }    
        else if (Adc_Obj.Task.Flags.Bits.Tick1mS)
        {                                           //Timeout - restart
            Adc_Obj.Task.State = ADC_TASK_STATE_THERM_END;
        }    
        break;
        
      case ADC_TASK_STATE_THERM_DISC_CAP2:          //discharge capacitor 2
        if (Adc_Obj.Task.Flags.Bits.Tick1mS)
        {                                           //Timeout over - capacitor is discharged
          uint16 LoopLimit = ADC_C1IF_LOOP_LIMIT;
            Adc_IntObj.Task.Timer = ADC_DISC_CAP_MAX_TIME;
            Adc_Obj.Task.Flags.Bits.Tick1mS = 0;
            main_DisFlag_Clr();
            Adc_Hw_SetDischargeMeasure();
            PIR2bits.C1IF = 0;
            PIE2bits.C1IE = 1;
            Adc_Hw_SetDischargeTherm();             //Start discharge via thermistor
            Adc_Obj.Task.State = ADC_TASK_STATE_THERM_CALC_TEMP;
        }    
        break;
        
      case ADC_TASK_STATE_THERM_CALC_TEMP:          //calculate temperature
        if (main_DisFlag_Get())
        {                                           //Capacitor was discharged
            Adc_Obj.Therm.ThermTime = Adc_Hw_GetTime();  //Save the time
                                                    //Calculate temperature
                                                    
                                                    //Back to ADC sampling
            Adc_Obj.Task.State = ADC_TASK_STATE_THERM_END;
        }    
        else if (Adc_Obj.Task.Flags.Bits.Tick1mS)
        {                                           //Timeout - restart
            Adc_Obj.Task.State = ADC_TASK_STATE_THERM_END;
        }    
        break;
        
      case ADC_TASK_STATE_THERM_END:                //End - calculate resistanse
        {
          uint32 Temp32C;
          uint16 Temp16C, Temp16Res, Temp16Delta;
          uint8 i;
            Temp32C = Adc_Obj.Therm.ThermTime;      //Calc resistance ratio
            Temp32C *= 1000;
            Temp32C /= Adc_Obj.Therm.RefTimer;
            Temp16C = (uint16)Temp32C;
            #ifdef SW_TEMP_ADD_RES_RATIO
                Adc_SavResRatio = Temp16C;
            #endif
            for (i=0; ((i<ADC_TEMPTABLE_LEN) && (Temp16C<ADC_TEMP_TABLE[i].ResRatio)); i++) {}  //Search temperature
                                                    //Check related state between degrees
            Temp16Res = Temp16C - ADC_TEMP_TABLE[i].ResRatio;
            Temp16Res *= 10;                        //Multply by 10 - 0.1C units
            Temp16Delta = ADC_TEMP_TABLE[i-1].ResRatio - ADC_TEMP_TABLE[i].ResRatio;
            Temp16Res /= Temp16Delta;
                                                    //Get temperature from table
            Adc_Obj.Input[ADC_IN_Temp_C] = ADC_TEMP_TABLE[i].Temp - Temp16Res;
            Adc_Obj.Input[ADC_IN_Temp_C] =  Adc_CalcAverage (ADC_IN_Temp_C, Adc_Obj.Input[ADC_IN_Temp_C]); //Save average
            #ifdef SW_DEBUG_TEMP_EMUL
              Adc_Obj.Input[ADC_IN_Temp_C] = Adc_Debug_Temp;
            #endif
            Adc_HwInit();
            Adc_IntObj.Task.Timer = ADC_SAMPLE_TIME;
            Adc_Obj.Task.State = ADC_TASK_STATE_START;
        }
        break;
    #endif
    }
}

/******************************************************************************
*  Function name: Adc_TaskTick1mS
*  Main tasks:
*    Timers of ADC task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called every 1mS, from an interrupt
******************************************************************************/
void Adc_TaskTick1mS (void)
{
    if (Adc_IntObj.Task.Timer)
    {
        Adc_IntObj.Task.Timer--;
    }
    if (Adc_IntObj.Task.Timer == 0)
    {
        Adc_IntObj.Task.Timer = ADC_SAMPLE_TIME;
        Adc_Obj.Task.Flags.Bits.Tick1mS = 1;
    }
    
    if (StartTimer) {StartTimer--;}
}    
