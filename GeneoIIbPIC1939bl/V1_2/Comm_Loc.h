/* ========================================
 *
 * Local definitions for Comm.c
 *
 * ========================================
*/

#ifndef _COMM_LOC_H_
#define _COMM_LOC_H_

#define BAUD 19200 //9600 //APP_BASE_BAUDRATE
#define BRG_VALUE ((SYSFOSC / (BAUD * 4L)) - 1L)

//Messages definitions
#ifdef SW_MAIN_PIC16F1937
    #define COMM_RX_BUF_LEN         120         //Rx buf length
    #define COMM_TX_BUF_LEN         143         //Tx buf length (longer, to enable config response)
#else
  #ifdef SW_UC_PIC16F
    #define COMM_RX_BUF_LEN         250 //260 //253         //Rx buf length (less than 260, ecause of total memory limit)
    #define COMM_TX_BUF_LEN         250 //260 //253         //Tx buf length
  #endif
  #ifdef SW_UC_PIC18F   //See how to define long arrays in PIC18 <<<<
    #define COMM_RX_BUF_LEN         254 //256 //253         //Rx buf length
    #define COMM_TX_BUF_LEN         254 //256 //260 //253         //Tx buf length
  #endif
#endif
#define COMM_MSG_START_BYTE     '^'         //Start byte of a message
#define COMM_MSG_CHECKSUM_LEN   2           //Message checksum length
#define COMM_MSG_LENGTH_LEN     2           //Message length length
#define COMM_MSG_HEADER_LEN     (1+COMM_MSG_CHECKSUM_LEN+COMM_MSG_LENGTH_LEN)
#define COMM_MSG_LENGTH_LOC     (1+COMM_MSG_CHECKSUM_LEN)
#define COMM_MSG_CHECKSUM_LOC   1
#define COMM_MSG_TEXT_LOC       COMM_MSG_HEADER_LEN
#define COMM_MSG_MAX_ATTRIB_VAL_LEN 5       //Max length of numeric value of an attribute

//Messages characters
#define COMM_MSG_TAG_START_BYTE '<'         //Tag start byte
#define COMM_MSG_TAG_END_BYTE   '>'         //Tag end byte
#define COMM_MSG_CHAR_EQUAL     '='
#define COMM_MSG_CHAR_SPACE     0x20
#define COMM_MSG_CHAR_QUOTES    '"'
#define COMM_MSG_CHAR_SLASH     '/'

//Timers
#ifdef SW_COMM_LONG_TIMER
    #define COMM_TIMER_RX_MAX       2500        //Max time between characters in received message
    #define COMM_TIMER_EVENT_TO_CONFIRM 3000    //Max time to receive confirmation to an event
    #define COMM_TIMER_SENT_TO_EVENT    2500    //Min time between sent message (response) to event
#else
    #define COMM_TIMER_RX_MAX       50          //Max time between characters in received message
    #define COMM_TIMER_EVENT_TO_CONFIRM 1000    //Max time to receive confirmation to an event
    #define COMM_TIMER_SENT_TO_EVENT    500     //Min time between sent message (response) to event
#endif
#define COMM_RETRIES_MAX        (3-1)       //Max retries to send event


//Communication task states
typedef enum{
    COMM_STATE_INIT = 0,
    COMM_STATE_START,
    COMM_STATE_WAIT_RCVD_MSG,
    COMM_STATE_EXEC_RCVD_MSG,
    COMM_STATE_PREP_SENT_MSG,
    COMM_STATE_SEND_MSG,
    COMM_STATE_WAIT_SENT_MSG_END
} COMM_STATE_e;

//Receive states
typedef enum{
    COMM_RX_STATE_WAIT_START_BYTE = 0,
    COMM_RX_STATE_WAIT_HEADER,
    COMM_RX_STATE_RX_MSG,
    COMM_RX_STATE_CHECK_RCVD_MSG,
    COMM_RX_STATE_MSG_READY
} COMM_RX_STATE_e;

//Receive status
typedef enum{
    COMM_RX_STATUS_IDLE = 0,
    COMM_RX_STATUS_BUSY,
    COMM_RX_STATUS_READY,
    COMM_RX_STATUS_ERROR
} COMM_RX_STATUS_e;        

//Parcer states
typedef enum{
    COMM_RX_PARSER_STATE_BUSY = 0,
    COMM_RX_PARSER_STATE_ERROR,
    COMM_RX_PARSER_STATE_DONE,
    COMM_RX_PARSER_STATE_SEND_RPLY
} COMM_RX_PARSER_STATE_e;


//Parameters codes
typedef enum{
    COMM_PAR_NONE = 0,
    COMM_PAR_id,
    COMM_PAR_type,
    COMM_PAR_status,
    COMM_PAR_pulses,
    COMM_PAR_version,
    COMM_PAR_errorcode,
    COMM_PAR_rgbmode,
    COMM_PAR_rgbcolor,
    COMM_PAR_rgbpower,
    COMM_PAR_rgbon,
    COMM_PAR_rgboff
} COMM_PAR_CODE_e;    


#endif //[] END OF FILE