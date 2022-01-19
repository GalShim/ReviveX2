/* ========================================
 *
 * General definitions
 *
 * ========================================
*/

#ifndef _GENERAL_H_
#define _GENERAL_H_


//***********************************************************************************
//***********************************************************************************
//Applicator type switches
//***********************************************************************************
//***********************************************************************************
                                        //Select applicator type
//#define SW_PROD_GeneoII
//#define SW_PROD_BLDC
//#define SW_PROD_Rf
//#define SW_PROD_Us
#define SW_PROD_ReviveRf
//#define SW_PROD_ReviveRf_RepeatPulse	//Just for test. Repeats pulses. 2020-09-30
//#define SW_PROD_ReviveUs

#define SW_DEBUG_GLOBAL_ENABLE          //Global enable debug switches

#ifdef SW_PROD_GeneoII
    #define SW_UC_PIC16F                //PIC16F uController
    //#define SW_UC_PIC18F                //PIC18F uController
    //#define SW_MOT_BLDC                 //BLDC motor applicator
    #define SW_MOT_X                    //Motor X exists
    #define SW_MOT_Y                    //Motor Y exists
    //#define SW_MOT_RF                   //RF exists (in motor task)
    #define SW_LED_RGB_VAR              //Variable intensity RGB LED exists
    //#define SW_LED_RGB_FIXED            //Fixed intensity RGB LED exists
    //#define SW_SENS_TEMP                //Temperature sensor exists
    //#define SW_JUMPER_PROD_TEST         //Jumper for production test
#endif

#ifdef SW_PROD_BLDC
    #define SW_UC_PIC16F                //PIC16F uController
    //#define SW_UC_PIC18F                //PIC18F uController
    #define SW_UC_INCLUDE   
    #define SW_MOT_BLDC                 //BLDC motor applicator
    #define SW_MOT_X                    //Motor X exists
    //#define SW_MOT_Y                    //Motor Y exists
    //#define SW_MOT_RF                   //RF exists (in motor task)
    #define SW_LED_RGB_VAR              //Variable intensity RGB LED exists
    //#define SW_LED_RGB_FIXED            //Fixed intensity RGB LED exists
    //#define SW_SENS_TEMP                //Temperature sensor exists
    //#define SW_JUMPER_PROD_TEST         //Jumper for production test
    #define SW_APP_OVER_CURRENT_FAULT   //Generate fault event in over current
#endif

#ifdef SW_PROD_Rf
    //#define SW_UC_PIC16F                //PIC16F uController
    #define SW_UC_PIC18F                //PIC18F uController
    //#define SW_UC_DEVBOARD              //PIC18F46K22 in development board
    //#define SW_MOT_BLDC                 //BLDC motor applicator
    //#define SW_MOT_X                    //Motor X exists
    //#define SW_MOT_Y                    //Motor Y exists
    #define SW_MOT_RF                   //RF exists (in motor task)
    //#define SW_LED_RGB_VAR              //Variable intensity RGB LED exists
    #define SW_LED_RGB_FIXED            //Fixed intensity RGB LED exists
    #define SW_SENS_TEMP                //Temperature sensor exists
    #define SW_JUMPER_PROD_TEST         //Jumper for production test
    #define SW_PLL_ON                   //Use PLL for internal clock
    //#define SW_TICK_100US               //100uS tick, instead of 1mS
    #define SW_MOT_RF_THREE_QUARTERS_POWER //Set power to 3/4 of the value in loadparam
    //#define SW_MOT_RF_HALF_POWER        //Set power to half of the value in loadparam
    //#define SW_MOT_RF_THIRD_POWER        //Set power to one third of the value in loadparam
    #define SW_ROM_CHECK                //Check ROM
    #define SW_ROM_ADDR_START   0x0004  //ROM start address (skip jump to BootLoader)
    #define SW_ROM_ADDR_END     0x7BFE  //ROM end address (not include BootLoader code, because already passed it)
    #define SW_ROM_CHECKSUM     0x77    //ROM checksum byte
    #define SW_STARTUP_BIT              //Execute StartUp BIT
    //#define SW_CONFIG_ONLY_XINST        //Set only config byte with XINST
    //#define SW_ADC_CHECK_I_ON_RF        //Check supply current only when RF is active
    //#define SW_PATCH_SYNC_OVERV         //Overvoltage BIT on first Sync
    //#define SW_APP_OVER_CURRENT_FAULT   //Generate fault event in over current
