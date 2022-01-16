/* ========================================
 *
 * Local definitions for Led.c
 *
 * ========================================
*/

#ifndef _LED_LOC_H_
#define _LED_LOC_H_

//Task states
typedef enum{
    LED_TASK_STATE_INIT = 0,
    LED_TASK_STATE_START
} LED_TASK_STATE_e;    

//Pulse/cont states
typedef enum{
    LED_PULSE_STATE_OFF = 0,
    LED_PULSE_STATE_ON,
    LED_PULSE_STATE_PULSE
} LED_PULSE_STATE_e;

//Color of Green LED
#ifdef SW_LED_RGB_FIXED
    typedef enum{
        LED_GREEN_COLOR_GREEN = 0,
        LED_GREEN_COLOR_BLUE,
        LED_GREEN_COLOR_RED,
        LED_GREEN_COLOR_ALL,
        LED_GREEN_COLOR_GREENnBLUE,
        LED_GREEN_COLOR_BLUEnRED,
        LED_GREEN_COLOR_REDnGREEN
    } LED_GREEN_COLOR_e;    
#endif


#endif
//[] END OF FILE
