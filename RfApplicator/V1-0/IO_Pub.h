/* ===================================================================================================================
File Name:   Keys_Pub.h
Description: Public definitions for Keys handler
====================================================================================================================*/
#ifndef __IO_PUB_H__
#define __IO_PUB_H__


typedef enum{
    IO_BUZ_CMND_OFF = 0,
    IO_BUZ_CMND_ON,
    IO_BUZ_CMND_BEEP,
    IO_BUZ_CMND_BEEP_LONG
} IO_BUZ_CMND_e;    

//Functions
extern void IO_TaskInit (void);
extern void IO_TaskMain (void);
extern void IO_1mS_tick(void);
extern void IO_BuzCmnd (IO_BUZ_CMND_e Cmnd);
extern uint8 IO_GetProdJumper (void);


#endif //__IO_PUB_H__
