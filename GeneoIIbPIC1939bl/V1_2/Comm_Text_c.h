/* ========================================
 *
 * text definitions for Comm.c
 *
 * ========================================
*/

#ifndef _COMM_TEXT_H_
#define _COMM_TEXT_H_


//Tag texts
  #define CommTxt_Tag_protocol_LEN 8
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_protocol[CommTxt_Tag_protocol_LEN] =                
#else
 const uint8 CommTxt_Tag_protocol[CommTxt_Tag_protocol_LEN] =                
#endif
                                                                            {'p','r','o','t','o','c','o','l'};
  #define CommTxt_Tag_endprotocol_LEN 9
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_endprotocol[CommTxt_Tag_endprotocol_LEN] =          
#else
 const uint8 CommTxt_Tag_endprotocol[CommTxt_Tag_endprotocol_LEN] =          
#endif
                                                                            {'/','p','r','o','t','o','c','o','l'};
  #define CommTxt_Tag_command_LEN 7
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_command[CommTxt_Tag_command_LEN] =                  
#else
 const uint8 CommTxt_Tag_command[CommTxt_Tag_command_LEN] =                  
#endif
                                                                            {'c','o','m','m','a','n','d'};
  #define CommTxt_Tag_response_LEN 8
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_response[CommTxt_Tag_response_LEN] =                
#else
 const uint8 CommTxt_Tag_response[CommTxt_Tag_response_LEN] =                
#endif
                                                                            {'r','e','s','p','o','n','s','e'};
  #define CommTxt_Tag_event_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_event[CommTxt_Tag_event_LEN] =                      
#else
 const uint8 CommTxt_Tag_event[CommTxt_Tag_event_LEN] =                      
#endif
                                                                            {'e','v','e','n','t'};
  #define CommTxt_Tag_confirm_LEN 7
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_confirm[CommTxt_Tag_confirm_LEN] =                  
#else
 const uint8 CommTxt_Tag_confirm[CommTxt_Tag_confirm_LEN] =                  
#endif
                                                                            {'c','o','n','f','i','r','m'};
  #define CommTxt_Tag_rgbled_LEN 6
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_rgbled[CommTxt_Tag_rgbled_LEN] =                    
#else
 const uint8 CommTxt_Tag_rgbled[CommTxt_Tag_rgbled_LEN] =                    
#endif
                                                                            {'r','g','b','l','e','d'};
  #define CommTxt_Tag_motor1_LEN 6
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_motor1[CommTxt_Tag_motor1_LEN] =                    
#else
 const uint8 CommTxt_Tag_motor1[CommTxt_Tag_motor1_LEN] =                    
#endif
                                                                            {'m','o','t','o','r','X'};
  #define CommTxt_Tag_motor2_LEN 6
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_motor2[CommTxt_Tag_motor2_LEN] =                    
#else
 const uint8 CommTxt_Tag_motor2[CommTxt_Tag_motor2_LEN] =                    
#endif
                                                                            {'m','o','t','o','r','Y'};
  #define CommTxt_Tag_module_LEN 6
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_module[CommTxt_Tag_module_LEN] =                    
#else
 const uint8 CommTxt_Tag_module[CommTxt_Tag_module_LEN] =                    
#endif
                                                                            {'m','o','d','u','l','e'};
  #define CommTxt_Tag_rfout_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_rfout[CommTxt_Tag_rfout_LEN] =                    
#else
 const uint8 CommTxt_Tag_rfout[CommTxt_Tag_rfout_LEN] =                    
#endif
//#ifdef SW_PROD_Us
                                                                            {'u','s','o','u','t'};
//#else
//#ifdef SW_PROD_ReviveRf
//                                                                            {'u','s','o','u','t'};
//#else
//                                                                            {'r','f','o','u','t'};
//#endif

  #define CommTxt_Tag_vdtip_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_vdtip[CommTxt_Tag_vdtip_LEN] =                    
