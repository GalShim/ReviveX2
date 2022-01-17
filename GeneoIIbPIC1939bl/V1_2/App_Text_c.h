/* ========================================
 *
 * Texts for App.c
 *
 * ========================================
*/

#ifndef _APP_TEXT_H_
#define _APP_TEXT_H_


//Firmware version number
#ifdef SW_PROD_GeneoII          //Updated 22.10.2013
    #define SW_VER_1 '1'
    #define SW_VER_2 '1'
    #define SW_VER_3 '0'
#endif
#ifdef SW_PROD_BLDC             //Updated 30.10.2013
    #define SW_VER_1 '1'
    #define SW_VER_2 '1'
    #define SW_VER_3 '4'
#endif
#ifdef SW_PROD_Rf               //Updated 13.07.2014
    #define SW_VER_1 '1'
    #define SW_VER_2 '2'
    #define SW_VER_3 '7'
#endif
#ifdef SW_PROD_Us               //Updated 29.09.2014
    #define SW_VER_1 '1'
    #define SW_VER_2 '2'
    #define SW_VER_3 '5'
#endif
#ifdef SW_PROD_ReviveRf         //Updated 07.12.2021
    #define SW_VER_1 '2'
    #define SW_VER_2 '2'
    #define SW_VER_3 '9'
#endif
#ifdef SW_PROD_ReviveUs         //Updated 16.11.2015
    #define SW_VER_1 '2'
    #define SW_VER_2 '0'
    #define SW_VER_3 '9'
#endif

//Texts sent from the application

//-----------------------------------------------------------------------------
#define APP_CONFIG_PARAM_ID_LEN         1
#ifdef SW_UC_PIC18F
 const uint8 APP_CONFIG_PARAM_ID[APP_CONFIG_PARAM_ID_LEN] =                          
#else
 const uint8 APP_CONFIG_PARAM_ID[APP_CONFIG_PARAM_ID_LEN] =                          
#endif
                                                                                    {'1'};
//#define APP_CONFIG_PARAM_TYPE_LEN       2
#ifdef SW_UC_PIC18F
 //const uint8 APP_CONFIG_PARAM_TYPE[APP_CONFIG_PARAM_TYPE_LEN] =                      
 #ifdef SW_PROD_Rf
   #define APP_CONFIG_PARAM_TYPE_LEN       2
   const uint8 APP_CONFIG_PARAM_TYPE[APP_CONFIG_PARAM_TYPE_LEN] =
                                                                                    {'r','f'};
 #endif
 #ifdef SW_PROD_Us
   #define APP_CONFIG_PARAM_TYPE_LEN       2
   const uint8 APP_CONFIG_PARAM_TYPE[APP_CONFIG_PARAM_TYPE_LEN] =
                                                                                    {'u','s'};
 #endif
 #ifdef SW_PROD_ReviveRf
   #define APP_CONFIG_PARAM_TYPE_LEN       2
   const uint8 APP_CONFIG_PARAM_TYPE[APP_CONFIG_PARAM_TYPE_LEN] =
                                                                                    {'f','r'}; //{'v','d'};
 #endif
 #ifdef SW_PROD_ReviveUs
   #define APP_CONFIG_PARAM_TYPE_LEN       3
   const uint8 APP_CONFIG_PARAM_TYPE[APP_CONFIG_PARAM_TYPE_LEN] =
                                                                                    {'u','s','r'};
 #endif
#else
 const uint8 APP_CONFIG_PARAM_TYPE[APP_CONFIG_PARAM_TYPE_LEN] =                      
                                                                                    {'m','s'};
#endif
#define APP_CONFIG_PARAM_STATUS_CONFIG_LEN     11
#ifdef SW_UC_PIC18F
 const uint8 APP_CONFIG_PARAM_STATUS_CONFIG[APP_CONFIG_PARAM_STATUS_CONFIG_LEN] =    
