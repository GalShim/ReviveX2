/* ========================================
 *
 * Tip memory
 *
 * ========================================
*/

//#include <xc.h>
#include "..\..\GeneoIIbPIC1939bl\V1_2\General.h"
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
#include "..\..\GeneoIIbPIC1939bl\V1_2\Types.h"
#include "..\..\GeneoIIbPIC1939bl\V1_2\Led_Pub.h"
#include "..\..\GeneoIIbPIC1939bl\V1_2\Motor_Pub.h"
#include "..\..\GeneoIIbPIC1939bl\V1_2\Comm_Pub.h"
#include "i2c.h"
#include "..\..\GeneoIIbPIC1939bl\V1_2\App_Pub.h"
#include "..\..\GeneoIIbPIC1939bl\V1_2\Eeprom_Pub.h"
#include "TipMem_Pub.h"
#include "TipMem_Loc.h"


typedef enum{                                   //Task states
    TIPMEM_STATE_Init = 0,
    TIPMEM_STATE_Start,
    TIPMEM_STATE_Idle,
    TIPMEM_STATE_ReadStart,
    TIPMEM_STATE_ReadCheck,
    TIPMEM_STATE_ReadEnd,
    TIPMEM_STATE_WriteStart,

    TIPMEM_STATE_WriteEnd
} TIPMEM_STATE_e;

#define TIPMEM_DATA_LEN 8
#define TIPMEM_DATA_LOC_ID 4                    //Location of Tip ID
#define TIPMEM_DATA_LOC_PULSES_L 5              //Location of Tip Pulses Low
#define TIPMEM_DATA_LOC_PULSES_H 6              //Location of Tip Pulses High
#define TIPMEM_DATA_LOC_CHECKSUM 0              //Location of checksum
#define TIPMEM_DATA_LOC_CHECKSUM_FIRST 1        //Location of first byte to calc checksum
#define TIPMEM_DATA_LOC_CHECKSUM_LAST 6         //Location of last byte to calc checksum
#define TIPMEM_ADDR_BASE 0x00;                  //Base address of the data in the tip memory
#define TIPMEM_ADDR_BACK 0x08;                  //Backup address of the data in the tip memory
#define TIPMEM_EEPROM_DATA_ADDR 0               //Data address in the EEPROM
#define TIPMEM_EEPROM_BACKUP_ADDR (TIPMEM_EEPROM_DATA_ADDR+TIPMEM_DATA_LEN) //Backup data address in the EEPROM

#define TIPMEM_READ_TIP_TIMEOUT 10              //Periodic TIP read. 100mS units
#define TIPMEM_WRITE_TIMEOUT 7                  //Timeout after write, before accessing the memory again
#define TIPMEM_WRITE_TIP_RETRIES 3              //Number of retries to write to TIP
#define TIPMEM_READ_TIP_RETRIES 3               //Number of retries to write to TIP

struct{
    struct {
        uint8   ToWrite[TIPMEM_DATA_LEN];       //Written when a parameter is updated
        uint8   Image[TIPMEM_DATA_LEN];         //"Official" image of the Tip Memory
    } Data;
    struct{
        uint8   Buf[TIPMEM_DATA_LEN];           //Data
    } Comm;    
    uint8   State;                              //Task state
    uint8   TimerWrite;                         //Write timer (1mS tick)
    uint8   Timer100mS;                         //100mS tick timer
    uint8   WriteRetries;                       //Retries to write to TIP
    uint8   ReadRetries;                        //Retries to read from TIP
    union{
        uint8   Byte;
        struct{
            uint8   DataToUpdate:1;             //Data is ready (in ToWriteBuf) to update
            uint8   b1:1;                       //Backup should be updated
            uint8   ReadData:1;
            uint8   b3:1;
            uint8   b4:1;
            uint8   b5:1;
            uint8   CommError:1;                //IIC communication error
            uint8   TipOk:1;                    //Tip memory was read and it's OK
        } Flags;    
    } Status;    
} TipMem_Obj;

#ifdef SW_TIP_MEM

