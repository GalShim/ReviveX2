/* ========================================
 *
 * Public codes definitions for Comm.c
 *
 * ========================================
*/

#ifndef _COMM_CODE_PUB_H_
#define _COMM_CODE_PUB_H_


//commands/replies content
typedef enum{
    COMM_TEXT_Tag_protocol = 0,
    COMM_TEXT_Tag_endprotocol,
    COMM_TEXT_Tag_command,
    COMM_TEXT_Tag_response,
    COMM_TEXT_Tag_event,
    COMM_TEXT_Tag_confirm,
    COMM_TEXT_Tag_rgbled,
    COMM_TEXT_Tag_motor1,
    COMM_TEXT_Tag_motor2,
    COMM_TEXT_Tag_module,
    COMM_TEXT_Tag_rfout,
    COMM_TEXT_Tag_vdtip,
    COMM_TEXT_Tag_tftip,
    COMM_TEXT_Tag_led,
    COMM_TEXT_Attrib_ver,
    COMM_TEXT_Attrib_name,
    COMM_TEXT_Attrib_mode,
    COMM_TEXT_Attrib_color,
    COMM_TEXT_Attrib_power_cmnd,
    COMM_TEXT_Attrib_on,
    COMM_TEXT_Attrib_off,
    COMM_TEXT_Attrib_onoff,
    COMM_TEXT_Attrib_power_event,
    COMM_TEXT_Attrib_led,
    COMM_TEXT_Attrib_code,
    COMM_TEXT_Attrib_deactivate,
    COMM_TEXT_Attrib_activate,
    COMM_TEXT_Attrib_id_config,
    COMM_TEXT_Attrib_type,
    COMM_TEXT_Attrib_id_module,
    COMM_TEXT_Attrib_status_config,
    COMM_TEXT_Attrib_status_module,
    COMM_TEXT_Attrib_reason,
    COMM_TEXT_Attrib_pulses,
    COMM_TEXT_Attrib_version,
    COMM_TEXT_Attrib_errorcode,
    COMM_TEXT_Attrib_intensity,
    COMM_TEXT_Attrib_lowlevel,
    COMM_TEXT_Attrib_highlevel,
    COMM_TEXT_Attrib_buzzerstatus,
    COMM_TEXT_Attrib_p1power,
    COMM_TEXT_Attrib_p1intensity,
    COMM_TEXT_Attrib_delay,
    COMM_TEXT_Attrib_p2power,
    COMM_TEXT_Attrib_p2intensity,
    COMM_TEXT_Attrib_state,
    COMM_TEXT_Cmnd_config,
    COMM_TEXT_Cmnd_loadparam,
    COMM_TEXT_Cmnd_activate,
    COMM_TEXT_Cmnd_deactivate,
    COMM_TEXT_Cmnd_synccom,
    COMM_TEXT_Cmnd_techmode,
    COMM_TEXT_Cmnd_tipstatus,
    COMM_TEXT_Cmnd_firepulse,
    COMM_TEXT_Cmnd_setled,
    COMM_TEXT_Resp_config,
    COMM_TEXT_Resp_loadparam,
    COMM_TEXT_Resp_activate,
    COMM_TEXT_Resp_deactivate,
    COMM_TEXT_Resp_synccom,
    COMM_TEXT_Resp_techmode,
    COMM_TEXT_Resp_tipstatus,
    COMM_TEXT_Event_buttons,
    COMM_TEXT_Event_fault,
    COMM_TEXT_Event_techinfo,
    COMM_TEXT_Event_pulsedone,
    COMM_TEXT_Event_tipinfo,
    COMM_TEXT_Event_bootup,
    COMM_TEXT_Confirm_buttons,
    COMM_TEXT_Confirm_fault,
    COMM_TEXT_Confirm_techinfo,
    COMM_TEXT_Confirm_pulsedone,
    COMM_TEXT_Confirm_tipinfo,
    COMM_TEXT_Confirm_bootup,
    COMM_TEXT_num,
    COMM_TEXT_fail = 0xfe,
    COMM_TEXT_NONE = 0xff
} COMM_TEXT_CODE_e;

#define COMM_TEXT_Tag_First     COMM_TEXT_Tag_protocol
#define COMM_TEXT_Tag_Last      COMM_TEXT_Tag_led //COMM_TEXT_Tag_tftip
#define COMM_TEXT_Attrib_First  COMM_TEXT_Attrib_ver
#define COMM_TEXT_Attrib_Last   COMM_TEXT_Attrib_state //COMM_TEXT_Attrib_p2intensity
#define COMM_TEXT_Cmnd_First    COMM_TEXT_Cmnd_config
#define COMM_TEXT_Cmnd_Last     COMM_TEXT_Cmnd_setled //COMM_TEXT_Cmnd_tipstatus 
#define COMM_TEXT_Resp_First    COMM_TEXT_Resp_config
#define COMM_TEXT_Resp_Last     COMM_TEXT_Resp_tipstatus
#define COMM_TEXT_Event_First   COMM_TEXT_Event_buttons
#define COMM_TEXT_Event_Last    COMM_TEXT_Event_bootup
#define COMM_TEXT_Confirm_First COMM_TEXT_Confirm_buttons
#define COMM_TEXT_Confirm_Last  COMM_TEXT_Confirm_bootup


typedef enum{
    COMM_ERROR_SUCCESS = 0,
    COMM_ERROR_FAIL
} COMM_ERROR_e;    






#endif
//[] END OF FILE
