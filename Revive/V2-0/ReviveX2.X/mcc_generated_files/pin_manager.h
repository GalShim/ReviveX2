/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.6
        Device            :  PIC18F45K22
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.30 and above
        MPLAB 	          :  MPLAB X 5.40	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set channel_AN0 aliases
#define channel_AN0_TRIS                 TRISAbits.TRISA0
#define channel_AN0_LAT                  LATAbits.LATA0
#define channel_AN0_PORT                 PORTAbits.RA0
#define channel_AN0_ANS                  ANSELAbits.ANSA0
#define channel_AN0_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define channel_AN0_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define channel_AN0_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define channel_AN0_GetValue()           PORTAbits.RA0
#define channel_AN0_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define channel_AN0_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define channel_AN0_SetAnalogMode()      do { ANSELAbits.ANSA0 = 1; } while(0)
#define channel_AN0_SetDigitalMode()     do { ANSELAbits.ANSA0 = 0; } while(0)

// get/set channel_AN1 aliases
#define channel_AN1_TRIS                 TRISAbits.TRISA1
#define channel_AN1_LAT                  LATAbits.LATA1
#define channel_AN1_PORT                 PORTAbits.RA1
#define channel_AN1_ANS                  ANSELAbits.ANSA1
#define channel_AN1_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define channel_AN1_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define channel_AN1_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define channel_AN1_GetValue()           PORTAbits.RA1
#define channel_AN1_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define channel_AN1_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define channel_AN1_SetAnalogMode()      do { ANSELAbits.ANSA1 = 1; } while(0)
#define channel_AN1_SetDigitalMode()     do { ANSELAbits.ANSA1 = 0; } while(0)

// get/set channel_AN2 aliases
#define channel_AN2_TRIS                 TRISAbits.TRISA2
#define channel_AN2_LAT                  LATAbits.LATA2
#define channel_AN2_PORT                 PORTAbits.RA2
#define channel_AN2_ANS                  ANSELAbits.ANSA2
#define channel_AN2_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define channel_AN2_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define channel_AN2_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define channel_AN2_GetValue()           PORTAbits.RA2
#define channel_AN2_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define channel_AN2_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define channel_AN2_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define channel_AN2_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set OUT_RF_ON_EN aliases
#define OUT_RF_ON_EN_TRIS                 TRISAbits.TRISA4
#define OUT_RF_ON_EN_LAT                  LATAbits.LATA4
#define OUT_RF_ON_EN_PORT                 PORTAbits.RA4
#define OUT_RF_ON_EN_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define OUT_RF_ON_EN_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define OUT_RF_ON_EN_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define OUT_RF_ON_EN_GetValue()           PORTAbits.RA4
#define OUT_RF_ON_EN_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define OUT_RF_ON_EN_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)

// get/set OUT_LED_GRN aliases
#define OUT_LED_GRN_TRIS                 TRISAbits.TRISA7
#define OUT_LED_GRN_LAT                  LATAbits.LATA7
#define OUT_LED_GRN_PORT                 PORTAbits.RA7
#define OUT_LED_GRN_SetHigh()            do { LATAbits.LATA7 = 1; } while(0)
#define OUT_LED_GRN_SetLow()             do { LATAbits.LATA7 = 0; } while(0)
#define OUT_LED_GRN_Toggle()             do { LATAbits.LATA7 = ~LATAbits.LATA7; } while(0)
#define OUT_LED_GRN_GetValue()           PORTAbits.RA7
#define OUT_LED_GRN_SetDigitalInput()    do { TRISAbits.TRISA7 = 1; } while(0)
#define OUT_LED_GRN_SetDigitalOutput()   do { TRISAbits.TRISA7 = 0; } while(0)