/******************************************************************************
*  Hardware functions
******************************************************************************/
//Config IIC
void TipMem_IIC_HwConfig (void)
{
    ANSELDbits.ANSD1 = 0;                       //Digital input D1 enabled
    ANSELDbits.ANSD0 = 0;                       //Digital input D0 enabled
    IPR3bits.SSP2IP = 0;                        //Serial port 2 interrupt priority low
    IPR3bits.BCL2IP = 0;                        //Bus collision interrupt priority low
    PIE3bits.BCL2IE = 0;                        //Bus collision interrupt disable (only one master)
    PIE3bits.SSP2IE = 0;                        //Serial port interrupt disable
    SSP2ADD = 0xff;                             //~58KHz
    TRISDbits.TRISD1 = 1;                       //Port confg as tri state input
    TRISDbits.TRISD0 = 1;                       //Port confg as tri state input
}
//Start IIC timeout
#ifdef SW_TIP_TIMEOUT
    void TipMem_IIC_Start_Timeout (void)
    {
        TMR3H = TMR3H_BYTE;
        TMR3L = TMR3L_BYTE;
    }    
#endif

/******************************************************************************
*  Internal functions
******************************************************************************/
//Read Pulse value
uint16 TipMem_ReadPulseVal (void)
{
    uint16 TempW = 0;
    TempW = (uint16)TipMem_Obj.Data.Image[TIPMEM_DATA_LOC_PULSES_H] * 0x0100;
    TempW += (uint16)TipMem_Obj.Data.Image[TIPMEM_DATA_LOC_PULSES_L];
    return (TempW);
}    

/******************************************************************************
*  Function name: TipMem_CalcChecksum
*  Main tasks:
*    Calc checksum of a buffer
*  Parameters:
*    Address of the buffer
*    Length
*  Returned values:
*    Checksum
*  Remarks:
*    None
******************************************************************************/
uint8 TipMem_CalcChecksum(uint8* Addr, uint8 Len)
{
  uint8 Checksum = 0x55;
  uint8 i = TIPMEM_DATA_LOC_CHECKSUM_FIRST;
    while (Len)
    {
        Checksum += *Addr;
        if (i == TIPMEM_DATA_LOC_ID)
        {                                                   //Type location
            if (*Addr > 1)
            {                                               //Type>1: Add 1 to checksum
                Checksum++;
            }    
        }    
        i++;
        Addr++;
        Len--;
    }    
    return(Checksum);
}    

/******************************************************************************
*  Function name: TipMem_GetStatus
*  Main tasks:
*    Get tip state
*  Parameters:
*    None
*  Returned values:
*    OK / Fault
*  Remarks:
*    None
******************************************************************************/
uint8 TipMem_GetStatus (void)
{
    return ((uint8)TipMem_Obj.Status.Flags.TipOk);
}

/******************************************************************************
*  Function name: TipMem_GetId
*  Main tasks:
*    Get tip ID
*  Parameters:
*    None
*  Returned values:
*    ID
*  Remarks:
*    None
******************************************************************************/
uint8 TipMem_GetId (void)
{
  uint8 TipID;
    if (TipMem_Obj.Status.Flags.TipOk)
    {                                           //Tip OK - return ID
        TipID = TipMem_Obj.Data.Image[TIPMEM_DATA_LOC_ID];
    }
    else
    {                                           //Tip not OK - return fault
        TipID = TIPMEM_TIP_ID_NA;
    }
    TipID |= 0x30;
    return(TipID);
}    

/******************************************************************************
*  Function name: TipMem_GetPulses
*  Main tasks:
*    Get tip pulses
*  Parameters:
*    None
*  Returned values:
*    Pulses
*  Remarks:
*    None
******************************************************************************/
uint16 TipMem_GetPulses (void)
{
    if (TipMem_Obj.Status.Flags.TipOk)
    {                                           //Tip OK - return pulses
        return (TipMem_ReadPulseVal());
    }
    else
    {                                           //Tip not OK - return 0
        return (0);
    }        
}

