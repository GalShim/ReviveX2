/* ========================================
 *
 * Application
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
#include "Comm_Codes_Pub.h"
#include "App_Codes_Pub.h"
#include "Comm_Pub.h"
#include "Eeprom_Pub.h"
#include "Led_Pub.h"
#include "Motor_Pub.h"
#include "Keys_Pub.h"
#include "Comm_Pub.h"
#include "Adc_Pub.h"
#ifdef SW_UC_PIC18F
    #include "..\..\RfApplicator\V1-0\IO_Pub.h"
#endif
#include "App_Pub.h"
#include "App_Loc.h"
#ifdef SW_TIP_MEM
    #include "..\..\Revive\V1-0\TipMem_Pub.h"
#endif
#ifdef SW_VSUP_GEN
    #include "..\..\Revive\V1-0\Revive_Vsup_Pub.h"
#endif

volatile struct{
    uint16      Timer;          //Task timer
    uint16      TemperatureTimer; //Temperature fault timer
    uint16      PulseTimer;     //One pulse timer
    uint8       TestNPTimer;    //No Pulse state test timer
	uint8		FirePulse;
} App_IntObj;

struct{
    APP_STATE_e     State;      //Task state
    APP_DEVICE_e    Device;     //Device #
    union{
        uint16      Byte;
        struct {
            uint8       RgbLedState:1;      //RGB LED state
            uint8       RgbLedAct:1;        //RGB LED active
            uint8       MotorXFault:1;      //MotorX fault
            uint8       MotorYFault:1;      //MotorY fault
            uint8       RgbLedFault:1;      //RGB LED fault
            uint8       Voltage12VFault:1;  //12V fault
            uint8       Voltage5VFault:1;   //5V fault
            uint8       TempFault:1;        //Temperature fault
            uint8       BuzWasAct:1;        //Buzzer beep was activated
            uint8       CurrentFault:1;     //Input current fault
            uint8       SafetyTestState:1;  //Safety test: On or En
            uint8       OverHeat:1;         //Temperature not in fault, but over heat
            uint8       OverHeatState:1;    //Over heat state in active mode
            uint8       PulseDone:1;        //Pulse done (Revive)
            uint8       KeyLastState:1;     //Last state of key (1-pressed)
            uint8       TestSendTechTemp:1; //Test mode. Send Tech event with temperature
        }   Flags;
    } Status;
    struct{
        APP_EVENT_STATE_e   State;      //Event state
    } Event[APP_EVENT_num];
    APP_RF_FAULT_e   RfFault;               //RF fault function was excecuted
    struct{                                 //Test
        uint16      SendMsgTimer;           //Timer to send messages
        uint16      KeyPressTimer;          //Timer of key pressed
    } Test;    
} App_Obj;    

#ifdef SW_MOT_RF
    #define APP_RF_TEST_SAMPLE_NUM  3           //Number of samples
    #define APP_RF_TEST_FAULT_NUM   3           //Max number of faults
    struct{
        APP_RF_TEST_STATE_e State;
        APP_RF_TEST_STATE_e StateNP;
        //uint8               FaultCounter;
        uint8               SampleCounter;
        uint16              M1;
        uint16              M2;
        uint16              M3;
        uint8               PulseOvrCurCounter; //Pulse over current counter
        uint8               Timer;              //Timer to test during work mode
        uint8               FaultCounter;       //Count faults before generating an event
        uint8               BitTestCounter;     //Count BIT RF tests
    } App_RfTest;    
#endif

#ifdef SW_UC_PIC18F
   const rom uint8* TextAddr;
#else
   const uint8* TextAddr;
#endif
uint8 TextLen;

#ifdef SW_DEBUG_TRAP_EVENT
    volatile uint8 App_EventSav = APP_EVENT_num;
#endif

#include "App_Text_c.h"

#define APP_CONFIG_PARAM_MAX_LEN        APP_TECHINFO_PARAM_ID_TEMP_LEN
uint8 App_ConfigParamTempBuf[APP_CONFIG_PARAM_MAX_LEN];
RESULT_t App_TestRf (uint8 Init);

//#define APP_PULSE_CURRENT_MAX_CHECK
#ifdef APP_PULSE_CURRENT_MAX_CHECK
    uint16 App_PulseCurrentMax1 = 0;
    uint16 App_PulseCurrentMax2 = 0;
#endif
#ifdef SW_APP_OVERCURRENT_KEY
    #define OVERCURRENT_SIMULATOR_TIME 500
    uint16 Overcurrent_Simulator_Timer = OVERCURRENT_SIMULATOR_TIME;
#endif

#ifdef SW_PROD_ReviveRf_RepeatPulse
	#define ReviveRf_RepeatPulse_TIME 3000 //8.11.2020   2000 //2 sec
	struct{
		uint8	Act;
		uint16	Timer;
	} ReviveRf_RepeatPulse;
#endif

#ifdef SW_PATCH_SYNC_OVERV
    void App_SetEvent (APP_EVENT_CODE_e EventCode);
    void App_Exec_OvervBit (void)
    {
      uint16 TempVal;
        TempVal = Adc_GetVal(ADC_IN_12V_V);
        if (TempVal >= APP_BIT_12V_MAX)
        {
            //while (1) {ClrWdt();}
            App_SetEvent(APP_EVENT_FAULT_OverVolt);
        }
    }
#endif
/******************************************************************************
*  Function name: App_PulseDone
*  Main tasks:
*    Pulse is done - info from motor
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Revive motor
******************************************************************************/
#ifdef SW_PROD_ReviveRf
 void App_PulseDone (void)
 {
     App_Obj.Status.Flags.PulseDone = 1;
 }
#endif

/******************************************************************************
*  Function name: App_SetEvent
*  Main tasks:
*    Set event
*  Parameters:
*    Event code
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void App_SetEvent (APP_EVENT_CODE_e EventCode)
{
    if (App_Obj.Event[EventCode].State == APP_EVENT_STATE_IDLE)
    {                                           //Event not active - generate
        Comm_SetEvent (EventCode, TRUE);        //Send event
        App_Obj.Event[EventCode].State = APP_EVENT_STATE_SENT;
    }    
    #ifdef SW_DEBUG_TRAP_EVENT
        switch (EventCode)
        {
          case APP_EVENT_FAULT_OverVolt:
          case APP_EVENT_FAULT_Temperature:
          case APP_EVENT_FAULT_OverCur:
          case APP_EVENT_FAULT_Rf:
          case APP_EVENT_FAULT_Tip:
            App_EventSav = EventCode;
            break;
        }    
    #endif
}    

/******************************************************************************
*  Function name: App_ClearEvent
*  Main tasks:
*    Clear event
*  Parameters:
*    Event code
*  Returned values:
*    None
*  Remarks:
*    If event was not yet sent via RS232 - it will still be sent.
******************************************************************************/
void App_ClearEvent (APP_EVENT_CODE_e EventCode)
{
    App_Obj.Event[EventCode].State = APP_EVENT_STATE_IDLE;
}    

/******************************************************************************
*  Function name: App_CheckDevice
*  Main tasks:
*    Check devices
*  Parameters:
*    Device code
*  Returned values:
*    None
*  Remarks:
*    Activate the device for a short period
*    If fault - generate fault event
******************************************************************************/
//-------------------------------------------------
uint16 App_CheckAdcSampleEvent (uint16 AdcSample, uint16 MinVal, uint16 MaxVal)
{
  uint8 Result = TRUE;
    if ((AdcSample >= MinVal) && (AdcSample <= MaxVal))
    {
        Result = FALSE;     //No event
    }    
    return (Result);
}    
//-------------------------------------------------
void App_CheckDevice (APP_DEVICE_e Device)
{
  #define ACT_TO_SAMPLE_MOTOR_DELAY   1000                  //Longer, current need time to stable state.
  #define ACT_TO_SAMPLE_RGBLED_DELAY  20                    //Short, not to see the color flash
  uint16 Delay;
    switch (Device)
    {
      #ifdef SW_MOT_X
        case APP_DEVICE_MOTX:                               //Check motor X
          #ifndef SW_PROD_BLDC                              //In BLDC applicator - no motor X current test
            Motor_SetMotor(MOT_ID_X, TRUE);                 //Activate the device
            for (Delay=0; Delay<ACT_TO_SAMPLE_MOTOR_DELAY; Delay++); //Delay
            if (App_CheckAdcSampleEvent(Adc_SampleRequest (ADC_IN_MOT_X_I, FALSE), ADC_MOT_X_CURRENT_MIN, ADC_MOT_X_CURRENT_MAX))
            {                                               //Error - generate fault event
                App_SetEvent(APP_EVENT_FAULT_motorX);
            }
            else
            {                                               //No error - clear event
                App_ClearEvent(APP_EVENT_FAULT_motorX);
            }    
            Motor_SetMotor(MOT_ID_X, FALSE);
          #endif
          break;
        #endif
      #ifdef SW_MOT_Y
        case APP_DEVICE_MOTY:                               //check motor Y
            Motor_SetMotor(MOT_ID_Y, TRUE);                 //Activate the device
            for (Delay=0; Delay<ACT_TO_SAMPLE_MOTOR_DELAY; Delay++); //Delay
            if (App_CheckAdcSampleEvent(Adc_SampleRequest (ADC_IN_MOT_Y_I, FALSE), ADC_MOT_Y_CURRENT_MIN, ADC_MOT_Y_CURRENT_MAX))
            {                                               //Error - generate fault event
                App_SetEvent(APP_EVENT_FAULT_motorY);
            }    
            else
            {                                                           //No error - clear event
                App_ClearEvent(APP_EVENT_FAULT_motorY);
            }    
            Motor_SetMotor(MOT_ID_Y, FALSE);
          break;
        #endif
      #ifdef SW_LED_RGB_VAR
      case APP_DEVICE_RGBLED:                                           //Check RGB LED
        Led_CheckRgb (LED_RGB_ID_R);                                    //Activate only R
        for (Delay=0; Delay<ACT_TO_SAMPLE_RGBLED_DELAY; Delay++);
        if (App_CheckAdcSampleEvent(Adc_SampleRequest (ADC_IN_RGB_LED_I, FALSE), ADC_RGBLED_R_CURRENT_MIN, ADC_RGBLED_R_CURRENT_MAX))
        {                                                               //Error in R - generate fault event
            App_SetEvent(APP_EVENT_FAULT_rgbled);
        }    
        else
        {
            Led_CheckRgb (LED_RGB_ID_G);
            for (Delay=0; Delay<ACT_TO_SAMPLE_RGBLED_DELAY; Delay++);
            if (App_CheckAdcSampleEvent(Adc_SampleRequest (ADC_IN_RGB_LED_I, FALSE), ADC_RGBLED_G_CURRENT_MIN, ADC_RGBLED_G_CURRENT_MAX))
            {                                                           //Error in G - generate fault event
                App_SetEvent(APP_EVENT_FAULT_rgbled);
            }    
            else
            {
                Led_CheckRgb (LED_RGB_ID_B);
                for (Delay=0; Delay<ACT_TO_SAMPLE_RGBLED_DELAY; Delay++);
                if (App_CheckAdcSampleEvent(Adc_SampleRequest (ADC_IN_RGB_LED_I, FALSE), ADC_RGBLED_B_CURRENT_MIN, ADC_RGBLED_B_CURRENT_MAX))
                {                                                   //Error in B - generate fault event
                    App_SetEvent(APP_EVENT_FAULT_rgbled);
                }    
                else
                {
                    App_ClearEvent(APP_EVENT_FAULT_rgbled);         //No error in any LED - clear fault event
                }
            }
        }    
        Led_TaskInit();
        break;
        #endif
    }    
}    

