/* ========================================
 *
 * Handle motors
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
#include "General.h"
#include "Types.h"
#include "Eeprom_Pub.h"
#include "Revive_Vsup_Pub.h"
#include "Motor_Pub.h"
#include "Motor_Loc.h"
#include "..\..\GeneoIIbPIC1939bl\V1_2\App_Pub.h"

#ifdef SW_PROD_ReviveRf
    typedef enum{                                //Revive motor states
        MOTORREVIVE_STATE_IDLE = 0,
        MOTORREVIVE_STATE_START,
        MOTORREVIVE_STATE_PRE_MOTOR,
        MOTORREVIVE_STATE_P1,
        MOTORREVIVE_STATE_DELAY,
        MOTORREVIVE_STATE_P2,
        MOTORREVIVE_STATE_END
    } MOTORREVIVE_STATE_e;
    #define MOTORREVIVE_PRE_MOTOR_TIME  30      //Time to activate motor before the pulse
#endif

volatile struct{
    struct{
        union{
            uint8       Byte;
            struct {
                uint8       Tick1mS;            //1mSec tick
                uint8       MotXActive:1;       //Motor X signal is active
                uint8       MotYActive:1;       //Motor Y signal is active
                uint8       RfActive:1;         //RF signal is active
                uint8       LedRActive:1;       //LED R signal is active
                uint8       LedGActive:1;       //LED G signal is active
                uint8       LedBActive:1;       //LED B signal is active
                uint8       UpdParams:1;        //New parameters - update
            } Bits;
        } Flags;
        uint8 TickCounter;
    } Task;    
} Motor_IntObj;    
    
struct{
    struct{
        MOTOR_TASK_STATE_e  State;              //State of motor task
        uint8               Timer;
    } Task;
    struct{
        struct{                                 //Parameters as defined by the host
            MOTOR_PARAM_MODE_e  Mode;           //Mode of operation
            uint8               Power;          //1-10
            uint8               OnTime;         //3-100 (0.1Sec units)
            uint8               OffTime;        //3-100 (0.1Sec units)
            #ifdef SW_PROD_ReviveRf
              uint8             doublepulse;    //TRUE/FALSE
              uint8             p1power;        //1-150 (10V increments)
              uint16            p1intensity;    //1-20 x10
              uint16            delay;          //1-20 x10
              uint8             p2power;        //1-50
              uint16            p2intensity;    //1-70 x10
            #endif
        } Params;
        struct{                                 //variable in use
            struct{                             //PWM time - 67-200 mS
                uint8           Total;          //Total time
                uint8           On;             //On (active) time
                uint8           Current;        //Current time (count up)
                #ifdef SW_MOT_RF
                MOTOR_RF_PULSE_MON_STATE_e MonState; //Pulse monitor state
                #endif
                uint16          PauseTimer;     //Timer of pulse off time
            } PwmTime;    
            struct{                             //Cycle time - 600-20000 mS
                uint16          Total;          //Total time
                uint16          On;             //On (burst) time
                uint16          Current;        //Current time (count up)
            } CycleTime;
            struct{                             //Motor state
                MOTOR_PARAM_MODE_e  Motor;      //Off/Continous/Pulses
                MOTOR_PULSE_STATE_e Pulse;
            } State;
        } Action;    
    } Motor[MOT_ID_num];    
} Motor_Obj;    

#ifdef SW_MOT_RF
    MOTOR_RF_EN_STATE_e RfEnState; //State of RF enable (ON and EN)
#endif

//Power units converted to PWM cycle time
const uint8 Motor_PowerToCycleConvert [MOT_PARAM_POWER_STEP_NUM] = {200, 185, 170, 156, 141, 126, 111, 96, 81, 67};

//=============================================================================
//=============================================================================
//Hardware functions
//=============================================================================
//=============================================================================
#ifdef SW_MOT_X                             //H
    #define MOT_PORT_MOTX           PORTE
    #define MOT_PORT_BIT_MOTX       0x04
#endif
#ifdef SW_MOT_Y                             //V
    #define MOT_PORT_MOTY           PORTD
    #define MOT_PORT_BIT_MOTY       0x02
#endif
#ifdef SW_MOT_RF                            //RF
    #define MOT_PORT_RF_ON          LATC //PORTC
    #define MOT_PORT_BIT_RF_ON      0x01
    #define MOT_PORT_RF_ON_ACT      MOT_PORT_RF_ON|=MOT_PORT_BIT_RF_ON
    #ifdef SW_RF_SHORT_ON
        #define MOT_PORT_RF_ON_DIS      MOT_PORT_RF_ON_ACT
    #else
        #define MOT_PORT_RF_ON_DIS      MOT_PORT_RF_ON&=~MOT_PORT_BIT_RF_ON
    #endif
    #define MOT_PORT_RF_EN          LATA //PORTA
    #define MOT_PORT_BIT_RF_EN      0x10
    #define MOT_PORT_RF_EN_ACT      MOT_PORT_RF_EN|=MOT_PORT_BIT_RF_EN
    #ifdef SW_RD_SHORT_EN
        #define MOT_PORT_RF_EN_DIS      MOT_PORT_RF_EN_ACT
    #else
        #define MOT_PORT_RF_EN_DIS      MOT_PORT_RF_EN&=~MOT_PORT_BIT_RF_EN
    #endif
#endif
#ifdef SW_HW_REVIVE
    #define REVIVE_MOT_INIT_PORT    TRISDbits.TRISD4=0
    #define REVIVE_MOT_PORT         LATD
    #define REVIVE_MOT_BIT          0x10
    #define REVIVE_MOT_ON           REVIVE_MOT_PORT|=REVIVE_MOT_BIT
    #define REVIVE_MOT_OFF          REVIVE_MOT_PORT&=~REVIVE_MOT_BIT
    void Mot_Revive_Motor_Off (void)
    {
        REVIVE_MOT_OFF;
    }    
#endif

#ifdef SW_MOT_X                                             //Motor X activate/deactivate
    void Motor_ActMotorX (void)
    {
        MOT_PORT_MOTX |= MOT_PORT_BIT_MOTX;
    }    
    void Motor_DisMotorX (void)
    {
        MOT_PORT_MOTX &= ~MOT_PORT_BIT_MOTX;
    }
#endif
#ifdef SW_MOT_Y                                             //Motor Y activate/deactivate
    void Motor_ActMotorY (void)
    {
        MOT_PORT_MOTY |= MOT_PORT_BIT_MOTY;
    }    
    void Motor_DisMotorY (void)
    {
        MOT_PORT_MOTY &= ~MOT_PORT_BIT_MOTY;
    }
#endif
#ifdef SW_MOT_RF                                            //RF activate/deactivate
    void Motor_ActRf (void)
    {
        MOT_PORT_RF_ON_ACT;                                 //MOT_PORT_RF_ON |= MOT_PORT_BIT_RF_ON;
        #ifndef SW_RF_NO_ACT
            MOT_PORT_RF_EN_ACT;                             //MOT_PORT_RF_EN |= MOT_PORT_BIT_RF_EN;
        #endif
        RfEnState = MOTOR_RF_EN_STATE_ON1_EN1;
    }
    void Motor_DisRfForce (void)
    {
        MOT_PORT_RF_ON_DIS;                                 //MOT_PORT_RF_ON &= ~MOT_PORT_BIT_RF_ON;
        MOT_PORT_RF_EN_DIS;                                 //MOT_PORT_RF_EN &= ~MOT_PORT_BIT_RF_EN;
        RfEnState = MOTOR_RF_EN_STATE_ON0_EN0;
    }    
    void Motor_DisRf (void)
    {
        if ((RfEnState != MOTOR_RF_EN_STATE_ON0_EN1) && (RfEnState != MOTOR_RF_EN_STATE_ON1_EN0))
        {                                       //Check, in order not to change disable test mode
            Motor_DisRfForce();
        }
    }
    void Motor_DisRf_EN0 (void)
    {
        MOT_PORT_RF_ON_ACT;                                 //MOT_PORT_RF_ON |= MOT_PORT_BIT_RF_ON;
        MOT_PORT_RF_EN_DIS;                                 //MOT_PORT_RF_EN &= ~MOT_PORT_BIT_RF_EN;
        RfEnState = MOTOR_RF_EN_STATE_ON1_EN0;
    }
    void Motor_DisRf_ON0 (void)
    {
        MOT_PORT_RF_ON_DIS;                                 //MOT_PORT_RF_ON &= ~MOT_PORT_BIT_RF_ON;
        MOT_PORT_RF_EN_ACT;                                 //MOT_PORT_RF_EN |= MOT_PORT_BIT_RF_EN;
        RfEnState = MOTOR_RF_EN_STATE_ON0_EN1;
    }
    MOTOR_RF_EN_STATE_e Motor_Rf_EnState_Get (void)
    {
        return(RfEnState);
    }
    uint8 Motor_Rf_Sample_Time_Act(void)
    {
      uint8 Result = FALSE;
        if (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Current >= (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.On / 2))
        {
            Result = TRUE;
        }    
        return(Result);
    }    
    uint8 Motor_Rf_Sample_Time_Dis(uint8 DeltaTime)
    {
      uint8 Result = FALSE;
      uint8 TempTime = Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.On;
        #ifndef SW_MOT_US                                                       //For US - start from end of on time
         TempTime += Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Total;           //For RF - start from middle of off time
         TempTime /= 2;
        #endif
        TempTime += DeltaTime;
        #ifdef SW_MOT_US
         if (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Current >= TempTime)      //To minimize time for 100Hz
        #else
         if (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Current > TempTime)
        #endif
        {
            Result = TRUE;
        }    
        return(Result);
    }    
    uint8 Motor_Rf_Sample_Time_PulseStart(void)         //Check if time to measure pulse current, on start of pulse
    {
      uint8 Result = FALSE;
        if ((Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState == MOTOR_RF_PULSE_MON_STATE_START) &&
            (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Current >= MOT_CUR_SAMPLE_PULSE_START))
        {
            Result = TRUE;
        }    
        return(Result);
    }    
    uint8 Motor_Rf_Sample_Time_PulseEnd(void)           //Check if time to measure pulse current, on end of pulse
    {
      uint8 Result = FALSE;
      uint8 TimeOn = Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.On;
      uint8 TimeCurrent = Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Current;
        if ((Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState == MOTOR_RF_PULSE_MON_STATE_WAIT_END) &&
            (TimeOn > TimeCurrent))
        {
            if ((TimeOn - TimeCurrent) <= MOT_CUR_SAMPLE_PULSE_END)
            {
                Result = TRUE;
            }
        }    
        return(Result);
    }
    uint8 Motor_Rf_Prod_Sample_Time_PulseEnd(void)         //Check if time to measure pulse current, on end of pulse, in Production mode
    {
      uint8 Result = FALSE;
      uint8 TimeOn = Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.On;
      uint8 TimeCurrent = Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Current;
        if ((Motor_Rf_EnState_Get() == MOTOR_RF_EN_STATE_ON1_EN1) &&
            (TimeOn > TimeCurrent))
        {
            if ((TimeOn - TimeCurrent) <= MOT_CUR_SAMPLE_PULSE_END)
            {
                Result = TRUE;
            }
        }    
        return(Result);
    }
    uint8 Motor_Rf_Sample_Time_SetMonState(MOTOR_RF_MON_STATE_SET_e MonState)
    {
        switch (MonState)
        {
          case MOTOR_RF_MON_STATE_SET_NEXT:
            if (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState == MOTOR_RF_PULSE_MON_STATE_START)
            {                                               //Start current was sampled
                Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState = MOTOR_RF_PULSE_MON_STATE_WAIT_END;
            }    
            else if (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState == MOTOR_RF_PULSE_MON_STATE_WAIT_END)
            {                                               //End current was sampled
                Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState = MOTOR_RF_PULSE_MON_STATE_END;
            }    
            break;
          case MOTOR_RF_MON_STATE_SET_PULSE_OFF:            //RF off for this pulse
            Motor_SetMotor(MOT_ID_RF, FALSE);
            Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState = MOTOR_RF_PULSE_MON_STATE_PULSE_OFF;
            break;
          case MOTOR_RF_MON_STATE_SET_PULSE_PAUSE:          //RF off for a period
            Motor_SetMotor(MOT_ID_RF, FALSE);
            Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.PauseTimer = MOT_PULSE_PAUSE_TIME;
            Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState = MOTOR_RF_PULSE_MON_STATE_PULSE_PAUSE;
            break;
        }    
    }    
#endif

void Motor_HwInit(void)                                     //Init hardware
{
    #ifdef SW_MOT_X
        Motor_DisMotorX();
        TRISE2 = 0;                                         //Set port
        ANSELE &= ~MOT_PORT_BIT_MOTX;                       //Disable analog function
    #endif
    #ifdef SW_MOT_Y
        Motor_DisMotorY();
        TRISD1 = 0;                                         //Set output ports
    #endif
    #ifdef SW_MOT_RF
        Motor_DisRf();
        TRISCbits.TRISC0 = 0;                               //Set output ports
        TRISAbits.TRISA4 = 0;                               //Set output ports
    #endif
}    
//=============================================================================
//=============================================================================

/******************************************************************************
*  Function name: Motor_UpdParams
*  Main tasks:
*    Set flags that the parameters were updated
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
#ifdef SW_PROD_ReviveRf
  void Motor_UpdParams(void)
  {
      Motor_IntObj.Task.Flags.Bits.UpdParams = 1;
  }    
#endif

/******************************************************************************
*  Function name: Motor_SetMotor
*  Main tasks:
*    Set motor ports
*  Parameters:
*    Motor id
*    On/Off - TRUE/FALSE
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void Motor_SetMotor(MOT_ID_e MotorId, uint8 Action)
{
    switch (MotorId)
    {
      #ifdef SW_MOT_Y
          case MOT_ID_Y:
            if (Action)
            {
                Motor_ActMotorY();
                Motor_IntObj.Task.Flags.Bits.MotYActive = 1;
            }
            else
            {
                Motor_DisMotorY();
                Motor_IntObj.Task.Flags.Bits.MotYActive = 0;
            }        
            break;
      #endif
      #ifdef SW_MOT_X
          case MOT_ID_X:
            if (Action)
            {
                Motor_ActMotorX();
                Motor_IntObj.Task.Flags.Bits.MotXActive = 1;
            }
            else
            {
                Motor_DisMotorX();
                Motor_IntObj.Task.Flags.Bits.MotXActive = 0;
            }        
            break;
      #endif
      #ifdef SW_MOT_RF
          case MOT_ID_RF:
            if (Action)
            {
                Motor_ActRf();
                Motor_IntObj.Task.Flags.Bits.RfActive = 1;
            }
            else
            {
                Motor_DisRf();
                Motor_IntObj.Task.Flags.Bits.RfActive = 0;
            }        
            break;
      #endif
    }    
}    

/******************************************************************************
*  Function name: Motor_GetMotor
*  Main tasks:
*    Get motor state
*  Parameters:
*    Motor id
*  Returned values:
*    On/Off - TRUE/FALSE
*  Remarks:
*    None
******************************************************************************/
uint8 Motor_GetMotor(MOT_ID_e MotorId)
{
  uint8 Result = FALSE;
    switch (MotorId)
    {
      case MOT_ID_RF:
        if (Motor_IntObj.Task.Flags.Bits.RfActive)
        {
            Result = TRUE;
        }    
        break;
      #ifndef SW_PROD_ReviveRf
          case MOT_ID_Y:
            if (Motor_IntObj.Task.Flags.Bits.MotYActive)
            {
                Result = TRUE;
            }    
            break;
          case MOT_ID_LEDR:
            if (Motor_IntObj.Task.Flags.Bits.LedRActive)
            {
                Result = TRUE;
            }    
            break;
          case MOT_ID_LEDG:
            if (Motor_IntObj.Task.Flags.Bits.LedGActive)
            {
                Result = TRUE;
            }    
            break;
          case MOT_ID_LEDB:
            if (Motor_IntObj.Task.Flags.Bits.LedBActive)
            {
                Result = TRUE;
            }    
            break;
      #endif
    }
    return(Result);
}    