/******************************************************************************
*  Function name: TipMem_IncPulses
*  Main tasks:
*    Increment pulses
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void TipMem_IncPulses (void)
{
  uint16 TempW1 = TipMem_ReadPulseVal();      //Current pulses
    
    memcpy (&TipMem_Obj.Data.ToWrite[0], (const uint8*)&TipMem_Obj.Data.Image[0], TIPMEM_DATA_LEN);
    if (TempW1 < 0xffff)
    {
        TempW1++;                               //Increment
    }
    TipMem_Obj.Data.ToWrite[TIPMEM_DATA_LOC_PULSES_L] = (uint8)TempW1; //Update to buf
    TempW1 /= 0x100;
    TipMem_Obj.Data.ToWrite[TIPMEM_DATA_LOC_PULSES_H] = (uint8)TempW1;
    TipMem_Obj.Data.ToWrite[0] = TipMem_CalcChecksum(&TipMem_Obj.Data.ToWrite[1], (TIPMEM_DATA_LEN-2));
    TipMem_Obj.Status.Flags.DataToUpdate = 1;   //Flag to update Tip memory
    TipMem_Obj.WriteRetries = TIPMEM_WRITE_TIP_RETRIES;
}    

/******************************************************************************
*  Function name: TipMem_TaskMain
*  Main tasks:
*    Main TipMem task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called periodically
******************************************************************************/
void TipMem_TaskMain (void)
{
    switch (TipMem_Obj.State)
    {
      case TIPMEM_STATE_Init:
        OpenI2C2(  0x28,  159 );
        TipMem_Obj.State = TIPMEM_STATE_Start;
        break;
      case TIPMEM_STATE_Start:
        TipMem_Obj.State = TIPMEM_STATE_Idle;
        break;
      case TIPMEM_STATE_Idle:
        if (TipMem_Obj.Status.Flags.DataToUpdate)
        {
            memcpy (&TipMem_Obj.Comm.Buf[0], (const uint8*)&TipMem_Obj.Data.ToWrite[0], TIPMEM_DATA_LEN);
            TipMem_Obj.State = TIPMEM_STATE_WriteStart;
        }
        else if ((TipMem_Obj.Status.Flags.ReadData)&&(!App_RfIsOn()))
        {
            TipMem_Obj.ReadRetries = TIPMEM_READ_TIP_RETRIES;
            TipMem_Obj.State = TIPMEM_STATE_ReadStart;
        }
        break;
        
      case TIPMEM_STATE_ReadStart:
        if (TipMem_Obj.TimerWrite == 0)                     //Check if to wait before accessing the memory
        {
            TipMem_Obj.Comm.Buf[0] = ~TipMem_Obj.Comm.Buf[0]; //In case of remained valid data in the buffer
            #ifdef SW_TIP_TIMEOUT
                TipMem_IIC_Start_Timeout();
            #endif
            TipMem_Obj.Status.Flags.CommError = EESequentialRead2( 0xa0, 0, &TipMem_Obj.Comm.Buf[0], TIPMEM_DATA_LEN);
            if (TipMem_Obj.Status.Flags.CommError == 0)
            {                                               //Reading successful
                TipMem_Obj.State = TIPMEM_STATE_ReadCheck;
            }
            else
            {                                               //Reading failed
                #ifdef SW_TIP_TIMEOUT         
                  Motor_DisRf();                            //Stop RF
                #endif
                TipMem_Obj.State = TIPMEM_STATE_ReadEnd;
            }        
        }
        break;
      case TIPMEM_STATE_ReadCheck:
        if (TipMem_CalcChecksum(&TipMem_Obj.Comm.Buf[1], (TIPMEM_DATA_LEN-2)) != TipMem_Obj.Comm.Buf[0])
        {                                                   //Bad checksum
            App_SetEvent(APP_EVENT_FAULT_Tip);              //Send Tip fault event
            TipMem_Obj.Status.Flags.CommError = 1;
        }    
        TipMem_Obj.State = TIPMEM_STATE_ReadEnd;
        break;
      case TIPMEM_STATE_ReadEnd:
        if (TipMem_Obj.Status.Flags.CommError)
        {                                                   //Communication error
             TipMem_Obj.Status.Flags.CommError = 0;
             if (TipMem_Obj.ReadRetries)
             {                                              //Read fault - retry
                 TipMem_Obj.ReadRetries--;
                 TipMem_Obj.State = TIPMEM_STATE_ReadStart;
             }
             else
             {                                              //Last read retry - fault state
                if (TipMem_Obj.Status.Flags.TipOk)
                {
                    App_SetEvent(APP_EVENT_TIPINFO_info);   //Tip was OK and now not OK - send TipInfo
                    //App_SetEvent(APP_EVENT_FAULT_Tip);      //Tip was OK and now fault - send fault. Required??? <<<<
                }    
                TipMem_Obj.Status.Flags.TipOk = 0;
                TipMem_Obj.Status.Flags.ReadData = 0;
                TipMem_Obj.State = TIPMEM_STATE_Idle;
             }        
        }
        else
        {                                                   //Communication OK
            memcpy (&TipMem_Obj.Data.Image[0], (const uint8*)&TipMem_Obj.Comm.Buf[0], TIPMEM_DATA_LEN);
            if (TipMem_Obj.Status.Flags.TipOk == 0)
            {                                               //Tip was not OK and now OK
              uint8 TipID = TipMem_Obj.Data.Image[TIPMEM_DATA_LOC_ID];
              uint8 DoublePulse = 0;
                if ((TipID == SW_TIP_TYPE_GEN12) || (TipID == SW_TIP_TYPE_GEN36)) {DoublePulse = 1;}
                Eeprom_WriteParam(EEPROM_PARAM_TASK_MOTOR, MOTOR_EE_PARAM_RFOUT_DOUBLEPULSE, DoublePulse);
                App_SetEvent(APP_EVENT_TIPINFO_info);       //Send TipInfo
            }        
            TipMem_Obj.Status.Flags.TipOk = 1;
            TipMem_Obj.Status.Flags.ReadData = 0;
            TipMem_Obj.State = TIPMEM_STATE_Idle;
        }
        break;

      case TIPMEM_STATE_WriteStart:
        if (TipMem_Obj.TimerWrite == 0)                     //Check if to wait before accessing the memory
        {
            memcpy (&TipMem_Obj.Comm.Buf[0], (const uint8*)&TipMem_Obj.Data.ToWrite[0], TIPMEM_DATA_LEN);
            #ifdef SW_TIP_TIMEOUT
                TipMem_IIC_Start_Timeout();
            #endif
            TipMem_Obj.Status.Flags.CommError = EEPageWrite2L(0xa0, 0, &TipMem_Obj.Comm.Buf[0], TIPMEM_DATA_LEN);
            TipMem_Obj.TimerWrite = TIPMEM_WRITE_TIMEOUT;   //Time after write, before next access to memory
            TipMem_Obj.State = TIPMEM_STATE_WriteEnd;
        }
        break;
      case TIPMEM_STATE_WriteEnd:
        if (TipMem_Obj.Status.Flags.CommError)
        {                                                   //Communication error
             #ifdef SW_TIP_TIMEOUT         
               Motor_DisRf();                            //Stop RF
             #endif
             TipMem_Obj.Status.Flags.CommError = 0;
             if (TipMem_Obj.WriteRetries)
             {                                              //Write fail - retry
                 TipMem_Obj.WriteRetries--;
                 TipMem_Obj.State = TIPMEM_STATE_WriteStart;
             }
             else
             {                                              //Last retry
                TipMem_Obj.Status.Flags.DataToUpdate = 0;
                TipMem_Obj.Status.Flags.ReadData = 1;       //After write - read data from TIP, to image buffer
                TipMem_Obj.State = TIPMEM_STATE_Idle;
             }        
        }
        else
        {                                                   //Communication OK
            TipMem_Obj.Status.Flags.DataToUpdate = 0;
            TipMem_Obj.Status.Flags.ReadData = 1;           //After write - read data from TIP, to image buffer
            TipMem_Obj.State = TIPMEM_STATE_Idle;
        }
        break;
    }
}    

