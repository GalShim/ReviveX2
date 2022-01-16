/* ===================================================================================================================
File Name:   Revive_Vsup_Pub.h
Description: Public definitions for Revive Vsupply
====================================================================================================================*/
#ifndef __REVIVE_VSUP_PUB_H__
#define __REVIVE_VSUP_PUB_H__


typedef enum{
    REVIVE_VSUP_CMND_OFF = 0,
    REVIVE_VSUP_CMND_ON
} REVIVE_VSUP_CMND_e;    

//Functions
extern void Revive_Vsup_TaskInit (void);
extern void Revive_Vsup_TaskMain (void);
extern void Revive_Vsup_1mS_tick(void);
extern void Revive_Vsup_Cmnd (REVIVE_VSUP_CMND_e Cmnd, uint8 Val);


#endif //__IO_PUB_H__