/******************************************************************************
*  Function name: Motor_GetPower
*  Main tasks:
*    Get motor power
*  Parameters:
*    None
*  Returned values:
*    Power (1-10). 0-if both motors are Off.
*  Remarks:
*    None
******************************************************************************/
#ifdef SW_MOT_X
uint8 Motor_GetPower (void)
{
  uint8 Result = 0xff;
  MOT_ID_e MotorId = MOT_ID_X;
    if (Motor_Obj.Motor[MotorId].Params.Mode == MOTOR_PARAM_MODE_OFF)
    {                                                   //Motor X is Off
        MotorId = MOT_ID_Y;
        if (Motor_Obj.Motor[MotorId].Params.Mode == MOTOR_PARAM_MODE_OFF)
        {                                               //Motor Y is also Off
            Result = 0;                                 //Power is 0
        }    
    }
    if (Result)
    {
        Result = Motor_Obj.Motor[MotorId].Params.Power;
    }
    return(Result);    
}    
#endif

/******************************************************************************
*  Function name: Motor_UpdMotorSetting
*  Main tasks:
*    Update motor setting
*  Parameters:
*    Update
*  Returned values:
*    TRUE/FALSE - reached limit
*  Remarks:
*    None
******************************************************************************/
//-------------------------------
#ifndef SW_MOT_RF
    MOT_ID_e Motor_OtherMotor (MOT_ID_e MotorId)
    {
      MOT_ID_e Result = MOT_ID_Y;
        if (MotorId == MOT_ID_Y)
        {
            Result = MOT_ID_X;
        }
        return(Result);
    }