// get/set IN_SW1 aliases
#define IN_SW1_TRIS                 TRISBbits.TRISB0
#define IN_SW1_LAT                  LATBbits.LATB0
#define IN_SW1_PORT                 PORTBbits.RB0
#define IN_SW1_WPU                  WPUBbits.WPUB0
#define IN_SW1_ANS                  ANSELBbits.ANSB0
#define IN_SW1_SetHigh()            do { LATBbits.LATB0 = 1; } while(0)
#define IN_SW1_SetLow()             do { LATBbits.LATB0 = 0; } while(0)
#define IN_SW1_Toggle()             do { LATBbits.LATB0 = ~LATBbits.LATB0; } while(0)
#define IN_SW1_GetValue()           PORTBbits.RB0
#define IN_SW1_SetDigitalInput()    do { TRISBbits.TRISB0 = 1; } while(0)
#define IN_SW1_SetDigitalOutput()   do { TRISBbits.TRISB0 = 0; } while(0)
#define IN_SW1_SetPullup()          do { WPUBbits.WPUB0 = 1; } while(0)
#define IN_SW1_ResetPullup()        do { WPUBbits.WPUB0 = 0; } while(0)
#define IN_SW1_SetAnalogMode()      do { ANSELBbits.ANSB0 = 1; } while(0)
#define IN_SW1_SetDigitalMode()     do { ANSELBbits.ANSB0 = 0; } while(0)

// get/set IN_SW_Q2 aliases
#define IN_SW_Q2_TRIS                 TRISBbits.TRISB1
#define IN_SW_Q2_LAT                  LATBbits.LATB1
#define IN_SW_Q2_PORT                 PORTBbits.RB1
#define IN_SW_Q2_WPU                  WPUBbits.WPUB1
#define IN_SW_Q2_ANS                  ANSELBbits.ANSB1
#define IN_SW_Q2_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define IN_SW_Q2_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define IN_SW_Q2_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define IN_SW_Q2_GetValue()           PORTBbits.RB1
#define IN_SW_Q2_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define IN_SW_Q2_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define IN_SW_Q2_SetPullup()          do { WPUBbits.WPUB1 = 1; } while(0)
#define IN_SW_Q2_ResetPullup()        do { WPUBbits.WPUB1 = 0; } while(0)
#define IN_SW_Q2_SetAnalogMode()      do { ANSELBbits.ANSB1 = 1; } while(0)
#define IN_SW_Q2_SetDigitalMode()     do { ANSELBbits.ANSB1 = 0; } while(0)

// get/set IN_SW_Q11 aliases
#define IN_SW_Q11_TRIS                 TRISBbits.TRISB2
#define IN_SW_Q11_LAT                  LATBbits.LATB2
#define IN_SW_Q11_PORT                 PORTBbits.RB2
#define IN_SW_Q11_WPU                  WPUBbits.WPUB2
#define IN_SW_Q11_ANS                  ANSELBbits.ANSB2
#define IN_SW_Q11_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define IN_SW_Q11_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define IN_SW_Q11_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define IN_SW_Q11_GetValue()           PORTBbits.RB2
#define IN_SW_Q11_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define IN_SW_Q11_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define IN_SW_Q11_SetPullup()          do { WPUBbits.WPUB2 = 1; } while(0)
#define IN_SW_Q11_ResetPullup()        do { WPUBbits.WPUB2 = 0; } while(0)
#define IN_SW_Q11_SetAnalogMode()      do { ANSELBbits.ANSB2 = 1; } while(0)
#define IN_SW_Q11_SetDigitalMode()     do { ANSELBbits.ANSB2 = 0; } while(0)

// get/set ADC_IN_OUTV aliases
#define ADC_IN_OUTV_TRIS                 TRISBbits.TRISB3
#define ADC_IN_OUTV_LAT                  LATBbits.LATB3
#define ADC_IN_OUTV_PORT                 PORTBbits.RB3
#define ADC_IN_OUTV_WPU                  WPUBbits.WPUB3
#define ADC_IN_OUTV_ANS                  ANSELBbits.ANSB3
#define ADC_IN_OUTV_SetHigh()            do { LATBbits.LATB3 = 1; } while(0)
#define ADC_IN_OUTV_SetLow()             do { LATBbits.LATB3 = 0; } while(0)
#define ADC_IN_OUTV_Toggle()             do { LATBbits.LATB3 = ~LATBbits.LATB3; } while(0)
#define ADC_IN_OUTV_GetValue()           PORTBbits.RB3
#define ADC_IN_OUTV_SetDigitalInput()    do { TRISBbits.TRISB3 = 1; } while(0)
#define ADC_IN_OUTV_SetDigitalOutput()   do { TRISBbits.TRISB3 = 0; } while(0)
#define ADC_IN_OUTV_SetPullup()          do { WPUBbits.WPUB3 = 1; } while(0)
#define ADC_IN_OUTV_ResetPullup()        do { WPUBbits.WPUB3 = 0; } while(0)
#define ADC_IN_OUTV_SetAnalogMode()      do { ANSELBbits.ANSB3 = 1; } while(0)
#define ADC_IN_OUTV_SetDigitalMode()     do { ANSELBbits.ANSB3 = 0; } while(0)