#else
 const uint8 CommTxt_Tag_vdtip[CommTxt_Tag_vdtip_LEN] =                    
#endif
                                                                            {'v','d','t','i','p'};
  #define CommTxt_Tag_tftip_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_tftip[CommTxt_Tag_tftip_LEN] =                    
#else
 const uint8 CommTxt_Tag_tftip[CommTxt_Tag_tftip_LEN] =                    
#endif
                                                                            {'t','f','t','i','p'};
  #define CommTxt_Tag_led_LEN 3
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Tag_led[CommTxt_Tag_led_LEN] =                    
#else
 const uint8 CommTxt_Tag_led[CommTxt_Tag_led_LEN] =                    
#endif
                                                                            {'l','e','d'};



//Attribute texts
  #define CommTxt_Attrib_ver_LEN 3
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_ver[CommTxt_Attrib_ver_LEN] =                    
#else
 const uint8 CommTxt_Attrib_ver[CommTxt_Attrib_ver_LEN] =                    
#endif
                                                                            {'v','e','r'};
  #define CommTxt_Attrib_name_LEN 4
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_name[CommTxt_Attrib_name_LEN] =                  
#else
 const uint8 CommTxt_Attrib_name[CommTxt_Attrib_name_LEN] =                  
#endif
                                                                            {'n','a','m','e'};
  #define CommTxt_Attrib_mode_LEN 4
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_mode[CommTxt_Attrib_mode_LEN] =                  
#else
 const uint8 CommTxt_Attrib_mode[CommTxt_Attrib_mode_LEN] =                  
#endif
                                                                            {'m','o','d','e'};
  #define CommTxt_Attrib_color_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_color[CommTxt_Attrib_color_LEN] =                
#else
 const uint8 CommTxt_Attrib_color[CommTxt_Attrib_color_LEN] =                
#endif
                                                                            {'c','o','l','o','r'};
  #define CommTxt_Attrib_power_cmnd_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_power_cmnd[CommTxt_Attrib_power_cmnd_LEN] =      
#else
 const uint8 CommTxt_Attrib_power_cmnd[CommTxt_Attrib_power_cmnd_LEN] =      
#endif
                                                                            {'p','o','w','e','r'};
  #define CommTxt_Attrib_on_LEN 2
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_on[CommTxt_Attrib_on_LEN] =                      
#else
 const uint8 CommTxt_Attrib_on[CommTxt_Attrib_on_LEN] =                      
#endif
                                                                            {'o','n'};
  #define CommTxt_Attrib_off_LEN 3
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_off[CommTxt_Attrib_off_LEN] =                    
#else
 const uint8 CommTxt_Attrib_off[CommTxt_Attrib_off_LEN] =                    
#endif
                                                                            {'o','f','f'};
  #define CommTxt_Attrib_onoff_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_onoff[CommTxt_Attrib_onoff_LEN] =                
#else
 const uint8 CommTxt_Attrib_onoff[CommTxt_Attrib_onoff_LEN] =                
#endif
                                                                            {'o','n','o','f','f'};
  #define CommTxt_Attrib_power_event_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_power_event[CommTxt_Attrib_power_event_LEN] =    
#else
 const uint8 CommTxt_Attrib_power_event[CommTxt_Attrib_power_event_LEN] =    
#endif
                                                                            {'p','o','w','e','r'};
  #define CommTxt_Attrib_led_LEN 3
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_led[CommTxt_Attrib_led_LEN] =                    
#else
 const uint8 CommTxt_Attrib_led[CommTxt_Attrib_led_LEN] =                    
#endif
                                                                            {'l','e','d'};
  #define CommTxt_Attrib_code_LEN 4
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_code[CommTxt_Attrib_code_LEN] =                  
#else
 const uint8 CommTxt_Attrib_code[CommTxt_Attrib_code_LEN] =                  
#endif
                                                                            {'c','o','d','e'};
  #define CommTxt_Attrib_deactivate_LEN 10
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_deactivate[CommTxt_Attrib_deactivate_LEN] =      
#else
 const uint8 CommTxt_Attrib_deactivate[CommTxt_Attrib_deactivate_LEN] =      
