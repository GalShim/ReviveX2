/* ========================================
 *
 * Public definitions for App.c
 *
 * ========================================
*/

#ifndef _APP_PUB_H_
#define _APP_PUB_H_

#include "..\..\GeneoIIbPIC1939bl\V1_2\Comm_Codes_Pub.h"
#include "..\..\GeneoIIbPIC1939bl\V1_2\App_Codes_Pub.h"

//Devices
typedef enum{
    #ifdef SW_MOT_X
        APP_DEVICE_MOTX = 0,
    #endif
    #ifdef SW_MOT_Y
        APP_DEVICE_MOTY,
    #endif
    #ifdef SW_LED_RGB_VAR
        APP_DEVICE_RGBLED,
    #endif
    APP_DEVICE_num
} APP_DEVICE_e;    

#ifdef SW_PATCH_SYNC_OVERV
    extern void App_Exec_OvervBit (void);
#endif
extern COMM_ERROR_e App_CommCmnd (COMM_TEXT_CODE_e Cmnd);
extern void App_GetInfo (COMM_TEXT_CODE_e AttribCode, uint8* AttribAddr, uint8* AttribLen, APP_EVENT_CODE_e EventCode);
extern void App_CommConfirm (COMM_TEXT_CODE_e Confirm,  APP_EVENT_CODE_e EventCode);
extern void App_TaskInit (void);
extern void App_TaskMain (void);
extern void App_TaskTick1mS (void);
extern void App_SetEvent (APP_EVENT_CODE_e EventCode);
#ifdef SW_PROD_ReviveRf
 extern void App_PulseDone (void);
 extern uint8 App_RfIsOn (void);
#endif
#endif
//[] END OF FILE