// get/set ADC_IN_CUR aliases
#define ADC_IN_CUR_TRIS                 TRISBbits.TRISB4
#define ADC_IN_CUR_LAT                  LATBbits.LATB4
#define ADC_IN_CUR_PORT                 PORTBbits.RB4
#define ADC_IN_CUR_WPU                  WPUBbits.WPUB4
#define ADC_IN_CUR_ANS                  ANSELBbits.ANSB4
#define ADC_IN_CUR_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define ADC_IN_CUR_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define ADC_IN_CUR_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define ADC_IN_CUR_GetValue()           PORTBbits.RB4
#define ADC_IN_CUR_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define ADC_IN_CUR_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define ADC_IN_CUR_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define ADC_IN_CUR_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define ADC_IN_CUR_SetAnalogMode()      do { ANSELBbits.ANSB4 = 1; } while(0)
#define ADC_IN_CUR_SetDigitalMode()     do { ANSELBbits.ANSB4 = 0; } while(0)

// get/set ADC_IN_VCC aliases
#define ADC_IN_VCC_TRIS                 TRISBbits.TRISB5
#define ADC_IN_VCC_LAT                  LATBbits.LATB5
#define ADC_IN_VCC_PORT                 PORTBbits.RB5
#define ADC_IN_VCC_WPU                  WPUBbits.WPUB5
#define ADC_IN_VCC_ANS                  ANSELBbits.ANSB5
#define ADC_IN_VCC_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define ADC_IN_VCC_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define ADC_IN_VCC_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define ADC_IN_VCC_GetValue()           PORTBbits.RB5
#define ADC_IN_VCC_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define ADC_IN_VCC_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define ADC_IN_VCC_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define ADC_IN_VCC_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define ADC_IN_VCC_SetAnalogMode()      do { ANSELBbits.ANSB5 = 1; } while(0)
#define ADC_IN_VCC_SetDigitalMode()     do { ANSELBbits.ANSB5 = 0; } while(0)

// get/set OUT_RF_ON aliases
#define OUT_RF_ON_TRIS                 TRISCbits.TRISC0
#define OUT_RF_ON_LAT                  LATCbits.LATC0
#define OUT_RF_ON_PORT                 PORTCbits.RC0
#define OUT_RF_ON_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define OUT_RF_ON_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define OUT_RF_ON_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define OUT_RF_ON_GetValue()           PORTCbits.RC0
#define OUT_RF_ON_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define OUT_RF_ON_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)

// get/set OUT_LED_ORNG aliases
#define OUT_LED_ORNG_TRIS                 TRISCbits.TRISC2
#define OUT_LED_ORNG_LAT                  LATCbits.LATC2
#define OUT_LED_ORNG_PORT                 PORTCbits.RC2
#define OUT_LED_ORNG_ANS                  ANSELCbits.ANSC2
#define OUT_LED_ORNG_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define OUT_LED_ORNG_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define OUT_LED_ORNG_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define OUT_LED_ORNG_GetValue()           PORTCbits.RC2
#define OUT_LED_ORNG_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define OUT_LED_ORNG_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define OUT_LED_ORNG_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define OUT_LED_ORNG_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set RC6 procedures
#define RC6_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define RC6_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define RC6_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define RC6_GetValue()              PORTCbits.RC6
#define RC6_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define RC6_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define RC6_SetAnalogMode()         do { ANSELCbits.ANSC6 = 1; } while(0)
#define RC6_SetDigitalMode()        do { ANSELCbits.ANSC6 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()              PORTCbits.RC7
#define RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define RC7_SetAnalogMode()         do { ANSELCbits.ANSC7 = 1; } while(0)
#define RC7_SetDigitalMode()        do { ANSELCbits.ANSC7 = 0; } while(0)

// get/set RD0 procedures
#define RD0_SetHigh()            do { LATDbits.LATD0 = 1; } while(0)
#define RD0_SetLow()             do { LATDbits.LATD0 = 0; } while(0)
#define RD0_Toggle()             do { LATDbits.LATD0 = ~LATDbits.LATD0; } while(0)
#define RD0_GetValue()              PORTDbits.RD0
#define RD0_SetDigitalInput()    do { TRISDbits.TRISD0 = 1; } while(0)
#define RD0_SetDigitalOutput()   do { TRISDbits.TRISD0 = 0; } while(0)
#define RD0_SetAnalogMode()         do { ANSELDbits.ANSD0 = 1; } while(0)
#define RD0_SetDigitalMode()        do { ANSELDbits.ANSD0 = 0; } while(0)

