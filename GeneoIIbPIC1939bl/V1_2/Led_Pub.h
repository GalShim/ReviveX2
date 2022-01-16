/* ========================================
 *
 * Public definitions for Led.c
 *
 * ========================================
*/

#ifndef _LED_PUB_H_
#define _LED_PUB_H_

#define LED_NUM_OF_RGBLEDS     3

//Green LED states
typedef enum{
    LED_GREEN_MODE_OFF = 0,   //Off
    LED_GREEN_MODE_CONT,      //Continuous Green
    LED_GREEN_MODE_PULSE,     //Pulses Green
    LED_BLUE_MODE_OFF,        //Off
    LED_BLUE_MODE_CONT,       //Continuous Blue
    LED_BLUE_MODE_PULSE,       //Pulses Blue
    //LED_RED_MODE_OFF,         //Off
    LED_RED_MODE_CONT,        //Continuous Red
    LED_ALL_MODE_CONT,        //Continuous All LEDs
    LED_GREENnBLUE_MODE_CONT, //Continuous GREENnBLUE LEDs
    LED_BLUEnRED_MODE_CONT,   //Continuous BLUEnRED LEDs
    LED_REDnGREEN_MODE_CONT   //Continuous REDnGREEN LEDs
    //LED_RED_MODE_PULSE        //Pulses Red
} LED_GREEN_MODE_e;    

//RGB LED states
typedef enum{
    LED_RGB_MODE_OFF = 0,   //Off
    LED_RGB_MODE_CONT,      //Continuous
    LED_RGB_MODE_PULSE,     //Pulses
    LED_RGB_MODE_PULSEnOFF, //Pulses and OFF
    LED_RGB_MODE_PULSEnCONT //Pulses and CONT
} LED_RGB_MODE_e;
#define LED_ORANGE_MODE_OFF     LED_RGB_MODE_OFF
#define LED_ORANGE_MODE_CONT    LED_RGB_MODE_CONT
#define LED_ORANGE_MODE_PULSE   LED_RGB_MODE_PULSE

//RGB LEDs
typedef enum{
    LED_RGB_ID_R = 0,
    LED_RGB_ID_G,
    LED_RGB_ID_B,
    LED_RGB_ID_ALL
} LED_RGB_ID_e;
#define LED_RGB_ID_ORANGE LED_RGB_ID_R

//RGB LED parameters
enum{
    LED_EE_PARAM_RGB_MODE = 0,
    LED_EE_PARAM_RGB_COLOR,
    LED_EE_PARAM_RGB_POWER,
    LED_EE_PARAM_RGB_ON,
    LED_EE_PARAM_RGB_OFF,
    LED_EE_PARAM_num
};    

typedef enum{
	LED_CMD_SET_STATE_AUTO = '1',
	LED_CMD_SET_STATE_OFF,
	LED_CMD_SET_STATE_ON_GREEN,
	LED_CMD_SET_STATE_BLINK_GREEN,
	LED_CMD_SET_STATE_ON_BLUE,
	LED_CMD_SET_STATE_BLINK_BLUE,
	LED_CMD_SET_STATE_ON_ORANGE,
	LED_CMD_SET_STATE_BLINK_ORANGE
}LED_CMD_SET_STATE_e;

extern void Led_TaskInit (void);
extern void Led_TaskMain (void);
extern void Led_TaskTick100mS (void);
extern void Led_ActRgb (uint8 Act);
extern void Led_ActGreen (LED_GREEN_MODE_e Mode, uint8 TotalTime, uint8 PwmTime);
extern void Led_CheckRgb (LED_RGB_ID_e LedId);
extern void Led_ConfigRgbled (void);
extern void Led_UnConfigRgbled (void);
#ifdef SW_LED_RGB_FIXED
    extern void Led_ActRgbled (LED_RGB_ID_e LedId, uint8 Action);
    extern void Led_ActOrange (LED_RGB_MODE_e Mode, uint8 TotalTime, uint8 PwmTime);
    extern void LED_Test_ActBlue(void);
    extern void LED_Test_ActGreen(void);
#endif
extern void Led_Cmd_SetState (LED_CMD_SET_STATE_e State);

#endif
//[] END OF FILE