/******************************************************************************
*  Function name: App_RfPulseMonitor
*  Main tasks:
*    Monitor RF pulses
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
#ifdef SW_MOT_RF
    void App_RfPulseMonitor (void)
    {
      uint16 TempCur;
        if (Motor_Rf_EnState_Get() == MOTOR_RF_EN_STATE_ON1_EN1)
        {                                           //Pulse active
            if (Motor_Rf_Sample_Time_PulseStart())
            {                                       //Time to sample pulse on start
                TempCur = Adc_SampleRequest (ADC_IN_SupCur_I, FALSE);
                #ifdef APP_PULSE_CURRENT_MAX_CHECK
                    if (TempCur >App_PulseCurrentMax1) {App_PulseCurrentMax1 = TempCur;}
                #endif
                #ifdef SW_APP_OVERCURRENT_KEY
                 if (Overcurrent_Simulator_Timer == 0)
                #else
               if (TempCur > APP_PULSE_CUR_MAX_LEVEL)
                #endif
                {                                   //Over current of pulse start - pulse off
                    #ifdef SW_APP_OVERCURRENT_KEY
                     Overcurrent_Simulator_Timer = OVERCURRENT_SIMULATOR_TIME;
                    #endif
                    Motor_Rf_Sample_Time_SetMonState(MOTOR_RF_MON_STATE_SET_PULSE_OFF);
                }
                else
                {                                   //No over current - no more samples till end
                    Motor_Rf_Sample_Time_SetMonState(MOTOR_RF_MON_STATE_SET_NEXT);
                }    
            }
            else if (Motor_Rf_Sample_Time_PulseEnd())
            {                                       //Time to sample pulse near end
                TempCur = Adc_SampleRequest (ADC_IN_SupCur_I, FALSE);
                #ifdef APP_PULSE_CURRENT_MAX_CHECK
                    if (TempCur >App_PulseCurrentMax2) {App_PulseCurrentMax2 = TempCur;}
                #endif
                if (TempCur > APP_PULSE_CUR_MAX_LEVEL)
                {                                   //Over current - count
                    App_RfTest.PulseOvrCurCounter++;
                    if (App_RfTest.PulseOvrCurCounter > APP_PULSE_CUR_OVER_COUNT)
                    {                               //Continous over current - pause pulses
                        Motor_Rf_Sample_Time_SetMonState(MOTOR_RF_MON_STATE_SET_PULSE_PAUSE);
                        App_RfTest.PulseOvrCurCounter = 0;
                    }
                    else
                    {
                        Motor_Rf_Sample_Time_SetMonState(MOTOR_RF_MON_STATE_SET_NEXT);
                    }    
                }
                else
                {                                   //No over current - restart counting
                    Motor_Rf_Sample_Time_SetMonState(MOTOR_RF_MON_STATE_SET_NEXT);
                    App_RfTest.PulseOvrCurCounter = 0;
                }    
            }
        }
    }
#endif

/******************************************************************************
*  Function name: App_CheckFaultDeactivate
*  Main tasks:
*    Check if a fault that should deactivate state
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void App_CheckFaultDeactivate (void)
{
    if ((App_Obj.Status.Flags.Voltage12VFault == 1) ||
        (App_Obj.Status.Flags.CurrentFault == 1) ||
        (App_Obj.Status.Flags.TempFault == 1))
    {
        App_Obj.State = APP_STATE_DEACTIVATE;
    }    
}    
   
/******************************************************************************
*  Function name: App_Monitor
*  Main tasks:
*    Monitor devices
*  Parameters:
*    TRUE - Motor off if error
*    FALSE - No motor control
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
#ifdef SW_MOT_RF
    void App_RfFaultState(uint8 BlinkOrange)
    {
        if (App_Obj.RfFault != APP_RF_FAULT_FAULT)
        {
            Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_OFF);    //RF off
            App_Obj.RfFault = APP_RF_FAULT_FAULT;
        }
    }    
#endif
//------------------------------------------------
void App_Monitor(uint8 MotCtrl)
{
  uint16 TempVal;
  uint16 Temperature = Adc_GetVal(ADC_IN_Temp_C);           //Monitor temperature
  uint16 TemperatureLevel;

    #ifdef SW_MOT_BLDC
        if (Motor_GetMotor(MOT_ID_X))               //Monitor motor X (BLDC)
        {                                           //MotorX active - check
            if ((MotorBLDC_CheckRdStuck())
             #ifndef SW_MOT_BLDC_NO_SPEED_CHECK
             || (MotorBLDC_CheckSpeedError())
             #endif
               )
            {                                       //Motor stucked - generate fault event
                if (MotCtrl)
                    {App_SetEvent(APP_EVENT_FAULT_motorX);}
            }    
        }    
    #endif

    #ifdef SW_MOT_RF
       #ifndef SW_IGNORE_THERMISTOR
       #ifndef SW_MOT_RF
        if ((Temperature < (APP_TEMP_MIN_VAL)) || (Temperature > (APP_TEMP_MAX_VAL)))
        {                                                       //Temperature out of valid range
            if (App_IntObj.TemperatureTimer == 0)
            {                                                   //Temperature stable out of valid range - generate event
                App_Obj.Status.Flags.TempFault = 1;
                if (MotCtrl)
                    {App_SetEvent(APP_EVENT_FAULT_Temperature);}//Generate error event
                App_RfFaultState(TRUE);                         //Set fault state
                App_Obj.Status.Flags.BuzWasAct = 0;             //Next high temperature - beep
            }
        }
        else
        {
            App_Obj.Status.Flags.TempFault = 0;
        }
       #endif
       #endif
        {
           #ifndef SW_IGNORE_THERMISTOR
            //App_IntObj.TemperatureTimer = APP_TEMP_FAULT_TIME;  //Temperature in valid range - restart time counting
            //App_Obj.Status.Flags.TempFault = 0;
            //TemperatureLevel = Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_HIGHLEVEL) * 10; //Convert to 0.1C units
            //if (Temperature > TemperatureLevel)
            //{                                                   //Temperature > HighLevel
            //    App_RfFaultState(FALSE);                        //Set fault state
            //    if (App_Obj.Status.Flags.BuzWasAct == 0)
            //    {
            //        IO_BuzCmnd(IO_BUZ_CMND_BEEP_LONG);          //Buzzer beep
            //        App_Obj.Status.Flags.BuzWasAct = 1;         //Only one beep
            //    }
            //}
            //else
           #endif
            {                                                   
                App_Obj.Status.Flags.BuzWasAct = 0;
                
                //Test Supply voltage ----------------------
                TempVal = Adc_GetVal(ADC_IN_12V_V);
                if (TempVal > APP_12V_MAX_VAL)
                {                                               //Over voltage
                    App_Obj.Status.Flags.Voltage12VFault = 1;
                    if (MotCtrl)
                        {App_SetEvent(APP_EVENT_FAULT_OverVolt);} //Generate error event
                    App_RfFaultState(TRUE);                     //Set fault state
                }    
                else
                {
                    App_Obj.Status.Flags.Voltage12VFault = 0;
                }
                //Test Supply current ----------------------
                if (Adc_GetVal(ADC_IN_SupCur_I) > APP_CUR_MAX_LEVEL)
                {                                               //Over current
                    #ifdef SW_APP_OVER_CURRENT_FAULT
                        App_Obj.Status.Flags.CurrentFault = 1;
                        if (MotCtrl)
                            {App_SetEvent(APP_EVENT_FAULT_OverCur);} //Generate error event
                        App_RfFaultState(TRUE);                 //Set fault state
                    #endif
                }
                else
                {
                    App_Obj.Status.Flags.CurrentFault = 0;
                }
                //Test Temperature -------------------------
                #ifndef SW_IGNORE_THERMISTOR
                #ifdef SW_MOT_RF
                 if ((Temperature < (APP_TEMP_MIN_VAL)) || (Temperature > (APP_TEMP_MAX_VAL)))
                 {                                          //Temperature out of range
                     App_Obj.Status.Flags.TempFault = 1;
                     if (MotCtrl)
                     {
                         App_SetEvent(APP_EVENT_FAULT_Temperature); //Generate error event
                     }
                     App_RfFaultState(TRUE);
                 }
                 else
                 {
                     App_Obj.Status.Flags.TempFault = 0;
                 }
                
                 TemperatureLevel = Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_LOWLEVEL);
                 if (Temperature > TemperatureLevel)
                 {                                           //Temperature > LowLevel
                     Led_ActOrange(LED_ORANGE_MODE_CONT,0 ,0); //LED2 orange on
                 }    
                 else
                 {                                           //Temperature < LowLevel
                     Led_ActOrange(LED_ORANGE_MODE_OFF,0 ,0); //LED2 orange off
                 }
                 TemperatureLevel = Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_HIGHLEVEL);
                 if (Temperature > TemperatureLevel)
                 {
                     App_Obj.Status.Flags.OverHeat = 1;
                 }
                 else
                 {
                     App_Obj.Status.Flags.OverHeat = 0;
                 }        
                #endif
                #endif
                
                if ((App_Obj.Status.Flags.Voltage12VFault == 0) &&
                    (App_Obj.Status.Flags.CurrentFault == 0) &&
                    (App_Obj.Status.Flags.TempFault == 0))    
                {                                               //RF active
                    if (App_Obj.RfFault != APP_RF_FAULT_OK)
                    {                                           //First time after fault
                        App_Obj.RfFault = APP_RF_FAULT_OK;
                        if (MotCtrl)
                            {Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_CONT);}//RF on
                    }    
                }        
            }    
        }        
    #endif
}    

/******************************************************************************
*  Function name: App_CommCmnd
*  Main tasks:
*    Receive command from communication
*  Parameters:
*    Command code
*  Returned values:
*    succees/fail
*  Remarks:
*    None
******************************************************************************/
//----------------------------------------------------------
void App_ClearFaultFlags (void) //Clear faults with deactivate="yes"
{
    App_ClearEvent(APP_EVENT_FAULT_OverVolt);
    App_ClearEvent(APP_EVENT_FAULT_Temperature);
    App_ClearEvent(APP_EVENT_FAULT_OverCur);
    App_ClearEvent(APP_EVENT_FAULT_Rf);
    App_ClearEvent(APP_EVENT_FAULT_Tip);
}    
//----------------------------------------------------------
COMM_ERROR_e App_CommCmnd (COMM_TEXT_CODE_e Cmnd)
{
    COMM_ERROR_e Result = COMM_ERROR_FAIL;
    switch(Cmnd)
    {
      case COMM_TEXT_Cmnd_activate:
        if (App_Obj.State == APP_STATE_ON_STAY)
        {                                           //Activate
            App_Obj.State = APP_STATE_ACTIVE_START;
            Result = COMM_ERROR_SUCCESS;
        }
        App_ClearFaultFlags();
        break;
      case COMM_TEXT_Cmnd_deactivate:
        if ((App_Obj.State == APP_STATE_ACTIVE_STAY) ||
            (App_Obj.State == APP_STATE_PAUSE_STAY) ||
            (App_Obj.State == APP_STATE_LOADED_STAY)
            #ifdef SW_PROD_ReviveRf
            || (App_Obj.State == APP_STATE_ONE_PULSE_STEP1) 
            || (App_Obj.State == APP_STATE_ONE_PULSE_STEP2)
            #endif
           )
        {                                           //Deactivate
            App_Obj.State = APP_STATE_DEACTIVATE;
            Result = COMM_ERROR_SUCCESS;
        }
        App_ClearFaultFlags();
        break;
      case COMM_TEXT_Cmnd_techmode:                 //Generate TechMode events of data to be sent.
        App_SetEvent(APP_EVENT_TECH_INFO_applicator);
        #ifdef SW_MOT_X
            App_SetEvent(APP_EVENT_TECH_INFO_motorX);
        #endif
        #ifdef SW_MOT_Y
            App_SetEvent(APP_EVENT_TECH_INFO_motorY);
        #endif
        #ifdef SW_LED_RGB_VAR
            App_SetEvent(APP_EVENT_TECH_INFO_rgbled);
        #endif
        App_SetEvent(APP_EVENT_TECH_INFO_12V);
        App_SetEvent(APP_EVENT_TECH_INFO_5V);
        #ifdef SW_SENS_TEMP
            App_SetEvent(APP_EVENT_TECH_INFO_Temp);
        #endif
        App_SetEvent(APP_EVENT_TECH_INFO_Cur);
        #ifdef SW_TIP_MEM
            App_SetEvent(APP_EVENT_TECH_INFO_TipCount);
            App_SetEvent(APP_EVENT_TECH_INFO_TipId);
        #endif
        break;
      case COMM_TEXT_Cmnd_tipstatus:
        #ifdef SW_TIP_MEM
            App_SetEvent(APP_EVENT_TIPINFO_info);
        #endif
        break;
      case COMM_TEXT_Cmnd_loadparam:                //Loadparam
        #ifdef SW_MOT_X
            Motor_ParamFromEeprom(MOT_ID_X);        //Get motors' parameter from EEPROM
        #endif
        #ifdef SW_MOT_Y
            Motor_ParamFromEeprom(MOT_ID_Y);
        #endif
        if (App_Obj.State == APP_STATE_IDLE_STAY)
        {                                           //Loaded
            App_Obj.State = APP_STATE_LOADED_START;
            Result = COMM_ERROR_SUCCESS;
        }    
        break;
	  case COMM_TEXT_Cmnd_firepulse:
		if (App_Obj.State == APP_STATE_ONE_PULSE_STEP2)
		{
			App_IntObj.FirePulse = 1;
            Result = COMM_ERROR_SUCCESS;
		}
		else
		{
            Result = COMM_ERROR_FAIL;
		}
		break;
    }
    return(Result);    
}    

