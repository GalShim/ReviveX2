/* ========================================
 *
 * Public codes definitions for App.c
 *
 * ========================================
*/

#ifndef _APP_CODES_PUB_H_
#define _APP_CODES_PUB_H_

//Events' codes
typedef enum{
    APP_EVENT_buttons = 0,
    APP_EVENT_FAULT_motorX,
    APP_EVENT_FAULT_motorY,
    APP_EVENT_FAULT_rgbled,
    APP_EVENT_FAULT_OverVolt,
    APP_EVENT_FAULT_Temperature,
    APP_EVENT_FAULT_OverCur,
    APP_EVENT_FAULT_Rf,
    APP_EVENT_FAULT_Tip,
    APP_EVENT_TECH_INFO_motorX,
    APP_EVENT_TECH_INFO_motorY,
    APP_EVENT_TECH_INFO_rgbled,
    APP_EVENT_TECH_INFO_applicator,
    APP_EVENT_TECH_INFO_12V,
    APP_EVENT_TECH_INFO_5V,
    APP_EVENT_TECH_INFO_Temp,
    APP_EVENT_TECH_INFO_Cur,
    APP_EVENT_TECH_INFO_TipCount,
    APP_EVENT_TECH_INFO_TipId,
    APP_EVENT_PULSEDONE_done,
    APP_EVENT_TIPINFO_info,
    APP_EVENT_BOOT_up,
    APP_EVENT_num,
    APP_EVENT_TECH_none = 0xff
} APP_EVENT_CODE_e;

#endif
//[] END OF FILE