#endif
                                                                            {'d','e','a','c','t','i','v','a','t','e'};
  #define CommTxt_Attrib_id_config_LEN 2
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_id_config[CommTxt_Attrib_id_config_LEN] =        
#else
 const uint8 CommTxt_Attrib_id_config[CommTxt_Attrib_id_config_LEN] =        
#endif
                                                                            {'i','d'};
  #define CommTxt_Attrib_type_LEN 4
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_type[CommTxt_Attrib_type_LEN] =                  
#else
 const uint8 CommTxt_Attrib_type[CommTxt_Attrib_type_LEN] =                  
#endif
                                                                            {'t','y','p','e'};
  #define CommTxt_Attrib_id_module_LEN 2
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_id_module[CommTxt_Attrib_id_module_LEN] =        
#else
 const uint8 CommTxt_Attrib_id_module[CommTxt_Attrib_id_module_LEN] =        
#endif
                                                                            {'i','d'};
  #define CommTxt_Attrib_status_config_LEN 6
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_status_config[CommTxt_Attrib_status_config_LEN] =
#else
 const uint8 CommTxt_Attrib_status_config[CommTxt_Attrib_status_config_LEN] =
#endif
                                                                            {'s','t','a','t','u','s'};
  #define CommTxt_Attrib_status_module_LEN 6
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_status_module[CommTxt_Attrib_status_module_LEN] =
#else
 const uint8 CommTxt_Attrib_status_module[CommTxt_Attrib_status_module_LEN] =
#endif
                                                                            {'s','t','a','t','u','s'};
  #define CommTxt_Attrib_reason_LEN 6
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_reason[CommTxt_Attrib_reason_LEN] =              
#else
 const uint8 CommTxt_Attrib_reason[CommTxt_Attrib_reason_LEN] =              
#endif
                                                                            {'r','e','a','s','o','n'};
  #define CommTxt_Attrib_pulses_LEN 6
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_pulses[CommTxt_Attrib_pulses_LEN] =              
#else
 const uint8 CommTxt_Attrib_pulses[CommTxt_Attrib_pulses_LEN] =              
#endif
                                                                            {'p','u','l','s','e','s'};
  #define CommTxt_Attrib_version_LEN 7
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_version[CommTxt_Attrib_version_LEN] =            
#else
 const uint8 CommTxt_Attrib_version[CommTxt_Attrib_version_LEN] =            
#endif
                                                                            {'v','e','r','s','i','o','n'};
  #define CommTxt_Attrib_errorcode_LEN 9
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_errorcode[CommTxt_Attrib_errorcode_LEN] =        
#else
 const uint8 CommTxt_Attrib_errorcode[CommTxt_Attrib_errorcode_LEN] =        
#endif
                                                                            {'e','r','r','o','r','c','o','d','e'};
  #define CommTxt_Attrib_intensity_LEN 9
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_intensity[CommTxt_Attrib_intensity_LEN] =        
#else
 const uint8 CommTxt_Attrib_intensity[CommTxt_Attrib_intensity_LEN] =        
#endif
                                                                            {'i','n','t','e','n','s','i','t','y'};
  #define CommTxt_Attrib_lowlevel_LEN 8
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_lowlevel[CommTxt_Attrib_lowlevel_LEN] =        
#else
 const uint8 CommTxt_Attrib_lowlevel[CommTxt_Attrib_lowlevel_LEN] =        
#endif
                                                                            {'l','o','w','l','e','v','e','l'};
  #define CommTxt_Attrib_highlevel_LEN 9
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_highlevel[CommTxt_Attrib_highlevel_LEN] =        
#else
 const uint8 CommTxt_Attrib_highlevel[CommTxt_Attrib_highlevel_LEN] =        