#else
 const uint8 APP_CONFIG_PARAM_STATUS_CONFIG[APP_CONFIG_PARAM_STATUS_CONFIG_LEN] =    
#endif
                                                                                    {'o','p','e','r','a','t','i','o','n','a','l'};
#define APP_CONFIG_PARAM_PULSES_LEN     1
#ifdef SW_UC_PIC18F
 const uint8 APP_CONFIG_PARAM_PULSES[APP_CONFIG_PARAM_PULSES_LEN] =                  
#else
 const uint8 APP_CONFIG_PARAM_PULSES[APP_CONFIG_PARAM_PULSES_LEN] =                  
#endif
                                                                                    {'0'};
#define APP_CONFIG_PARAM_VERSION_LEN    5
#ifdef SW_UC_PIC18F
 const uint8 APP_CONFIG_PARAM_VERSION[APP_CONFIG_PARAM_VERSION_LEN] =                
#else
 const uint8 APP_CONFIG_PARAM_VERSION[APP_CONFIG_PARAM_VERSION_LEN] =                
#endif
                                                                                    {SW_VER_1,'.',SW_VER_2,'.',SW_VER_3};
//-----------------------------------------------------------------------------
#define APP_EVENT_PARAM_ON_LEN          2
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_PARAM_ON[APP_EVENT_PARAM_ON_LEN] =                            
#else
 const uint8 APP_EVENT_PARAM_ON[APP_EVENT_PARAM_ON_LEN] =                            
#endif
                                                                                    {'o','n'};
#define APP_EVENT_PARAM_OFF_LEN         3
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_PARAM_OFF[APP_EVENT_PARAM_OFF_LEN] =                          
#else
 const uint8 APP_EVENT_PARAM_OFF[APP_EVENT_PARAM_OFF_LEN] =                          
#endif
                                                                                    {'o','f','f'};
#define APP_EVENT_PARAM_NO_LEN          2
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_PARAM_NO[APP_EVENT_PARAM_NO_LEN] =                            
#else
 const uint8 APP_EVENT_PARAM_NO[APP_EVENT_PARAM_NO_LEN] =                            
#endif
                                                                                    {'n','o'};
#define APP_EVENT_PARAM_YES_LEN         3
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_PARAM_YES[APP_EVENT_PARAM_YES_LEN] =                            
#else
 const uint8 APP_EVENT_PARAM_YES[APP_EVENT_PARAM_YES_LEN] =                            
#endif
                                                                                    {'y','e','s'};