#endif

#ifdef SW_PROD_Us
    //#define SW_UC_PIC16F                //PIC16F uController
    #define SW_UC_PIC18F                //PIC18F uController
    //#define SW_UC_DEVBOARD              //PIC18F46K22 in development board
    //#define SW_MOT_BLDC                 //BLDC motor applicator
    //#define SW_MOT_X                    //Motor X exists
    //#define SW_MOT_Y                    //Motor Y exists
    #define SW_MOT_RF                   //RF exists (in motor task). Note: Used also in US applicator
    #define SW_MOT_US                   //US applicator
    //#define SW_LED_RGB_VAR              //Variable intensity RGB LED exists
    #define SW_LED_RGB_FIXED            //Fixed intensity RGB LED exists
    #define SW_SENS_TEMP                //Temperature sensor exists
    #define SW_JUMPER_PROD_TEST         //Jumper for production test
    #define SW_PLL_ON                   //Use PLL for internal clock
    //#define SW_TICK_100US               //100uS tick, instead of 1mS
    #define SW_MOT_RF_FULL_POWER        //Set power to the value in loadparam
    //#define SW_MOT_RF_THREE_QUARTERS_POWER //Set power to 3/4 of the value in loadparam
    //#define SW_MOT_RF_HALF_POWER        //Set power to half of the value in loadparam
    //#define SW_MOT_RF_THIRD_POWER        //Set power to one third of the value in loadparam
    #define SW_ROM_CHECK                //Check ROM
    #define SW_ROM_ADDR_START   0x0004  //ROM start address (skip jump to BootLoader)
    #define SW_ROM_ADDR_END     0x7BFE  //ROM end address (not include BootLoader code, because already passed it)
    #define SW_ROM_CHECKSUM     0x94    //ROM checksum byte
    #define SW_STARTUP_BIT              //Execute StartUp BIT
    //#define SW_CONFIG_ONLY_XINST        //Set only config byte with XINST
    //#define SW_ADC_CHECK_I_ON_RF        //Check supply current only when RF is active
    #define SW_APP_OVER_CURRENT_FAULT   //Generate fault event in over current
    #define SW_TEMPERATURE_FIX_LIMITS   //Fixed temperature limits
    #define SW_TEMPERATURE_FIX_LOW  390 //39  //Fied low temperature limit
    #define SW_TEMPERATURE_FIX_HIGH 410 //41  //Fied high temperature limit
    #define SW_HW_SUP_24V               //24V supply
#endif

