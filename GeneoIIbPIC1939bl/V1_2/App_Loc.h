/* ========================================
 *
 * Local definitions for App.c
 *
 * ========================================
*/

#ifndef _APP_LOC_H_
#define _APP_LOC_H_

//states of Application task
typedef enum{
    APP_STATE_INIT = 0,
    APP_STATE_INIT1,
    APP_STATE_BIT,
    APP_STATE_POWER_UP_TEST,
    APP_STATE_POWER_UP_TEST_RF1,
    APP_STATE_POWER_UP_TEST_RF2,
    APP_STATE_START,
    APP_STATE_PROD_TEST_RED,
    APP_STATE_PROD_TEST_GREEN,
    APP_STATE_PROD_TEST_BLUE,
    APP_STATE_PROD_TEST_ORANGE,
    APP_STATE_PROD_TEST_NO_LED,
    APP_STATE_PROD_TEST_THERM_STRT,
    APP_STATE_PROD_TEST_THERM_ERR,
    APP_STATE_PROD_TEST_5V_STRT,
    APP_STATE_PROD_TEST_5V_ERR,
    APP_STATE_PROD_TEST_12V_STRT,
    APP_STATE_PROD_TEST_12V_ERR,
    APP_STATE_PROD_TEST_RF,
    APP_STATE_PROD_TEST_RF_CHECK,
    APP_STATE_PROD_TEST_RF_WAIT,
    APP_STATE_PROD_TEST_RF_FAILa,
    APP_STATE_PROD_TEST_RF_FAILb,
    APP_STATE_PROD_TEST_RF_FAILend,
    APP_STATE_PROD_TEST_KEYnRF,
    APP_STATE_PROD_TEST_KEYnRF_TST,
    APP_STATE_PROD_TEST_KEYnRF_WT_PULSE,
    APP_STATE_PROD_TEST_KEYnRF_WT,
    APP_STATE_PROD_TEST_KEYnRF_ERR,
    APP_STATE_SETUP,
    APP_STATE_IDLE_START,
    APP_STATE_IDLE_STAY,
    APP_STATE_LOADED_START,
    APP_STATE_LOADED_STAY,
    APP_STATE_ON_START,
    APP_STATE_ON_STAY,
    APP_STATE_ACTIVE_START,
    APP_STATE_ACTIVE_STAY,
    APP_STATE_PAUSE_START,
    APP_STATE_PAUSE_STAY,
    #ifdef SW_PROD_ReviveRf
     APP_STATE_ONE_PULSE_STEP1,
     APP_STATE_ONE_PULSE_STEP2,
    #endif
    #ifdef SW_DEBUG_TRAP_EVENT
     APP_STATE_BLOCK,
     APP_STATE_BLOCK1,
    #endif
    APP_STATE_DEACTIVATE
} APP_STATE_e;    

//Event states
typedef enum{
    APP_EVENT_STATE_IDLE = 0,
    APP_EVENT_STATE_SENT,
    APP_EVENT_STATE_CONFIRMED
} APP_EVENT_STATE_e;    

//RfFault states
typedef enum{
    APP_RF_FAULT_IDLE = 0,
    APP_RF_FAULT_OK,
    APP_RF_FAULT_FAULT
} APP_RF_FAULT_e;

//Rf test states
typedef enum{
    APP_RF_TEST_STATE_INIT = 0,
    APP_RF_TEST_STATE_M1,
    APP_RF_TEST_STATE_PRE_M2,
    APP_RF_TEST_STATE_M2,
    APP_RF_TEST_STATE_PRE_M3,
    APP_RF_TEST_STATE_M3,
    APP_RF_TEST_STATE_CALC
} APP_RF_TEST_STATE_e;

typedef enum{
    APP_RF_NP_TEST_STATE_INIT = 0,
    APP_RF_NP_TEST_STATE_WAIT_TEST1,
    APP_RF_NP_TEST_STATE_WAIT_DELAY,
    APP_RF_NP_TEST_STATE_WAIT_TEST2,
    APP_RF_NP_TEST_STATE_DONE
} APP_RF_NP_TEST_STATE_e;    

#define APP_INIT_TIME           500     //Time for input voltage to be stable
#define APP_PROD_JUMPER_TIME    10
#define APP_PROD_TEST_TIME      1000 //500
#define APP_PROD_TEST_DELAY_TIME 14 //7 //(5*APP_RF_PROD_PULSE_TIME) //100    //Used for Rvive RF, for repetitive one pulse
#ifdef SW_HW_REVIVE
  #define APP_PROD_TEST_TIME_LONG 1000