/******************************************************************************
*  Function name: App_CommConfirm
*  Main tasks:
*    Receive confirmation from communication
*  Parameters:
*    Confirmation code
*    Event code
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void App_CommConfirm (COMM_TEXT_CODE_e Confirm,  APP_EVENT_CODE_e EventCode)
{
    switch (EventCode)
    {
      case APP_EVENT_buttons:
      case APP_EVENT_TECH_INFO_motorX:              //Info events. Ready for next change, or request for info.
      case APP_EVENT_TECH_INFO_motorY:
      case APP_EVENT_TECH_INFO_rgbled:
      case APP_EVENT_TECH_INFO_applicator:
      case APP_EVENT_TECH_INFO_12V:
      case APP_EVENT_TECH_INFO_5V:
      case APP_EVENT_TECH_INFO_Temp:
      case APP_EVENT_TECH_INFO_Cur:
      case APP_EVENT_TECH_INFO_TipCount:
      case APP_EVENT_TECH_INFO_TipId:
      case APP_EVENT_PULSEDONE_done:
      case APP_EVENT_TIPINFO_info:
      case APP_EVENT_BOOT_up:
      case APP_EVENT_FAULT_Tip:
        if (App_Obj.Event[EventCode].State != APP_EVENT_STATE_IDLE)
        {
            App_Obj.Event[EventCode].State = APP_EVENT_STATE_IDLE;
        }    
        break;
      case APP_EVENT_FAULT_motorX:                  //Fault events. Not to send again.
      case APP_EVENT_FAULT_motorY:
      case APP_EVENT_FAULT_rgbled:
      case APP_EVENT_FAULT_OverVolt:
      case APP_EVENT_FAULT_Temperature:
      case APP_EVENT_FAULT_OverCur:
      case APP_EVENT_FAULT_Rf:
        if (App_Obj.Event[EventCode].State == APP_EVENT_STATE_SENT)
        {
            App_Obj.Event[EventCode].State = APP_EVENT_STATE_CONFIRMED;
        }    
        break;
    }    
    Comm_SetEvent (EventCode, FALSE);           //Clear event in comm module
}    

/******************************************************************************
*  Function name: App_GetInfo
*  Main tasks:
*    Get aplication info (for Communication task)
*  Parameters:
*    Configuration attribute code
*    Address to write attribute value data
*    Address to write attribute value length
*    Event code
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
//Prepare value in string, for transmission.
//Units: 0.1. Max value: 99.9
void App_PrepVal(uint16 Value)
{
    App_ConfigParamTempBuf[3] = (Value%10) | 0x30;
    Value /= 10;
    App_ConfigParamTempBuf[2] = '.';
    App_ConfigParamTempBuf[1] = (Value%10) | 0x30;
    Value /= 10;
    App_ConfigParamTempBuf[0] = Value | 0x30;
    if (App_ConfigParamTempBuf[0] == '0')
    {
        #ifdef SW_UC_PIC18F
            TextAddr = (const rom uint8*)&App_ConfigParamTempBuf[1];
        #else
            TextAddr = (const uint8*)&App_ConfigParamTempBuf[1];
        #endif
        TextLen = 3;
    }
    else
    {
        #ifdef SW_UC_PIC18F
            TextAddr = (const rom uint8*)&App_ConfigParamTempBuf[0];
        #else
            TextAddr = (const uint8*)&App_ConfigParamTempBuf[0];
        #endif
        TextLen = 4;
    }
}    

//Prepare an integer in a string for transmission
//Max: 5 digits
void App_PrepInt (uint16 IntVal)
{
    uint8 i = 4;
    
    App_ConfigParamTempBuf[i] = (IntVal%10) | 0x30;
    IntVal /= 10;
    while (IntVal != 0)
    {
        i--;
        App_ConfigParamTempBuf[i] = (IntVal%10) | 0x30;
        IntVal /= 10;
    }
    //TextAddr = (const rom uint8*)&App_ConfigParamTempBuf[0] + 4;
    TextAddr = (const rom uint8*)&App_ConfigParamTempBuf[0] + i;
    TextLen = 5 - i;
}    


//------------------------------------------------------------------------
void App_GetInfo (COMM_TEXT_CODE_e AttribCode, uint8* AttribAddr, uint8* AttribLen, APP_EVENT_CODE_e EventCode)
{
  uint8 i;
  uint8 TextFromRom = TRUE;
    switch (AttribCode)
    {
      case COMM_TEXT_Attrib_id_config:                      //Send configuration ("1")
        TextAddr = &APP_CONFIG_PARAM_ID[0];
        TextLen = APP_CONFIG_PARAM_ID_LEN;
        break;
        
      case COMM_TEXT_Attrib_type:                           //Send type
        TextAddr = &APP_CONFIG_PARAM_TYPE[0];
        TextLen = APP_CONFIG_PARAM_TYPE_LEN;
        break;
        
      case COMM_TEXT_Attrib_status_config:                  //Send configuration {"operational"}
        TextAddr = &APP_CONFIG_PARAM_STATUS_CONFIG[0];
        TextLen =APP_CONFIG_PARAM_STATUS_CONFIG_LEN ;
        break;
        
      #ifdef SW_TIP_MEM
      case COMM_TEXT_Attrib_pulses:                         //Send pulses
        App_PrepInt (TipMem_GetPulses());
        TextFromRom = FALSE;
        break;
      #endif
        
      case COMM_TEXT_Attrib_version:                        //Send version
        TextAddr = &APP_CONFIG_PARAM_VERSION[0];
        TextLen = APP_CONFIG_PARAM_VERSION_LEN;
        break;
        
      case COMM_TEXT_Attrib_onoff:                          //Send on/off state
        if ((App_Obj.State == APP_STATE_ACTIVE_START) ||
            (App_Obj.State == APP_STATE_ACTIVE_STAY) ||
            (App_Obj.State == APP_STATE_ON_STAY))
        {                                                   //Motors active
            TextAddr = &APP_EVENT_PARAM_ON[0];
            TextLen = APP_EVENT_PARAM_ON_LEN;
        }
        else
        {                                                   //Motors not active
            TextAddr = &APP_EVENT_PARAM_OFF[0];
            TextLen = APP_EVENT_PARAM_OFF_LEN;
        }
        break;
        
      case COMM_TEXT_Attrib_power_event:                    //Send power
        #ifdef SW_MOT_X
         i = Motor_GetPower();                              //Motor exists
        #else
         i = 1;                                             //Motor does not exist
        #endif
        if (i >= 10)
        {                                                   //Return 10
            App_ConfigParamTempBuf[0] = '1';
            App_ConfigParamTempBuf[1] = (i%10) | 0x30;
            TextLen = 2;
        }
        else
        {                                                   //Return 1-9
            App_ConfigParamTempBuf[0] = (i%10) | 0x30;
            TextLen = 1;
        }
        #ifdef SW_UC_PIC18F
            TextAddr = (const rom uint8*)&App_ConfigParamTempBuf[0];
        #else
            TextAddr = &App_ConfigParamTempBuf[0];
        #endif
        TextFromRom = FALSE;
        break;
        
      case COMM_TEXT_Attrib_led:                            //Send RGB state
        if (App_Obj.Status.Flags.RgbLedState)
        {                                                   //RGB LED is On
            TextAddr = &APP_EVENT_PARAM_ON[0];
            TextLen = APP_EVENT_PARAM_ON_LEN;
        }
        else
        {                                                   //RGB LED is Off
            TextAddr = &APP_EVENT_PARAM_OFF[0];
            TextLen = APP_EVENT_PARAM_OFF_LEN;
        }
        break;
        
      case COMM_TEXT_Attrib_code:                           //Send fault event code
        switch (EventCode)
        {                            
          case APP_EVENT_FAULT_OverVolt:
            TextAddr = &APP_EVENT_FAULT_CODE_OVRVOLT[0];
            TextLen = APP_EVENT_FAULT_CODE_OVRVOLT_LEN;
            break;
          case APP_EVENT_FAULT_Temperature:
            TextAddr = &APP_EVENT_FAULT_CODE_TEMP[0];
            TextLen = APP_EVENT_FAULT_CODE_TEMP_LEN;
            break;
          case APP_EVENT_FAULT_motorX:
            TextAddr = &APP_EVENT_FAULT_CODE_MOTX[0];
            TextLen = APP_EVENT_FAULT_CODE_MOTX_LEN;
            break;
          case APP_EVENT_FAULT_motorY:
            TextAddr = &APP_EVENT_FAULT_CODE_MOTY[0];
            TextLen = APP_EVENT_FAULT_CODE_MOTY_LEN;
            break;
          case APP_EVENT_FAULT_rgbled:
            TextAddr = &APP_EVENT_FAULT_CODE_RGBLED[0];
            TextLen = APP_EVENT_FAULT_CODE_RGBLED_LEN;
            break;
          case APP_EVENT_FAULT_OverCur:
            TextAddr = &APP_EVENT_FAULT_CODE_OVRCUR[0];
            TextLen = APP_EVENT_FAULT_CODE_OVRCUR_LEN;
            break;
          case APP_EVENT_FAULT_Rf:
            TextAddr = &APP_EVENT_FAULT_CODE_RF[0];
            TextLen = APP_EVENT_FAULT_CODE_RF_LEN;
            break;
          case APP_EVENT_FAULT_Tip:
            TextAddr = &APP_EVENT_FAULT_CODE_TIP[0];
            TextLen = APP_EVENT_FAULT_CODE_TIP_LEN;
            break;
        }
        break;
        
      case COMM_TEXT_Attrib_deactivate:                     //Send deactivate ("no")
        TextAddr = &APP_EVENT_PARAM_NO[0];
        TextLen = APP_EVENT_PARAM_NO_LEN;
        break;
        
      case COMM_TEXT_Attrib_activate:                       //Send activate ("yes")
        TextAddr = &APP_EVENT_PARAM_YES[0];
        TextLen = APP_EVENT_PARAM_YES_LEN;
        break;
        
      case COMM_TEXT_Attrib_id_module:                      //Send module id for TechInfo event
        switch (EventCode)
        {
          case APP_EVENT_TECH_INFO_motorX:
            TextAddr = &APP_TECHINFO_PARAM_ID_MOTORX[0];
            TextLen = APP_TECHINFO_PARAM_ID_MOTORX_LEN;
            break;
          case APP_EVENT_TECH_INFO_motorY:
            TextAddr = &APP_TECHINFO_PARAM_ID_MOTORY[0];
            TextLen = APP_TECHINFO_PARAM_ID_MOTORY_LEN;
            break;
          case APP_EVENT_TECH_INFO_rgbled:
            TextAddr = &APP_TECHINFO_PARAM_ID_RGBLED[0];
            TextLen = APP_TECHINFO_PARAM_ID_RGBLED_LEN;
            break;
          case APP_EVENT_TECH_INFO_applicator:
            TextAddr = &APP_TECHINFO_PARAM_ID_APP[0];
            TextLen = APP_TECHINFO_PARAM_ID_APP_LEN;
            break;
          case APP_EVENT_TECH_INFO_12V:
            TextAddr = &APP_TECHINFO_PARAM_ID_12V[0];
            TextLen = APP_TECHINFO_PARAM_ID_12V_LEN;
            break;
          case APP_EVENT_TECH_INFO_5V:
            TextAddr = &APP_TECHINFO_PARAM_ID_5V[0];
            TextLen = APP_TECHINFO_PARAM_ID_5V_LEN;
            break;
          case APP_EVENT_TECH_INFO_Temp:
            TextAddr = &APP_TECHINFO_PARAM_ID_TEMP[0];
            TextLen = APP_TECHINFO_PARAM_ID_TEMP_LEN;
            break;
          case APP_EVENT_TECH_INFO_Cur:
            TextAddr = &APP_TECHINFO_PARAM_ID_CUR[0];
            TextLen = APP_TECHINFO_PARAM_ID_CUR_LEN;
            break;
          #ifdef SW_TIP_MEM
              case APP_EVENT_TECH_INFO_TipCount:
                TextAddr = &APP_TECHINFO_PARAM_ID_TIPCOUNT[0];
                TextLen = APP_TECHINFO_PARAM_ID_TIPCOUNT_LEN;
                break;
              case APP_EVENT_TECH_INFO_TipId:
                TextAddr = &APP_TECHINFO_PARAM_ID_TIPID[0];
                TextLen = APP_TECHINFO_PARAM_ID_TIPID_LEN;
                break;
              case APP_EVENT_TIPINFO_info:
                App_ConfigParamTempBuf[0] = TipMem_GetId();
                TextAddr = (const rom uint8*)&App_ConfigParamTempBuf[0];
                TextLen = 1;
                TextFromRom = FALSE;
                break;
          #endif
        }
        break;
        
      case COMM_TEXT_Attrib_status_module:                  //Send module status for TechInfo event
        switch (EventCode)
        {
        uint16 TempValue;
          case APP_EVENT_TECH_INFO_motorX:
            if (App_Obj.Status.Flags.MotorXFault)
            {
                TextAddr = &APP_TECHINFO_PARAM_STATUS_BRK[0];
                TextLen = APP_TECHINFO_PARAM_STATUS_BRK_LEN;
            }
            else
            {
                TextAddr = &APP_TECHINFO_PARAM_STATUS_OP[0];
                TextLen = APP_TECHINFO_PARAM_STATUS_OP_LEN;
            }        
            break;
          case APP_EVENT_TECH_INFO_motorY:
            if (App_Obj.Status.Flags.MotorYFault)
            {
                TextAddr = &APP_TECHINFO_PARAM_STATUS_BRK[0];
                TextLen = APP_TECHINFO_PARAM_STATUS_BRK_LEN;
            }
            else
            {
                TextAddr = &APP_TECHINFO_PARAM_STATUS_OP[0];
                TextLen = APP_TECHINFO_PARAM_STATUS_OP_LEN;
            }        
            break;
          case APP_EVENT_TECH_INFO_rgbled:
            if (App_Obj.Status.Flags.RgbLedFault)
            {
                TextAddr = &APP_TECHINFO_PARAM_STATUS_BRK[0];
                TextLen = APP_TECHINFO_PARAM_STATUS_BRK_LEN;
            }
            else
            {
                TextAddr = &APP_TECHINFO_PARAM_STATUS_OP[0];
                TextLen = APP_TECHINFO_PARAM_STATUS_OP_LEN;
            }        
            break;
          case APP_EVENT_TECH_INFO_applicator:
            TextAddr = &APP_TECHINFO_PARAM_STAT_OP[0];
            TextLen = APP_TECHINFO_PARAM_STAT_OP_LEN;
            break;
          case APP_EVENT_TECH_INFO_12V:
            App_PrepVal(Adc_GetVal(ADC_IN_12V_V));
            TextFromRom = FALSE;
            break;
          case APP_EVENT_TECH_INFO_5V:
            App_PrepVal(Adc_GetVal(ADC_IN_5V_V));
            TextFromRom = FALSE;
            break;
          #ifdef SW_UC_PIC18F
           case APP_EVENT_TECH_INFO_Temp:
             App_PrepVal(Adc_GetVal(ADC_IN_Temp_C));
             TextFromRom = FALSE;
             break;
           case APP_EVENT_TECH_INFO_Cur:
             TempValue = Adc_GetVal(ADC_IN_SupCur_I);           //Prepare current in mA. No decimal point.
             App_ConfigParamTempBuf[3] = (TempValue%10) | 0x30;
             TempValue /= 10;
             App_ConfigParamTempBuf[2] = (TempValue%10) | 0x30;
             TempValue /= 10;
             App_ConfigParamTempBuf[1] = (TempValue%10) | 0x30;
             TempValue /= 10;
             App_ConfigParamTempBuf[0] = TempValue | 0x30;
             TextAddr = (const rom uint8*)&App_ConfigParamTempBuf[0];
             TextLen = 4;
             if (App_ConfigParamTempBuf[0] == '0')
             {
                 TextAddr++;
                 TextLen--;
                 if (App_ConfigParamTempBuf[1] == '0')
                 {
                     TextAddr++;
                     TextLen--;
                     if (App_ConfigParamTempBuf[2] == '0')
                     {
                         TextAddr++;
                         TextLen--;
                     }    
                 }    
             }    
             TextFromRom = FALSE;
             break;
           #ifdef SW_TIP_MEM
               case APP_EVENT_TIPINFO_info:
                if (TipMem_GetStatus())
                {                                           //Tip connected
                    TextAddr = &APP_TIPINFO_PARAM_CONNECTED[0];
                    TextLen = APP_TIPINFO_PARAM_CONNECTED_LEN;
                }
                else
                {                                           //tip disconnected
                    TextAddr = &APP_TIPINFO_PARAM_DISCONNECTED[0];
                    TextLen = APP_TIPINFO_PARAM_DISCONNECTED_LEN;
                }    
                break;
               case APP_EVENT_TECH_INFO_TipCount:
                 App_PrepInt (TipMem_GetPulses());
                 TextFromRom = FALSE;
                 break;
               case APP_EVENT_TECH_INFO_TipId:
                 App_ConfigParamTempBuf[0] = TipMem_GetId();
                 TextAddr = (const rom uint8*)&App_ConfigParamTempBuf[0];
                 TextLen = 1;
                 TextFromRom = FALSE;
                 break;
           #endif
          #endif
        }    
        break;
        
      case COMM_TEXT_Attrib_reason:                         //Send reason
        switch (EventCode)
        {
          case APP_EVENT_TECH_INFO_motorX:
            if (App_Obj.Status.Flags.MotorXFault)
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_FAIL[0];
                TextLen = APP_TECHINFO_PARAM_REASON_FAIL_LEN;
            }
            else
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_PASS[0];
                TextLen = APP_TECHINFO_PARAM_REASON_PASS_LEN;
            }        
            break;
          case APP_EVENT_TECH_INFO_motorY:
            if (App_Obj.Status.Flags.MotorYFault)
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_FAIL[0];
                TextLen = APP_TECHINFO_PARAM_REASON_FAIL_LEN;
            }
            else
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_PASS[0];
                TextLen = APP_TECHINFO_PARAM_REASON_PASS_LEN;
            }        
            break;
          case APP_EVENT_TECH_INFO_rgbled:
            if (App_Obj.Status.Flags.RgbLedFault)
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_FAIL[0];
                TextLen = APP_TECHINFO_PARAM_REASON_FAIL_LEN;
            }
            else
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_PASS[0];
                TextLen = APP_TECHINFO_PARAM_REASON_PASS_LEN;
            }        
            break;
          case APP_EVENT_TECH_INFO_applicator:
            TextAddr = &APP_TECHINFO_PARAM_REASON_FIRM[0];
            TextLen = APP_TECHINFO_PARAM_REASON_FIRM_LEN;
            break;
          case APP_EVENT_TECH_INFO_12V:
            if (App_Obj.Status.Flags.Voltage12VFault)
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_FAIL[0];
                TextLen = APP_TECHINFO_PARAM_REASON_FAIL_LEN;
            }
            else
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_PASS[0];
                TextLen = APP_TECHINFO_PARAM_REASON_PASS_LEN;
            }        
            break;
          case APP_EVENT_TECH_INFO_5V:
            if (App_Obj.Status.Flags.Voltage5VFault)
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_FAIL[0];
                TextLen = APP_TECHINFO_PARAM_REASON_FAIL_LEN;
            }
            else
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_PASS[0];
                TextLen = APP_TECHINFO_PARAM_REASON_PASS_LEN;
            }        
            break;
          case APP_EVENT_TECH_INFO_Temp:
            if (App_Obj.Status.Flags.TempFault)
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_FAIL[0];
                TextLen = APP_TECHINFO_PARAM_REASON_FAIL_LEN;
            }
            else
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_PASS[0];
                TextLen = APP_TECHINFO_PARAM_REASON_PASS_LEN;
            }        
            break;
          case APP_EVENT_TECH_INFO_Cur:
            if (App_Obj.Status.Flags.CurrentFault)
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_FAIL[0];
                TextLen = APP_TECHINFO_PARAM_REASON_FAIL_LEN;
            }
            else
            {
                TextAddr = &APP_TECHINFO_PARAM_REASON_PASS[0];
                TextLen = APP_TECHINFO_PARAM_REASON_PASS_LEN;
            }        
            break;
          #ifdef SW_TIP_MEM
              case APP_EVENT_TECH_INFO_TipCount:
                TextAddr = &APP_TECHINFO_PARAM_REASON_PULSES[0];
                TextLen = APP_TECHINFO_PARAM_REASON_PULSES_LEN;
                break;
              case APP_EVENT_TECH_INFO_TipId:
                TextAddr = &APP_TECHINFO_PARAM_REASON_TIPID[0];
                TextLen = APP_TECHINFO_PARAM_REASON_TIPID_LEN;
                break;
          #endif
        }    
        break;
    }
    if (TextFromRom)
    {                                                       //Text from ROM - copy to RAM buffer
        for (i=0; i<TextLen; i++)
        {
            App_ConfigParamTempBuf[i] = *TextAddr;
            TextAddr++;
        }
        #ifdef SW_UC_PIC18F
            TextAddr = (const rom uint8*)&App_ConfigParamTempBuf[0];
        #else
            TextAddr = (const uint8*)&App_ConfigParamTempBuf[0];
        #endif
    }
    memcpy (AttribLen, (const uint8*)&TextLen, sizeof(uint8));            //Length of the text
    memcpy (AttribAddr, (const uint8*)&TextAddr, sizeof(uint16));         //Address of the text
}    

/******************************************************************************
*  Function name: App_CheckRgbLed
*  Main tasks:
*    check if light on key was pressed, and toggle state
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void App_RgbLedAct(void)                                    //Activate RGB
{
    Led_ActRgb(TRUE);
    App_Obj.Status.Flags.RgbLedAct = 1;
    App_Obj.Status.Flags.RgbLedState = 1;
}
//----------------------------------------    
void App_RgbLedDisact(void)                                 //Deactivate RGB
{
    Led_ActRgb(FALSE);
    App_Obj.Status.Flags.RgbLedAct = 0;
    App_Obj.Status.Flags.RgbLedState = 0;
}
//----------------------------------------    
void App_CheckRgbLed (void)
{
    #ifdef SW_LED_RGB_VAR
    if (Keys_read_keys_new_event(KEY_MASK_LIGHT_EVENT) & KEY_MASK_LIGHT_EVENT)
    {                                                       //Light on key was pressed
        App_SetEvent(APP_EVENT_buttons);                    //Generate Buttons event
        if (App_Obj.Status.Flags.RgbLedAct)
        {                                                   //Light is on - turn off
            App_RgbLedDisact();
        }
        else
        {                                               //Light is off - turn on
            App_CheckDevice(APP_DEVICE_RGBLED);
            App_RgbLedAct();
        }        
    }
    #endif
}

/******************************************************************************
*  Function name: App_TestRfNoPulse
*  Main tasks:
*    Test Rf when pulse is not active
*  Parameters:
*    TRUE/FALSE - init test
*  Returned values:
*    Busy/Success/Fail
*  Remarks:
//*    Because of hardware problem (capacitor discharge time?)
//*    Ech time, after a pulse, only one is tested (ON or EN)
******************************************************************************/
#ifdef SW_PROD_ReviveRf
 RESULT_t App_TestRfNoPulse (uint8 Init) 
 {
  RESULT_t Result = RESULT_RF_TEST_BUSY;
    if (Init)
    {
        App_RfTest.StateNP = APP_RF_NP_TEST_STATE_INIT;
        Result = RESULT_RF_TEST_SUCCESS;
    }
    else
    {
        switch (App_RfTest.StateNP)
        {
          case APP_RF_NP_TEST_STATE_INIT:
            Adc_SetOneChannel(TRUE, ADC_IN_SupCur_I);       //ADC task measures only supply current
            Motor_DisRf_EN0();                              //Activate only RF_EN
            App_IntObj.TestNPTimer = APP_RF_NP_TEST_TIMER;
            App_RfTest.StateNP = APP_RF_NP_TEST_STATE_WAIT_TEST1;
            break;
          case APP_RF_NP_TEST_STATE_WAIT_TEST1:
            if (Adc_SampleRequest(ADC_IN_SupCur_I, FALSE) > APP_RF_NP_FAIL_I_VAL)
            {                                               //Failure
                Result = RESULT_RF_TEST_FAIL;
                Motor_DisRfForce();
                Adc_SetOneChannel(FALSE, ADC_IN_SupCur_I);  //ADC returns to measure all inputs
                App_RfTest.StateNP = APP_RF_NP_TEST_STATE_DONE;
            }
            else if (App_IntObj.TestNPTimer == 0)
            {                                               //Success
                Motor_DisRfForce();                         //Deactivate (let the cap discharge)
                App_IntObj.TestNPTimer = 255;               //APP_RF_NP_TEST_TIMER;
                App_RfTest.StateNP = APP_RF_NP_TEST_STATE_WAIT_DELAY;
            }        
            break;
          case APP_RF_NP_TEST_STATE_WAIT_DELAY:
            if (App_IntObj.TestNPTimer == 0)
            {
                Motor_DisRf_ON0();                          //Activate only RF_ON
                App_IntObj.TestNPTimer = APP_RF_NP_TEST_TIMER;
                App_RfTest.StateNP = APP_RF_NP_TEST_STATE_WAIT_TEST2;
            }    
            break;
          case APP_RF_NP_TEST_STATE_WAIT_TEST2:
            if (Adc_SampleRequest(ADC_IN_SupCur_I, FALSE) > APP_RF_NP_FAIL_I_VAL)
            {                                               //Failure
                Result = RESULT_RF_TEST_FAIL;
                Motor_DisRfForce();
                Adc_SetOneChannel(FALSE, ADC_IN_SupCur_I);  //ADC returns to measure all inputs
                App_RfTest.StateNP = APP_RF_NP_TEST_STATE_DONE;
            }
            else if (App_IntObj.TestNPTimer == 0)
            {                                               //Success
                Result = RESULT_RF_TEST_SUCCESS;
                Motor_DisRfForce();
                Adc_SetOneChannel(FALSE, ADC_IN_SupCur_I);  //ADC returns to measure all inputs
                App_RfTest.StateNP = APP_RF_NP_TEST_STATE_DONE;
            }
            break;
          case APP_RF_NP_TEST_STATE_DONE:
            break;
        }    
    }
    return(Result);
 }    
