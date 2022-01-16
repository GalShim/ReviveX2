/* ========================================
 *
 * Public definitions for Comm.c
 *
 * ========================================
*/

#ifndef _COMM_PUB_H_
#define _COMM_PUB_H_

#include "..\..\GeneoIIbPIC1939bl\V1_2\Comm_Codes_Pub.h"
#include "..\..\GeneoIIbPIC1939bl\V1_2\App_Codes_Pub.h"



extern void Comm_TaskInit (void);
extern void Comm_TaskMain (void);
extern void Comm_TaskTick1mS (void);
#ifdef SW_UC_PIC18F
  volatile extern void Comm_RxInterrupt (void);
  volatile extern void Comm_TxInterrupt (void);
#else
  extern void Comm_RxInterrupt (void);
  extern void Comm_TxInterrupt (void);
#endif
extern void Comm_SetEvent (APP_EVENT_CODE_e Event, uint8 State);
#ifdef SW_COMM_LED_ON_EVENT
  extern uint8 Comm_GetEventState (uint8 EventCode);
#endif






#endif
//[] END OF FILE