#endif
//-------------------------------
uint8 Motor_UpdMotorSetting (MOTOR_UPD_MOT_SETTING_e Upd)
{
  uint8 Result = TRUE;
  uint16 TempW;
    
    #ifndef SW_MOT_RF
        MOT_ID_e MotorId = MOT_ID_X;
        if (Motor_Obj.Motor[MotorId].Params.Mode == MOTOR_PARAM_MODE_OFF)
        {                                           //Motor idle - use the other (at least one must be pule or cont mode)
            MotorId = Motor_OtherMotor(MotorId);
        }    
        switch (Upd)
        {
          case MOTOR_UPD_MOT_SETTING_DEC:           //Decrement power step
            if (Motor_Obj.Motor[MotorId].Params.Power > MOT_PARAM_POWER_STEP_MIN)
            {
                Motor_Obj.Motor[MotorId].Params.Power--;
                Result = FALSE;
            }
            Motor_Obj.Motor[Motor_OtherMotor(MotorId)].Params.Power = Motor_Obj.Motor[MotorId].Params.Power; //Other motor, if not idle, must have the same power.
            break;
          case MOTOR_UPD_MOT_SETTING_INC:           //Increment power step
            if (Motor_Obj.Motor[MotorId].Params.Power < MOT_PARAM_POWER_STEP_MAX)
            {
                Motor_Obj.Motor[MotorId].Params.Power++;
                Result = FALSE;
            }
            Motor_Obj.Motor[Motor_OtherMotor(MotorId)].Params.Power = Motor_Obj.Motor[MotorId].Params.Power; //Other motor, if not idle, must have the same power.
            break;
        }
    #endif
    
    #ifdef SW_MOT_RF
        Motor_Obj.Motor[MOT_ID_RF].Action.State.Motor =       Motor_Obj.Motor[MOT_ID_RF].Params.Mode;   //Update motor RF parameters
        TempW =                                               Motor_Obj.Motor[MOT_ID_RF].Params.OnTime;
        TempW *=                                              MOT_PARAM_CYCLE_FACTOR;
        Motor_Obj.Motor[MOT_ID_RF].Action.CycleTime.On =      TempW;
        TempW =                                               Motor_Obj.Motor[MOT_ID_RF].Params.OffTime;
        TempW *=                                              MOT_PARAM_CYCLE_FACTOR;
        TempW +=                                              Motor_Obj.Motor[MOT_ID_RF].Action.CycleTime.On;
        Motor_Obj.Motor[MOT_ID_RF].Action.CycleTime.Total =   TempW;
        Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Total =     100;                                      //10Hz
        TempW =                                               Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Total;
        TempW /=                                              100;
        TempW *=                                              Motor_Obj.Motor[MOT_ID_RF].Params.Power;  //On in %
        #ifdef SW_MOT_RF_HALF_POWER
            TempW /= 2;                                                                                 //Half value
        #endif
        #ifdef SW_MOT_RF_THIRD_POWER
            TempW /= 3;                                                                                 //1/3 value
        #endif
        #ifdef SW_MOT_RF_THREE_QUARTERS_POWER
            TempW /= 4;                                                                                 //3/4 value
            TempW *= 3;
        #endif
        #ifdef SW_MOT_RF_FULL_POWER                                                                     //No change in value
        #endif
        Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.On =        (uint8)TempW;
        #ifdef SW_MOT_US                                                                                //Ultrasound - 100Hz
            Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Total /= 10;
            Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.On /= 10;
        #endif
        #ifdef SW_TICK_100US                                                                            //100Us tick. 10 times faster.
            Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.Total *= 10;
            Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.On *= 10;
        #endif
    #else
        Motor_Obj.Motor[MOT_ID_X].Action.State.Motor =       Motor_Obj.Motor[MOT_ID_X].Params.Mode;   //Update motor X parameters
        Motor_Obj.Motor[MOT_ID_X].Action.CycleTime.On =      Motor_Obj.Motor[MOT_ID_X].Params.OnTime * MOT_PARAM_CYCLE_FACTOR;
        Motor_Obj.Motor[MOT_ID_X].Action.CycleTime.Total =   Motor_Obj.Motor[MOT_ID_X].Action.CycleTime.On + (Motor_Obj.Motor[MOT_ID_X].Params.OffTime * MOT_PARAM_CYCLE_FACTOR);
        Motor_Obj.Motor[MOT_ID_X].Action.PwmTime.Total =     Motor_PowerToCycleConvert[Motor_Obj.Motor[MOT_ID_X].Params.Power - 1];
        TempW =                                              ((uint16)(Motor_Obj.Motor[MOT_ID_X].Action.PwmTime.Total) * MOT_PARAM_DUTY_CYCLE) / MOT_PARAM_POWER_FACTOR;
        Motor_Obj.Motor[MOT_ID_X].Action.PwmTime.On =        (uint8)TempW;
        
        Motor_Obj.Motor[MOT_ID_Y].Action.State.Motor =       Motor_Obj.Motor[MOT_ID_Y].Params.Mode;   //Update motor Y parameters
        Motor_Obj.Motor[MOT_ID_Y].Action.CycleTime.On =      Motor_Obj.Motor[MOT_ID_Y].Params.OnTime * MOT_PARAM_CYCLE_FACTOR;
        Motor_Obj.Motor[MOT_ID_Y].Action.CycleTime.Total =   Motor_Obj.Motor[MOT_ID_Y].Action.CycleTime.On + (Motor_Obj.Motor[MOT_ID_Y].Params.OffTime * MOT_PARAM_CYCLE_FACTOR);
        Motor_Obj.Motor[MOT_ID_Y].Action.PwmTime.Total =     Motor_PowerToCycleConvert[Motor_Obj.Motor[MOT_ID_Y].Params.Power - 1];
        TempW =                                              ((uint16)(Motor_Obj.Motor[MOT_ID_Y].Action.PwmTime.Total) * MOT_PARAM_DUTY_CYCLE) / MOT_PARAM_POWER_FACTOR;
        Motor_Obj.Motor[MOT_ID_Y].Action.PwmTime.On =        (uint8)TempW;
    #endif
    return(Result);
}    