#else
  #define APP_PROD_TEST_TIME_LONG 2000
#endif
#define APP_TEST_TECH_TEMP_TIME 4000
#define APP_TEST_PRESS_KEY_TIME 6000
#define APP_PROD_RVIVE_V        31 //40
#define APP_TEMP_FAULT_TIME     3000   //1mS time to generate temperature fault
#define APP_RF_SAFETY_TEST_TIMER 100   //RF safety test timer
#define APP_RF_ACTIVE_FAULT_NUM 3      //Only a sequence of faults generates an event

#define APP_TEMP_MAX_VAL    500         //0.1C units
#define APP_TEMP_MIN_VAL    50          //0.1C units
#define APP_TEMP_PROD_MAX_VAL    255    //0.1C units
#define APP_TEMP_PROD_MIN_VAL    245    //0.1C units
#define APP_5V_PROD_MAX_VAL    53       //0.1V units
#define APP_5V_PROD_MIN_VAL    47       //0.1V units
#ifdef SW_HW_SUP_24V
 #define APP_12V_PROD_MAX_VAL   260      //0.1V units
 #define APP_12V_PROD_MIN_VAL   220      //0.1V units
 #define APP_12V_MAX_VAL        260      //0.1V units
#else
 #define APP_12V_PROD_MAX_VAL   123      //0.1V units
 #define APP_12V_PROD_MIN_VAL   115      //0.1V units
 #define APP_12V_MAX_VAL        140      //0.1V units
#endif
#ifdef SW_PROD_ReviveRf
 #define APP_I_PROD_MIN_VAL      170 //110 //20 //110 //140 //170 //200 //300 //500      //1mA units
#else
 #define APP_I_PROD_MIN_VAL      170 //200 //300 //500      //1mA units
#endif
#define APP_CUR_MAX_LEVEL   1800        //1mA units (6W-1500, 9W-1800)
#define APP_PULSE_CUR_MAX_LEVEL 1800 //1000    //1mA units (6W-1500, 9W-1800)
#define APP_PULSE_CUR_OVER_COUNT 10     //Number of over current, before pause

#define APP_BIT_12V_MAX     APP_12V_MAX_VAL         //0.1V units
#define APP_BIT_I_SUP_MAX   1500        //1mA units
#define APP_BIT_I_RF_MAX    APP_BIT_I_SUP_MAX        //1mA units. Has to be the same, as the two tests are done sequentially, with no pause between.
#define APP_BIT_TEMP_MIN    50 //40          //0.1C units
#define APP_BIT_TEMP_MAX    500 //510         //0.1C units

//#define APP_PULSE_TIME      250         //Wait till pulse end
#define APP_PULSE_WAIT     1000         //Wait after pulse (time for blue LED)

//Timing for RF test (1mS units)                               
#ifdef SW_PROD_Rf                   //50Hz
    #define APP_RF_TEST_M2_PRE      0
    #define APP_RF_TEST_M2_TEST     1
    #define APP_RF_TEST_M3_PRE      6
    #define APP_RF_TEST_M3_TEST     7
#endif
#ifdef SW_PROD_Us                   //100Hz
    #define APP_RF_TEST_M2_PRE      2 //0
    #define APP_RF_TEST_M2_TEST     3 //1 //0
    #define APP_RF_TEST_M3_PRE      3 //1 //2
    #define APP_RF_TEST_M3_TEST     4 //2
#endif
#ifdef SW_PROD_ReviveRf             //100Hz
    #define APP_RF_TEST_M2_PRE      2 //0
    #define APP_RF_TEST_M2_TEST     3 //1 //0
    #define APP_RF_TEST_M3_PRE      3 //1 //2
    #define APP_RF_TEST_M3_TEST     4 //2
    #define APP_RF_NP_TEST_TIMER    100 //1mS units
    //#define APP_RF_NP_FAIL_VAL      100 //?? Original definition was voltage.
    #define APP_RF_NP_FAIL_I_VAL    500 //1mA units. Should test current.
    #define APP_RF_PROD_PULSE_TIME  6 //3 //20
    //#define APP_REVIVE_TEST_SAFETY_CONST  //Test safety constantly
#endif
#ifdef SW_PROD_ReviveUs             //100Hz
    #define APP_RF_TEST_M2_PRE      2 //0
    #define APP_RF_TEST_M2_TEST     3 //1 //0
    #define APP_RF_TEST_M3_PRE      3 //1 //2
    #define APP_RF_TEST_M3_TEST     4 //2
#endif

#endif
//[] END OF FILE
