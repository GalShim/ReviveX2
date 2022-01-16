/* ===================================================================================================================
File Name:   Keys_Pub.h
Description: Public definitions for Keys handler
====================================================================================================================*/
#ifndef __KEYS_PUB_H__
#define __KEYS_PUB_H__
//=============================================================================
//=============================================================================
//Hardware functions
//=============================================================================
//=============================================================================
    #ifdef SW_UC_PIC16F
        #define KEYS_NUM 4
        #define KEY_MASK_UP             0x02                //Keys bits
        #define KEY_MASK_DOWN           0x01
        #define KEY_MASK_START          0x08
        #define KEY_MASK_LIGHT          0x04
        #define KEY_READ_PORT           ((PORTD>>3)&0x0f)   //Read keys from port
        #define KEY_INIT_PORT           {TRISD|=0x78;ANSELD=0;} //Init ports
    #endif
    #ifdef SW_UC_PIC18F
        #define KEYS_NUM 4
        #define KEY_MASK_UP             0x02                //Keys bits (only Start exists. the rest are dummy)
        #define KEY_MASK_DOWN           0x08
        #define KEY_MASK_START          0x01
        #define KEY_MASK_LIGHT          0x04
        #define KEY_READ_PORT           ((PORTB&0x01)|0x0e) //Read keys from port (only one key. The rest - set to not active)
        #define KEY_INIT_PORT           {TRISBbits.TRISB0=1;ANSELBbits.ANSB0=0;} //Init ports
    #endif
//=============================================================================
//=============================================================================

#define KEYS_SHIFT_STABLE 12

//Keys states
#define KEY_MASK_STABLE         (((uint16)KEY_MASK_UP+(uint16)KEY_MASK_DOWN+(uint16)KEY_MASK_START+(uint16)KEY_MASK_LIGHT)<<KEYS_SHIFT_STABLE)
#define KEY_MASK_STABLE_START_SETTING  (((uint16)KEY_MASK_UP+(uint16)KEY_MASK_DOWN)<<KEYS_SHIFT_STABLE)
#define KEY_MASK_UP_EVENT             ((uint16)KEY_MASK_UP<<KEYS_SHIFT_STABLE)
#define KEY_MASK_DOWN_EVENT           ((uint16)KEY_MASK_DOWN<<KEYS_SHIFT_STABLE)
#define KEY_MASK_START_EVENT          ((uint16)KEY_MASK_START<<KEYS_SHIFT_STABLE)
#define KEY_MASK_LIGHT_EVENT          ((uint16)KEY_MASK_LIGHT<<KEYS_SHIFT_STABLE)

//Functions
extern uint16 Keys_read_keys_state (uint16 Mask);
extern uint16 Keys_read_keys_new_event (uint16 Mask);
extern void Keys_TaskInit (void);
extern void Keys_TaskMain (void);
extern void Keys_1mS_tick(void);



#endif //__SHADE_KEYS_PUB_H__
