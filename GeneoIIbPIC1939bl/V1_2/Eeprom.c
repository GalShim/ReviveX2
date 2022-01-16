/* ========================================
 *
 * Handle ADC
 *
 * ========================================
*/

//#include <xc.h>
#include "General.h"
#ifdef SW_UC_PIC16F
    #include <htc.h>
#endif
#ifdef SW_UC_PIC18F
    #ifdef SW_UC_DEVBOARD
        #include <p18f46k22.h>
    #else
        #ifdef SW_UC_PIC18F45K22
            #include <p18f45k22.h>
        #else
            #include <p18f25k22.h>
        #endif
    #endif
#endif
#include <stdlib.h>
#include <string.h>
#include "Types.h"
#include "Led_Pub.h"
#include "Motor_Pub.h"
#include "Comm_Pub.h"
#include "Eeprom_Pub.h"
#include "Eeprom_Loc.h"

#define EEPROM_PARAMCODE_CONVERT_LEN (LED_EE_PARAM_num+MOTOR_EE_PARAM_num)
    const struct{
        uint8   Tag;
        uint8   Attrib;
        uint8   EEtaskcode;
        uint8   EEparamcode;
    } Eprom_ParamCodeConvert [EEPROM_PARAMCODE_CONVERT_LEN] = {
        {COMM_TEXT_Tag_rgbled, COMM_TEXT_Attrib_mode,           EEPROM_PARAM_TASK_LED,      LED_EE_PARAM_RGB_MODE},
        {COMM_TEXT_Tag_rgbled, COMM_TEXT_Attrib_color,          EEPROM_PARAM_TASK_LED,      LED_EE_PARAM_RGB_COLOR},
        {COMM_TEXT_Tag_rgbled, COMM_TEXT_Attrib_power_cmnd,     EEPROM_PARAM_TASK_LED,      LED_EE_PARAM_RGB_POWER},
        {COMM_TEXT_Tag_rgbled, COMM_TEXT_Attrib_on,             EEPROM_PARAM_TASK_LED,      LED_EE_PARAM_RGB_ON},
        {COMM_TEXT_Tag_rgbled, COMM_TEXT_Attrib_off,            EEPROM_PARAM_TASK_LED,      LED_EE_PARAM_RGB_OFF},
        {COMM_TEXT_Tag_motor1, COMM_TEXT_Attrib_mode,           EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_MOTOR1_MODE},
        {COMM_TEXT_Tag_motor1, COMM_TEXT_Attrib_power_cmnd,     EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_MOTOR1_POWER},
        {COMM_TEXT_Tag_motor1, COMM_TEXT_Attrib_on,             EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_MOTOR1_ON},
        {COMM_TEXT_Tag_motor1, COMM_TEXT_Attrib_off,            EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_MOTOR1_OFF},
        {COMM_TEXT_Tag_motor2, COMM_TEXT_Attrib_mode,           EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_MOTOR2_MODE},
        {COMM_TEXT_Tag_motor2, COMM_TEXT_Attrib_power_cmnd,     EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_MOTOR2_POWER},
        {COMM_TEXT_Tag_motor2, COMM_TEXT_Attrib_on,             EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_MOTOR2_ON},
        {COMM_TEXT_Tag_motor2, COMM_TEXT_Attrib_off,            EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_MOTOR2_OFF},
        #ifdef SW_UC_PIC18F
            {COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_mode,           EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_MODE},
            {COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_intensity,      EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_INTENSITY},
            {COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_on,             EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_ON},
            {COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_off,            EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_OFF},
            {COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_lowlevel,       EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_LOWLEVEL},
            {COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_highlevel,      EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_HIGHLEVEL},
            {COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_buzzerstatus,   EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_BUZSTATUS},
            //{COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_p1power,        EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P1POWER},
            //{COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_p1intensity,    EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P1INTENSITY},
            //{COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_delay,          EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_DELAY},
            //{COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_p2power,        EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P2POWER},
            //{COMM_TEXT_Tag_rfout,  COMM_TEXT_Attrib_p2intensity,    EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P2INTENSITY},
        #endif
        #ifdef SW_PROD_ReviveRf
            {COMM_TEXT_Tag_vdtip,  COMM_TEXT_Tag_vdtip,             EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_DOUBLEPULSE},
            {COMM_TEXT_Tag_vdtip,  COMM_TEXT_Attrib_p1power,        EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P1POWER},
            {COMM_TEXT_Tag_vdtip,  COMM_TEXT_Attrib_p1intensity,    EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P1INTENSITY},
            {COMM_TEXT_Tag_vdtip,  COMM_TEXT_Attrib_delay,          EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_DELAY},
            {COMM_TEXT_Tag_vdtip,  COMM_TEXT_Attrib_p2power,        EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P2POWER},
            {COMM_TEXT_Tag_vdtip,  COMM_TEXT_Attrib_p2intensity,    EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P2INTENSITY},
            //{COMM_TEXT_Tag_tftip,  COMM_TEXT_Attrib_p1power,        EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P1POWERb},
            {COMM_TEXT_Tag_tftip,  COMM_TEXT_Attrib_p1power,        EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P1POWER},
            //{COMM_TEXT_Tag_tftip,  COMM_TEXT_Attrib_p1intensity,    EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P1INTENSITYb}
            {COMM_TEXT_Tag_tftip,  COMM_TEXT_Attrib_p1intensity,    EEPROM_PARAM_TASK_MOTOR,    MOTOR_EE_PARAM_RFOUT_P1INTENSITY}
        #endif
    };
    
