/* ========================================
 *
 * Handle communication with host
 *
 * ========================================
*/

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
#include "Comm_Codes_Pub.h"
#include "App_Codes_Pub.h"
#include "Adc_Pub.h"
#include "App_Pub.h"
#include "Motor_Pub.h"
#include "Comm_Pub.h"
#include "Comm_Loc.h"
#include "Eeprom_Pub.h"
#include "Led_Pub.h"

#ifdef SW_UC_PIC16F
    #define COMM_IN_CYC_BUF_LEN         100 //50 //25   //In 19200bps: 25 problems. 50 works. 100 margin from 50.
#endif
#ifdef SW_UC_PIC18F   //See how to define long arrays in PIC18 <<<<
    #define COMM_IN_CYC_BUF_LEN         100 //50 //25   //In 19200bps: 25 problems. 50 works. 100 margin from 50.
#endif
#define COMM_IN_CYC_BUF_BATCH           10              //Number of bytes to read in one batch (before returning to scheduler
#define COMM_TEMP_EEPROM_BUF_LEN    5

//RX and TX messages
#ifdef SW_UC_PIC18F
 #pragma udata bigdata_rx
  uint8 Comm_RxBuf[COMM_RX_BUF_LEN];         //RX buffer
 #pragma udata
#endif
#ifdef SW_UC_PIC16F
 volatile uint8 Comm_RxBuf[COMM_RX_BUF_LEN];
#endif

#ifdef SW_UC_PIC18F
 #pragma udata bigdata_tx
  uint8 Comm_TxBuf[COMM_TX_BUF_LEN];         //TX buffer
 #pragma udata
#endif
#ifdef SW_UC_PIC16F
 volatile uint8 Comm_TxBuf[COMM_TX_BUF_LEN];
#endif
         
volatile struct{
    struct{
		uint16	Timer;                              //General timer of the task
		uint16  ResponseTimer;                      //Timer to receive confirmation to sent event
		uint16  MinEventSendTimer;                  //Minimum time from sent message to sending an event (to avoid problem in the S.O.M. side)
    } Task;    
    struct{                                         //Data received from RS232 line (in interrupt)
        uint8   Buf[COMM_IN_CYC_BUF_LEN];           //Cyclic buffer
        uint8   InIndex;                            //In data (from RS232)
        uint8   OutIndex;                           //Out data (to Comm_RxBuf)
        uint8   TempInIndex;
    } InData;
    struct{                                         //Data sent to RS232 (in interrupt)
        uint16   TxIndex;                           //Index of the transmitted message
        uint16   TxRemLen;                          //Remain bytes to transmit
    } OutData;        
} Comm_IntObj;    

struct{
    struct{     //Communication task handler
        uint8   RecByte;        //Last received byte
        uint8   MainState;      //Task main state
        uint8   RxState;        //State of the received message
        uint16   RxIndex;       //Index of the received message
        uint16   RxRemLen;      //Remain bytes to receive
        uint8   TxState;        //State of the transmitted message
		uint8   ResponseRetries;//Retries to send event and wait for response
		uint8   RxBatchCounter; //Count bytes to read from cyclic buffer in one batch
    } Task;
    struct{
        uint8*  Addr;           //Current address of the parcer
        uint16   Index;          //Index in buffer (used instead of the address)
        uint16   Len;            //Length
        uint8   TagNum;         //Number of current tag
        COMM_TEXT_CODE_e Tag;   //Tag code
        COMM_TEXT_CODE_e Cmnd;  //Command code
        COMM_TEXT_CODE_e Attrib; //Attribute code
    } Parcer;    
    union{
        uint8       Byte;
        struct {
            uint8       EventMode:1;        //Event mode (initiated by Applicator)
            uint8       AddCloseEventTag:1; //Add Close Event Tag (used only in TechInfo
            uint8       SyncReceived:1;     //Indicates a sync command was received
            uint8       GetCnfgReceived:1;  //Get Config command was reseived
            uint8       b4:1;
            uint8       b5:1;
            uint8       b6:1;
            uint8       b7:1;
        }   Flags;
    } Status;
    struct{
        //uint16              Flags;          //Bit flags of events to be sent. Order according to APP_EVENT_CODE_e
        uint32              Flags;          //Bit flags of events to be sent. Order according to APP_EVENT_CODE_e
        APP_EVENT_CODE_e    ActiveEvent;    //Code of the event currently active (in communication)
    } Event;
} Comm_Obj;

//Temporary variables
uint16 AttribValAddr;
uint8 AttribValLen;
uint8 CommTempEepromBuf[COMM_TEMP_EEPROM_BUF_LEN];              //Temporary save data (parameter) to EEPROM