/******************************************************************************
*  Function name: Motor_ParamFromEeprom
*  Main tasks:
*    Get motor parameter from memory
*  Parameters:
*    Motor id
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void Motor_ParamFromEeprom(MOT_ID_e MotorId)
{
    #ifdef SW_PROD_ReviveRf
        Motor_Obj.Motor[MotorId].Params.doublepulse= Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_DOUBLEPULSE);
        Motor_Obj.Motor[MotorId].Params.p1power =    Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_P1POWER);
          if (Motor_Obj.Motor[MotorId].Params.p1power > SW_VSUP_MAX_POWER) {Motor_Obj.Motor[MotorId].Params.p1power = SW_VSUP_MAX_POWER;}
          if (Motor_Obj.Motor[MotorId].Params.p1power < SW_VSUP_MIN_POWER) {Motor_Obj.Motor[MotorId].Params.p1power = SW_VSUP_MIN_POWER;}
        Motor_Obj.Motor[MotorId].Params.p1intensity =Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_P1INTENSITY)/* * 10*/;
        Motor_Obj.Motor[MotorId].Params.delay =      Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_DELAY)/* * 10*/;
        Motor_Obj.Motor[MotorId].Params.p2power =    Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_P2POWER);
          if (Motor_Obj.Motor[MotorId].Params.p2power > SW_VSUP_MAX_POWER) {Motor_Obj.Motor[MotorId].Params.p2power = SW_VSUP_MAX_POWER;}
          if (Motor_Obj.Motor[MotorId].Params.p2power < SW_VSUP_MIN_POWER) {Motor_Obj.Motor[MotorId].Params.p2power = SW_VSUP_MIN_POWER;}
        Motor_Obj.Motor[MotorId].Params.p2intensity =Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_P2INTENSITY)/* * 10*/;
	  #ifdef SW_PROD_ReviveRf_RepeatPulse
