/* ===================================================================================================================
File Name:   Revive_Vsup_Loc.h
Description: Local definitions for Revive Vsupply
====================================================================================================================*/
#ifndef __REVIVE_VSUP_H__
#define __REVIVE_VSUP_H__

//1mS units timer
typedef enum{
    REVIVE_VSUP_STATE_INIT = 0,
    REVIVE_VSUP_STATE_START,
    REVIVE_VSUP_STATE_IDLE,
    REVIVE_VSUP_STATE_SEND,
    REVIVE_VSUP_STATE_DONE
} REVIVE_VSUP_STATE_e;    

#endif //__IO_LOC_H__