#endif

/******************************************************************************
*  Function name: App_TestRf
*  Main tasks:
*    Test Rf
*  Parameters:
*    TRUE/FALSE - init test
*  Returned values:
*    Busy/Success/Fail
*  Remarks:
*    None
******************************************************************************/
#ifdef SW_MOT_RF
 RESULT_t App_TestRf (uint8 Init)
 {
  RESULT_t Result = RESULT_RF_TEST_BUSY;
    if (Init)
    {
        App_RfTest.State = APP_RF_TEST_STATE_INIT;
        Result = RESULT_RF_TEST_SUCCESS;
    }
    else
    {
        switch (App_RfTest.State)
        {
          case APP_RF_TEST_STATE_INIT:              //Init
            App_RfTest.SampleCounter = 0;
            App_RfTest.M1 = 0;
            App_RfTest.M2 = 0;
            App_RfTest.M3 = 0;
            App_RfTest.State = APP_RF_TEST_STATE_M1;
            break;
          case APP_RF_TEST_STATE_M1:                //3 M1 samples
            if ((Motor_Rf_EnState_Get() == MOTOR_RF_EN_STATE_ON1_EN1) &&
                (Motor_Rf_Sample_Time_Act()))
            {                                       //RF in active
                App_RfTest.M1 += Adc_SampleRequest (ADC_IN_SupCur_I, FALSE);
                App_RfTest.SampleCounter++;
                if (App_RfTest.SampleCounter >= APP_RF_TEST_SAMPLE_NUM)
                {
                    App_RfTest.SampleCounter = 0;
                    App_RfTest.State = APP_RF_TEST_STATE_PRE_M2;
                }    
            }    
            break;
          case APP_RF_TEST_STATE_PRE_M2:            //Prepare to M2 tests
            if (Motor_Rf_Sample_Time_Dis(APP_RF_TEST_M2_PRE))
            {
                Motor_DisRf_ON0();
                App_RfTest.State = APP_RF_TEST_STATE_M2;
            }    
            break;
          case APP_RF_TEST_STATE_M2:                //3 M2 tests
            if ((Motor_Rf_EnState_Get() == MOTOR_RF_EN_STATE_ON0_EN1) &&
                (Motor_Rf_Sample_Time_Dis(APP_RF_TEST_M2_TEST)))
            {                                       //RF is in M2 test
                App_RfTest.M2 += Adc_SampleRequest (ADC_IN_SupCur_I, FALSE);
                App_RfTest.SampleCounter++;
                if (App_RfTest.SampleCounter >= APP_RF_TEST_SAMPLE_NUM)
                {
                    App_RfTest.SampleCounter = 0;
                    Motor_DisRfForce();             //Diable ON and EN
                    App_RfTest.State = APP_RF_TEST_STATE_PRE_M3;
                }    
            }    
            break;
          case APP_RF_TEST_STATE_PRE_M3:            //Prepare to M3 tests
            if (Motor_Rf_Sample_Time_Dis(APP_RF_TEST_M3_PRE))
            {
                Motor_DisRf_EN0();
                App_RfTest.State = APP_RF_TEST_STATE_M3;
            }    
            break;
          case APP_RF_TEST_STATE_M3:                //3 M3 tests
            if ((Motor_Rf_EnState_Get() == MOTOR_RF_EN_STATE_ON1_EN0) &&
                (Motor_Rf_Sample_Time_Dis(APP_RF_TEST_M3_TEST)))
            {                                       //RF is not active, or already in M3 test
                App_RfTest.M3 += Adc_SampleRequest (ADC_IN_SupCur_I, FALSE);
                App_RfTest.SampleCounter++;
                if (App_RfTest.SampleCounter >= APP_RF_TEST_SAMPLE_NUM)
                {
                    Motor_DisRfForce();
                    App_RfTest.State = APP_RF_TEST_STATE_CALC;
                }    
            }    
            break;
          case APP_RF_TEST_STATE_CALC:              //Calculate
            App_RfTest.M1 /= 2;                     // 1/2 M1      
            if ((App_RfTest.M2 > App_RfTest.M1) ||
                (App_RfTest.M3 > App_RfTest.M1))
            {                                       //Fault
                Result = RESULT_RF_TEST_FAIL;
                #ifdef SW_DEBUG_NO_RF_TEST
                  Result = RESULT_RF_TEST_SUCCESS;
                #endif
            }
            else
            {                                       //No fault
                Result = RESULT_RF_TEST_SUCCESS;
            }    
            break;
        }    
    }    
    return(Result); 
 }    