/******************************************************************************
*  Function name: TipMem_TaskInit
*  Main tasks:
*    Initialize TipMem task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called on start up
******************************************************************************/
void TipMem_TaskInit (void)
{
    TipMem_IIC_HwConfig();
    TipMem_Obj.Status.Byte = 0;
    TipMem_Obj.Status.Flags.ReadData = 1;
    TipMem_Obj.State = TIPMEM_STATE_Init;
    TipMem_Obj.Timer100mS = TIPMEM_READ_TIP_TIMEOUT;
    TipMem_Obj.WriteRetries = 0;
    TipMem_Obj.ReadRetries = 0;
    #ifdef SW_TIP_TIMEOUT
        T3CON = 0b00110001;
        T3GCON = 0;
        TMR3H = 0;
        TMR3L = 0;
    #endif
}

/******************************************************************************
*  Function name: TipMem_TaskTick100mS
*  Main tasks:
*    Task tick
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called every 100mS
******************************************************************************/
void TipMem_TaskTick100mS (void)
{
    if (TipMem_Obj.Timer100mS)
        {TipMem_Obj.Timer100mS--;}    
    if (TipMem_Obj.Timer100mS == 0)
        {
            TipMem_Obj.Status.Flags.ReadData = 1;
            TipMem_Obj.Timer100mS = TIPMEM_READ_TIP_TIMEOUT;
        }
}

#endif //SW_TIP_MEM
/******************************************************************************
*  Function name: TipMem_TaskTick1mS
*  Main tasks:
*    Task tick
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called every 1mS
******************************************************************************/
void TipMem_TaskTick1mS (void)
{
    if (TipMem_Obj.TimerWrite)
        {TipMem_Obj.TimerWrite--;}    
}