// get/set RD1 procedures
#define RD1_SetHigh()            do { LATDbits.LATD1 = 1; } while(0)
#define RD1_SetLow()             do { LATDbits.LATD1 = 0; } while(0)
#define RD1_Toggle()             do { LATDbits.LATD1 = ~LATDbits.LATD1; } while(0)
#define RD1_GetValue()              PORTDbits.RD1
#define RD1_SetDigitalInput()    do { TRISDbits.TRISD1 = 1; } while(0)
#define RD1_SetDigitalOutput()   do { TRISDbits.TRISD1 = 0; } while(0)
#define RD1_SetAnalogMode()         do { ANSELDbits.ANSD1 = 1; } while(0)
#define RD1_SetDigitalMode()        do { ANSELDbits.ANSD1 = 0; } while(0)

// get/set IN_RUN aliases
#define IN_RUN_TRIS                 TRISDbits.TRISD2
#define IN_RUN_LAT                  LATDbits.LATD2
#define IN_RUN_PORT                 PORTDbits.RD2
#define IN_RUN_ANS                  ANSELDbits.ANSD2
#define IN_RUN_SetHigh()            do { LATDbits.LATD2 = 1; } while(0)
#define IN_RUN_SetLow()             do { LATDbits.LATD2 = 0; } while(0)
#define IN_RUN_Toggle()             do { LATDbits.LATD2 = ~LATDbits.LATD2; } while(0)
#define IN_RUN_GetValue()           PORTDbits.RD2
#define IN_RUN_SetDigitalInput()    do { TRISDbits.TRISD2 = 1; } while(0)
#define IN_RUN_SetDigitalOutput()   do { TRISDbits.TRISD2 = 0; } while(0)
#define IN_RUN_SetAnalogMode()      do { ANSELDbits.ANSD2 = 1; } while(0)
#define IN_RUN_SetDigitalMode()     do { ANSELDbits.ANSD2 = 0; } while(0)

// get/set IN_CS aliases
#define IN_CS_TRIS                 TRISDbits.TRISD4
#define IN_CS_LAT                  LATDbits.LATD4
#define IN_CS_PORT                 PORTDbits.RD4
#define IN_CS_ANS                  ANSELDbits.ANSD4
#define IN_CS_SetHigh()            do { LATDbits.LATD4 = 1; } while(0)
#define IN_CS_SetLow()             do { LATDbits.LATD4 = 0; } while(0)
#define IN_CS_Toggle()             do { LATDbits.LATD4 = ~LATDbits.LATD4; } while(0)
#define IN_CS_GetValue()           PORTDbits.RD4
#define IN_CS_SetDigitalInput()    do { TRISDbits.TRISD4 = 1; } while(0)
#define IN_CS_SetDigitalOutput()   do { TRISDbits.TRISD4 = 0; } while(0)
#define IN_CS_SetAnalogMode()      do { ANSELDbits.ANSD4 = 1; } while(0)
#define IN_CS_SetDigitalMode()     do { ANSELDbits.ANSD4 = 0; } while(0)

// get/set RD6 procedures
#define RD6_SetHigh()            do { LATDbits.LATD6 = 1; } while(0)
#define RD6_SetLow()             do { LATDbits.LATD6 = 0; } while(0)
#define RD6_Toggle()             do { LATDbits.LATD6 = ~LATDbits.LATD6; } while(0)
#define RD6_GetValue()              PORTDbits.RD6
#define RD6_SetDigitalInput()    do { TRISDbits.TRISD6 = 1; } while(0)
#define RD6_SetDigitalOutput()   do { TRISDbits.TRISD6 = 0; } while(0)
#define RD6_SetAnalogMode()         do { ANSELDbits.ANSD6 = 1; } while(0)
#define RD6_SetDigitalMode()        do { ANSELDbits.ANSD6 = 0; } while(0)