#ifdef SW_PROD_ReviveRf
    //#define SW_UC_PIC16F                //PIC16F uController
    #define SW_UC_PIC18F                //PIC18F uController
    #define SW_UC_PIC18F45K22           //p18f45k22 uController
    //#define SW_UC_DEVBOARD              //PIC18F46K22 in development board
    //#define SW_MOT_BLDC                 //BLDC motor applicator
    //#define SW_MOT_X                    //Motor X exists
    //#define SW_MOT_Y                    //Motor Y exists
    #define SW_MOT_RF                   //RF exists (in motor task). Note: Used also in US applicator
    #define SW_MOT_US                   //US applicator
    //#define SW_LED_RGB_VAR              //Variable intensity RGB LED exists
    #define SW_LED_RGB_FIXED            //Fixed intensity RGB LED exists
    #define SW_SENS_TEMP                //Temperature sensor exists
    #define SW_JUMPER_PROD_TEST         //Jumper for production test
    #define SW_PLL_ON                   //Use PLL for internal clock
    //#define SW_TICK_100US               //100uS tick, instead of 1mS
    #define SW_MOT_RF_FULL_POWER        //Set power to the value in loadparam
    //#define SW_MOT_RF_THREE_QUARTERS_POWER //Set power to 3/4 of the value in loadparam
    //#define SW_MOT_RF_HALF_POWER        //Set power to half of the value in loadparam
    //#define SW_MOT_RF_THIRD_POWER        //Set power to one third of the value in loadparam
    //#define SW_ROM_CHECK                //Check ROM
    #define SW_ROM_ADDR_START   0x0004  //ROM start address (skip jump to BootLoader)
    #define SW_ROM_ADDR_END     0x7BFE  //ROM end address (not include BootLoader code, because already passed it)
  #ifdef SW_PROD_ReviveRf_RepeatPulse
    #define SW_ROM_CHECKSUM     0xE7    //ROM checksum byte
  #else
    #define SW_ROM_CHECKSUM     0xE6    //ROM checksum byte
  #endif
    #define SW_STARTUP_BIT              //Execute StartUp BIT
    //#define SW_CONFIG_ONLY_XINST        //Set only config byte with XINST
    //#define SW_ADC_CHECK_I_ON_RF        //Check supply current only when RF is active
    //#define SW_APP_OVER_CURRENT_FAULT   //Generate fault event in over current
    #define SW_TEMPERATURE_FIX_LIMITS   //Fixed temperature limits
    #define SW_TEMPERATURE_FIX_LOW  390 //39  //Fied low temperature limit
    #define SW_TEMPERATURE_FIX_HIGH 410 //41  //Fied high temperature limit
    #define SW_REVIVE_BOARD             //Revive PCB
    #define SW_TIP_MEM                  //Use memory tip
    #define SW_TIP_TYPE_GEN12 1
    #define SW_TIP_TYPE_GEN36 0
    #define SW_TIP_TYPE_H7x7 2
    #define SW_VSUP_GEN                 //Supply voltage generator on board
    #define SW_VSUP_MAX_POWER 137       //Max p1power, p2power
    #define SW_VSUP_MIN_POWER 1         //Min p1power, p2power
    #define SW_IGNORE_THERMISTOR        //Don't check the thermistor
    #define SW_HW_REVIVE                //Revive hardware
    #define SW_HW_SUP_24V               //24V supply
    #define SW_TIP_TIMEOUT              //timeout in accessing TIP
#endif

#ifdef SW_PROD_ReviveUs
    //#define SW_UC_PIC16F                //PIC16F uController
    #define SW_UC_PIC18F                //PIC18F uController
    #define SW_UC_PIC18F45K22           //p18f45k22 uController
    //#define SW_UC_DEVBOARD              //PIC18F46K22 in development board
    //#define SW_MOT_BLDC                 //BLDC motor applicator
    //#define SW_MOT_X                    //Motor X exists
    //#define SW_MOT_Y                    //Motor Y exists
    #define SW_MOT_RF                   //RF exists (in motor task). Note: Used also in US applicator
    #define SW_MOT_US                   //US applicator
    //#define SW_LED_RGB_VAR              //Variable intensity RGB LED exists
    #define SW_LED_RGB_FIXED            //Fixed intensity RGB LED exists
    #define SW_SENS_TEMP                //Temperature sensor exists
    #define SW_JUMPER_PROD_TEST         //Jumper for production test
    #define SW_PLL_ON                   //Use PLL for internal clock
    //#define SW_TICK_100US               //100uS tick, instead of 1mS
    #define SW_MOT_RF_FULL_POWER        //Set power to the value in loadparam
    //#define SW_MOT_RF_THREE_QUARTERS_POWER //Set power to 3/4 of the value in loadparam
    //#define SW_MOT_RF_HALF_POWER        //Set power to half of the value in loadparam
    //#define SW_MOT_RF_THIRD_POWER        //Set power to one third of the value in loadparam
    #define SW_ROM_CHECK                //Check ROM
    #define SW_ROM_ADDR_START   0x0004  //ROM start address (skip jump to BootLoader)
    #define SW_ROM_ADDR_END     0x7BFE  //ROM end address (not include BootLoader code, because already passed it)
    #define SW_ROM_CHECKSUM     0x93    //ROM checksum byte
    #define SW_STARTUP_BIT              //Execute StartUp BIT
    //#define SW_CONFIG_ONLY_XINST        //Set only config byte with XINST
    //#define SW_ADC_CHECK_I_ON_RF        //Check supply current only when RF is active
    //#define SW_APP_OVER_CURRENT_FAULT   //Generate fault event in over current
    #define SW_TEMPERATURE_FIX_LIMITS   //Fixed temperature limits
    #define SW_TEMPERATURE_FIX_LOW  390 //39  //Fied low temperature limit
    #define SW_TEMPERATURE_FIX_HIGH 410 //41  //Fied high temperature limit
    #define SW_REVIVE_BOARD             //Revive PCB
    //#define SW_TIP_MEM                  //Use memory tip
    #define SW_VSUP_GEN                 //Supply voltage generator on board
    //#define SW_IGNORE_THERMISTOR        //Don't check the thermistor
    #define SW_HW_REVIVE                //Revive hardware
    #define SW_HW_SUP_24V               //24V supply
    //#define SW_DEBUG_TEMP_EMUL          //Emulate thermistor temperature
    //#define SW_DEBUG_TEMP_EMUL_VAL 250  //Temperature value
    //#define SW_DEBUG_NO_RF_TEST         //No RF test
