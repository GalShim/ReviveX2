/* ========================================
 *
 * Local definitions for Adc.c
 *
 * ========================================
*/

#ifndef _ADC_LOC_H_
#define _ADC_LOC_H_

typedef enum{
    ADC_TASK_STATE_INIT = 0,
    ADC_TASK_STATE_START,
    ADC_TASK_STATE_CHANNEL_SAMPLE,
    ADC_TASK_STATE_THERM_LOAD_CAP1,
    ADC_TASK_STATE_THERM_DISC_CAP1,
    ADC_TASK_STATE_THERM_LOAD_CAP2,
    ADC_TASK_STATE_THERM_DISC_CAP2,
    ADC_TASK_STATE_THERM_CALC_TEMP,
    ADC_TASK_STATE_THERM_END,
    ADC_TASK_STATE_WAIT_SAMPLE
} ADC_TASK_STATE_e;    

#define ADC_STARTUP_DELAY       0xff
#define ADC_SAMPLE_TIME         100     //1mS units
#define ADC_LOAD_CAP_TIME       100 //10
#define ADC_DISC_CAP_MAX_TIME   2000 //10      //Max discharge time
#ifdef SW_UC_PIC18F
    #define ADC_SAMPLES_NUM         3       //Number of samples to calc average
#else
    #define ADC_SAMPLES_NUM         2       //Number of samples to calc average
#endif
#define ADC_C1IF_LOOP_LIMIT     0xffff      //16MHz clock:
                                            //  CAP1 discharge - count ~61000
                                            //  CAP2 discharge - count ~180
#define ADC_CONVERT_nDONE_LOOP_LIMIT 0xffff //16MHz clock: count ~6
                                            //16MHz clock: 0xffff == ~1sec

#endif
//[] END OF FILE