//Added 2.11.2020
//8.11.2020 canceled, becuse tests in Pollogen were done with version from 26.10.2020
//		Motor_Obj.Motor[MotorId].Params.doublepulse = 0;
//		Motor_Obj.Motor[MotorId].Params.p1power = 100;
//		Motor_Obj.Motor[MotorId].Params.p1intensity = 40;
	  #endif
    #else
        Motor_Obj.Motor[MotorId].Params.Mode =      Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, (MOTOR_EE_PARAM_NumParamPerMotor*MotorId)+MOTOR_EE_PARAM_MOTOR1_MODE);
        Motor_Obj.Motor[MotorId].Params.Power =     Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, (MOTOR_EE_PARAM_NumParamPerMotor*MotorId)+MOTOR_EE_PARAM_MOTOR1_POWER);
        Motor_Obj.Motor[MotorId].Params.OnTime =    Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, (MOTOR_EE_PARAM_NumParamPerMotor*MotorId)+MOTOR_EE_PARAM_MOTOR1_ON);
        Motor_Obj.Motor[MotorId].Params.OffTime =   Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, (MOTOR_EE_PARAM_NumParamPerMotor*MotorId)+MOTOR_EE_PARAM_MOTOR1_OFF);
    #endif
}    

/******************************************************************************
*  Function name: Motor_ActMotor
*  Main tasks:
*    Activate/deactivate motor
*  Parameters:
*    Motor id
*    On/Off - TRUE/FALSE
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void Motor_ActMotor(MOT_ID_e MotorId, uint8 Action)
{
    if (Action)
    {                                           //Start motor
        #ifdef SW_DEBUG_TEST_WD
            while (1) {}
        #endif
        Motor_ParamFromEeprom(MotorId);
        Motor_Obj.Motor[MotorId].Params.Mode = Eeprom_ReadParam(EEPROM_PARAM_TASK_MOTOR, (MOTOR_EE_PARAM_NumParamPerMotor*MotorId)+MOTOR_EE_PARAM_MOTOR1_MODE);
        Motor_UpdMotorSetting(MOTOR_UPD_MOT_SETTING_NONE);
        
        Motor_Obj.Motor[MotorId].Action.CycleTime.Current = 0;
        Motor_Obj.Motor[MotorId].Action.PwmTime.Current = 0;
        
    }
    else
    {                                           //Stop motor
        Motor_Obj.Motor[MotorId].Params.Mode = MOTOR_PARAM_MODE_OFF;
        Motor_Obj.Motor[MotorId].Action.State.Motor = Motor_Obj.Motor[MotorId].Params.Mode;
    }        
}    
/******************************************************************************
*  Function name: Motor_StartPulse
*  Main tasks:
*    Start motor pulse
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Used for Revive RF
******************************************************************************/
#ifdef SW_PROD_ReviveRf
    void Motor_StartPulse (void)
    {
        Motor_Obj.Motor[MOT_ID_REVIVE].Action.State.Motor = MOTORREVIVE_STATE_START;
    }    