#ifdef SW_UC_PIC18F
 const rom COMM_TEXT_CODE_e Comm_ConvertAppEventToNameCode [APP_EVENT_num] = {
#else
 const COMM_TEXT_CODE_e Comm_ConvertAppEventToNameCode [APP_EVENT_num] = {
#endif
    COMM_TEXT_Event_buttons,            //Codes - see APP_EVENT_CODE_e
    COMM_TEXT_Event_fault,
    COMM_TEXT_Event_fault,
    COMM_TEXT_Event_fault,
    COMM_TEXT_Event_fault,
    COMM_TEXT_Event_fault,
    COMM_TEXT_Event_fault,
    COMM_TEXT_Event_fault,
    COMM_TEXT_Event_fault,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_pulsedone,
    COMM_TEXT_Event_tipinfo,
    COMM_TEXT_Event_bootup
};

uint8 CommRxInt_RxByte;

#ifdef SW_DEBUG_LOG_STACK
uint8 CheckSP_Comm;
uint8 MaxSP_Comm = 0;
#endif

volatile uint8 Comm_Flag_C1IF = 0;

#include "Comm_Text_c.h"

#ifdef SW_COMM_LED_ON_EVENT
uint8 Comm_GetEventState (uint8 EventCode)
{
  uint8 Result = 0;
  //uint16 EventData = Comm_Obj.Event.Flags;
  uint32 EventData = Comm_Obj.Event.Flags;
    while (EventCode)
    {
        EventData = EventData >> 1;
        EventCode--;
    }
    EventData &= 0x0001;
    if (EventData) {Result = 1;}
    return(Result);
}    
#endif
//=============================================================================
//=============================================================================
//Hardware functions
//=============================================================================
//=============================================================================
//Registers
    #ifdef SW_UC_PIC16F
        #define COMM_HW_RX_OVF_RELEASE  if(OERR){CREN=0;CREN=1;} //Release overflow error
        #define COMM_HW_TX_RESTART      {TXEN=0;TXEN=1;}    //Disable transmitter and enable
        #define COMM_HW_RX_REG          RCREG               //Receive register
        #define COMM_HW_TX_REG          TXREG               //Transmit register
        #define COMM_HW_TX_INT_ENA      {TXIE=1;}           //Transmit interrupt enable
        #define COMM_HW_TX_INT_DIS      {TXIE=0;}           //Transmit interrupt disable
    #endif
    #ifdef SW_UC_PIC18F
        #define COMM_HW_RX_OVF_RELEASE  if(RCSTA1bits.OERR){RCSTA1bits.CREN=0;RCSTA1bits.CREN=1;} //Release overflow error
        #define COMM_HW_TX_RESTART      {TXSTA1bits.TXEN=0;TXSTA1bits.TXEN=1;}    //Disable transmitter and enable
        #define COMM_HW_RX_REG          RCREG1              //Receive register
        #define COMM_HW_TX_REG          TXREG1              //Transmit register
        #define COMM_HW_TX_INT_DIS      {PIE1bits.TX1IE=0;}          //Transmit interrupt disable
        #ifdef SW_COMM_NO_INTERRUPT
            #define COMM_HW_TX_INT_ENA      COMM_HW_TX_INT_DIS           //Transmit interrupt enable
        #else
            #define COMM_HW_TX_INT_ENA      {PIE1bits.TX1IE=1;}          //Transmit interrupt enable
        #endif
    #endif
//Hardware config
    #ifdef SW_UC_PIC16F
        #define COMM_RX_INT_EN  RCIE=1;
        #define COMM_RX_INT_DIS RCIE=0;
        void Comm_HwCnfg(void)
        {
            TRISC6 = 0;       //making output port
            TRISC7 = 1;       //making input port
         
            BRG16 = 1;          // use 16 bit BRG
            BRGH = 0; 
            SPBRGH  = BRG_VALUE >> 8;
            SPBRG   = BRG_VALUE & 0xFF;
            BRGH   = 1;         //high data rate for sending
            SYNC   = 0;         //asynchronous
            SPEN   = 1;         //enable serial port pins
            //SREN   = 0;         //no effect
            TX9    = 0;           //8-bit transmission
            RX9    = 0;           //8-bit reception
            CREN    = 1;        //enable the reciever
            RCIE    = 1;        //Enable receiver interrupt
        }    
    #endif
    #ifdef SW_UC_PIC18F
        #define COMM_RX_INT_DIS {PIE1bits.RCIE=0;}
        #ifdef SW_COMM_NO_INTERRUPT
            #define COMM_RX_INT_EN  COMM_RX_INT_DIS
        #else
            #define COMM_RX_INT_EN  {PIE1bits.RCIE=1;}
        #endif
       void Comm_HwCnfg(void)
        {
            TRISCbits.TRISC6 = 0;           //making output port
            TRISCbits.TRISC7 = 1;           //making input port
            ANSELCbits.ANSC7 = 0;
            ANSELCbits.ANSC6 = 0;
         
            BAUD1CONbits.BRG16 = 1;         // use 16 bit BRG
            TXSTA1bits.BRGH = 0; 
            SPBRGH1  = BRG_VALUE >> 8;
            SPBRG1   = BRG_VALUE & 0xFF;
            TXSTA1bits.BRGH   = 1;          //high data rate for sending
            TXSTA1bits.SYNC   = 0;          //asynchronous
            RCSTA1bits.SPEN   = 1;          //enable serial port pins
            //SREN   = 0;                   //no effect
            TXSTA1bits.TX9    = 0;          //8-bit transmission
            RCSTA1bits.RX9    = 0;           //8-bit reception
            RCSTA1bits.CREN    = 1;         //enable the reciever
            COMM_RX_INT_EN; //PIE1bits.RCIE    = 1;           //Enable receiver interrupt
        }    
    #endif
//=============================================================================
//=============================================================================




/******************************************************************************
*  Utilities routines
******************************************************************************/
#ifdef SW_UC_PIC18F
    void Comm_CpyRom2Ram (uint8* Target, rom const uint8* Source, uint8 Length)
    {
        while (Length)
        {
            *Target = *Source;
            Target++;
            Source++;
            Length--;
        }    
    }    
#endif

/******************************************************************************
*  Event routines
******************************************************************************/
//----------------------------
// Returns TRUE if event to handle, and place the event code in Comm_Obj.Event.ActiveEvent.
uint8 Comm_NextEvent (void)
{
  uint8 Result = FALSE;
  uint8 i = 0;
  uint32 TempW = Comm_Obj.Event.Flags;
    Comm_Obj.Event.ActiveEvent = APP_EVENT_TECH_none;
    while ((i < 32) && (Result == FALSE))
    {
        if (TempW & 0x0001)
        {
            Comm_Obj.Event.ActiveEvent = i;
            Result = TRUE;
        }
        TempW = TempW >> 1;
        i++;
    }
    return(Result);
}    
//----------------------------
//Set the flag of an event to handle
void Comm_SetEvent (APP_EVENT_CODE_e Event, uint8 State)
{
  uint32 TempW = 0x00000001;
    if (Event < APP_EVENT_num)
    {
        while (Event)                   //Find flag location
        {
            TempW = TempW << 1;
            Event--;
        }
        if (State)
        {                               //Activate flag
            Comm_Obj.Event.Flags |= TempW;
        }
        else
        {                               //Deactivate flag
            Comm_Obj.Event.Flags &= ~TempW;
        }
    }        
}    
//----------------------------
/******************************************************************************
*  Service routines
******************************************************************************/
//----------------------------
//Get received byte from cyclic buffer
uint8 Comm_GetRxByte(void)
{   //Get received byte from host
  uint8 Result = FALSE;
    COMM_RX_INT_DIS;                                    //Disable receiver interrupt
    COMM_HW_RX_OVF_RELEASE;                             //RX overflow - release error
    if (Comm_IntObj.InData.OutIndex != Comm_IntObj.InData.InIndex)
    {
        Comm_Obj.Task.RecByte = Comm_IntObj.InData.Buf[Comm_IntObj.InData.OutIndex];
        Comm_IntObj.InData.OutIndex = (Comm_IntObj.InData.OutIndex+1) % (uint8)COMM_IN_CYC_BUF_LEN;
        Comm_IntObj.Task.Timer = COMM_TIMER_RX_MAX;    //Init timer between received bytes
        Result = TRUE;
    }
    COMM_RX_INT_EN;                                     //Enable receiver interrupt
    return(Result);
}

//----------------------------
//Memory compare RX buffer to text
#ifdef SW_UC_PIC18F
 uint8 Comm_RxBufMemcmp (uint8 Index, const rom uint8* Addr, uint8 Len)
#else
 uint8 Comm_RxBufMemcmp (uint8 Index, const uint8* Addr, uint8 Len)
#endif
{
  uint8 Result = TRUE;
    while ((Len) && (Result == TRUE))
    {
        if (Comm_RxBuf[Index] != *Addr)
        {
            Result = FALSE;
        }    
        Index++;
        Addr++;
        Len--;
    }    
    return(Result);
}    

//----------------------------
//Memory copy RX buffer to buffer
void Comm_RxBufMemcpy (uint8 Index, uint8* Addr, uint8 Len)
{
    while (Len)
    {
        *Addr = Comm_RxBuf[Index];
        Index++;
        Addr++;
        Len--;
    }    
}    

//----------------------------
//Calculate checksum
void Comm_CalcChecksum (uint8* Buffer, uint8* LengthAddr, uint8* ResultToWrite)
{
  uint16 Checksum = 0;
  uint16 Length;
    memcpy (&Length, (const uint8*)LengthAddr, sizeof(Length));
    #ifdef SW_COMM_IGNORE_RX_LEN_HIGH
        Length &= 0x00ff;                                   //Use only 8 bits. Upper 8 bits are not 00 - to enable to display it in the Terminal application in the PC.
    #endif
    while (Length)
    {
        Checksum += *Buffer;
        Buffer++;
        Length--;
    }
    memcpy (ResultToWrite, (const uint8*)&Checksum, sizeof(Checksum));
}    
//----------------------------
//=============================================================================
//  Parser functions
//=============================================================================

/******************************************************************************
*  Function name: Comm_ParserCheckText
*  Main tasks:
*    Check a text in the message, from a list of texts
*  Parameters:
*    First code to search
*    Last code to search
*  Returned values:
*    Code of the text (if found)
*    COMM_TEXT_NONE (if not found)
*  Remarks:
*    Update parser address and length, with the length of the text
******************************************************************************/
COMM_TEXT_CODE_e Comm_ParserFindText (COMM_TEXT_CODE_e First, COMM_TEXT_CODE_e Last)
{
  COMM_TEXT_CODE_e Result = COMM_TEXT_NONE;
  uint16 TempLen;
    while ((First <= Last) && (Result == COMM_TEXT_NONE))
    {
        TempLen = CommTextTable[First].TextLen;
        if (Comm_RxBufMemcmp (Comm_Obj.Parcer.Index, CommTextTable[First].TextAddr, TempLen) == TRUE)
        {                                               //Text found
            Comm_Obj.Parcer.Index += TempLen;
            Comm_Obj.Parcer.Len -= TempLen;
            Result = First;
        }
        if (Comm_Obj.Parcer.Len < TempLen)
        {                                               //End of message
            TempLen = Comm_Obj.Parcer.Len;
            First = Last + 1;
        }    
        First++;
    }    
#ifdef SW_DEBUG_LOG_STACK
CheckSP_Comm = STKPTR & 0x1f;
if (CheckSP_Comm > MaxSP_Comm)
{
    MaxSP_Comm = CheckSP_Comm;
}
#endif
    return (Result);
}    

/******************************************************************************
*  Function name: Comm_ParserNextTag
*  Main tasks:
*    Set parser address to the beginning of next tag (after the '<')
*  Parameters:
*    None
*  Returned values:
*    TRUE/FALSE - next tag found
*  Remarks:
*    Update parser address and length
******************************************************************************/
uint8 Comm_ParserNextTag (void)
{
  uint8 Result = FALSE;
    while ((Result == FALSE) && (Comm_Obj.Parcer.Len))
    {
        if (Comm_RxBuf[Comm_Obj.Parcer.Index] == COMM_MSG_TAG_START_BYTE)
        {                                               //Tag start found
            Comm_Obj.Parcer.TagNum++;
            Result = TRUE;
        }
        Comm_Obj.Parcer.Index++;
        Comm_Obj.Parcer.Len--;
    }    
    return (Result);
}    

/******************************************************************************
*  Function name: Comm_ParserNextText
*  Main tasks:
*    Set parser address to the beginning of next text
*  Parameters:
*    None
*  Returned values:
*    TRUE/FALSE - next text found
*  Remarks:
*    Skip Space, =, "
*    Update parser address and length
******************************************************************************/
uint8 Comm_ParserNextText (void)
{
  uint8 Result = FALSE;
  uint8 EndOfTag = FALSE;
    while ((Result == FALSE) && (Comm_Obj.Parcer.Len) && (EndOfTag == FALSE))
    {
        Comm_Obj.Parcer.Index++;
        Comm_Obj.Parcer.Len--;
        if ((Comm_RxBuf[Comm_Obj.Parcer.Index] != COMM_MSG_CHAR_EQUAL) &&
            (Comm_RxBuf[Comm_Obj.Parcer.Index] != COMM_MSG_CHAR_SPACE) &&
            (Comm_RxBuf[Comm_Obj.Parcer.Index] != COMM_MSG_CHAR_QUOTES) &&
            (Comm_RxBuf[Comm_Obj.Parcer.Index] != COMM_MSG_TAG_START_BYTE) &&
            (Comm_RxBuf[Comm_Obj.Parcer.Index] != COMM_MSG_TAG_END_BYTE))
        {                                               //Text found
            Result = TRUE;
        }
        if (Comm_RxBuf[Comm_Obj.Parcer.Index] == COMM_MSG_TAG_END_BYTE)
        {                                               //Reached end of tag
            EndOfTag = TRUE;
            Result = FALSE;
        }
        if (Comm_Obj.Parcer.Index >= COMM_RX_BUF_LEN)
        {                                               //Overflow protection
            Comm_Obj.Parcer.Len = 0;
            Result = TRUE;
        }    
    }    
    return (Result);
}    

/******************************************************************************
*  Function name: Comm_ParserTextLength
*  Main tasks:
*    Get the length of the text
*  Parameters:
*    None
*  Returned values:
*    Length
*  Remarks:
*    Length till next space, =, or "
*    Parser address in the 1st byte of the text
******************************************************************************/
uint8 Comm_ParserTextLength(void)
{
  uint16 Len = 0;
  uint16 Index = Comm_Obj.Parcer.Index;
  uint8 Result = FALSE;
    while (Result == FALSE)
    {
        Index++;
        Len++;
        if ((Comm_RxBuf[Index] == COMM_MSG_CHAR_EQUAL) ||
            (Comm_RxBuf[Index] == COMM_MSG_CHAR_SPACE) ||
            (Comm_RxBuf[Index] == COMM_MSG_CHAR_QUOTES))
        {                                               //Text end found
            Result = TRUE;
        }
        if (Index >= COMM_RX_BUF_LEN)                   //Overflow protection
        {
            Result = TRUE;
        }    
    }    
    return(Len);
}    

/******************************************************************************
*  Function name: Comm_ParserExecRxMsg
*  Main tasks:
*    Parse and execute Rx message
*  Parameters:
*    None
*  Returned values:
*    state
*  Remarks:
*    Called repeatadly until read/error
******************************************************************************/
COMM_RX_STATUS_e Comm_ParserExecRxMsg(void)
{
  COMM_RX_PARSER_STATE_e Result = COMM_RX_PARSER_STATE_BUSY;
  COMM_TEXT_CODE_e TempCode;
    
    if (Comm_ParserNextTag())
    {                                               //Tag found
        switch (Comm_Obj.Parcer.TagNum)
        {
          case 0:                                   //1st tag, should be protocol
            if (Comm_ParserFindText(COMM_TEXT_Tag_protocol, COMM_TEXT_Tag_protocol) != COMM_TEXT_Tag_protocol)
            {                                       //Not protocol - error
                Result = COMM_RX_PARSER_STATE_ERROR;
            }    
            break;
            
          case 1:                                   //2nd tag, should be command
             if (Comm_Obj.Status.Flags.EventMode)
            {                                       //Event - wait for Response tag
                TempCode = COMM_TEXT_Tag_confirm;
            }
            else
            {                                       //not event - wait for Command tag
                TempCode = COMM_TEXT_Tag_command;
            }        
            if (Comm_ParserFindText(TempCode, TempCode) == TempCode)
            {                                       //command/confirm
                if (Comm_ParserNextText())
                {
                    if (Comm_ParserFindText(COMM_TEXT_Attrib_name, COMM_TEXT_Attrib_name) == COMM_TEXT_Attrib_name)
                    {                               //name
                        if (Comm_ParserNextText())
                        {                           //Name value
                            if (Comm_Obj.Status.Flags.EventMode)
                            {                       //Event - check confirm
                                TempCode = Comm_ConvertAppEventToNameCode[Comm_Obj.Event.ActiveEvent];
                                if (Comm_ParserFindText(COMM_TEXT_Event_First, COMM_TEXT_Event_Last) == TempCode)
                                {                   //Confirm to the event that was sent
                                    App_CommConfirm (TempCode,  Comm_Obj.Event.ActiveEvent); //Inform App task (that will reset the event flag in Comm task)
                                    Comm_Obj.Status.Flags.EventMode = 0;
                                    Result = COMM_RX_PARSER_STATE_DONE;
                                }
                                else    
                                {                   //Wrong confirm
                                    Result = COMM_RX_PARSER_STATE_ERROR;
                                }    
                            }
                            else
                            {                       //Not event - check command
                                Result = COMM_RX_PARSER_STATE_SEND_RPLY;
                                Comm_Obj.Parcer.Cmnd = Comm_ParserFindText(COMM_TEXT_Cmnd_First, COMM_TEXT_Cmnd_Last); //Gal 2015-05-19
                                switch (Comm_Obj.Parcer.Cmnd)
                                {
                                  case COMM_TEXT_Cmnd_config:
                                    #ifdef SW_PATCH_SYNC_OVERV
                                        if (Comm_Obj.Status.Flags.GetCnfgReceived == 0)
                                        {                               //first Sync - execute Overvoltage BIT
                                            Comm_Obj.Status.Flags.GetCnfgReceived = 1;
                                            App_Exec_OvervBit();
                                        }    
                                    #endif
                                    break;
                                  case COMM_TEXT_Cmnd_loadparam: //loadparam - continue to next tags
                                    Result = COMM_RX_PARSER_STATE_BUSY;
                                    break;
                                  case COMM_TEXT_Cmnd_activate:
                                    App_CommCmnd(Comm_Obj.Parcer.Cmnd); //Send command to application task
                                    break;
                                  case COMM_TEXT_Cmnd_deactivate:
                                    App_CommCmnd(Comm_Obj.Parcer.Cmnd); //Send command to application task
                                    break;
                                  case COMM_TEXT_Cmnd_synccom:
                                    break;
                                  case COMM_TEXT_Cmnd_techmode:
                                    App_CommCmnd(Comm_Obj.Parcer.Cmnd); //Send command to application task
                                    break;
                                  case COMM_TEXT_Cmnd_tipstatus:
                                    App_CommCmnd(Comm_Obj.Parcer.Cmnd); //Send command to application task
                                    break;
                                  case COMM_TEXT_Cmnd_firepulse:
                                    App_CommCmnd(Comm_Obj.Parcer.Cmnd); //Send command to application task
                                    break;
                                  case COMM_TEXT_Cmnd_setled:
									Result = COMM_RX_PARSER_STATE_BUSY;
                                    break;
                                  default:
                                    Result = COMM_RX_PARSER_STATE_ERROR;
                                    break;
                                }
                            }    
                        }
                        else {Result = COMM_RX_PARSER_STATE_ERROR;} //No text found - error
                    }
                    else {Result = COMM_RX_PARSER_STATE_ERROR;} //Not name - error
                }
                else {Result = COMM_RX_PARSER_STATE_ERROR;} //No text found - error
            }
            else {Result = COMM_RX_PARSER_STATE_ERROR;}     //Not command name - error
            break;
            
          default:                                          //The rest - command data
            Comm_Obj.Parcer.Tag = Comm_ParserFindText(COMM_TEXT_Tag_First, COMM_TEXT_Tag_Last);
            switch (Comm_Obj.Parcer.Tag)
            {
              case COMM_TEXT_Tag_vdtip:
              case COMM_TEXT_Tag_tftip:
                {
                  uint8 TempB = '0';;
                    if (Comm_Obj.Parcer.Tag == COMM_TEXT_Tag_vdtip)
                        {TempB = '1';}                     //Double pulse
                    Eeprom_StoreParam (Comm_Obj.Parcer.Tag, COMM_TEXT_Tag_vdtip, &TempB, 1);  //Send to EEPROM
                }
              case COMM_TEXT_Tag_rgbled:
              case COMM_TEXT_Tag_motor1:
              case COMM_TEXT_Tag_motor2:
              case COMM_TEXT_Tag_rfout:
                while (Comm_ParserNextText())
                {                                           //Next attribute - send to EEPROM
                    Comm_Obj.Parcer.Attrib = Comm_ParserFindText(COMM_TEXT_Attrib_First, COMM_TEXT_Attrib_Last);
                    if (Comm_Obj.Parcer.Attrib != COMM_TEXT_NONE)
                    {                                       //Attribute found - save data in EEPROM
                      uint8 TempTextLen;
                        Comm_ParserNextText();
                        TempTextLen = Comm_ParserTextLength();
                        Comm_RxBufMemcpy (Comm_Obj.Parcer.Index, &CommTempEepromBuf[0], COMM_TEMP_EEPROM_BUF_LEN);  //Copy to temporary length (max temp buf length)
                        Eeprom_StoreParam (Comm_Obj.Parcer.Tag, Comm_Obj.Parcer.Attrib, &CommTempEepromBuf[0], TempTextLen);  //Send to EEPROM
                        if (TempTextLen > Comm_Obj.Parcer.Len)
                        {                           //Overflow protection
                            TempTextLen = Comm_Obj.Parcer.Len;
                        }    
                        Comm_Obj.Parcer.Index += TempTextLen;
                        Comm_Obj.Parcer.Len -= TempTextLen;
                    }
                }
                break;
			  case COMM_TEXT_Tag_led:
                if (Comm_ParserNextText())
                {                                           //Next attribute - send to EEPROM
                    Comm_Obj.Parcer.Attrib = Comm_ParserFindText(COMM_TEXT_Attrib_First, COMM_TEXT_Attrib_Last);
                    if (Comm_Obj.Parcer.Attrib != COMM_TEXT_NONE)
					{
                      uint8 TempTextLen;
					  uint8 Cmd;
                        Comm_ParserNextText();
                        TempTextLen = Comm_ParserTextLength();
						if (TempTextLen == 1)
						{
                        	Comm_RxBufMemcpy (Comm_Obj.Parcer.Index, &Cmd, TempTextLen);
							Led_Cmd_SetState (Cmd);
						}
					}
				}
				break;
              case COMM_TEXT_Tag_endprotocol:
                if (Comm_Obj.Parcer.Cmnd == COMM_TEXT_Cmnd_loadparam)
                {                                           //Load parameter done
                    App_CommCmnd(Comm_Obj.Parcer.Cmnd);     //Inform application
                    #ifdef SW_PROD_ReviveRf
                      Motor_UpdParams();                    //Inform motor
                    #endif
                }    
                Result = COMM_RX_PARSER_STATE_SEND_RPLY;
                break;
            }    
            break;
        }    
    }
    else
    {                                               //Tag not found
        Result = COMM_RX_PARSER_STATE_DONE;
    }    
    return(Result);
}    

//=============================================================================
//  TX message builder functions
//=============================================================================
/******************************************************************************
*  Function name: Comm_BuilderCloseMsg
*  Main tasks:
*    Close a message
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Write length and checksum to message
******************************************************************************/
void Comm_BuilderCloseMsg (void)
{
  uint16 TempW = Comm_IntObj.OutData.TxIndex;                       //total message length
    TempW -= COMM_MSG_HEADER_LEN;                                   //Only XML message length
    Comm_TxBuf[COMM_MSG_LENGTH_LOC] = (uint8)(TempW >> 8);
    Comm_TxBuf[COMM_MSG_LENGTH_LOC + 1] = (uint8)TempW;
    TempW += COMM_MSG_LENGTH_LEN;                                   //Length of data to calc checksum
    Comm_CalcChecksum ((uint8*)&Comm_TxBuf[COMM_MSG_LENGTH_LOC],    //Calculate checksum
                       (uint8*)&TempW,
                       (uint8*)&TempW);
    Comm_TxBuf[COMM_MSG_CHECKSUM_LOC] = (uint8)(TempW >> 8);        //Save checksum
    Comm_TxBuf[COMM_MSG_CHECKSUM_LOC + 1] = (uint8)TempW;
}    

/******************************************************************************
*  Function name: Comm_BuilderAddChar
*  Main tasks:
*    Add a character to the message
*  Parameters:
*    Char
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void Comm_BuilderAddChar (uint8 NewChar)
{
    Comm_TxBuf[Comm_IntObj.OutData.TxIndex] = NewChar;
    Comm_IntObj.OutData.TxIndex++;
}    

/******************************************************************************
*  Function name: Comm_BuilderAddText
*  Main tasks:
*    Add text to the message
*  Parameters:
*    Text address
*    Text length
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
#ifdef SW_UC_PIC18F
    void Comm_BuilderAddText (const rom uint8* TextAddr, uint8 TextLen)
    {
        Comm_CpyRom2Ram (&Comm_TxBuf[Comm_IntObj.OutData.TxIndex], (rom const uint8*)TextAddr, TextLen);
        Comm_IntObj.OutData.TxIndex += TextLen;
    }
#else
    void Comm_BuilderAddText (const uint8* TextAddr, uint8 TextLen)
    {
        memcpy(&Comm_TxBuf[Comm_IntObj.OutData.TxIndex], TextAddr, TextLen);
        Comm_IntObj.OutData.TxIndex += TextLen;
    }
#endif
//-----------------------------------------------------------------------------
    void Comm_BuilderAddTextRam (uint8* TextAddr, uint8 TextLen)
    {
        memcpy(&Comm_TxBuf[Comm_IntObj.OutData.TxIndex], (const uint8*)TextAddr, TextLen);
        Comm_IntObj.OutData.TxIndex += TextLen;
    }


/******************************************************************************
*  Add resistanse ratio for temperature measurement
******************************************************************************/
#ifdef SW_TEMP_ADD_RES_RATIO
    void Comm_AddResRatio (void)
    {
      uint8 TempBuf[5];
      uint16 TempRatio;
      uint8 i = 5;
        TempRatio = Adc_GetResRatio();
        do{
            TempBuf[i-1] = (TempRatio % 10) | 0x30;
            TempRatio /= 10;
            i--;
        }while (i != 0);    
        Comm_BuilderAddChar(COMM_MSG_CHAR_SPACE);
        Comm_BuilderAddChar(COMM_MSG_CHAR_QUOTES);
        Comm_BuilderAddTextRam(&TempBuf[0], 5);
        Comm_BuilderAddChar(COMM_MSG_CHAR_QUOTES);
    }    
#endif

/******************************************************************************
*  Function name: Comm_BuilderAddAttribText
*  Main tasks:
*    Add attribute (name and text value) to the message
*  Parameters:
*    Attribute name address
*    Attribute name length
*    Attribute value address
*    Attribute value length
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
#ifdef SW_UC_PIC18F
 void Comm_BuilderAddAttribText (const rom uint8* AttribNameAddr, uint8 AttribNameLen, const rom uint8* AttribValAddr, uint8 AttribValLen)
#else
 void Comm_BuilderAddAttribText (const uint8* AttribNameAddr, uint8 AttribNameLen, const uint8* AttribValAddr, uint8 AttribValLen)
#endif
{
    Comm_BuilderAddChar(COMM_MSG_CHAR_SPACE);
    Comm_BuilderAddText(AttribNameAddr, AttribNameLen);
    Comm_BuilderAddChar(COMM_MSG_CHAR_EQUAL);
    Comm_BuilderAddChar(COMM_MSG_CHAR_QUOTES);
    Comm_BuilderAddText(AttribValAddr, AttribValLen);
    Comm_BuilderAddChar(COMM_MSG_CHAR_QUOTES);
}
//-----------------------------------------------------------------------------
#ifdef SW_UC_PIC18F
 void Comm_BuilderAddAttribTextRam (const rom uint8* AttribNameAddr, uint8 AttribNameLen, uint8* AttribValAddr, uint8 AttribValLen)
#else
 void Comm_BuilderAddAttribTextRam (const uint8* AttribNameAddr, uint8 AttribNameLen, uint8* AttribValAddr, uint8 AttribValLen)
#endif
{
    Comm_BuilderAddChar(COMM_MSG_CHAR_SPACE);
    Comm_BuilderAddText(AttribNameAddr, AttribNameLen);
    Comm_BuilderAddChar(COMM_MSG_CHAR_EQUAL);
    Comm_BuilderAddChar(COMM_MSG_CHAR_QUOTES);
    Comm_BuilderAddTextRam(AttribValAddr, AttribValLen);
    Comm_BuilderAddChar(COMM_MSG_CHAR_QUOTES);
}

/******************************************************************************
*  Function name: Comm_BuilderAddAttribNum
*  Main tasks:
*    Add attribute (name and numeric value) to the message
*  Parameters:
*    Attribute name address
*    Attribute name length
*    Attribute value
*    Attribute value length
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
#ifdef SW_UC_PIC18F
 void Comm_BuilderAddAttribNum (const rom uint8* AttribNameAddr, uint8 AttribNameLen, uint16 AttribVal, uint8 AttribValLen)
#else
 void Comm_BuilderAddAttribNum (const uint8* AttribNameAddr, uint8 AttribNameLen, uint16 AttribVal, uint8 AttribValLen)
#endif
{
  uint8 AttribValBuf[COMM_MSG_MAX_ATTRIB_VAL_LEN];
  uint8 i = COMM_MSG_MAX_ATTRIB_VAL_LEN - 1;
  uint8 Val;
    do{
        Val = AttribVal % 10;
        AttribVal /= 10;
        AttribValBuf[i] = Val | 0x30;
        i--;
    } while (AttribVal);
    i++;
    Comm_BuilderAddAttribTextRam (AttribNameAddr, AttribNameLen, &AttribValBuf[i], (COMM_MSG_MAX_ATTRIB_VAL_LEN-i));
}    

/******************************************************************************
*  Function name: Comm_BuilderStartMsg
*  Main tasks:
*    Start a message
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Write ^, and leave space for checksum and length
******************************************************************************/
void Comm_BuilderStartMsg (void)
{
    Comm_TxBuf[0] = COMM_MSG_START_BYTE;
    Comm_IntObj.OutData.TxIndex = COMM_MSG_HEADER_LEN;
    Comm_BuilderAddChar(COMM_MSG_TAG_START_BYTE);
    Comm_BuilderAddText (CommTextTable[COMM_TEXT_Tag_protocol].TextAddr, CommTextTable[COMM_TEXT_Tag_protocol].TextLen);
    Comm_BuilderAddAttribNum (CommTextTable[COMM_TEXT_Attrib_ver].TextAddr, CommTextTable[COMM_TEXT_Attrib_ver].TextLen, 1, 1);
    Comm_BuilderAddChar(COMM_MSG_TAG_END_BYTE);
    Comm_BuilderAddChar(COMM_MSG_TAG_START_BYTE);
}    

/******************************************************************************
*  Function name: Comm_BuilderRespAndCloseMsg
*  Main tasks:
*    Add /></protocol> and Close a message
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    None
******************************************************************************/
void Comm_BuilderRespAndCloseMsg (void)
{
    Comm_BuilderAddChar(COMM_MSG_CHAR_SLASH);
    Comm_BuilderAddChar(COMM_MSG_TAG_END_BYTE);
    if (Comm_Obj.Status.Flags.AddCloseEventTag)
    {                                               //Add </event> for TechInfo
        Comm_BuilderAddChar(COMM_MSG_TAG_START_BYTE);
        Comm_BuilderAddChar(COMM_MSG_CHAR_SLASH);
        Comm_BuilderAddText (CommTextTable[COMM_TEXT_Tag_event].TextAddr, CommTextTable[COMM_TEXT_Tag_event].TextLen);
        Comm_BuilderAddChar(COMM_MSG_TAG_END_BYTE);
    }    
    Comm_BuilderAddChar(COMM_MSG_TAG_START_BYTE);
    Comm_BuilderAddText (CommTextTable[COMM_TEXT_Tag_endprotocol].TextAddr, CommTextTable[COMM_TEXT_Tag_endprotocol].TextLen);
    Comm_BuilderAddChar(COMM_MSG_TAG_END_BYTE);
    #ifdef SW_COMM_ADD_CRLF
        Comm_BuilderAddChar(0x0D);
        Comm_BuilderAddChar(0x0A);
    #endif
    Comm_BuilderCloseMsg();
}    


//=============================================================================
//  Task functions
//=============================================================================

/******************************************************************************
*  Function name: Comm_TaskInit
*  Main tasks:
*    Initialize Communication task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called on start up
******************************************************************************/
void Comm_TaskInit (void)
{
    Comm_HwCnfg();
    COMM_HW_TX_RESTART;
    COMM_HW_TX_INT_ENA;
    COMM_RX_INT_EN;

    Comm_Obj.Task.MainState = COMM_STATE_INIT;
    Comm_Obj.Task.RxState = COMM_STATE_INIT;
    Comm_Obj.Task.TxState = COMM_STATE_INIT;
    Comm_Obj.Event.Flags = 0;
    Comm_Obj.Status.Byte = 0;
    
    Comm_IntObj.OutData.TxRemLen = 0;
}	


/******************************************************************************
*  Function name: Comm_RxData
*  Main tasks:
*    Communication receive data
*  Parameters:
*    To re-start - TRUE/FALSE
*  Returned values:
*    Idle/Busy/Ready/Error
*  Remarks:
*    None
******************************************************************************/
//----------------------------
void Comm_RxData_Restart (void)
{
  uint16 index1;
    for (index1=0; index1<COMM_RX_BUF_LEN; index1++)
    {
        Comm_RxBuf[index1] = 0xff;
    }    
    for (index1=0; index1<COMM_TX_BUF_LEN; index1++)
    {
        Comm_TxBuf[index1] = 0xff;
    }    
    Comm_Obj.Task.RxIndex = 0;
    Comm_Obj.Task.RxState = COMM_RX_STATE_WAIT_START_BYTE;
}    
//----------------------------
void Comm_RxData_SaveByte (void)
{
  uint8 TempIndex = Comm_Obj.Task.RxIndex;
  uint8 TempByte = Comm_Obj.Task.RecByte;
    Comm_RxBuf[TempIndex] = TempByte;
    Comm_Obj.Task.RxIndex++;
}    
//----------------------------
COMM_RX_STATUS_e Comm_RxData (uint8 Restart)
{
  COMM_RX_STATUS_e Result = COMM_RX_STATUS_BUSY;
  uint16 TempW, TempW1;
    if (Restart == TRUE)
    {
        Comm_RxData_Restart();
    }    
    switch (Comm_Obj.Task.RxState)
    {
      case COMM_RX_STATE_WAIT_START_BYTE:                   //Wait for a start byte
        Result = COMM_RX_STATUS_IDLE;
        if (Comm_GetRxByte())
        {                                                   //Byte received
            if (Comm_Obj.Task.RecByte == COMM_MSG_START_BYTE)
            {                                               //Start received
                Comm_RxData_SaveByte();
                Comm_Obj.Task.RxState = COMM_RX_STATE_WAIT_HEADER;
                Result = COMM_RX_STATUS_BUSY;
            }
        }
        break;
        
      case COMM_RX_STATE_WAIT_HEADER:                       //Receive the header
        if (Comm_GetRxByte())
        {                                                   //Byte received
            Comm_RxData_SaveByte();
            if (Comm_Obj.Task.RxIndex >= COMM_MSG_HEADER_LEN)
            {                                               //Header received
                Comm_Obj.Task.RxRemLen = Comm_RxBuf[COMM_MSG_LENGTH_LOC + 1]; //Length of remaining data
                #ifndef SW_COMM_IGNORE_RX_LEN_HIGH
                    Comm_Obj.Task.RxRemLen += (uint16)Comm_RxBuf[COMM_MSG_LENGTH_LOC] << 8;
                #endif
                Comm_Obj.Task.RxState = COMM_RX_STATE_RX_MSG;
            }    
        }
        else if (Comm_IntObj.Task.Timer == 0)
        {                                                   //Timout waiting for a byte - error
            Result = COMM_RX_STATUS_ERROR;
        }    
        break;
        
      case COMM_RX_STATE_RX_MSG:                            //Receive the tags
        if (Comm_GetRxByte())
        {                                                   //Byte received
            Comm_RxData_SaveByte();
            Comm_Obj.Task.RxRemLen--;
            if (Comm_Obj.Task.RxIndex > (COMM_RX_BUF_LEN-1))
            {                                               //Overflow - error
                Comm_RxData_Restart();
                Result = COMM_RX_STATUS_ERROR;
            }
            else if (Comm_Obj.Task.RxRemLen == 0)
            {                                               //End of Rx data
                Comm_Obj.Task.RxState = COMM_RX_STATE_CHECK_RCVD_MSG;
            }    
        }    
        else if (Comm_IntObj.Task.Timer == 0)
        {                                                   //Timout waiting for a byte - error
            Result = COMM_RX_STATUS_ERROR;
        }    
        break;
        
      case COMM_RX_STATE_CHECK_RCVD_MSG:                    //Check the received message (checksum)
        TempW = (uint16)Comm_RxBuf[COMM_MSG_LENGTH_LOC] << 8;
        TempW += Comm_RxBuf[COMM_MSG_LENGTH_LOC + 1];
        TempW += COMM_MSG_LENGTH_LEN;
        Comm_CalcChecksum((uint8*)&Comm_RxBuf[COMM_MSG_LENGTH_LOC],
                          (uint8*)&TempW,
                          (uint8*)&TempW);     //Calculate checksum
        #ifdef SW_COMM_CRC_NO_CHECK
            if (1)
        #else
            TempW1 = (uint16)Comm_RxBuf[COMM_MSG_CHECKSUM_LOC] << 8;
            TempW1 += Comm_RxBuf[COMM_MSG_CHECKSUM_LOC + 1];
            if (TempW == TempW1)
        #endif
        {                                                   //Correct checksum - continue
            Result = COMM_RX_STATUS_READY;
            Comm_Obj.Task.RxState = COMM_RX_STATE_MSG_READY;
        }    
        else
        {                                                   //Checksum error - restart
            Result = COMM_RX_STATUS_ERROR;
        }    
        break;
        
      case COMM_RX_STATE_MSG_READY:                         //Message ready
        Result = COMM_RX_STATUS_READY;
        break;
    }
    return(Result);
}    

/******************************************************************************
*  Function name: Comm_TaskMain
*  Main tasks:
*    Handle Communication task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called periodically
******************************************************************************/
//----------------------------
void Comm_GetAppInfo(COMM_TEXT_CODE_e Attrib_id)
{
    App_GetInfo (Attrib_id, (uint8*)&AttribValAddr, &AttribValLen, Comm_Obj.Event.ActiveEvent);
    Comm_BuilderAddAttribTextRam (CommTextTable[Attrib_id].TextAddr, CommTextTable[Attrib_id].TextLen, (uint8*)AttribValAddr, AttribValLen);
}    
//----------------------------
void Comm_TaskMain (void)
{
    switch (Comm_Obj.Task.MainState)
    {
      case COMM_STATE_INIT:                                 //Initialize comm
        Comm_IntObj.InData.InIndex = 0;
        Comm_IntObj.InData.OutIndex = 0;
        Comm_Obj.Task.MainState = COMM_STATE_START;
        break;
        
      case COMM_STATE_START:                                //Start comm
        Comm_RxData(TRUE);
        Comm_Obj.Task.MainState = COMM_STATE_WAIT_RCVD_MSG;
        break;

      case COMM_STATE_WAIT_RCVD_MSG:                        //Start comm
        Comm_Obj.Task.RxBatchCounter = COMM_IN_CYC_BUF_BATCH;
        while ((Comm_Obj.Task.RxBatchCounter) &&            //Get a batch of received bytes from cyclic buffer
               (Comm_Obj.Task.MainState == COMM_STATE_WAIT_RCVD_MSG))
        {
            switch (Comm_RxData(FALSE))
            {
              case COMM_RX_STATUS_IDLE:
                Comm_Obj.Task.RxBatchCounter = 0;
                if (Comm_Obj.Status.Flags.EventMode)
                {                                               //Already in event mode - wait for confirm
                    if (Comm_IntObj.Task.ResponseTimer == 0)
                    {                                           //Timeout waiting for response
                        if (Comm_Obj.Task.ResponseRetries)
                        {                                       //Another retry
                            Comm_Obj.Task.ResponseRetries--;
                            Comm_Obj.Task.MainState = COMM_STATE_PREP_SENT_MSG;
                        }
                        else
                        {                                       //That was last retry
                            App_CommConfirm (COMM_TEXT_fail,  Comm_Obj.Event.ActiveEvent); //Inform failure to App task (that will reset the event flag in Comm task)
                            Comm_Obj.Status.Flags.EventMode = 0;
                        }        
                    }    
                }    
                else if (Comm_IntObj.Task.MinEventSendTimer == 0)
                {
                    if ((Comm_Obj.Status.Flags.SyncReceived) && (Comm_NextEvent()))
                    {                                               //Event to send (first retry)
                        Comm_Obj.Task.ResponseRetries = COMM_RETRIES_MAX;
                        Comm_Obj.Status.Flags.EventMode = 1;
                        Comm_Obj.Task.MainState = COMM_STATE_PREP_SENT_MSG;
                    }
                }
                break;
              case COMM_RX_STATUS_BUSY:
                Comm_Obj.Task.RxBatchCounter--;
                break;
              case COMM_RX_STATUS_READY:
                Comm_Obj.Parcer.Index = COMM_MSG_TEXT_LOC;
                Comm_Obj.Parcer.Len = Comm_Obj.Task.RxIndex - Comm_Obj.Parcer.Index;
                Comm_Obj.Parcer.TagNum = 0;
                Comm_Obj.Parcer.TagNum--;   //-1, to get to 0 in the first "next tag"
                Comm_Obj.Task.MainState = COMM_STATE_EXEC_RCVD_MSG;
                break;
              case COMM_RX_STATUS_ERROR:
                Comm_Obj.Task.MainState = COMM_STATE_START;
                break;
            }
        }
        break;

      case COMM_STATE_EXEC_RCVD_MSG:
        switch (Comm_ParserExecRxMsg())
        {
          case COMM_RX_PARSER_STATE_BUSY:
            break;
          case COMM_RX_PARSER_STATE_ERROR:
            Comm_IntObj.InData.OutIndex =Comm_IntObj.InData.InIndex ;
            Comm_Obj.Task.MainState = COMM_STATE_START;
            break;
          case COMM_RX_PARSER_STATE_DONE:
            Comm_Obj.Task.MainState = COMM_STATE_START;
            break;
          case COMM_RX_PARSER_STATE_SEND_RPLY:
            Comm_Obj.Task.MainState = COMM_STATE_PREP_SENT_MSG;
            break;
        }  
        break;
        
      case COMM_STATE_PREP_SENT_MSG:
        Comm_Obj.Status.Flags.AddCloseEventTag = 0;
        Comm_BuilderStartMsg();                         //Start message + protocol tag
        if (Comm_Obj.Status.Flags.EventMode)
        {                                               //Event mode - prepare event message
          COMM_TEXT_CODE_e NameCode = Comm_ConvertAppEventToNameCode[Comm_Obj.Event.ActiveEvent];
            Comm_BuilderAddText (CommTextTable[COMM_TEXT_Tag_event].TextAddr, CommTextTable[COMM_TEXT_Tag_event].TextLen);
            Comm_BuilderAddAttribText (CommTextTable[COMM_TEXT_Attrib_name].TextAddr, CommTextTable[COMM_TEXT_Attrib_name].TextLen,
                                       CommTextTable[NameCode].TextAddr, CommTextTable[NameCode].TextLen);
            switch (Comm_Obj.Event.ActiveEvent)
            {
              case APP_EVENT_buttons:                   //Buttons status event
                Comm_GetAppInfo(COMM_TEXT_Attrib_onoff);
                #ifndef SW_MOT_RF
                    Comm_GetAppInfo(COMM_TEXT_Attrib_power_event);
                    Comm_GetAppInfo(COMM_TEXT_Attrib_led);
                #endif
                break;
              case APP_EVENT_FAULT_motorX:              //system fault event (no deactivate)
              case APP_EVENT_FAULT_motorY:
              case APP_EVENT_FAULT_rgbled:
                Comm_GetAppInfo(COMM_TEXT_Attrib_code);
                Comm_GetAppInfo(COMM_TEXT_Attrib_deactivate);
                break;
              case APP_EVENT_FAULT_OverVolt:              //system fault event (yes deactivate)
              case APP_EVENT_FAULT_Temperature:
              case APP_EVENT_FAULT_OverCur:
              case APP_EVENT_FAULT_Rf:
              case APP_EVENT_FAULT_Tip:
                Comm_GetAppInfo(COMM_TEXT_Attrib_code);
                Comm_GetAppInfo(COMM_TEXT_Attrib_activate);
                break;
              case APP_EVENT_TECH_INFO_motorX:          //Tech info events
              case APP_EVENT_TECH_INFO_motorY:
              case APP_EVENT_TECH_INFO_rgbled:
              case APP_EVENT_TECH_INFO_applicator:
              case APP_EVENT_TECH_INFO_12V:
              case APP_EVENT_TECH_INFO_5V:
              case APP_EVENT_TECH_INFO_Cur:
              case APP_EVENT_TECH_INFO_TipCount:
              case APP_EVENT_TECH_INFO_TipId:
                Comm_Obj.Status.Flags.AddCloseEventTag = 1;
                Comm_BuilderAddChar(COMM_MSG_TAG_END_BYTE);
                Comm_BuilderAddChar(COMM_MSG_TAG_START_BYTE);
                Comm_BuilderAddText((const rom uint8*)&CommTxt_Tag_module, CommTxt_Tag_module_LEN);
                Comm_GetAppInfo(COMM_TEXT_Attrib_id_module);
                Comm_GetAppInfo(COMM_TEXT_Attrib_status_module);
                Comm_GetAppInfo(COMM_TEXT_Attrib_reason);
                break;
              case APP_EVENT_TECH_INFO_Temp:
                Comm_Obj.Status.Flags.AddCloseEventTag = 1;
                Comm_BuilderAddChar(COMM_MSG_TAG_END_BYTE);
                Comm_BuilderAddChar(COMM_MSG_TAG_START_BYTE);
                Comm_BuilderAddText((const rom uint8*)&CommTxt_Tag_module, CommTxt_Tag_module_LEN);
                Comm_GetAppInfo(COMM_TEXT_Attrib_id_module);
                #ifdef SW_IGNORE_THERMISTOR
                  {                                                     //Thermistor not in use - send "NA"
                    uint8 TempBuf[APP_TECHINFO_PARAM_ID_TEMP_NA_LEN];
                      Comm_CpyRom2Ram (&TempBuf[0], (rom const uint8*)&APP_TECHINFO_PARAM_ID_TEMP_NA[0], APP_TECHINFO_PARAM_ID_TEMP_NA_LEN);
                      Comm_BuilderAddAttribTextRam (CommTextTable[COMM_TEXT_Attrib_status_module].TextAddr, CommTextTable[COMM_TEXT_Attrib_status_module].TextLen, (uint8*)&TempBuf[0], APP_TECHINFO_PARAM_ID_TEMP_NA_LEN);
                  }
                #else
                  Comm_GetAppInfo(COMM_TEXT_Attrib_status_module);
                #endif
                #ifdef SW_TEMP_ADD_RES_RATIO
                    if (Comm_Obj.Event.ActiveEvent == APP_EVENT_TECH_INFO_Temp)
                    {
                        Comm_AddResRatio();
                    }
                #endif
                Comm_GetAppInfo(COMM_TEXT_Attrib_reason);
                break;
              case APP_EVENT_PULSEDONE_done:            //Just event name. No additional data.
                break;
              case APP_EVENT_BOOT_up:                   //Just event name. No additional data.
                Comm_GetAppInfo(COMM_TEXT_Attrib_type);
                break;
              #ifdef SW_TIP_MEM
                  case APP_EVENT_TIPINFO_info:
                    Comm_GetAppInfo(COMM_TEXT_Attrib_status_module);
                    Comm_GetAppInfo(COMM_TEXT_Attrib_id_module);
                    Comm_GetAppInfo(COMM_TEXT_Attrib_pulses);
                    break;
              #endif
              default:
                break;
            }
            Comm_IntObj.Task.ResponseTimer = COMM_TIMER_EVENT_TO_CONFIRM; //Init timer to receive response
        }
        else
        {                                               //Command mode - prepare response message
            Comm_BuilderAddText (CommTextTable[COMM_TEXT_Tag_response].TextAddr, CommTextTable[COMM_TEXT_Tag_response].TextLen);
            Comm_BuilderAddAttribText (CommTextTable[COMM_TEXT_Attrib_name].TextAddr, CommTextTable[COMM_TEXT_Attrib_name].TextLen,
                                       CommTextTable[Comm_Obj.Parcer.Cmnd].TextAddr, CommTextTable[Comm_Obj.Parcer.Cmnd].TextLen);
            switch (Comm_Obj.Parcer.Cmnd)
            {
              case COMM_TEXT_Cmnd_config:
                Comm_GetAppInfo(COMM_TEXT_Attrib_id_config);
                Comm_GetAppInfo(COMM_TEXT_Attrib_type);
                Comm_GetAppInfo(COMM_TEXT_Attrib_status_config);
                #ifdef SW_TIP_MEM
                  Comm_GetAppInfo(COMM_TEXT_Attrib_pulses);
                #endif
                Comm_GetAppInfo(COMM_TEXT_Attrib_version);
                break;
              case COMM_TEXT_Cmnd_loadparam:
              case COMM_TEXT_Cmnd_activate:
              case COMM_TEXT_Cmnd_deactivate:
              case COMM_TEXT_Cmnd_techmode:
              case COMM_TEXT_Cmnd_tipstatus:
              case COMM_TEXT_Cmnd_firepulse:
              case COMM_TEXT_Cmnd_setled:
                break;
              case COMM_TEXT_Cmnd_synccom:
                Comm_Obj.Status.Flags.SyncReceived = 1; //Mark sync received, after sending the response
                break;
              default:
                break;
            }
            Comm_BuilderAddAttribNum (CommTextTable[COMM_TEXT_Attrib_errorcode].TextAddr, CommTextTable[COMM_TEXT_Attrib_errorcode].TextLen, 0, 1);
        }
        Comm_BuilderRespAndCloseMsg();                  //Add /></protocol> and close message
        Comm_Obj.Task.MainState = COMM_STATE_SEND_MSG;
        break;
        
      case COMM_STATE_SEND_MSG:                         //Start TX message
        COMM_RX_INT_DIS;                                //Disable RX interrupt
        Comm_IntObj.OutData.TxRemLen = Comm_IntObj.OutData.TxIndex - 1;
        Comm_IntObj.OutData.TxIndex = 1;
        COMM_HW_TX_REG = Comm_TxBuf[0];                 //Transmit 1st byte
        COMM_HW_TX_INT_ENA;                             //Enable TX interrupt
        Comm_Obj.Task.MainState = COMM_STATE_WAIT_SENT_MSG_END;
        break;
        
      case COMM_STATE_WAIT_SENT_MSG_END:                //Wait till TX message sent
        if (Comm_IntObj.OutData.TxRemLen == 0)
        {                                               //Message sent
            Comm_IntObj.Task.MinEventSendTimer = COMM_TIMER_SENT_TO_EVENT;
            COMM_RX_INT_EN;                             //Enable RX interrupt
            Comm_Obj.Task.MainState = COMM_STATE_START;
        }    
        break;
        
    }    
}	

/******************************************************************************
*  Function name: Comm_TaskTick1mS
*  Main tasks:
*    Timers of Communication task
*  Parameters:
*    None
*  Returned values:
*    None
*  Remarks:
*    Called every 1mS, from an interrupt
******************************************************************************/
void Comm_TaskTick1mS (void)
{
	if (Comm_IntObj.Task.Timer)
	{
		Comm_IntObj.Task.Timer--;
	}
	if (Comm_IntObj.Task.ResponseTimer)
	{
		Comm_IntObj.Task.ResponseTimer--;
	}
	if (Comm_IntObj.Task.MinEventSendTimer)
	{
		Comm_IntObj.Task.MinEventSendTimer--;
	}
}	
/******************************************************************************
*  Communication RX and TX interrupt
*    Called from an interrupt
******************************************************************************/
//RX interrupt
#ifdef SW_UC_PIC18F
volatile void Comm_RxInterrupt (void)
#else
void Comm_RxInterrupt (void)
#endif
{
    Comm_IntObj.InData.Buf[Comm_IntObj.InData.InIndex] = COMM_HW_RX_REG;
    Comm_IntObj.InData.TempInIndex = Comm_IntObj.InData.InIndex+1;  //Implemented modulo operation, in order not to use internal function.
    if (Comm_IntObj.InData.TempInIndex >= COMM_IN_CYC_BUF_LEN)
    {
        Comm_IntObj.InData.TempInIndex -= COMM_IN_CYC_BUF_LEN;
    }    
    if (Comm_IntObj.InData.TempInIndex != Comm_IntObj.InData.OutIndex)
    {   //Internal buffer not full - store the received byte
        Comm_IntObj.InData.InIndex = Comm_IntObj.InData.TempInIndex;
    }
}

//TX interrupt
#ifdef SW_UC_PIC18F
volatile void Comm_TxInterrupt (void)
#else
void Comm_TxInterrupt (void)
#endif
{
    if (Comm_IntObj.OutData.TxRemLen)
    {   //More data to send
        COMM_HW_TX_REG = Comm_TxBuf[Comm_IntObj.OutData.TxIndex];
        Comm_IntObj.OutData.TxIndex++;
        Comm_IntObj.OutData.TxRemLen--;
    }
    else
    {   //No more data - disable tx interrupts
        COMM_HW_TX_INT_DIS;	        
    }    
}


/* [] END OF FILE */
