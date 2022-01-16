/* ========================================
 *
 * Local definitions for Motor.c
 *
 * ========================================
*/

#ifndef _MOTOR_LOC_H_
#define _MOTOR_LOC_H_

typedef enum{
    MOTOR_TASK_STATE_INIT = 0,
    MOTOR_TASK_STATE_START
} MOTOR_TASK_STATE_e;    

typedef enum{
    MOTOR_PULSE_STATE_OFF = 0,
    MOTOR_PULSE_STATE_ON
} MOTOR_PULSE_STATE_e;    

#ifdef SW_MOT_RF
    typedef enum{
        MOTOR_RF_PULSE_MON_STATE_IDLE,
        MOTOR_RF_PULSE_MON_STATE_START,
        MOTOR_RF_PULSE_MON_STATE_WAIT_END,
        MOTOR_RF_PULSE_MON_STATE_END,
        MOTOR_RF_PULSE_MON_STATE_PULSE_OFF,
        MOTOR_RF_PULSE_MON_STATE_PULSE_PAUSE
    } MOTOR_RF_PULSE_MON_STATE_e;
#endif

#define MOT_PARAM_FREQ_DEF      10      //Default motor PWM frequency
#define MOT_PARAM_POWER_FACTOR  100     //Power (duty cycle) in %
#define MOT_PARAM_DUTY_CYCLE    30      //%
#define MOT_PARAM_CYCLE_FACTOR  100     //100mS units
#define MOT_PARAM_POWER_STEP_MAX     10
#define MOT_PARAM_POWER_STEP_MIN     1
#define MOT_PARAM_POWER_STEP_NUM ((MOT_PARAM_POWER_STEP_MAX-MOT_PARAM_POWER_STEP_MIN)+1)

//#ifdef SW_TICK_100US
//    #define MOT_CUR_SAMPLE_TIME 30
//#else
//    #define MOT_CUR_SAMPLE_TIME 3
//#endif
#define MOT_CUR_SAMPLE_PULSE_START  2       //mS after pulse start, to measure current
#define MOT_CUR_SAMPLE_PULSE_END    2       //mS before pulse end, to measure current
#define MOT_PULSE_PAUSE_TIME        1000    //mS, pulse pause time

#endif
//[] END OF FILE