#endif

/******************************************************************************
*  Function name: Motor_SetVsup
*  Main tasks:
*    Set motor supply voltage
*  Parameters:
*    TRUE/FALSE
*  Returned values:
*    None
*  Remarks:
*    Used for Revive RF
******************************************************************************/
#ifdef SW_PROD_ReviveRf
    void Motor_SetVsup (uint8 Action)
    {
        if (Action)
        {
            Revive_Vsup_Cmnd (REVIVE_VSUP_CMND_ON, Motor_Obj.Motor[MOT_ID_REVIVE].Params.p1power); //Activate motor in P1 amplitude
        }
        else
        {
            Revive_Vsup_Cmnd (REVIVE_VSUP_CMND_OFF, 0);
        }        
    }
#endif

/******************************************************************************
*  Function name: Motor_TaskInit
*  Main tasks:
*    Initialize Motors task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called on start up
******************************************************************************/
void Motor_TaskInit (void)
{
  uint8 MotorId;
    Motor_HwInit();                             //Init hardware
    
    Motor_IntObj.Task.Flags.Byte = 0;
    Motor_IntObj.Task.TickCounter = 0;
    
    for (MotorId=0; MotorId<MOT_ID_num; MotorId++)
    {
        Motor_Obj.Motor[MotorId].Action.State.Motor = MOTOR_PARAM_MODE_OFF;
        Motor_Obj.Motor[MotorId].Params.Mode = MOTOR_PARAM_MODE_OFF;
    }    
    
    #ifdef SW_MOT_X
        Motor_ParamFromEeprom(MOT_ID_X);        //Get motor parameter from EEPROM
        Motor_ActMotor(MOT_ID_X, FALSE);        //Disable motor
    #endif
    #ifdef SW_MOT_Y
        Motor_ParamFromEeprom(MOT_ID_Y);
        Motor_ActMotor(MOT_ID_Y, FALSE);
    #endif
    #ifdef SW_MOT_RF
        Motor_ParamFromEeprom(MOT_ID_RF);
        Motor_ActMotor(MOT_ID_RF, FALSE);
        Motor_SetMotor(MOT_ID_RF, FALSE);
    #endif
    #ifdef SW_HW_REVIVE
        REVIVE_MOT_INIT_PORT;
        REVIVE_MOT_OFF;
    #endif
}


