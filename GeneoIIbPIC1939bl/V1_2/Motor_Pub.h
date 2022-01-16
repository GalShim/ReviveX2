/* ========================================
 *
 * Public definitions for Motor.c
 *
 * ========================================
*/

#ifndef _MOTOR_PUB_H_
#define _MOTOR_PUB_H_

#ifdef SW_PROD_ReviveRf
 typedef enum{
    MOT_ID_RF = 0,      //Handle RF Pulse/PWM in software
    MOT_ID_num
 } MOT_ID_e;
 #define MOT_ID_REVIVE MOT_ID_RF
#else
 typedef enum{
    MOT_ID_X = 0,   //H
    MOT_ID_Y,       //V
    //MOT_ID_both,
    MOT_ID_RF,      //Handle RF Pulse/PWM in software
    MOT_ID_LEDR,    //Handle LED Pulse/PWM in software
    MOT_ID_LEDG,    //Handle LED Pulse/PWM in software
    MOT_ID_LEDB,    //Handle LED Pulse/PWM in software
    MOT_ID_num
 } MOT_ID_e;
 //#define MOT_ID_REVIVE MOT_ID_RF
#endif

typedef enum{
    MOTOR_PARAM_MODE_OFF = 0,   //Off
    MOTOR_PARAM_MODE_CONT,      //Continuous
    MOTOR_PARAM_MODE_PULSE      //Pulses
} MOTOR_PARAM_MODE_e;    

typedef enum{
    MOTOR_UPD_MOT_SETTING_NONE = 0,
    MOTOR_UPD_MOT_SETTING_INC,
    MOTOR_UPD_MOT_SETTING_DEC
} MOTOR_UPD_MOT_SETTING_e;    

enum{
    MOTOR_EE_PARAM_MOTOR1_MODE = 0,
    MOTOR_EE_PARAM_MOTOR1_POWER,
    MOTOR_EE_PARAM_MOTOR1_ON,
    MOTOR_EE_PARAM_MOTOR1_OFF,
    MOTOR_EE_PARAM_MOTOR2_MODE,
    MOTOR_EE_PARAM_MOTOR2_POWER,
    MOTOR_EE_PARAM_MOTOR2_ON,
    MOTOR_EE_PARAM_MOTOR2_OFF,
    #ifdef SW_UC_PIC18F
        MOTOR_EE_PARAM_RFOUT_MODE,
        MOTOR_EE_PARAM_RFOUT_INTENSITY,
        MOTOR_EE_PARAM_RFOUT_ON,
        MOTOR_EE_PARAM_RFOUT_OFF,
        MOTOR_EE_PARAM_RFOUT_LOWLEVEL,
        MOTOR_EE_PARAM_RFOUT_HIGHLEVEL,
        MOTOR_EE_PARAM_RFOUT_BUZSTATUS,
    #endif
    #ifdef SW_PROD_ReviveRf
        MOTOR_EE_PARAM_RFOUT_DOUBLEPULSE,
        MOTOR_EE_PARAM_RFOUT_P1POWER,
        MOTOR_EE_PARAM_RFOUT_P1INTENSITY,
        MOTOR_EE_PARAM_RFOUT_DELAY,
        MOTOR_EE_PARAM_RFOUT_P2POWER,
        MOTOR_EE_PARAM_RFOUT_P2INTENSITY,
        MOTOR_EE_PARAM_RFOUT_P1POWERb,                      //Dummy
        MOTOR_EE_PARAM_RFOUT_P1INTENSITYb,                  //Dummy
    #endif
    MOTOR_EE_PARAM_num
};    

#ifdef SW_MOT_RF
    typedef enum{
        MOTOR_RF_EN_STATE_ON0_EN0 = 0,
        MOTOR_RF_EN_STATE_ON0_EN1,
        MOTOR_RF_EN_STATE_ON1_EN0,
        MOTOR_RF_EN_STATE_ON1_EN1
    } MOTOR_RF_EN_STATE_e;
    typedef enum{
        MOTOR_RF_MON_STATE_SET_NEXT = 0,        //Next monitor state
        MOTOR_RF_MON_STATE_SET_PULSE_OFF,       //Off this pulse
        MOTOR_RF_MON_STATE_SET_PULSE_PAUSE      //Pause pulses for a period
    } MOTOR_RF_MON_STATE_SET_e;    
#endif

#define MOTOR_EE_PARAM_NumParamPerMotor (MOTOR_EE_PARAM_MOTOR2_MODE-MOTOR_EE_PARAM_MOTOR1_MODE)

extern void Motor_ActRf (void);
extern void Motor_DisRf (void);
extern void Motor_DisRfForce (void);
extern void Motor_DisRf_EN0 (void);
extern void Motor_DisRf_ON0 (void);
#ifdef SW_MOT_RF
  extern MOTOR_RF_EN_STATE_e Motor_Rf_EnState_Get (void);
  extern uint8 Motor_Rf_Sample_Time_PulseStart(void);
  extern uint8 Motor_Rf_Sample_Time_PulseEnd(void);
  extern uint8 Motor_Rf_Prod_Sample_Time_PulseEnd(void);
  extern uint8 Motor_Rf_Sample_Time_SetMonState(MOTOR_RF_MON_STATE_SET_e MonState);
#endif
extern uint8 Motor_Rf_Sample_Time_Act(void);
extern uint8 Motor_Rf_Sample_Time_Dis(uint8 DeltaTime);
extern void Motor_TaskInit (void);
extern void Motor_TaskMain (void);
#ifdef SW_UC_PIC18F
 #ifdef SW_TICK_100US
     extern void Motor_TaskTick (uint8 IncCounter);
 #else
     extern void Motor_TaskTick (void);
 #endif
#else
 extern void Motor_TaskTick1mS (void);
#endif
extern void Motor_ActMotor(MOT_ID_e MotorId, uint8 Action);
extern uint8 Motor_UpdMotorSetting (/*MOT_ID_e MotorId,*/ MOTOR_UPD_MOT_SETTING_e Upd);
extern void Motor_ParamFromEeprom(MOT_ID_e MotorId);
extern void Motor_SetMotor(MOT_ID_e MotorId, uint8 Action);
extern uint8 Motor_GetMotor(MOT_ID_e MotorId);
#ifdef SW_MOT_X
    extern uint8 Motor_GetPower (void);
#endif
#ifdef SW_MOT_BLDC
    extern uint8 MotorBLDC_CheckRdStuck (void);
    extern uint8 MotorBLDC_CheckSpeedError (void);
    extern void MotorBLDC_TaskTick100mS (void);
#endif
#ifdef SW_PROD_ReviveRf
    extern void Motor_UpdParams(void);
    extern void Motor_StartPulse (void);
    extern void Motor_SetVsup (uint8 Action);
    extern void Mot_Revive_Motor_Off (void);
#endif

#endif
//[] END OF FILE