//-----------------------------------------------------------------------------
#define APP_TECHINFO_PARAM_ID_MOTORX_LEN          6
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_MOTORX[APP_TECHINFO_PARAM_ID_MOTORX_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_MOTORX[APP_TECHINFO_PARAM_ID_MOTORX_LEN] =        
#endif
                                                                                    {'m','o','t','o','r','X'};
#define APP_TECHINFO_PARAM_ID_MOTORY_LEN          6
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_MOTORY[APP_TECHINFO_PARAM_ID_MOTORY_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_MOTORY[APP_TECHINFO_PARAM_ID_MOTORY_LEN] =        
#endif
                                                                                    {'m','o','t','o','r','Y'};
#define APP_TECHINFO_PARAM_ID_RGBLED_LEN          6
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_RGBLED[APP_TECHINFO_PARAM_ID_RGBLED_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_RGBLED[APP_TECHINFO_PARAM_ID_RGBLED_LEN] =        
#endif
                                                                                    {'r','g','b','l','e','d'};
#ifdef SW_UC_PIC18F
 #ifdef SW_PROD_Rf
  #define APP_TECHINFO_PARAM_ID_APP_LEN          9
  const uint8 APP_TECHINFO_PARAM_ID_APP[APP_TECHINFO_PARAM_ID_APP_LEN] =        
                                                                                    {'T','r','i','P','o','l','l','a','r'};
 #endif
 #ifdef SW_PROD_Us
  #define APP_TECHINFO_PARAM_ID_APP_LEN          11
  const uint8 APP_TECHINFO_PARAM_ID_APP[APP_TECHINFO_PARAM_ID_APP_LEN] =        
                                                                                    {'T','r','i','P','o','l','l','a','r','U','s'};
 #endif
 #ifdef SW_PROD_ReviveRf
  #define APP_TECHINFO_PARAM_ID_APP_LEN          8
  const uint8 APP_TECHINFO_PARAM_ID_APP[APP_TECHINFO_PARAM_ID_APP_LEN] =        
                                                                                    {'V','o','l','u','D','e','r','m'};
 #endif
 #ifdef SW_PROD_ReviveUs
  #define APP_TECHINFO_PARAM_ID_APP_LEN          10
  const uint8 APP_TECHINFO_PARAM_ID_APP[APP_TECHINFO_PARAM_ID_APP_LEN] =        
                                                                                    {'U','l','t','r','a','s','o','u','n','d'};
 #endif
#else
 #define APP_TECHINFO_PARAM_ID_APP_LEN          8
 const uint8 APP_TECHINFO_PARAM_ID_APP[APP_TECHINFO_PARAM_ID_APP_LEN] =        
                                                                                    {'O','x','y','G','e','n','e','o'};
#endif

#define APP_TECHINFO_PARAM_ID_5V_LEN          9
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_5V[APP_TECHINFO_PARAM_ID_5V_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_5V[APP_TECHINFO_PARAM_ID_5V_LEN] =        
#endif
                                                                                    {'v','o','l','t','a','g','e','5','V'};
#define APP_TECHINFO_PARAM_ID_12V_LEN          10
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_12V[APP_TECHINFO_PARAM_ID_12V_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_12V[APP_TECHINFO_PARAM_ID_12V_LEN] =        
#endif
#ifdef SW_HW_SUP_24V
                                                                                    {'v','o','l','t','a','g','e','2','4','V'};
#else
                                                                                    {'v','o','l','t','a','g','e','1','2','V'};
#endif

#define APP_TECHINFO_PARAM_ID_TEMP_LEN          18
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_TEMP[APP_TECHINFO_PARAM_ID_TEMP_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_TEMP[APP_TECHINFO_PARAM_ID_TEMP_LEN] =        
#endif
                                                                                    {'T','e','m','p','e','r','a','t','u','r','e',' ','s','e','n','s','o','r'};
#define APP_TECHINFO_PARAM_ID_CUR_LEN          7
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_CUR[APP_TECHINFO_PARAM_ID_CUR_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_CUR[APP_TECHINFO_PARAM_ID_CUR_LEN] =        
#endif
                                                                                    {'C','u','r','r','e','n','t'};
#define APP_TECHINFO_PARAM_ID_TIPCOUNT_LEN          8
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_TIPCOUNT[APP_TECHINFO_PARAM_ID_TIPCOUNT_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_TIPCOUNT[APP_TECHINFO_PARAM_ID_TIPCOUNT_LEN] =        
#endif
                                                                                    {'T','i','p','C','o','u','n','t'};
#define APP_TECHINFO_PARAM_ID_TIPID_LEN          2
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_TIPID[APP_TECHINFO_PARAM_ID_TIPID_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_TIPID[APP_TECHINFO_PARAM_ID_TIPID_LEN] =        
#endif
                                                                                    {'i','d'};
#define APP_TECHINFO_PARAM_ID_STATUS_LEN          6
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_STATUS[APP_TECHINFO_PARAM_ID_STATUS_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_STATUS[APP_TECHINFO_PARAM_ID_STATUS_LEN] =        
#endif
                                                                                    {'s','t','a','t','u','s'};
#define APP_TECHINFO_PARAM_ID_PULSES_LEN          6
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_ID_PULSES[APP_TECHINFO_PARAM_ID_PULSES_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_PULSES[APP_TECHINFO_PARAM_ID_PULSES_LEN] =        
#endif
                                                                                    {'p','u','l','s','e','s'};
//-----------------------------------------------------------------------------
#define APP_TECHINFO_PARAM_STATUS_OP_LEN   9
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_STATUS_OP[APP_TECHINFO_PARAM_STATUS_OP_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_STATUS_OP[APP_TECHINFO_PARAM_STATUS_OP_LEN] =        
#endif
                                                                                    {'o','p','e','r','a','t','i','o','n'};
#define APP_TECHINFO_PARAM_STATUS_BRK_LEN      6
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_STATUS_BRK[APP_TECHINFO_PARAM_STATUS_BRK_LEN] =      
#else
 const uint8 APP_TECHINFO_PARAM_STATUS_BRK[APP_TECHINFO_PARAM_STATUS_BRK_LEN] =      
#endif
                                                                                    {'b','r','o','k','e','n'};
#define APP_TECHINFO_PARAM_STAT_OP_LEN   APP_CONFIG_PARAM_STATUS_CONFIG_LEN
#define APP_TECHINFO_PARAM_STAT_OP       APP_CONFIG_PARAM_STATUS_CONFIG
//-----------------------------------------------------------------------------
#define APP_TECHINFO_PARAM_REASON_PASS_LEN        4
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_REASON_PASS[APP_TECHINFO_PARAM_REASON_PASS_LEN] =    
#else
 const uint8 APP_TECHINFO_PARAM_REASON_PASS[APP_TECHINFO_PARAM_REASON_PASS_LEN] =    
#endif
                                                                                    {'p','a','s','s'};
#define APP_TECHINFO_PARAM_REASON_FAIL_LEN        4
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_REASON_FAIL[APP_TECHINFO_PARAM_REASON_FAIL_LEN] =    
#else
 const uint8 APP_TECHINFO_PARAM_REASON_FAIL[APP_TECHINFO_PARAM_REASON_FAIL_LEN] =    
#endif
                                                                                    {'f','a','i','l'};
#define APP_TECHINFO_PARAM_REASON_FIRM_LEN        10
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_REASON_FIRM[APP_TECHINFO_PARAM_REASON_FIRM_LEN] =    
#else
 const uint8 APP_TECHINFO_PARAM_REASON_FIRM[APP_TECHINFO_PARAM_REASON_FIRM_LEN] =    
#endif
                                                                                    {'f','i','r','m',' ',SW_VER_1,'.',SW_VER_2,'.',SW_VER_3};
#define APP_TECHINFO_PARAM_REASON_PULSES_LEN       6
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_REASON_PULSES[APP_TECHINFO_PARAM_REASON_PULSES_LEN] =    
#else
 const uint8 APP_TECHINFO_PARAM_REASON_PULSES[APP_TECHINFO_PARAM_REASON_PULSES_LEN] =    
#endif
                                                                                    {'p','u','l','s','e','s'};
#define APP_TECHINFO_PARAM_REASON_TIPID_LEN       5
#ifdef SW_UC_PIC18F
 const uint8 APP_TECHINFO_PARAM_REASON_TIPID[APP_TECHINFO_PARAM_REASON_TIPID_LEN] =    
#else
 const uint8 APP_TECHINFO_PARAM_REASON_TIPID[APP_TECHINFO_PARAM_REASON_TIPID_LEN] =    
#endif
                                                                                    {'T','i','p','I','d'};
//-----------------------------------------------------------------------------
  #define APP_TIPINFO_PARAM_DISCONNECTED_LEN 12
#ifdef SW_UC_PIC18F
 const uint8 APP_TIPINFO_PARAM_DISCONNECTED[APP_TIPINFO_PARAM_DISCONNECTED_LEN] =        
#else
 const uint8 APP_TIPINFO_PARAM_DISCONNECTED[APP_TIPINFO_PARAM_DISCONNECTED_LEN] =        
#endif
                                                                            {'d','i','s','c','o','n','n','e','c','t','e','d'};
  #define APP_TIPINFO_PARAM_CONNECTED_LEN 9
#ifdef SW_UC_PIC18F
 const uint8 APP_TIPINFO_PARAM_CONNECTED[APP_TIPINFO_PARAM_CONNECTED_LEN] =        
#else
 const uint8 APP_TIPINFO_PARAM_CONNECTED[APP_TIPINFO_PARAM_CONNECTED_LEN] =        
#endif
                                                                            {'c','o','n','n','e','c','t','e','d'};
//-----------------------------------------------------------------------------
#define APP_EVENT_FAULT_CODE_OVRVOLT_LEN         1
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_FAULT_CODE_OVRVOLT[APP_EVENT_FAULT_CODE_OVRVOLT_LEN] =                          
#else
 const uint8 APP_EVENT_FAULT_CODE_OVRVOLT[APP_EVENT_FAULT_CODE_OVRVOLT_LEN] =                          
#endif
                                                                                    {'1'};
#define APP_EVENT_FAULT_CODE_TEMP_LEN         1
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_FAULT_CODE_TEMP[APP_EVENT_FAULT_CODE_TEMP_LEN] =                          
#else
 const uint8 APP_EVENT_FAULT_CODE_TEMP[APP_EVENT_FAULT_CODE_TEMP_LEN] =                          
#endif
                                                                                    {'2'};
#define APP_EVENT_FAULT_CODE_MOTX_LEN         1
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_FAULT_CODE_MOTX[APP_EVENT_FAULT_CODE_MOTX_LEN] =                          
#else
 const uint8 APP_EVENT_FAULT_CODE_MOTX[APP_EVENT_FAULT_CODE_MOTX_LEN] =                          
#endif
                                                                                    {'3'};
#define APP_EVENT_FAULT_CODE_MOTY_LEN         1
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_FAULT_CODE_MOTY[APP_EVENT_FAULT_CODE_MOTY_LEN] =                          
#else
 const uint8 APP_EVENT_FAULT_CODE_MOTY[APP_EVENT_FAULT_CODE_MOTY_LEN] =                          
#endif
                                                                                    {'4'};
#define APP_EVENT_FAULT_CODE_RGBLED_LEN         1
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_FAULT_CODE_RGBLED[APP_EVENT_FAULT_CODE_RGBLED_LEN] =                          
#else
 const uint8 APP_EVENT_FAULT_CODE_RGBLED[APP_EVENT_FAULT_CODE_RGBLED_LEN] =                          
#endif
                                                                                    {'5'};
#define APP_EVENT_FAULT_CODE_OVRCUR_LEN         1
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_FAULT_CODE_OVRCUR[APP_EVENT_FAULT_CODE_OVRCUR_LEN] =                          
#else
 const uint8 APP_EVENT_FAULT_CODE_OVRCUR[APP_EVENT_FAULT_CODE_OVRCUR_LEN] =                          
#endif
                                                                                    {'6'};
#define APP_EVENT_FAULT_CODE_RF_LEN         1
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_FAULT_CODE_RF[APP_EVENT_FAULT_CODE_RF_LEN] =                          
#else
 const uint8 APP_EVENT_FAULT_CODE_RF[APP_EVENT_FAULT_CODE_RF_LEN] =                          
#endif
#ifdef SW_COMM_RF_FAULT_6
                                                                                    {'6'};
#else
                                                                                    {'7'};
#endif                                                                                    

#define APP_EVENT_FAULT_CODE_TIP_LEN         1
#ifdef SW_UC_PIC18F
 const uint8 APP_EVENT_FAULT_CODE_TIP[APP_EVENT_FAULT_CODE_TIP_LEN] =                          
#else
 const uint8 APP_EVENT_FAULT_CODE_TIP[APP_EVENT_FAULT_CODE_TIP_LEN] =                          
#endif
                                                                                    {'8'};
//-----------------------------------------------------------------------------

#endif
//[] END OF FILE