#ifndef SW_EEPROM_USE_NON_VOLATILE
    #ifdef SW_UC_PIC18F
     #pragma udata bigdata_ee
      uint8 Eeprom_ParamBuf[EEPROM_PARAMCODE_CONVERT_LEN*2];
     #pragma udata
    #endif
    #ifdef SW_UC_PIC16F
     volatile uint8 Eeprom_ParamBuf[EEPROM_PARAMCODE_CONVERT_LEN*2];
    #endif
#endif

#ifdef SW_EEPROM_USE_NON_VOLATILE
    void eeprom_write1(unsigned char addr, unsigned char value)
    {
    	while (WR) continue;
    	EEADRL = addr;
    	EEDATL = value;
    	WREN = 1;
    	GIE = 0;
    	EECON2 = 0x55;
    	EECON2 = 0xAA;
    	WR = 1;
    	GIE = 1;
    	WREN = 0;
    	while (WR) continue;
    }
#endif




//--------------------------------------------
uint8 Eeprom_ParamOffset (EEPROM_PARAM_TASK_e Task, uint8 Param)
{
  uint8 Offset = 0;
    switch (Task)
    {
      case EEPROM_PARAM_TASK_MOTOR:
        Offset = 0;
        break;
      case EEPROM_PARAM_TASK_LED:
        Offset = MOTOR_EE_PARAM_num;
        break;
    }    
    Offset += Param;
    Offset *= 2;                        //16 bits parameter to store
    return(Offset);
}   
//--------------------------------------------

/******************************************************************************
*  Function name: Eeprom_ReadParam
*  Main tasks:
*    Read parameter from EEPROM
*  Parameters:
*    Task code
*    Parameter code
*  Returned values:
*    Parameter value
*  Remarks:
*    None
******************************************************************************/
uint16 Eeprom_ReadParam(EEPROM_PARAM_TASK_e Task, uint8 Param)
{
  uint8 Addr = 0;
  uint16 Data;
    Addr += Eeprom_ParamOffset(Task, Param);
    #ifdef SW_EEPROM_USE_NON_VOLATILE
        Data = eeprom_read (Addr);
    #else
        Data = Eeprom_ParamBuf[Addr];
    #endif
    Addr++;
    Data = Data << 8;
    
    #ifdef SW_EEPROM_USE_NON_VOLATILE           //Use non-volatile memory?
        Data += eeprom_read (Addr);
    #else
        Data += Eeprom_ParamBuf[Addr];
    #endif
    
    #ifdef SW_TEMPERATURE_FIX_LIMITS            //Fixed temperature limits?
        if (Param == MOTOR_EE_PARAM_RFOUT_LOWLEVEL)
            {Data = SW_TEMPERATURE_FIX_LOW;}
        if (Param == MOTOR_EE_PARAM_RFOUT_HIGHLEVEL)
            {Data = SW_TEMPERATURE_FIX_HIGH;}
    #endif
    
    return (Data);
}    

/******************************************************************************
*  Function name: Eeprom_WriteParam
*  Main tasks:
*    Write parameter to EEPROM
*  Parameters:
*    Task code
*    Parameter code
*    Parameter value
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void Eeprom_WriteParam(EEPROM_PARAM_TASK_e Task, uint8 Param, uint16 Data)
{
  uint16 Addr = 0;
    Addr += Eeprom_ParamOffset(Task, Param);
    #ifdef SW_EEPROM_USE_NON_VOLATILE
        eeprom_write ((uint8)Addr, (uint8)(Data >> 8));
    #else
        Eeprom_ParamBuf[(uint8)Addr] = (uint8)(Data >> 8);
    #endif
    Addr++;
    #ifdef SW_EEPROM_USE_NON_VOLATILE
        eeprom_write ((uint8)Addr, (uint8)Data);
    #else
        Eeprom_ParamBuf[(uint8)Addr] = (uint8)Data;
    #endif
}    

/******************************************************************************
*  Function name: Eeprom_StoreParam
*  Main tasks:
*    Receive a parameter from communication to store in EEPROM
*  Parameters:
*    Tag code
*    Attrib code
*    Parameter value address
*    Parameter value length
*  Returned values:
*    None
*  Remarks:
*    Convert parameter from ASCII to value
******************************************************************************/
void Eeprom_StoreParam (COMM_TEXT_CODE_e Tag, COMM_TEXT_CODE_e Attrib, uint8* Value, uint8 Len)
{
  uint8 EEtaskcode;
  uint8 EEparamcode;
  uint16 ParamVal = 0;
  uint8 i;
  uint16 Decimal = 1;
  uint16 TempVal;
  
    for (i=0; i<EEPROM_PARAMCODE_CONVERT_LEN; i++)  //Convert code
    {
        if ((Eprom_ParamCodeConvert[i].Tag == Tag) && (Eprom_ParamCodeConvert[i].Attrib == Attrib))
        {                                           //Parameter found - convert code
            EEtaskcode = Eprom_ParamCodeConvert[i].EEtaskcode;
            EEparamcode = Eprom_ParamCodeConvert[i].EEparamcode;
            i = EEPROM_PARAMCODE_CONVERT_LEN;
        }    
    }
    
    Value += (Len-1); 
    for (i=0; i<Len; i++)                           //Convert ASCII to number
    {
        TempVal = (*Value) & 0x0f;
        TempVal *= Decimal;
        ParamVal += TempVal;
        Value--;
        Decimal *= 10;
    }
    
    Eeprom_WriteParam(EEtaskcode, EEparamcode, ParamVal);
}    

/******************************************************************************
*  Function name: Eeprom_TaskInit
*  Main tasks:
*    Initialize EEPROM task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called on start up
******************************************************************************/
void Eeprom_TaskInit (void)
{
    #ifndef SW_EEPROM_USE_NON_VOLATILE
      uint8 i;
        for (i = 0; i < sizeof(Eeprom_ParamBuf); i++)
        {
           Eeprom_ParamBuf[i] = 0;
        }
    #endif
}