// get/set RD7 procedures
#define RD7_SetHigh()            do { LATDbits.LATD7 = 1; } while(0)
#define RD7_SetLow()             do { LATDbits.LATD7 = 0; } while(0)
#define RD7_Toggle()             do { LATDbits.LATD7 = ~LATDbits.LATD7; } while(0)
#define RD7_GetValue()              PORTDbits.RD7
#define RD7_SetDigitalInput()    do { TRISDbits.TRISD7 = 1; } while(0)
#define RD7_SetDigitalOutput()   do { TRISDbits.TRISD7 = 0; } while(0)
#define RD7_SetAnalogMode()         do { ANSELDbits.ANSD7 = 1; } while(0)
#define RD7_SetDigitalMode()        do { ANSELDbits.ANSD7 = 0; } while(0)

// get/set OUT_LED_BLUE aliases
#define OUT_LED_BLUE_TRIS                 TRISEbits.TRISE0
#define OUT_LED_BLUE_LAT                  LATEbits.LATE0
#define OUT_LED_BLUE_PORT                 PORTEbits.RE0
#define OUT_LED_BLUE_ANS                  ANSELEbits.ANSE0
#define OUT_LED_BLUE_SetHigh()            do { LATEbits.LATE0 = 1; } while(0)
#define OUT_LED_BLUE_SetLow()             do { LATEbits.LATE0 = 0; } while(0)
#define OUT_LED_BLUE_Toggle()             do { LATEbits.LATE0 = ~LATEbits.LATE0; } while(0)
#define OUT_LED_BLUE_GetValue()           PORTEbits.RE0
#define OUT_LED_BLUE_SetDigitalInput()    do { TRISEbits.TRISE0 = 1; } while(0)
#define OUT_LED_BLUE_SetDigitalOutput()   do { TRISEbits.TRISE0 = 0; } while(0)
#define OUT_LED_BLUE_SetAnalogMode()      do { ANSELEbits.ANSE0 = 1; } while(0)
#define OUT_LED_BLUE_SetDigitalMode()     do { ANSELEbits.ANSE0 = 0; } while(0)

// get/set OUT_PGOOD aliases
#define OUT_PGOOD_TRIS                 TRISEbits.TRISE1
#define OUT_PGOOD_LAT                  LATEbits.LATE1
#define OUT_PGOOD_PORT                 PORTEbits.RE1
#define OUT_PGOOD_ANS                  ANSELEbits.ANSE1
#define OUT_PGOOD_SetHigh()            do { LATEbits.LATE1 = 1; } while(0)
#define OUT_PGOOD_SetLow()             do { LATEbits.LATE1 = 0; } while(0)
#define OUT_PGOOD_Toggle()             do { LATEbits.LATE1 = ~LATEbits.LATE1; } while(0)
#define OUT_PGOOD_GetValue()           PORTEbits.RE1
#define OUT_PGOOD_SetDigitalInput()    do { TRISEbits.TRISE1 = 1; } while(0)
#define OUT_PGOOD_SetDigitalOutput()   do { TRISEbits.TRISE1 = 0; } while(0)
#define OUT_PGOOD_SetAnalogMode()      do { ANSELEbits.ANSE1 = 1; } while(0)
#define OUT_PGOOD_SetDigitalMode()     do { ANSELEbits.ANSE1 = 0; } while(0)

// get/set ADC_OUT_V aliases
#define ADC_OUT_V_TRIS                 TRISEbits.TRISE2
#define ADC_OUT_V_LAT                  LATEbits.LATE2
#define ADC_OUT_V_PORT                 PORTEbits.RE2
#define ADC_OUT_V_ANS                  ANSELEbits.ANSE2
#define ADC_OUT_V_SetHigh()            do { LATEbits.LATE2 = 1; } while(0)
#define ADC_OUT_V_SetLow()             do { LATEbits.LATE2 = 0; } while(0)
#define ADC_OUT_V_Toggle()             do { LATEbits.LATE2 = ~LATEbits.LATE2; } while(0)
#define ADC_OUT_V_GetValue()           PORTEbits.RE2
#define ADC_OUT_V_SetDigitalInput()    do { TRISEbits.TRISE2 = 1; } while(0)
#define ADC_OUT_V_SetDigitalOutput()   do { TRISEbits.TRISE2 = 0; } while(0)
#define ADC_OUT_V_SetAnalogMode()      do { ANSELEbits.ANSE2 = 1; } while(0)
#define ADC_OUT_V_SetDigitalMode()     do { ANSELEbits.ANSE2 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/