#endif


/******************************************************************************
*  Function name: App_RfIsOn
*  Main tasks:
*    Check if RF is active
*  Parameters:
*    None
*  Returned values:
*    TRUE/FALSE
******************************************************************************/
#ifdef SW_PROD_ReviveRf
uint8 App_RfIsOn (void)
{
    return(App_Obj.State == APP_STATE_ONE_PULSE_STEP1);
}
#endif

/******************************************************************************
*  Function name: App_TaskInit
*  Main tasks:
*    Initialize Application task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called on start up
******************************************************************************/
void App_TaskInit (void)
{
  uint8 i;
    App_Obj.State = APP_STATE_INIT;
    App_IntObj.Timer = 0;
    App_Obj.Status.Byte = 0;
    App_Obj.RfFault = APP_RF_FAULT_IDLE;
    App_IntObj.TemperatureTimer = APP_TEMP_FAULT_TIME;
    App_RfTest.Timer = 0;
    #ifdef SW_MOT_RF
        App_RfTest.PulseOvrCurCounter = 0;
        App_TestRf(TRUE);
    #endif
    #ifdef SW_TEMP_CONST_SEND
        App_Obj.Test.KeyPressTimer = 0;
        App_Obj.Test.SendMsgTimer = 0;
    #endif
	#ifdef SW_PROD_ReviveRf_RepeatPulse
		ReviveRf_RepeatPulse.Act = 0;
		ReviveRf_RepeatPulse.Timer = 0;
	#endif
    App_IntObj.FirePulse = 0;

    for (i=0; i<APP_EVENT_num; i++)
    {
        App_Obj.Event[i].State = APP_EVENT_STATE_IDLE;
    }    
}    