#endif

//***********************************************************************************
//***********************************************************************************


//Compilation switches
//#define SW_MAIN_PIC16F1937          //Use PIC16F1937 (instead of PIC16F1939
//#define SW_ADC_NO_PERIODIC_SAMPLES  //No periodic samples of ADCs
//#define SW_EEPROM_USE_NON_VOLATILE  //Use no volatile memory to save parameters

//==================================================
//Debugging switches
#ifdef SW_DEBUG_GLOBAL_ENABLE
    //#define SW_COMM_CRC_NO_CHECK        //No CRC check for incomming messages
    //#define SW_COMM_IGNORE_RX_LEN_HIGH  //Ignore high of RX length
    //#define SW_COMM_LONG_TIMER          //Long timers on comm
    //#define SW_RF_NO_ACT                //Disable RF transmission during debugging
    //#define SW_TEMP_CONST_SEND          //Contstant send temperature
    //#define SW_TEMP_ADD_RES_RATIO       //Add resistanse ratio for temperature measurement
    //#define SW_COMM_ADD_CRLF            //Add CRLF in the end of a sent message
    //#define SW_MOT_BLDC_NO_SPEED_CHECK  //No speed check in BLDC motor
    //#define SW_DEBUG_LOG_STACK          //Log stack states
    //#define SW_DEBUG_LONG_WD            //Long watch dog timer
    //#define SW_DEBUG_TRAP_EVENT         //Trap RF applicator fault events
    //#define SW_COMM_NO_INTERRUPT        //Communication doesn't use an interrupt
    //#define SW_RF_SHORT_ON              //RF_ON always active
    //#define SW_RD_SHORT_EN              //RF_ON_EN always active
    //#define SW_APP_OVERCURRENT_KEY      //Pressing the key simulates an over kurrent
    //#define SW_COMM_LED_ON_EVENT        //activate LED when an event is active
    //#define SW_DEBUG_PROD_RF_CUR_TEST   //Set green and blue LEDs during RF current test in production mode
    //#define SW_DEBUG_TEST_WD            //Test if Watch Dog working
#endif
//==================================================
//Configuration switches
//#define SW_COMM_RF_FAULT_6          //Send RF fault as code 6, instead of 7
//==================================================

#ifdef SW_PLL_ON
    #define  SYSFOSC                    64000000L
#else
    #define  SYSFOSC                    16000000L
#endif

//Return result
typedef enum{
	//OK
	RESULT_OK = 0,
    
    //Communication
    RESULT_COMM_ERROR,
    
	//RF test
	RESULT_RF_TEST_BUSY,
	RESULT_RF_TEST_SUCCESS,
	RESULT_RF_TEST_FAIL,
	
	//Tip memory
	RESULT_TIPMEM_FAULT,
	
	//General
	RESULT_GENERAL_ERROR = 0xff
	
} RESULT_t;




#endif
//[] END OF FILE
