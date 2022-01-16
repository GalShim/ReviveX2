/* ========================================
 *
 * Public definitions for Adc.c
 *
 * ========================================
*/

#ifndef _EEPROM_PUB_H_
#define _EEPROM_PUB_H_
#include "..\..\GeneoIIbPIC1939bl\V1_2\Comm_Pub.h"




typedef enum{
    EEPROM_PARAM_TASK_MOTOR = 0,
    EEPROM_PARAM_TASK_LED
} EEPROM_PARAM_TASK_e;    

extern uint16 Eeprom_ReadParam(EEPROM_PARAM_TASK_e Task, uint8 Param);
extern void Eeprom_WriteParam(EEPROM_PARAM_TASK_e Task, uint8 Param, uint16 Data);
extern void Eeprom_StoreParam (COMM_TEXT_CODE_e Tag, COMM_TEXT_CODE_e Attrib, uint8* Value, uint8 Len);
extern void Eeprom_TaskInit (void);


#endif
//[] END OF FILE