/******************************************************************************
*  Function name: Motor_TaskMain
*  Main tasks:
*    Handle Motors task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called periodically
******************************************************************************/
void Motor_TaskMain (void)
{
  uint8 MotorId;
  uint8 TempCounter = Motor_IntObj.Task.TickCounter;
  Motor_IntObj.Task.TickCounter -= TempCounter;
    while (TempCounter) 
    {                                                           //Tick - handle motors
      TempCounter--;
      #ifdef SW_PROD_ReviveRf //------------------------------------------------
        switch (Motor_Obj.Motor[MOT_ID_REVIVE].Action.State.Motor)
        {
          case MOTORREVIVE_STATE_IDLE:
            if (Motor_IntObj.Task.Flags.Bits.UpdParams)
            {                                               //New parameters were received - update
                Motor_IntObj.Task.Flags.Bits.UpdParams = 0;
                Motor_ActMotor(MOT_ID_REVIVE, TRUE);        //Activate the motor with the new parameters
                Motor_SetVsup(TRUE);                        //Set Vsup

            }    
            break;
          case MOTORREVIVE_STATE_START:
            Motor_Obj.Motor[MOT_ID_REVIVE].Action.CycleTime.Current = 0;
            Motor_Obj.Motor[MOT_ID_REVIVE].Action.State.Motor = MOTORREVIVE_STATE_PRE_MOTOR;
            if (Motor_Obj.Motor[MOT_ID_REVIVE].Params.doublepulse)
            {                                               //Double pulse - activate brush motor
                REVIVE_MOT_ON;
            }
            else
            {                                               //Single pulse - skip pre motor
                Motor_Obj.Motor[MOT_ID_REVIVE].Action.CycleTime.Current = MOTORREVIVE_PRE_MOTOR_TIME - 1;
            }        
            break;
          case MOTORREVIVE_STATE_PRE_MOTOR:
            Motor_Obj.Motor[MOT_ID_REVIVE].Action.CycleTime.Current++;
            if (Motor_Obj.Motor[MOT_ID_REVIVE].Action.CycleTime.Current >= MOTORREVIVE_PRE_MOTOR_TIME)
            {                                               //Pre motor time end  - start pulse
                Motor_SetMotor(MOT_ID_REVIVE, TRUE);
                Motor_Obj.Motor[MOT_ID_REVIVE].Action.State.Motor = MOTORREVIVE_STATE_P1;
            }
            break;
          case MOTORREVIVE_STATE_P1:
            Motor_Obj.Motor[MOT_ID_REVIVE].Action.CycleTime.Current++;
            if (Motor_Obj.Motor[MOT_ID_REVIVE].Action.CycleTime.Current >= (MOTORREVIVE_PRE_MOTOR_TIME +
                                                                            Motor_Obj.Motor[MOT_ID_REVIVE].Params.p1intensity))
            {                                               //P1 time over - stop output
                if (Motor_Obj.Motor[MOT_ID_REVIVE].Params.doublepulse)
                {                                           //Double pulse - continue
                    Revive_Vsup_Cmnd (REVIVE_VSUP_CMND_ON, Motor_Obj.Motor[MOT_ID_REVIVE].Params.p2power); //Activate motor in P2 amplitude
                    Motor_Obj.Motor[MOT_ID_REVIVE].Action.State.Motor = MOTORREVIVE_STATE_DELAY;
                    if (Motor_Obj.Motor[MOT_ID_REVIVE].Params.delay == 0)
                    {                                       //Delay 0 - continue directly to next pulse
                        Motor_Obj.Motor[MOT_ID_REVIVE].Action.State.Motor = MOTORREVIVE_STATE_P2;
                    }
                    else
                        {Motor_SetMotor(MOT_ID_REVIVE, FALSE);} //Delay != 0 - Deactivate motor
                }
                else
                {                                           //Single pulse - end
                    Motor_SetMotor(MOT_ID_REVIVE, FALSE);   //Deactivate motor
                    Motor_Obj.Motor[MOT_ID_REVIVE].Action.State.Motor = MOTORREVIVE_STATE_END;
                }    
            }    
            break;
          case MOTORREVIVE_STATE_DELAY:
            Motor_Obj.Motor[MOT_ID_REVIVE].Action.CycleTime.Current++;
            if (Motor_Obj.Motor[MOT_ID_REVIVE].Action.CycleTime.Current >= (MOTORREVIVE_PRE_MOTOR_TIME +
                                                                            Motor_Obj.Motor[MOT_ID_REVIVE].Params.p1intensity +
                                                                            Motor_Obj.Motor[MOT_ID_REVIVE].Params.delay))
            {                                               //Delay time over - activate output
                Motor_SetMotor(MOT_ID_REVIVE, TRUE);
                Motor_Obj.Motor[MOT_ID_REVIVE].Action.State.Motor = MOTORREVIVE_STATE_P2;
            }    
            break;
          case MOTORREVIVE_STATE_P2:
            Motor_Obj.Motor[MOT_ID_REVIVE].Action.CycleTime.Current++;
            if (Motor_Obj.Motor[MOT_ID_REVIVE].Action.CycleTime.Current >= (MOTORREVIVE_PRE_MOTOR_TIME +
                                                                            Motor_Obj.Motor[MOT_ID_REVIVE].Params.p1intensity +
                                                                            Motor_Obj.Motor[MOT_ID_REVIVE].Params.delay +
                                                                            Motor_Obj.Motor[MOT_ID_REVIVE].Params.p2intensity))
            {                                               //P2 time over - stop output
                Motor_SetMotor(MOT_ID_REVIVE, FALSE);       //Deactivate motor
                Revive_Vsup_Cmnd (REVIVE_VSUP_CMND_ON, Motor_Obj.Motor[MOT_ID_REVIVE].Params.p1power); //Activate motor in P1 amplitude
                REVIVE_MOT_OFF;                             //Deactivate brush motor
                Motor_Obj.Motor[MOT_ID_REVIVE].Action.State.Motor = MOTORREVIVE_STATE_END;
            }    
            break;
          case MOTORREVIVE_STATE_END:
            App_PulseDone();
            Motor_Obj.Motor[MOT_ID_REVIVE].Action.State.Motor = MOTORREVIVE_STATE_IDLE;
            break;
        }    
      #else //------------------------------------------------------------------
        for (MotorId=0; MotorId<MOT_ID_num; MotorId++)
        {
            if (Motor_Obj.Motor[MotorId].Action.State.Motor == MOTOR_PARAM_MODE_OFF)
            {                                                   //Motor not active
                Motor_SetMotor(MotorId, FALSE);                 //Motor off (handled here, to save time)
            }
            else
            {                                                   //Motor active
                                                                //Handle burst on/off cycle time
                Motor_Obj.Motor[MotorId].Action.CycleTime.Current++;
                if (Motor_Obj.Motor[MotorId].Action.CycleTime.Current >= Motor_Obj.Motor[MotorId].Action.CycleTime.Total)
                {
                    Motor_Obj.Motor[MotorId].Action.CycleTime.Current = 0;
                }
                switch (Motor_Obj.Motor[MotorId].Action.State.Motor)
                {
                  case MOTOR_PARAM_MODE_OFF:                        //Off - motor not active
                    Motor_Obj.Motor[MotorId].Action.State.Pulse = MOTOR_PULSE_STATE_OFF;
                    break;
                  case MOTOR_PARAM_MODE_CONT:                       //Continous - motor active
                    Motor_Obj.Motor[MotorId].Action.State.Pulse = MOTOR_PULSE_STATE_ON;
                    break;
                  case MOTOR_PARAM_MODE_PULSE:                      //Pulses - active/not active
                    if (Motor_Obj.Motor[MotorId].Action.CycleTime.Current < Motor_Obj.Motor[MotorId].Action.CycleTime.On)
                    {
                        Motor_Obj.Motor[MotorId].Action.State.Pulse = MOTOR_PULSE_STATE_ON;
                    }
                    else
                    {
                        Motor_Obj.Motor[MotorId].Action.State.Pulse = MOTOR_PULSE_STATE_OFF;
                    }    
                    break;
                }
                
                                                                    //Handle PWM
                Motor_Obj.Motor[MotorId].Action.PwmTime.Current++;
                if (Motor_Obj.Motor[MotorId].Action.PwmTime.Current >= Motor_Obj.Motor[MotorId].Action.PwmTime.Total)
                {
                    Motor_Obj.Motor[MotorId].Action.PwmTime.Current = 0;
                    #ifdef SW_MOT_RF
                        if (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState == MOTOR_RF_PULSE_MON_STATE_PULSE_PAUSE)
                        {                                           //If pause - check that pause time is over
                            if (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.PauseTimer == 0)
                            {                                       //Pause time over - start monitor cycle
                                Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState = MOTOR_RF_PULSE_MON_STATE_START;
                            }    
                        }
                        else
                        {                                           //Else - start monitor cycle
                            Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState = MOTOR_RF_PULSE_MON_STATE_START;
                        }        
                    #endif
                }
                switch (Motor_Obj.Motor[MotorId].Action.State.Pulse)
                {
                  case MOTOR_PULSE_STATE_OFF:                       //Pulse off
                    Motor_SetMotor(MotorId, FALSE);
                    break;
                  case MOTOR_PULSE_STATE_ON:                        //Pulse on - check timer
                    if (Motor_Obj.Motor[MotorId].Action.PwmTime.Current < Motor_Obj.Motor[MotorId].Action.PwmTime.On)
                    {
                        #ifdef SW_MOT_RF
                            if ((Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState != MOTOR_RF_PULSE_MON_STATE_PULSE_OFF) &&   //Activate motor only if pulse not in off state (because of over current)
                                (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.MonState != MOTOR_RF_PULSE_MON_STATE_PULSE_PAUSE))
                        #endif
                        {
                            Motor_SetMotor(MotorId, TRUE);
                        }
                    }
                    else
                    {
                        Motor_SetMotor(MotorId, FALSE);
                    }        
                    break;
                }    
            }
        } //End "for"
      #endif
    }    
}

/******************************************************************************
*  Function name: Motor_TaskTick
*  Main tasks:
*    Timers of Motors task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called every 1mS, from an interrupt
******************************************************************************/
#ifndef SW_TICK_100US
    void Motor_TaskTick (void)
    {
        Motor_IntObj.Task.TickCounter++;
        #ifdef SW_MOT_RF
            if (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.PauseTimer)
            {
                Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.PauseTimer--;
            }    
        #endif
    }    
#else
    void Motor_TaskTick (uint8 IncCounter)
    {
        Motor_IntObj.Task.TickCounter += IncCounter;
        #ifdef SW_MOT_RF
            if (Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.PauseTimer)
            {
                Motor_Obj.Motor[MOT_ID_RF].Action.PwmTime.PauseTimer--;
            }    
        #endif
    }    
#endif