#endif
                                                                            {'h','i','g','h','l','e','v','e','l'};
  #define CommTxt_Attrib_buzzerstatus_LEN 12
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_buzzerstatus[CommTxt_Attrib_buzzerstatus_LEN] =        
#else
 const uint8 CommTxt_Attrib_buzzerstatus[CommTxt_Attrib_buzzerstatus_LEN] =        
#endif
                                                                            {'b','u','z','z','e','r','s','t','a','t','u','s'};
  #define CommTxt_Attrib_p1power_LEN 7
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_p1power[CommTxt_Attrib_p1power_LEN] =        
#else
 const uint8 CommTxt_Attrib_p1power[CommTxt_Attrib_p1power_LEN] =        
#endif
                                                                            {'p','1','p','o','w','e','r'};
  #define CommTxt_Attrib_p1intensity_LEN 11
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_p1intensity[CommTxt_Attrib_p1intensity_LEN] =        
#else
 const uint8 CommTxt_Attrib_p1intensity[CommTxt_Attrib_p1intensity_LEN] =        
#endif
                                                                            {'p','1','i','n','t','e','n','s','i','t','y'};
  #define CommTxt_Attrib_delay_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_delay[CommTxt_Attrib_delay_LEN] =        
#else
 const uint8 CommTxt_Attrib_delay[CommTxt_Attrib_delay_LEN] =        
#endif
                                                                            {'d','e','l','a','y'};
  #define CommTxt_Attrib_p2power_LEN 7
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_p2power[CommTxt_Attrib_p2power_LEN] =        
#else
 const uint8 CommTxt_Attrib_p2power[CommTxt_Attrib_p2power_LEN] =        
#endif
                                                                            {'p','2','p','o','w','e','r'};
  #define CommTxt_Attrib_p2intensity_LEN 11
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_p2intensity[CommTxt_Attrib_p2intensity_LEN] =        
#else
 const uint8 CommTxt_Attrib_p2intensity[CommTxt_Attrib_p2intensity_LEN] =        
#endif
                                                                            {'p','2','i','n','t','e','n','s','i','t','y'};
  #define CommTxt_Attrib_state_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Attrib_state[CommTxt_Attrib_state_LEN] =        
#else
 const uint8 CommTxt_Attrib_state[CommTxt_Attrib_state_LEN] =        
#endif
                                                                            {'s','t','a','t','e'};

//Cmnd texts
  #define CommTxt_Cmnd_config_LEN 6
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Cmnd_config[CommTxt_Cmnd_config_LEN] =                  
#else
 const uint8 CommTxt_Cmnd_config[CommTxt_Cmnd_config_LEN] =                  
#endif
                                                                            {'c','o','n','f','i','g'};
  #define CommTxt_Cmnd_loadparam_LEN 9
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Cmnd_loadparam[CommTxt_Cmnd_loadparam_LEN] =            
#else
 const uint8 CommTxt_Cmnd_loadparam[CommTxt_Cmnd_loadparam_LEN] =            
#endif
                                                                            {'l','o','a','d','p','a','r','a','m'};
  #define CommTxt_Cmnd_activate_LEN 8
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Cmnd_activate[CommTxt_Cmnd_activate_LEN] =              
#else
 const uint8 CommTxt_Cmnd_activate[CommTxt_Cmnd_activate_LEN] =              
#endif
                                                                            {'a','c','t','i','v','a','t','e'};
  #define CommTxt_Cmnd_deactivate_LEN 10
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Cmnd_deactivate[CommTxt_Cmnd_deactivate_LEN] =          
#else
 const uint8 CommTxt_Cmnd_deactivate[CommTxt_Cmnd_deactivate_LEN] =          
#endif
                                                                            {'d','e','a','c','t','i','v','a','t','e'};
  #define CommTxt_Cmnd_synccom_LEN 7
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Cmnd_synccom[CommTxt_Cmnd_synccom_LEN] =                
#else
 const uint8 CommTxt_Cmnd_synccom[CommTxt_Cmnd_synccom_LEN] =                
