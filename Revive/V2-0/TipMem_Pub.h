/* ========================================
 *
 * Public definitions for TipMem.c
 *
 * ========================================
*/

#ifndef _TIPMEM_PUB_H_
#define _TIPMEM_PUB_H_
#include "..\..\GeneoIIbPIC1939bl\V1_2\Comm_Pub.h"

#ifdef SW_TIP_TIMEOUT
    #ifdef SW_PLL_ON
        #define  TMR3H_BYTE 0x63    //32mS
        #define  TMR3L_BYTE 0xc0
    #else
        #define  TMR3H_BYTE 0x63    //To check
        #define  TMR3L_BYTE 0xc0
    #endif
#endif

extern void TipMem_TaskTick1mS (void);
extern void TipMem_TaskTick100mS (void);
extern void TipMem_TaskInit (void);
extern void TipMem_TaskMain (void);
extern void TipMem_IncPulses (void);
extern uint16 TipMem_GetPulses (void);
extern uint8 TipMem_GetId (void);
extern uint8 TipMem_GetStatus (void);




#endif
//[] END OF FILE