/******************************************************************************
*  Function name: Appm_TaskMain
*  Main tasks:
*    Handle Application task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called periodically
******************************************************************************/
void App_TaskMain (void)
{
  uint16 TempVal;
    switch (App_Obj.State)
    {
      //Init ------------------------------------------------------------------
      case APP_STATE_INIT:                                  //Init task
        App_Obj.Device = 0;
        if (Adc_CheckReady())
        {                                                   //Continue only when ADC is ready
            #ifdef SW_VSUP_GEN
                Revive_Vsup_Cmnd(REVIVE_VSUP_CMND_OFF, 0); //Revive - set V off.
            #endif
            App_IntObj.Timer = APP_INIT_TIME;               //Make sure input voltage is stable.
            App_Obj.State = APP_STATE_INIT1;
        }
        break;
        
      case APP_STATE_INIT1:
        if (App_IntObj.Timer == 0)
        {
            #ifdef SW_STARTUP_BIT
                App_Obj.State = APP_STATE_BIT;
            #else
                App_Obj.State = APP_STATE_POWER_UP_TEST;
            #endif
        }    
        break;
        
      case APP_STATE_BIT:                                   //BIT
        #ifdef SW_MOT_RF
            TempVal = Adc_GetVal(ADC_IN_12V_V);             //Check Vin
            if (TempVal >= APP_BIT_12V_MAX)
            {
                App_SetEvent(APP_EVENT_FAULT_OverVolt);
            }
            #ifdef SW_APP_OVER_CURRENT_FAULT
             TempVal = Adc_GetVal(ADC_IN_SupCur_I);      //Check Iin
             if (TempVal >= APP_BIT_I_SUP_MAX)
             {
                 App_SetEvent(APP_EVENT_FAULT_OverCur);
             }
            #endif
            #ifndef SW_IGNORE_THERMISTOR
             TempVal = Adc_GetVal(ADC_IN_Temp_C);            //Check temperature
             if ((TempVal >= APP_BIT_TEMP_MAX) || (TempVal <= APP_BIT_TEMP_MIN))
             {
                 App_SetEvent(APP_EVENT_FAULT_Temperature);
             }
            #endif
        #endif
        App_Obj.State = APP_STATE_POWER_UP_TEST;
        break;
        
      case APP_STATE_POWER_UP_TEST:                         //Check the devices
        App_CheckDevice(App_Obj.Device);                    //Check a device
        App_Obj.Device++;
        if (App_Obj.Device >= APP_DEVICE_num)
        {                                                   //Last device - continue
            #ifdef SW_PROD_Rf
             App_Obj.State = APP_STATE_POWER_UP_TEST_RF1;
            #else
             App_Obj.State = APP_STATE_START;
            #endif
        }    
        break;
        
      #ifdef SW_PROD_Rf
       case APP_STATE_POWER_UP_TEST_RF1:                    //Test RF
         Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_MODE, 1 /*2*/); //Set RF parameters
         Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_INTENSITY, 99 /*50*/); //Later it is divided by 2
         Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_ON, 20);
         Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_OFF, 30);
         //Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_LOWLEVEL, 20);
         Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_LOWLEVEL, 200);
         //Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_HIGHLEVEL, 47);
         Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_HIGHLEVEL, 470);
         Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_BUZSTATUS, 1);
         Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_CONT);
         App_RfTest.BitTestCounter = APP_RF_ACTIVE_FAULT_NUM * 2;   //In case of an errornous successfull test
         App_TestRf(TRUE);
         App_Obj.State = APP_STATE_POWER_UP_TEST_RF2;
         break;
       case APP_STATE_POWER_UP_TEST_RF2:
         switch(App_TestRf(FALSE))
         {
           case RESULT_RF_TEST_BUSY:
             break;
           case RESULT_RF_TEST_SUCCESS:                     //Success
             App_RfTest.BitTestCounter--;
             if (App_RfTest.BitTestCounter)
             {                                              //Retry
                 Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_CONT);
                 App_TestRf(TRUE);
             }
             else
             {                                              //Enough successfull tests - continue
                Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_OFF);
                App_Obj.State = APP_STATE_START;
             }
             break;
           case RESULT_RF_TEST_FAIL:                        //Fail
             Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_OFF);
             #ifndef SW_RF_NO_ACT
                App_SetEvent(APP_EVENT_FAULT_Rf);           //RF fault event
             #endif
             App_Obj.State = APP_STATE_START;
             break;
         }
         break;
      #endif
        
      case APP_STATE_START:                                 //Start
        Led_ActGreen (LED_GREEN_MODE_OFF, 0, 0);            //LEDs off
        Led_ActRgb(FALSE);
        #ifdef SW_MOT_Y
            Motor_ActMotor(MOT_ID_Y, FALSE);                //Disable motors
        #endif
        #ifdef SW_MOT_X
            Motor_ActMotor(MOT_ID_X, FALSE);
        #endif
        #ifdef SW_MOT_RF
            Motor_ActMotor(MOT_ID_RF, FALSE);
            Led_ActOrange(LED_ORANGE_MODE_OFF,0 ,0);        //LED2 orange off
            App_Obj.RfFault = APP_RF_FAULT_IDLE;
            App_Obj.Status.Flags.BuzWasAct = 0;
        #endif
        #ifdef SW_JUMPER_PROD_TEST
         if (IO_GetProdJumper())
         {
             App_IntObj.Timer = APP_PROD_JUMPER_TIME;       //To filer noise
             App_Obj.State = APP_STATE_PROD_TEST_RED;
         }
         else
        #endif
        {    
            App_Obj.State = APP_STATE_SETUP;
        }
        break;
      
      //Production test mode --------------------------------------------------
      #ifdef SW_JUMPER_PROD_TEST
       case APP_STATE_PROD_TEST_RED:                        //Test - Red LED
        if (App_IntObj.Timer == 0)
        {
            if (!IO_GetProdJumper())
            {                                               //Jumper not pressed - go to working mode
                App_Obj.State = APP_STATE_SETUP;
            }
            else
            {    
                Led_TaskInit();
                //Led_CheckRgb (LED_RGB_ID_R);              //Use this function to activate the Red LED
                Led_ActGreen (LED_RED_MODE_CONT, 0, 0);
                IO_BuzCmnd(IO_BUZ_CMND_BEEP);
                Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_OFF);
                App_IntObj.Timer = APP_PROD_TEST_TIME;
                App_Obj.State = APP_STATE_PROD_TEST_GREEN;
            }
        }
        break;
       case APP_STATE_PROD_TEST_GREEN:                      //Test - Green LED
        if (App_IntObj.Timer == 0)
        {
            Led_TaskInit();
            Led_ActGreen (LED_GREEN_MODE_CONT, 0, 0);
            App_IntObj.Timer = APP_PROD_TEST_TIME;
            App_Obj.State = APP_STATE_PROD_TEST_BLUE;
        }
        break;
       case APP_STATE_PROD_TEST_BLUE:                       //Test - Blue LED
        if (App_IntObj.Timer == 0)
        {
            Led_TaskInit();
            Led_ActGreen (LED_BLUE_MODE_CONT, 0, 0);
            App_IntObj.Timer = APP_PROD_TEST_TIME;
            App_Obj.State = APP_STATE_PROD_TEST_ORANGE;
        }
        break;
       case APP_STATE_PROD_TEST_ORANGE:                     //Test - Orange LED
        if (App_IntObj.Timer == 0)
        {
            Led_TaskInit();
            Led_ActOrange(LED_ORANGE_MODE_CONT, 0, 0);
            App_IntObj.Timer = APP_PROD_TEST_TIME;
            App_Obj.State = APP_STATE_PROD_TEST_NO_LED;
        }
        break;
       case APP_STATE_PROD_TEST_NO_LED:                     //Test - No LED
        if (App_IntObj.Timer == 0)
        {
            Led_TaskInit();
            App_IntObj.Timer = APP_PROD_TEST_TIME;
            App_Obj.State = APP_STATE_PROD_TEST_THERM_STRT;
        }
        break;
       case APP_STATE_PROD_TEST_THERM_STRT:                 //Test - thermistor
        if (App_IntObj.Timer == 0)
        {
          uint16 Temperature = Adc_GetVal(ADC_IN_Temp_C);
            Led_TaskInit();
            #ifndef SW_IGNORE_THERMISTOR
             if ((Temperature >= APP_TEMP_PROD_MIN_VAL) &&
                 (Temperature <= APP_TEMP_PROD_MAX_VAL))
             {                                               //Temperature in range
            #endif
              App_Obj.State = APP_STATE_PROD_TEST_5V_STRT;
            #ifndef SW_IGNORE_THERMISTOR
             }
             else
             {                                               //Temperature out of range
                 Led_ActGreen (LED_GREEN_MODE_CONT, 0, 0);
                 Led_ActOrange(LED_ORANGE_MODE_CONT, 0, 0);
                 IO_BuzCmnd(IO_BUZ_CMND_BEEP_LONG);
                 App_IntObj.Timer = APP_PROD_TEST_TIME_LONG;
                 App_Obj.State = APP_STATE_PROD_TEST_THERM_ERR;
             }
            #endif
        }
        break;
       case APP_STATE_PROD_TEST_THERM_ERR:                  //Thermistor error
        if (App_IntObj.Timer == 0)
        {
            App_Obj.State = APP_STATE_PROD_TEST_5V_STRT;
        }
        break;
       case APP_STATE_PROD_TEST_5V_STRT:                    //Test - 5V
        if (App_IntObj.Timer == 0)
        {
          uint16 V_5V = Adc_GetVal(ADC_IN_5V_V);
            Led_TaskInit();
            if ((V_5V >= APP_5V_PROD_MIN_VAL) &&
                (V_5V <= APP_5V_PROD_MAX_VAL))
            {                                               //5V in range
              App_Obj.State = APP_STATE_PROD_TEST_12V_STRT;
            }
            else
            {                                               //5V out of range
                Led_ActGreen (LED_BLUE_MODE_CONT, 0, 0);
                Led_ActOrange(LED_ORANGE_MODE_CONT, 0, 0);
                IO_BuzCmnd(IO_BUZ_CMND_BEEP_LONG);
                App_IntObj.Timer = APP_PROD_TEST_TIME_LONG;
                App_Obj.State = APP_STATE_PROD_TEST_5V_ERR;
            }        
        }
        break;
       case APP_STATE_PROD_TEST_5V_ERR:                     //Thermistor error
        if (App_IntObj.Timer == 0)
        {
            App_Obj.State = APP_STATE_PROD_TEST_12V_STRT;
        }
        break;
       case APP_STATE_PROD_TEST_12V_STRT:                   //Test - 12V
        if (App_IntObj.Timer == 0)
        {
          uint16 V_12V = Adc_GetVal(ADC_IN_12V_V);
            Led_TaskInit();
            if ((V_12V >= APP_12V_PROD_MIN_VAL) &&
                (V_12V <= APP_12V_PROD_MAX_VAL))
            {                                               //12V in range
              App_Obj.State = APP_STATE_PROD_TEST_RF;
            }
            else
            {                                               //12V out of range
                #ifdef SW_HW_REVIVE
                  Led_ActGreen (LED_GREENnBLUE_MODE_CONT, 0, 0);
                #else
                  Led_ActGreen (LED_RED_MODE_CONT, 0, 0);
                #endif
                Led_ActOrange(LED_ORANGE_MODE_CONT, 0, 0);
                IO_BuzCmnd(IO_BUZ_CMND_BEEP_LONG);
                App_IntObj.Timer = APP_PROD_TEST_TIME_LONG;
                App_Obj.State = APP_STATE_PROD_TEST_12V_ERR;
            }        
        }
        break;
       case APP_STATE_PROD_TEST_12V_ERR:                    //12V error
        if (App_IntObj.Timer == 0)
        {
              App_Obj.State = APP_STATE_PROD_TEST_RF;
        }
        break;
       case APP_STATE_PROD_TEST_RF:                         //Test - RF test
        if (App_IntObj.Timer == 0)
        {
            App_Obj.State = APP_STATE_PROD_TEST_RF_CHECK;
            Led_TaskInit();
            Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_MODE, 1 /*2*/); //Set RF parameters
            #ifdef SW_PROD_Rf
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_INTENSITY, 99 /*50*/); //Later it is divided by 2
            #endif
            #ifdef SW_PROD_Us
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_INTENSITY, 50);
              Revive_Vsup_Cmnd(REVIVE_VSUP_CMND_ON, 120); //Revive US - set V to 12V
            #endif
            #ifdef SW_PROD_ReviveRf                         //To delete
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_DOUBLEPULSE, 0);
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_P1POWER, APP_PROD_RVIVE_V);
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_P1INTENSITY, APP_RF_PROD_PULSE_TIME);
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_DELAY, 10);
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_P2POWER, 30);
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_P2INTENSITY, 35);
              Motor_ActMotor(MOT_ID_REVIVE, TRUE);        //Activate the motor with the new parameters
              Motor_SetVsup(TRUE);                        //Set Vsup
              App_Obj.State = APP_STATE_PROD_TEST_KEYnRF;
            #endif
            Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_ON, 20);
            Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_OFF, 30);
            //Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_LOWLEVEL, 20);
            Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_LOWLEVEL, 200);
            //Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_HIGHLEVEL, 47);
            Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_HIGHLEVEL, 470);
            Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_BUZSTATUS, 1);
            #ifdef SW_PROD_ReviveUs
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_INTENSITY, 50);
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_ON, 100);
              Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_OFF, 0);
              Revive_Vsup_Cmnd(REVIVE_VSUP_CMND_ON, APP_PROD_RVIVE_V);   //Set V to 40V
              Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_CONT);
              App_Obj.State = APP_STATE_PROD_TEST_KEYnRF;
            #endif
            App_IntObj.Timer = APP_PROD_TEST_TIME;
        }
        break;
       case APP_STATE_PROD_TEST_RF_CHECK:                   //Start check RF
        if (App_IntObj.Timer == 0)
        {
            Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_CONT);
            App_TestRf(TRUE);
            App_Obj.State = APP_STATE_PROD_TEST_RF_WAIT;
        }
        break;
       case APP_STATE_PROD_TEST_RF_WAIT:                    //Wait for RF test end
        switch(App_TestRf(FALSE))
        {
          case RESULT_RF_TEST_BUSY:
            break;
          case RESULT_RF_TEST_SUCCESS:                      //Success
            Led_TaskInit();
            Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_OFF);
            App_Obj.State = APP_STATE_PROD_TEST_KEYnRF;
            break;
          case RESULT_RF_TEST_FAIL:                         //Fail
            Led_TaskInit();
            Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_OFF);
            App_Obj.State = APP_STATE_PROD_TEST_RF_FAILa;
            break;
        }
        break;
       case APP_STATE_PROD_TEST_RF_FAILa:                   //RF fail
        Led_ActGreen (LED_GREEN_MODE_CONT, 0, 0);
        Led_ActOrange(LED_ORANGE_MODE_CONT, 0, 0);
        IO_BuzCmnd(IO_BUZ_CMND_BEEP_LONG);
        App_IntObj.Timer = APP_PROD_TEST_TIME_LONG;
        App_Obj.State = APP_STATE_PROD_TEST_RF_FAILb;
        break;
       case APP_STATE_PROD_TEST_RF_FAILb:
        if (App_IntObj.Timer == 0)
        {
            App_Obj.State = APP_STATE_PROD_TEST_RF_FAILend;
        }    
        break;
       case APP_STATE_PROD_TEST_RF_FAILend:
        if (App_IntObj.Timer == 0)
        {
            App_Obj.State = APP_STATE_PROD_TEST_KEYnRF;
        }    
        break;
       case APP_STATE_PROD_TEST_KEYnRF:                     //Test - key
        if(Keys_read_keys_state(KEY_MASK_START))
        {                                                   //Key pressed - RF on
            App_Obj.State = APP_STATE_PROD_TEST_KEYnRF_TST;
        }  
        else
        {
            Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_OFF);
            App_Obj.State = APP_STATE_PROD_TEST_RED;
        }      
        break;
       case APP_STATE_PROD_TEST_KEYnRF_TST:
        if (App_IntObj.Timer == 0)
        {
            Led_TaskInit();
            //Setting 50% duty cycle left from previous test
            Adc_SetOneChannel(TRUE, ADC_IN_SupCur_I);
            #ifdef SW_PROD_ReviveRf
              Motor_StartPulse();                           //Generate a pulse
              App_Obj.State = APP_STATE_PROD_TEST_KEYnRF_WT_PULSE;
            #else
              App_IntObj.Timer = APP_PROD_TEST_TIME;
              App_Obj.State = APP_STATE_PROD_TEST_KEYnRF_WT;
            #endif
        }
        break;
       #ifdef SW_PROD_ReviveRf
       case APP_STATE_PROD_TEST_KEYnRF_WT_PULSE:            //Wait till pulse start
        if (Motor_Rf_EnState_Get() == MOTOR_RF_EN_STATE_ON1_EN1)
        {                                                   //Pulse started
            App_IntObj.Timer = (APP_RF_PROD_PULSE_TIME/2 + 1);    //Wait till middle of pulse
            App_Obj.State = APP_STATE_PROD_TEST_KEYnRF_WT;
            #ifdef SW_DEBUG_PROD_RF_CUR_TEST
             //Led_ActRgbled(LED_RGB_ID_G, TRUE);
             LED_Test_ActGreen();
            #endif
        }    
        break;
       #endif
       case APP_STATE_PROD_TEST_KEYnRF_WT:
        #ifdef SW_PROD_ReviveRf
         if (App_IntObj.Timer == 0)
        #else
         if ((App_IntObj.Timer == 0) &&
            (Motor_Rf_Prod_Sample_Time_PulseEnd()))
        #endif
        {                                                   //Time to sample pulse on start
          uint16 Isupply = Adc_SampleRequest (ADC_IN_SupCur_I, FALSE);
            #ifdef SW_DEBUG_PROD_RF_CUR_TEST
             //Led_ActRgbled(LED_RGB_ID_B, TRUE);
             LED_Test_ActBlue();
            #endif
            if ((Isupply > APP_I_PROD_MIN_VAL) ||
                (Motor_Rf_EnState_Get() != MOTOR_RF_EN_STATE_ON1_EN1)) //Work around: if RF is disabled - ignore test
            {                                               //Current in range
                #ifdef SW_PROD_ReviveRf
                  App_IntObj.Timer = APP_PROD_TEST_DELAY_TIME;
                #endif
                App_Obj.State = APP_STATE_PROD_TEST_KEYnRF_TST;
            }
            else
            {                                               //Current out of range
                #ifdef SW_HW_REVIVE
                  Led_ActOrange(LED_ORANGE_MODE_CONT, 0, 0);
                  App_IntObj.Timer = APP_PROD_TEST_TIME;
                #else
                  Led_ActGreen (LED_RED_MODE_CONT, 0, 0);
                  IO_BuzCmnd(IO_BUZ_CMND_BEEP_LONG);
                  App_IntObj.Timer = APP_PROD_TEST_TIME_LONG;
                #endif
                App_Obj.State = APP_STATE_PROD_TEST_KEYnRF_TST; //APP_STATE_PROD_TEST_KEYnRF_ERR;
            }        
        }      
        break;
       case APP_STATE_PROD_TEST_KEYnRF_ERR:
        if (App_IntObj.Timer == 0)
        {
            APP_STATE_PROD_TEST_KEYnRF_TST;
        }      
        break;
      #endif
        
      //Working mode ----------------------------------------------------------
      case APP_STATE_SETUP:
        App_SetEvent (APP_EVENT_BOOT_up);                   //Set BootUp event
        App_Obj.State = APP_STATE_IDLE_START;
        break;
        
      case APP_STATE_IDLE_START:                //Start idle mode
        Led_ActGreen (LED_GREEN_MODE_CONT, 0, 0);//Green LED On
        Led_ActRgb(FALSE);
        #ifdef SW_MOT_Y
            Motor_ActMotor(MOT_ID_Y, FALSE);    //Disable motors
        #endif
        #ifdef SW_MOT_X
            Motor_ActMotor(MOT_ID_X, FALSE);
        #endif
        #ifdef SW_MOT_RF
            Motor_ActMotor(MOT_ID_RF, FALSE);
            Led_ActOrange(LED_ORANGE_MODE_OFF,0 ,0);     //LED2 orange off
            App_Obj.RfFault = APP_RF_FAULT_IDLE;
            App_Obj.Status.Flags.BuzWasAct = 0;
        #endif
        Keys_read_keys_new_event(KEY_MASK_STABLE); //Clean keys events
        #ifdef SW_TEMP_CONST_SEND
            App_Obj.Test.KeyPressTimer = APP_TEST_PRESS_KEY_TIME;
        #endif
        App_Obj.State = APP_STATE_IDLE_STAY;
        break;
        
      case APP_STATE_IDLE_STAY:                 //Idle mode, wait for event to continue
        App_Monitor(FALSE);
        #ifdef SW_TEMP_CONST_SEND
          #ifdef SW_MOT_RF
            if (App_Obj.Status.Flags.TestSendTechTemp == 0)
            {                                   //Only activation of test mode. Deactivate only by reset.
                if (App_Obj.Status.Flags.KeyLastState)
                {                                   //Key is pressed
                    if (Keys_read_keys_state(KEY_MASK_START))
                    {                               //Key still pressed
                        if (App_Obj.Test.KeyPressTimer == 0)
                        {                           //Long press - activate test mode (constantly sent temperature tech message)
                            App_Obj.Status.Flags.TestSendTechTemp = 1;
                            
                            IO_BuzCmnd(IO_BUZ_CMND_BEEP);
                        }    
                    }    
                }
                else
                {
                    if (Keys_read_keys_state(KEY_MASK_START))
                    {                               //Start press - start timer
                        App_Obj.Status.Flags.KeyLastState = 1;
                        App_Obj.Test.KeyPressTimer = APP_TEST_PRESS_KEY_TIME;
                    }    
                }
            }
          #endif
        #endif
        break;
        
      case APP_STATE_LOADED_START:              //Start loaded mode
        #ifdef SW_PROD_ReviveUs
            Revive_Vsup_Cmnd(REVIVE_VSUP_CMND_ON, 120); //Revive US - set V to 12V
        #endif
        App_Obj.Status.Flags.RgbLedAct = 0;
        App_Obj.Status.Flags.RgbLedState = 0;   //RGB LED not active
        Led_ActGreen (LED_GREEN_MODE_PULSE, 0, 20); //Green LED blink
        Keys_read_keys_new_event(KEY_MASK_STABLE); //Clean keys events
        App_Obj.State = APP_STATE_LOADED_STAY;
        break;
        
      case APP_STATE_LOADED_STAY:               //Loaded mode, wait for event to activate
        App_Monitor(FALSE);
        if (Keys_read_keys_new_event(KEY_MASK_START_EVENT))
        {                                       //On was pressed
            App_SetEvent(APP_EVENT_buttons);
            App_Obj.State = APP_STATE_ON_START;
        }
        App_CheckFaultDeactivate();
        break;
        
      case APP_STATE_ON_START:                  //Start On mode
        #ifdef SW_MOT_X
            App_CheckDevice(APP_DEVICE_MOTX);       //Check motors before start
        #endif
        #ifdef SW_MOT_Y
            App_CheckDevice(APP_DEVICE_MOTY);
        #endif
        App_Obj.State = APP_STATE_ON_STAY;
        break;
        
      case APP_STATE_ON_STAY:                               //On mode
        App_Monitor(FALSE);
        break;
        
      case APP_STATE_ACTIVE_START:                          //Start active mode
        Keys_read_keys_new_event(KEY_MASK_STABLE);          //Clean keys events
        #ifdef SW_PROD_ReviveUs
          Led_ActGreen (LED_BLUE_MODE_CONT, 0, 0);          //BLUE LED on
        #else
          Led_ActGreen (LED_GREEN_MODE_CONT, 0, 0);         //Green LED on
        #endif
        #ifdef SW_MOT_Y
            Motor_ActMotor(MOT_ID_Y, TRUE);                 //Enable motors
        #endif
        #ifdef SW_MOT_X
            Motor_ActMotor(MOT_ID_X, TRUE);
        #endif
        #ifdef SW_MOT_RF
            Motor_ActMotor(MOT_ID_RF, TRUE);
            App_RfTest.Timer = APP_RF_SAFETY_TEST_TIMER;
            App_RfTest.FaultCounter = 0;
            App_TestRf(TRUE);
        #endif
        #ifdef SW_PROD_ReviveRf
            Motor_ActMotor(MOT_ID_REVIVE, TRUE);            //Generate a pulse
            Motor_SetVsup(TRUE);
        #endif
        App_RgbLedAct();                                    //Activate RGB LED
        App_Obj.Status.Flags.BuzWasAct = 0;                 //Next high temperature - beep
        App_IntObj.TemperatureTimer = APP_TEMP_FAULT_TIME;
        #ifdef SW_PROD_ReviveRf
            App_IntObj.PulseTimer = 0;
            App_TestRfNoPulse(TRUE);
            App_Obj.State = APP_STATE_ONE_PULSE_STEP2;
        #else
            App_Obj.State = APP_STATE_ACTIVE_STAY;
            App_Obj.Status.Flags.OverHeatState = 0;
        #endif
        break;
        
      case APP_STATE_ACTIVE_STAY:                           //Active mode, wait for event to change
        App_CheckRgbLed();
        App_Monitor(TRUE);                                      //Check motor
        #ifdef SW_MOT_RF
         if (App_Obj.Status.Flags.OverHeat)
         {                                                  //Over heat - motor off, and green LED
            if (App_Obj.Status.Flags.OverHeatState == 0)
            {
                Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_OFF);
                Led_ActGreen (LED_GREEN_MODE_CONT, 0, 0);
                App_Obj.Status.Flags.OverHeatState = 1;
            }
         }
         else
         {                                                  //Heat OK
            if (App_Obj.Status.Flags.OverHeatState)
            {                                               //Was in over heat state - motor on, and blue LED
                Led_ActGreen (LED_BLUE_MODE_CONT, 0, 0);
                Motor_ActMotor(MOT_ID_RF, TRUE);
                App_Obj.Status.Flags.OverHeatState = 0;
            }
            App_RfPulseMonitor();
            if (App_RfTest.Timer == 0)
            {                                   //Monitor RF switching transistors (safety)
                switch(App_TestRf(FALSE))
                {
                  case RESULT_RF_TEST_BUSY:
                    break;
                  case RESULT_RF_TEST_SUCCESS:  //Success - Restart test timer
                    App_RfTest.Timer = APP_RF_SAFETY_TEST_TIMER;
                    App_RfTest.FaultCounter = 0;
                    App_TestRf(TRUE);
                    break;
                  case RESULT_RF_TEST_FAIL:     //Fail - Generate event and pause
                    App_RfTest.FaultCounter++;
                    if (App_RfTest.FaultCounter >= APP_RF_ACTIVE_FAULT_NUM)
                    {                           //A sequence of faults - generate an event
                        Motor_ActMotor(MOT_ID_RF, MOTOR_PARAM_MODE_OFF);
                        App_SetEvent(APP_EVENT_FAULT_Rf);
                        App_Obj.State = APP_STATE_PAUSE_START;
                    }
                    else
                    {                           //Not yet sequence of events - continue
                        App_RfTest.Timer = APP_RF_SAFETY_TEST_TIMER;
                        App_TestRf(TRUE);
                    }    
                    break;
                }
            }
         }
        #endif
        if (Keys_read_keys_new_event(KEY_MASK_UP_EVENT))
        {                                       //Increment motor power
            #ifdef SW_MOT_X
                if (Motor_UpdMotorSetting(/*MOT_ID_X,*/ MOTOR_UPD_MOT_SETTING_INC))
                {                                   //Motor reached upper limit - blink
                    Led_ActGreen (LED_RGB_MODE_PULSEnCONT, 20, 10); //Green LED blinks twice
                }
            #endif
            App_SetEvent(APP_EVENT_buttons);
        }    
        else if (Keys_read_keys_new_event(KEY_MASK_DOWN_EVENT))
        {                                       //Decrement motor power
            #ifdef SW_MOT_X
                if (Motor_UpdMotorSetting(/*MOT_ID_X,*/ MOTOR_UPD_MOT_SETTING_DEC))
                {                                   //Motor reached lower limit - blink
                    Led_ActGreen (LED_RGB_MODE_PULSEnCONT, 20, 10); //Green LED blinks twice
                }    
            #endif
            App_SetEvent(APP_EVENT_buttons);
        }
        else if (Keys_read_keys_new_event(KEY_MASK_START_EVENT))
        {                                       //Pause
            App_SetEvent(APP_EVENT_buttons);
            App_Obj.State = APP_STATE_PAUSE_START;
        }
        App_CheckFaultDeactivate();
        break;
        
      case APP_STATE_PAUSE_START:               //Start pause mode
        App_Obj.Status.Flags.RgbLedState = 0;
        Led_ActRgb(FALSE);                      //RGB LED off
        #ifdef SW_PROD_ReviveUs
          Led_ActGreen (LED_GREEN_MODE_CONT, 0, 0);           //Green LED on
        #else
          Led_ActGreen (LED_GREEN_MODE_PULSE, 0, 20); //Green LED blink
        #endif
        #ifdef SW_MOT_Y
            Motor_ActMotor(MOT_ID_Y, FALSE);    //Disable motors
        #endif
        #ifdef SW_MOT_X
            Motor_ActMotor(MOT_ID_X, FALSE);
        #endif
        #ifdef SW_MOT_RF
            Motor_ActMotor(MOT_ID_RF, FALSE);
            Led_ActOrange(LED_ORANGE_MODE_OFF,0 ,0); //LED2 orange off
        #endif
        App_Obj.State = APP_STATE_PAUSE_STAY;
        break;
        
      case APP_STATE_PAUSE_STAY:                            //Pause mode
        App_Monitor(TRUE);
        if (Keys_read_keys_new_event(KEY_MASK_START_EVENT))
        {                                                   //Resume
            App_SetEvent(APP_EVENT_buttons);
            if (App_Obj.Status.Flags.RgbLedAct)
            {                                               //RGB LED was on - re-activate
                App_Obj.Status.Flags.RgbLedState = 1;
                Led_ActRgb(TRUE);
            }    
            App_Obj.State = APP_STATE_ACTIVE_START;
        }
        App_CheckFaultDeactivate();
        break;
      
      #ifdef SW_PROD_ReviveRf  
          case APP_STATE_ONE_PULSE_STEP1:                       //One pulse - wait for pulse timeout
            if (App_Obj.Status.Flags.PulseDone)
            {
                App_Obj.Status.Flags.PulseDone = 0;
                #ifdef SW_TIP_TIMEOUT
                  TipMem_IncPulses();                       //Count the pulse, after RF is off
                #endif
                App_SetEvent(APP_EVENT_PULSEDONE_done);
                App_IntObj.PulseTimer = APP_PULSE_WAIT;
                App_TestRfNoPulse(TRUE);                    //Start test
                App_Obj.State = APP_STATE_ONE_PULSE_STEP2;
            }
            App_CheckFaultDeactivate();
            break;
            
          case APP_STATE_ONE_PULSE_STEP2:                   //One pulse - wait for next press
            App_Monitor(TRUE);
            if (App_IntObj.PulseTimer == 0)
            {
                Led_ActGreen (LED_GREEN_MODE_CONT, 0, 0);   //Green LED on
                if ((Keys_read_keys_new_event(KEY_MASK_START_EVENT) &&
                    TipMem_GetStatus()) ||                  //Key was pressed, and TIP is OK
					(App_IntObj.FirePulse == 1))			//Fire pulse command (regardless of TIP state)
                {                                           //Start another pulse
					App_IntObj.FirePulse = 0;
					#ifdef SW_PROD_ReviveRf_RepeatPulse
					 if (ReviveRf_RepeatPulse.Act)
					 {
						ReviveRf_RepeatPulse.Act = 0;
					 }
					 else
					 {
						ReviveRf_RepeatPulse.Act = 1;
						ReviveRf_RepeatPulse.Timer = ReviveRf_RepeatPulse_TIME;
					 }
					#endif
                    Motor_StartPulse();                     //Generate a pulse
                    #ifndef SW_TIP_TIMEOUT
                      TipMem_IncPulses();                   //Count the pulse
                    #endif
                    Led_ActGreen (LED_BLUE_MODE_CONT, 0, 0); //Blue LED on
                    App_Obj.State = APP_STATE_ONE_PULSE_STEP1;
                }
				#ifdef SW_PROD_ReviveRf_RepeatPulse
				 else if (ReviveRf_RepeatPulse.Act &&
						 (ReviveRf_RepeatPulse.Timer == 0))
				 {											//Repeat timer over
					Motor_StartPulse();                     //Generate a pulse
					Led_ActGreen (LED_BLUE_MODE_CONT, 0, 0); //Blue LED on
					App_Obj.State = APP_STATE_ONE_PULSE_STEP1;
					ReviveRf_RepeatPulse.Timer = ReviveRf_RepeatPulse_TIME;
				 }
				#endif
                else
                {
                    switch (App_TestRfNoPulse(FALSE))
                    {
                      case RESULT_RF_TEST_BUSY:
                        break;
                      case RESULT_RF_TEST_SUCCESS:
                        #ifdef APP_REVIVE_TEST_SAFETY_CONST
                          App_TestRfNoPulse(TRUE);          //Safety test constantly
                        #endif
                        break;
                      case RESULT_RF_TEST_FAIL:
                        App_SetEvent(APP_EVENT_FAULT_Rf);
                        break;
                    }    
                }    
            }
            else
            {                                               //Blue LED still on - clear key press flag
                Keys_read_keys_new_event(KEY_MASK_START_EVENT);
            }    
            App_CheckFaultDeactivate();
            break;
      #endif
        
      case APP_STATE_DEACTIVATE:                //Deactivate motors and RGB LED
        App_Obj.Status.Flags.RgbLedAct = 0;
        App_Obj.Status.Flags.RgbLedState = 0;
        Led_ActRgb(FALSE);                      //RGB LED off
        #ifdef SW_MOT_Y
            Motor_ActMotor(MOT_ID_Y, FALSE);    //Disable motors
        #endif
        #ifdef SW_MOT_X
            Motor_ActMotor(MOT_ID_X, FALSE);
        #endif
        #ifdef SW_MOT_RF
            Motor_ActMotor(MOT_ID_RF, FALSE);
            App_Obj.RfFault = APP_RF_FAULT_IDLE;
            App_Obj.Status.Flags.BuzWasAct = 0;
        #endif
        #ifdef SW_PROD_ReviveRf
            Motor_SetVsup(FALSE);
        #endif
        App_Obj.State = APP_STATE_IDLE_START;
        break;
        
      #ifdef SW_DEBUG_TRAP_EVENT
      case APP_STATE_BLOCK:
        switch (App_EventSav)
        {
          case APP_EVENT_FAULT_OverVolt:
            Led_ActGreen (LED_GREENnBLUE_MODE_CONT, 0, 0);
            break;
          case APP_EVENT_FAULT_Temperature:
            Led_ActGreen (LED_BLUEnRED_MODE_CONT, 0, 0);
            break;
          case APP_EVENT_FAULT_OverCur:
            Led_ActGreen (LED_REDnGREEN_MODE_CONT, 0, 0);
            break;
          case APP_EVENT_FAULT_Rf:
            Led_ActGreen (LED_ALL_MODE_CONT, 0, 0);
            break;
          case APP_EVENT_FAULT_Tip:
            Led_ActGreen (LED_ALL_MODE_CONT, 0, 0);
            break;
        }    
        Led_ActOrange(LED_ORANGE_MODE_OFF, 0, 0);
        App_Obj.State = APP_STATE_BLOCK1;
        break;
      case APP_STATE_BLOCK1:
        break;
      #endif
    }    
    
    #ifdef SW_DEBUG_TRAP_EVENT
    if (App_EventSav != APP_EVENT_num)
    {
        App_Obj.State = APP_STATE_BLOCK;
    }   
    #endif
     
    //Test ---------------------------------------------------------------------
    #ifdef SW_TEMP_CONST_SEND
        if ((App_Obj.Status.Flags.TestSendTechTemp) && (App_IntObj.Timer == 0))
        {
            App_IntObj.Timer = APP_TEST_TECH_TEMP_TIME;
            App_SetEvent(APP_EVENT_TECH_INFO_Temp);
        }    
    #endif
}    