#endif
                                                                            {'s','y','n','c','c','o','m'};
  #define CommTxt_Cmnd_techmode_LEN 8
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Cmnd_techmode[CommTxt_Cmnd_techmode_LEN] =              
#else
 const uint8 CommTxt_Cmnd_techmode[CommTxt_Cmnd_techmode_LEN] =              
#endif
                                                                            {'t','e','c','h','m','o','d','e'};
  #define CommTxt_Cmnd_tipstatus_LEN 9
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Cmnd_tipstatus[CommTxt_Cmnd_tipstatus_LEN] =              
#else
 const uint8 CommTxt_Cmnd_tipstatus[CommTxt_Cmnd_tipstatus_LEN] =              
#endif
                                                                            {'t','i','p','s','t','a','t','u','s'};
  #define CommTxt_Cmnd_firepulse_LEN 9
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Cmnd_firepulse[CommTxt_Cmnd_firepulse_LEN] =              
#else
 const uint8 CommTxt_Cmnd_firepulse[CommTxt_Cmnd_firepulse_LEN] =              
#endif
                                                                            {'f','i','r','e','p','u','l','s','e'};
  #define CommTxt_Cmnd_setled_LEN 6
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Cmnd_setled[CommTxt_Cmnd_setled_LEN] =              
#else
 const uint8 CommTxt_Cmnd_setled[CommTxt_Cmnd_setled_LEN] =              
#endif
                                                                            {'s','e','t','l','e','d'};


//Event texts
      #define CommTxt_Event_buttons_LEN 7
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Event_buttons[CommTxt_Event_buttons_LEN] =          
#else
 const uint8 CommTxt_Event_buttons[CommTxt_Event_buttons_LEN] =          
#endif
                                                                            {'b','u','t','t','o','n','s'};
      #define CommTxt_Event_fault_LEN 5
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Event_fault[CommTxt_Event_fault_LEN] =              
#else
 const uint8 CommTxt_Event_fault[CommTxt_Event_fault_LEN] =              
#endif
                                                                            {'f','a','u','l','t'};
      #define CommTxt_Event_techinfo_LEN 8
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Event_techinfo[CommTxt_Event_techinfo_LEN] =        
#else
 const uint8 CommTxt_Event_techinfo[CommTxt_Event_techinfo_LEN] =        
#endif
                                                                            {'t','e','c','h','i','n','f','o'};
      #define CommTxt_Event_pulsedone_LEN 9
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Event_pulsedone[CommTxt_Event_pulsedone_LEN] =        
#else
 const uint8 CommTxt_Event_pulsedone[CommTxt_Event_pulsedone_LEN] =        
#endif
                                                                            {'p','u','l','s','e','d','o','n','e'};
      #define CommTxt_Event_tipinfo_LEN 7
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Event_tipinfo[CommTxt_Event_tipinfo_LEN] =        
#else
 const uint8 CommTxt_Event_tipinfo[CommTxt_Event_tipinfo_LEN] =        
#endif
                                                                            {'t','i','p','i','n','f','o'};
      #define CommTxt_Event_bootup_LEN 4
#ifdef SW_UC_PIC18F
 const rom uint8 CommTxt_Event_bootup[CommTxt_Event_bootup_LEN] =        
#else
 const uint8 CommTxt_Event_bootup[CommTxt_Event_bootup_LEN] =        
#endif
                                                                            {'b','o','o','t'};
#define APP_TECHINFO_PARAM_ID_TEMP_NA_LEN          2
#ifdef SW_UC_PIC18F
 const rom uint8 APP_TECHINFO_PARAM_ID_TEMP_NA[APP_TECHINFO_PARAM_ID_TEMP_NA_LEN] =        
#else
 const uint8 APP_TECHINFO_PARAM_ID_TEMP_NA[APP_TECHINFO_PARAM_ID_TEMP_NA_LEN] =        
#endif
                                                                                    {'N','A'};

