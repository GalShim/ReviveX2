/* ========================================
 *
 * Public definitions for Adc.c
 *
 * ========================================
*/

#ifndef _ADC_PUB_H_
#define _ADC_PUB_H_


#ifdef SW_UC_PIC16F
    typedef enum{
        ADC_IN_MOT_X_I = 0,
        ADC_IN_MOT_Y_I,
        ADC_IN_RGB_LED_I,
        ADC_IN_MOT_X_V,
        ADC_IN_MOT_Y_V,
        ADC_IN_RGB_LED_V,
        ADC_IN_12V_V,
        ADC_IN_5V_V,
        ADC_IN_0V_V,
        ADC_IN_DAC_SAMP_V,
        ADC_IN_num
    } ADC_IN_e;
    #define ADC_IN_ChannelMax ADC_IN_DAC_SAMP_V
#endif
#ifdef SW_UC_PIC18F
    //#ifdef SW_PROD_ReviveRf
    #ifdef SW_HW_REVIVE
        typedef enum{
            ADC_IN_12V_V = 0,
            ADC_IN_5V_V,
            ADC_IN_SupCur_I,
            ADC_IN_OUT_V,
            ADC_IN_Temp_C,
            ADC_IN_num
        } ADC_IN_e;
        #define ADC_IN_ChannelMax ADC_IN_OUT_V
    #else
        typedef enum{
            ADC_IN_12V_V = 0,
            ADC_IN_5V_V,
            ADC_IN_SupCur_I,
            ADC_IN_Temp_C,
            ADC_IN_num
        } ADC_IN_e;
        #define ADC_IN_ChannelMax ADC_IN_SupCur_I
    #endif
#endif

//ADC inputs' error levels
#define ADC_MOT_X_CURRENT_MIN       30
#ifdef SW_MOT_BLDC
    #define ADC_MOT_X_CURRENT_MAX       1300
#else
    #define ADC_MOT_X_CURRENT_MAX       600
#endif
#define ADC_MOT_Y_CURRENT_MIN       30
#define ADC_MOT_Y_CURRENT_MAX       600
#define ADC_RGBLED_R_CURRENT_MIN    1 //3
#define ADC_RGBLED_R_CURRENT_MAX    60
#define ADC_RGBLED_G_CURRENT_MIN    1 //3
#define ADC_RGBLED_G_CURRENT_MAX    60
#define ADC_RGBLED_B_CURRENT_MIN    1 //3
#define ADC_RGBLED_B_CURRENT_MAX    60

#ifdef SW_TEMP_ADD_RES_RATIO
    extern uint16 Adc_GetResRatio(void);
#endif
extern uint8 Adc_CheckReady (void);
extern void Adc_SetOneChannel (uint8 Cmnd, ADC_IN_e Channel);
extern uint16 Adc_GetVal(ADC_IN_e Channel);
extern uint16 Adc_SampleRequest (ADC_IN_e Channel, uint8 UseAve);
extern uint16 Adc_GetInput(ADC_IN_e Input);
extern void Adc_TaskInit (void);
extern void Adc_TaskMain (void);
extern void Adc_TaskTick1mS (void);

#endif
//[] END OF FILE