/******************************************************************************
*  Function name: App_TaskTick1mS
*  Main tasks:
*    Timers of Application task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called every 1mS, from an interrupt
******************************************************************************/
void App_TaskTick1mS (void)
{
    if (App_IntObj.Timer)
    {
        App_IntObj.Timer--;
    }
    if (App_IntObj.TemperatureTimer)
    {
        App_IntObj.TemperatureTimer--;
    }
    #ifdef SW_MOT_RF
        if (App_RfTest.Timer)
        {
            App_RfTest.Timer--;
        }
    #endif
    
    #ifdef SW_TEMP_CONST_SEND
        if (App_Obj.Test.SendMsgTimer)
        {
            App_Obj.Test.SendMsgTimer--;
        }
        if (App_Obj.Test.KeyPressTimer)
        {
            App_Obj.Test.KeyPressTimer--;
        }
    #endif

    #ifdef SW_APP_OVERCURRENT_KEY
        if (Overcurrent_Simulator_Timer) {Overcurrent_Simulator_Timer--;}
    #endif
    
    #ifdef SW_PROD_ReviveRf
        if (App_IntObj.PulseTimer)
            {App_IntObj.PulseTimer--;}
        if (App_IntObj.TestNPTimer)
            {App_IntObj.TestNPTimer--;}
    #endif

	#ifdef SW_PROD_ReviveRf_RepeatPulse
		if (ReviveRf_RepeatPulse.Timer)
			{ReviveRf_RepeatPulse.Timer--;}
    #endif
}    