#ifdef SW_UC_PIC18F
 const rom struct{
    uint8   Code;                   //Tag's code
    const rom uint8*  TextAddr;     //Address of the text
    uint8   TextLen;                //Length of the text
#else
 const struct{
    uint8   Code;                   //Tag's code
    const uint8*  TextAddr;         //Address of the text
    uint8   TextLen;                //Length of the text
#endif
} CommTextTable [COMM_TEXT_num] = {
    {COMM_TEXT_Tag_protocol,        &CommTxt_Tag_protocol[0],       CommTxt_Tag_protocol_LEN},
    {COMM_TEXT_Tag_endprotocol,     &CommTxt_Tag_endprotocol[0],    CommTxt_Tag_endprotocol_LEN},
    {COMM_TEXT_Tag_command,         &CommTxt_Tag_command[0],        CommTxt_Tag_command_LEN},
    {COMM_TEXT_Tag_response,        &CommTxt_Tag_response[0],       CommTxt_Tag_response_LEN},
    {COMM_TEXT_Tag_event,           &CommTxt_Tag_event[0],          CommTxt_Tag_event_LEN},
    {COMM_TEXT_Tag_confirm,         &CommTxt_Tag_confirm[0],        CommTxt_Tag_confirm_LEN},
    {COMM_TEXT_Tag_rgbled,          &CommTxt_Tag_rgbled[0],         CommTxt_Tag_rgbled_LEN},
    {COMM_TEXT_Tag_motor1,          &CommTxt_Tag_motor1[0],         CommTxt_Tag_motor1_LEN},
    {COMM_TEXT_Tag_motor2,          &CommTxt_Tag_motor2[0],         CommTxt_Tag_motor2_LEN},
    {COMM_TEXT_Tag_module,          &CommTxt_Tag_module[0],         CommTxt_Tag_module_LEN},
    {COMM_TEXT_Tag_rfout,           &CommTxt_Tag_rfout[0],          CommTxt_Tag_rfout_LEN},
    {COMM_TEXT_Tag_vdtip,           &CommTxt_Tag_vdtip[0],          CommTxt_Tag_vdtip_LEN},
    {COMM_TEXT_Tag_tftip,           &CommTxt_Tag_tftip[0],          CommTxt_Tag_tftip_LEN},
    {COMM_TEXT_Tag_led,           	&CommTxt_Tag_led[0],          	CommTxt_Tag_led_LEN},
    {COMM_TEXT_Attrib_ver,          &CommTxt_Attrib_ver[0],         CommTxt_Attrib_ver_LEN},
    {COMM_TEXT_Attrib_name,         &CommTxt_Attrib_name[0],        CommTxt_Attrib_name_LEN},
    {COMM_TEXT_Attrib_mode,         &CommTxt_Attrib_mode[0],        CommTxt_Attrib_mode_LEN},
    {COMM_TEXT_Attrib_color,        &CommTxt_Attrib_color[0],       CommTxt_Attrib_color_LEN},
    {COMM_TEXT_Attrib_power_cmnd,   &CommTxt_Attrib_power_cmnd[0],  CommTxt_Attrib_power_cmnd_LEN},
    {COMM_TEXT_Attrib_on,           &CommTxt_Attrib_on[0],          CommTxt_Attrib_on_LEN},
    {COMM_TEXT_Attrib_off,          &CommTxt_Attrib_off[0],         CommTxt_Attrib_off_LEN},
    {COMM_TEXT_Attrib_onoff,        &CommTxt_Attrib_onoff[0],       CommTxt_Attrib_onoff_LEN},
    {COMM_TEXT_Attrib_power_event,  &CommTxt_Attrib_power_event[0], CommTxt_Attrib_power_event_LEN},
    {COMM_TEXT_Attrib_led,          &CommTxt_Attrib_led[0],         CommTxt_Attrib_led_LEN},
    {COMM_TEXT_Attrib_code,         &CommTxt_Attrib_code[0],        CommTxt_Attrib_code_LEN},
    {COMM_TEXT_Attrib_deactivate,   &CommTxt_Attrib_deactivate[0],  CommTxt_Attrib_deactivate_LEN},
    {COMM_TEXT_Attrib_activate,     &CommTxt_Attrib_deactivate[0],  CommTxt_Attrib_deactivate_LEN},     //Same text as deactivate
    {COMM_TEXT_Attrib_id_config,    &CommTxt_Attrib_id_config[0],   CommTxt_Attrib_id_config_LEN},
    {COMM_TEXT_Attrib_type,         &CommTxt_Attrib_type[0],        CommTxt_Attrib_type_LEN},
    {COMM_TEXT_Attrib_id_module,    &CommTxt_Attrib_id_module[0],   CommTxt_Attrib_id_module_LEN},
    {COMM_TEXT_Attrib_status_config,&CommTxt_Attrib_status_config[0],CommTxt_Attrib_status_config_LEN},
    {COMM_TEXT_Attrib_status_module,&CommTxt_Attrib_status_module[0],CommTxt_Attrib_status_module_LEN},
    {COMM_TEXT_Attrib_reason,       &CommTxt_Attrib_reason[0],      CommTxt_Attrib_reason_LEN},
    {COMM_TEXT_Attrib_pulses,       &CommTxt_Attrib_pulses[0],      CommTxt_Attrib_pulses_LEN},
    {COMM_TEXT_Attrib_version,      &CommTxt_Attrib_version[0],     CommTxt_Attrib_version_LEN},
    {COMM_TEXT_Attrib_errorcode,    &CommTxt_Attrib_errorcode[0],   CommTxt_Attrib_errorcode_LEN},
    {COMM_TEXT_Attrib_intensity,    &CommTxt_Attrib_intensity[0],   CommTxt_Attrib_intensity_LEN},
    {COMM_TEXT_Attrib_lowlevel,     &CommTxt_Attrib_lowlevel[0],    CommTxt_Attrib_lowlevel_LEN},
    {COMM_TEXT_Attrib_highlevel,    &CommTxt_Attrib_highlevel[0],   CommTxt_Attrib_highlevel_LEN},
    {COMM_TEXT_Attrib_buzzerstatus, &CommTxt_Attrib_buzzerstatus[0],CommTxt_Attrib_buzzerstatus_LEN},
    {COMM_TEXT_Attrib_p1power,      &CommTxt_Attrib_p1power[0],     CommTxt_Attrib_p1power_LEN},
    {COMM_TEXT_Attrib_p1intensity,  &CommTxt_Attrib_p1intensity[0], CommTxt_Attrib_p1intensity_LEN},
    {COMM_TEXT_Attrib_delay,        &CommTxt_Attrib_delay[0],       CommTxt_Attrib_delay_LEN},
    {COMM_TEXT_Attrib_p2power,      &CommTxt_Attrib_p2power[0],     CommTxt_Attrib_p2power_LEN},
    {COMM_TEXT_Attrib_p2intensity,  &CommTxt_Attrib_p2intensity[0], CommTxt_Attrib_p2intensity_LEN},
    {COMM_TEXT_Attrib_state,  		&CommTxt_Attrib_state[0], 		CommTxt_Attrib_state_LEN},
    //{COMM_TEXT_Attrib_tipstatus,    &CommTxt_Attrib_status_config[0],CommTxt_Attrib_status_config_LEN},
    //{COMM_TEXT_Attrib_tipid,        &CommTxt_Attrib_id_config[0],   CommTxt_Attrib_id_config_LEN},
    //{COMM_TEXT_Attrib_tippulses,    &CommTxt_Attrib_pulses[0],      CommTxt_Attrib_pulses_LEN},
    {COMM_TEXT_Cmnd_config,         &CommTxt_Cmnd_config[0],        CommTxt_Cmnd_config_LEN},
    {COMM_TEXT_Cmnd_loadparam,      &CommTxt_Cmnd_loadparam[0],     CommTxt_Cmnd_loadparam_LEN},
    {COMM_TEXT_Cmnd_activate,       &CommTxt_Cmnd_activate[0],      CommTxt_Cmnd_activate_LEN},
    {COMM_TEXT_Cmnd_deactivate,     &CommTxt_Cmnd_deactivate[0],    CommTxt_Cmnd_deactivate_LEN},
    {COMM_TEXT_Cmnd_synccom,        &CommTxt_Cmnd_synccom[0],       CommTxt_Cmnd_synccom_LEN},
    {COMM_TEXT_Cmnd_techmode,       &CommTxt_Cmnd_techmode[0],      CommTxt_Cmnd_techmode_LEN},
    {COMM_TEXT_Cmnd_tipstatus,      &CommTxt_Cmnd_tipstatus[0],     CommTxt_Cmnd_tipstatus_LEN},
    {COMM_TEXT_Cmnd_firepulse,		&CommTxt_Cmnd_firepulse[0],     CommTxt_Cmnd_firepulse_LEN},
    {COMM_TEXT_Cmnd_setled,      	&CommTxt_Cmnd_setled[0],     	CommTxt_Cmnd_setled_LEN},
    {COMM_TEXT_Resp_config,         &CommTxt_Cmnd_config[0],        CommTxt_Cmnd_config_LEN},
    {COMM_TEXT_Resp_loadparam,      &CommTxt_Cmnd_loadparam[0],     CommTxt_Cmnd_loadparam_LEN},
    {COMM_TEXT_Resp_activate,       &CommTxt_Cmnd_activate[0],      CommTxt_Cmnd_activate_LEN},
    {COMM_TEXT_Resp_deactivate,     &CommTxt_Cmnd_deactivate[0],    CommTxt_Cmnd_deactivate_LEN},
    {COMM_TEXT_Resp_synccom,        &CommTxt_Cmnd_synccom[0],       CommTxt_Cmnd_synccom_LEN},
    {COMM_TEXT_Resp_techmode,       &CommTxt_Cmnd_techmode[0],      CommTxt_Cmnd_techmode_LEN},
    {COMM_TEXT_Resp_tipstatus,      &CommTxt_Cmnd_tipstatus[0],     CommTxt_Cmnd_tipstatus_LEN},
    {COMM_TEXT_Event_buttons,       &CommTxt_Event_buttons[0],      CommTxt_Event_buttons_LEN},
    {COMM_TEXT_Event_fault,         &CommTxt_Event_fault[0],        CommTxt_Event_fault_LEN},
    {COMM_TEXT_Event_techinfo,      &CommTxt_Event_techinfo[0],     CommTxt_Event_techinfo_LEN},
    {COMM_TEXT_Event_pulsedone,     &CommTxt_Event_pulsedone[0],    CommTxt_Event_pulsedone_LEN},
    {COMM_TEXT_Event_tipinfo,       &CommTxt_Event_tipinfo[0],      CommTxt_Event_tipinfo_LEN},
    {COMM_TEXT_Event_bootup,        &CommTxt_Event_bootup[0],       CommTxt_Event_bootup_LEN},
    {COMM_TEXT_Confirm_buttons,     &CommTxt_Event_buttons[0],      CommTxt_Event_buttons_LEN},
    {COMM_TEXT_Confirm_fault,       &CommTxt_Event_fault[0],        CommTxt_Event_fault_LEN},
    {COMM_TEXT_Confirm_techinfo,    &CommTxt_Event_techinfo[0],     CommTxt_Event_techinfo_LEN},
    {COMM_TEXT_Confirm_pulsedone,   &CommTxt_Event_pulsedone[0],    CommTxt_Event_pulsedone_LEN},
    {COMM_TEXT_Confirm_tipinfo,     &CommTxt_Event_tipinfo[0],      CommTxt_Event_tipinfo_LEN},
    {COMM_TEXT_Confirm_bootup,      &CommTxt_Event_bootup[0],       CommTxt_Event_bootup_LEN}
};    


#endif
//[] END OF FILE
