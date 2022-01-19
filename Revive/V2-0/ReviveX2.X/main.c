/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.6
        Device            :  PIC18F45K22
        Driver Version    :  2.00
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

#include "mcc_generated_files/mcc.h"

#include "..\..\..\GeneoIIbPIC1939bl\V1_2\General.h"
#ifdef SW_UC_PIC16F
    #include <htc.h>
#endif
#ifdef SW_UC_PIC18F
    #ifdef SW_UC_DEVBOARD
        #include <p18f46k22.h>
    #else
        #ifdef SW_UC_PIC18F45K22
//            #include <p18f45k22.h>
        #else
            #include <p18f25k22.h>
        #endif
    #endif
#endif
#include <stdlib.h>
#include <string.h>
#include "..\..\..\GeneoIIbPIC1939bl\V1_2\Types.h"
#include "..\..\..\GeneoIIbPIC1939bl\V1_2\Comm_Pub.h"
#include "..\..\..\GeneoIIbPIC1939bl\V1_2\Motor_Pub.h"
#include "..\..\..\GeneoIIbPIC1939bl\V1_2\Led_Pub.h"
#include "..\..\..\GeneoIIbPIC1939bl\V1_2\Keys_Pub.h"
#include "..\..\..\GeneoIIbPIC1939bl\V1_2\Adc_Pub.h"
#include "..\..\..\GeneoIIbPIC1939bl\V1_2\App_Pub.h"
#include "..\..\..\GeneoIIbPIC1939bl\V1_2\Eeprom_Pub.h"
#include "..\..\..\RfApplicator\V1-0\IO_Pub.h"
#include "..\..\..\Revive\V2-0\TipMem_Pub.h"
#ifdef SW_VSUP_GEN
  #include "..\..\..\Revive\V2-0\Revive_Vsup_Pub.h"
#endif

#ifndef SW_CONFIG_ONLY_XINST
    //CONFIG1H
    #pragma config FOSC = INTIO67
    //#pragma config FOSC = INT
    #ifdef SW_PLL_ON
        #pragma config PLLCFG = ON
    #else
        #pragma config PLLCFG = OFF
    #endif
    #pragma config PRICLKEN = ON //OFF //ON
    #pragma config FCMEN = ON
    #pragma config IESO=OFF
    //CONFIG2L
    #pragma config PWRTEN = ON
    #pragma config BOREN = OFF //SBORDIS <<<<
    #pragma config BORV = 285
    //CONFIG2H
    #pragma config WDTEN = SWON
    #ifdef SW_DEBUG_LONG_WD
        #pragma config WDTPS = 32768            //32768 == 2:18Min
    #else
        //#pragma config WDTPS = 1024 //32768     //1024 == 4Sec
        #pragma config WDTPS = 256              //256 = 1Sec 4.7.2016
    #endif
    //CONFIG3H
    #pragma config CCP2MX = PORTC1
    #pragma config PBADEN = OFF
    #pragma config CCP3MX = PORTB5
    #pragma config HFOFST = ON //OFF
    #pragma config T3CMX = PORTC0   //Actually - don't care
    #ifdef SW_UC_PIC18F45K22
        #pragma config P2BMX = PORTD2   //Actually - don't care
    #else
        #pragma config P2BMX = PORTB5   //Actually - don't care
    #endif
    #pragma config MCLRE = EXTMCLR
#endif
    //CONFIG4L
    #pragma config STVREN = ON
    #pragma config LVP = ON
    #pragma config XINST = OFF
    #pragma config DEBUG = ON //OFF
#ifndef SW_CONFIG_ONLY_XINST
    //CONFIG5L
    #pragma config CP0 = OFF
    #pragma config CP1 = OFF
    #pragma config CP2 = OFF
    #pragma config CP3 = OFF
    //CONFIG5H
    #pragma config CPB = OFF
    #pragma config CPD = OFF
    //CONFIG6L
    #pragma config WRT0 = OFF
    #pragma config WRT1 = OFF
    #pragma config WRT2 = OFF
    #pragma config WRT3 = OFF
    //CONFIG6H
    #pragma config WRTC = OFF
    #pragma config WRTB = OFF
    #pragma config WRTD = OFF
    //CONFIG7L
    #pragma config EBTR0 = OFF
    #pragma config EBTR1 = OFF
    #pragma config EBTR2 = OFF
    #pragma config EBTR3 = OFF
    //CONFIG7H
    #pragma config EBTRB = OFF
#endif


void main_TickFunctions(void);

//======================================================================
//Bootloader:
// - Include isr.as in the project
// - ROM range: default,-3E38-3FFF
// - RAM range: default,-7E-7F  ???
// - codeoffset 404 ???
// - Define W_TEMP and PCLATH_TEMP ???
//unsigned char W_TEMP @ 0x7F;
//unsigned char PCLATH_TEMP @ 0x7E;
//======================================================================

#ifdef SW_PLL_ON
    #ifdef SW_TICK_100US
        #define  TMR1H_BYTE 0xFE
        #define  TMR1L_BYTE 0x70
    #else
        #define  TMR1H_BYTE 0xF0
        #define  TMR1L_BYTE 0x60
    #endif
#else
    #ifdef SW_TICK_100US
        #define  TMR1H_BYTE 0xFF
        #define  TMR1L_BYTE 0x9C
    #else
        #define  TMR1H_BYTE 0xFC
        #define  TMR1L_BYTE 0x18
    #endif
#endif

#ifdef SW_COMM_NO_INTERRUPT
    #define COMM_RX_HANDLE if(PIR1bits.RC1IF){Comm_RxInterrupt();PIR1bits.RC1IF=0;}
    #define COMM_TX_HANDLE if(PIR1bits.TX1IF){Comm_TxInterrupt();PIR1bits.TX1IF=0;}
#else
    #define COMM_RX_HANDLE {}
    #define COMM_TX_HANDLE {}
#endif



#define TICK_100US_COUNT    10
#define TICK_100MS_COUNT   100
uint8 Tick100uSCounter = TICK_100US_COUNT;
uint8 Tick100mSCounter = TICK_100MS_COUNT;
volatile struct {
    union{
        uint8       Byte;
        struct {
            uint8       Tick:1;                 //1mS system tick
            uint8       C1IF:1;                 //Capacitor discharge timer
            uint8       b2:1;
            uint8       b3:1;
            uint8       b4:1;
            uint8       b5:1;
            uint8       b6:1;
            uint8       b7:1;
        }   Flags;
    } Status;
    uint8   Tick100uSCounter;                   //Count 100uS interrupts, for motor tick
} main_obj;    
#define SYSTEM_TICK     if(main_obj.Status.Flags.Tick){main_TickFunctions();main_obj.Status.Flags.Tick=0;}  //To minimize stack
#define MOTOR_TICK      Motor_TaskTick(main_obj.Tick100uSCounter);main_obj.Tick100uSCounter=0;

//Debug
#ifdef SW_DEBUG_LOG_STACK
uint8 CheckSP_MainLoop;
uint8 MaxSP_MainLoop = 0;
uint8 MinSP_MainLoop = 0x0f;
uint8 CheckSP_Int;
uint8 MaxSP_Int = 0;
#endif

#ifdef SW_ROM_CHECK
 #ifdef SW_UC_PIC18F
    const uint8* RomCheckAddr = (const uint8*)SW_ROM_ADDR_START;
    const uint8* RomCheckAddrEnd = (const uint8*)SW_ROM_ADDR_END;
    extern const uint8 RomChecksumByte;
 #else
    const uint8* RomCheckAddr = (const uint8*)SW_ROM_ADDR_START;
    const uint8* RomCheckAddrEnd = (const uint8*)SW_ROM_ADDR_END;
    extern const uint8 RomChecksumByte;
 #endif
 uint8  RomCheckResult = 0; //SW_ROM_CHECKSUM;
#endif

uint8 main_DisFlag_Get (void)
{
    return((uint8)main_obj.Status.Flags.C1IF);
} 
void main_DisFlag_Clr (void)
{
    main_obj.Status.Flags.C1IF = 0;
}    


/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    //SYSTEM_Initialize(); //Currently MCC functions are not used <<<<
    //Disable watch dog
    WDTCONbits.SWDTEN = 0;
    
    OSCCON = 0b01110000; //system clock 16mhz 
    while(!OSCCONbits.HFIOFS); //wait for stable clock
    //OSCTUNE = 0x38;
    
    RCONbits.RI = 1;            //Clear reset source flags
    RCONbits.TO = 1; 
    RCONbits.PD = 1; 
    RCONbits.POR = 1; 
    RCONbits.BOR = 1; 
    STKPTRbits.STKFUL = 0;
    STKPTRbits.STKUNF = 0;
    
    #ifdef SW_PLL_ON
        OSCTUNEbits.PLLEN = 1; //enable pll for 64mhz clock 
        while(!OSCCONbits.HFIOFS);
    #else
        OSCCON = 0x7a;      //16MHz
        OSCTUNE = 0x00;
    #endif
//       ANSELC = 0x00;   
//--------------------------------------

    //Config interrupts
    RCONbits.IPEN = 1;
    INTCON =  0b11000000;
    INTCON2 = 0b10000000;
    INTCON3 = 0b00000000;
    PIR1 =    0b00000000;
    PIR2 =    0b00000000;
    PIR3 =    0b00000000;
    PIR4 =    0b00000000;
    PIR5 =    0b00000000;
    PIE1 =    0b00110001;
    PIE2 =    0b00000000;
    PIE3 =    0b00110000;
    PIE4 =    0b00000000;
    PIE5 =    0b00000000;
    IPR1 =    0b00110000;
    IPR2 =    0b00000000;
    IPR3 =    0b00000000;
    IPR4 =    0b00000000;
    IPR5 =    0b00000000;
    
    T1CON = 0b00100001; //Timer1 setup
    T1GCON = 0;
    TMR1H = TMR1H_BYTE; //Count 1000
    TMR1L = TMR1L_BYTE;

    //Config I2C <<<<
    #ifdef SW_PROD_ReviveRf
    #endif

    while (!OSCCON2bits.LFIOFS) {}           //Wait till LFINOSC is stable
    
    //Check code
    #ifdef SW_ROM_CHECK
     RomCheckResult++;
     while (RomCheckAddr < RomCheckAddrEnd)
     {
       uint8 TempRom = *RomCheckAddr;
        //TempRom++;                          //In order to change 0xff to 0x00, in order to ignore 0xff (not programmed parts)
        RomCheckResult ^= TempRom;
        RomCheckAddr++;
     }
     RomCheckResult ^= RomChecksumByte;
     if (RomCheckResult != RomChecksumByte)
     {                                      //Incorrect checksum - halt
         while (1) {}
     }    
    #endif
    
    
    WDTCONbits.SWDTEN = 1;                  //Enable watchdog

    main_obj.Status.Byte = 0;
    main_obj.Tick100uSCounter = 0;
    
    //Init
    Eeprom_TaskInit();
    ClrWdt();
    #ifdef SW_TIP_MEM
      TipMem_TaskInit();
      ClrWdt();
    #endif
    Comm_TaskInit();
    ClrWdt();
    Motor_TaskInit();
    ClrWdt();
    Led_TaskInit();
    ClrWdt();
    Keys_TaskInit();
    ClrWdt();
    Adc_TaskInit();
    ClrWdt();
    App_TaskInit();
    ClrWdt();
    IO_TaskInit();
    ClrWdt();
    #ifdef SW_VSUP_GEN
     Revive_Vsup_TaskInit();
     ClrWdt();
    #endif

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    while (1)
    {
        // Add your application code
        Comm_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
          #ifdef SW_TICK_100US
           MOTOR_TICK;
          #endif
        Motor_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        Led_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        Motor_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        Keys_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        Motor_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        Adc_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        Motor_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        App_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        Motor_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        IO_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        Motor_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        #ifdef SW_TIP_MEM
         //WDTCONbits.SWDTEN = 0;                             //Disable WD. Canceled 4.7.2016
//         TipMem_TaskMain();
           COMM_RX_HANDLE;
           COMM_TX_HANDLE;
           ClrWdt();
           //WDTCONbits.SWDTEN = 1;                           //Re-enable WD. Canceled 4.7.2016
           SYSTEM_TICK;
        #endif
        Motor_TaskMain();
          COMM_RX_HANDLE;
          COMM_TX_HANDLE;
          ClrWdt();
          SYSTEM_TICK;
        #ifdef SW_VSUP_GEN
         Revive_Vsup_TaskMain();
           COMM_RX_HANDLE;
           COMM_TX_HANDLE;
           ClrWdt();
           SYSTEM_TICK;
        #endif
          
//Check stack pointer
#ifdef SW_DEBUG_LOG_STACK
 CheckSP_MainLoop = STKPTR & 0x1f;
 if (CheckSP_MainLoop != 1)
 {
     CheckSP_MainLoop++;
     CheckSP_MainLoop--;
 }    
 if (CheckSP_MainLoop > MaxSP_MainLoop)
 {
     MaxSP_MainLoop = CheckSP_MainLoop;
     MinSP_MainLoop = CheckSP_MainLoop;
 }
 else if (CheckSP_MainLoop < MinSP_MainLoop)
 {
     MinSP_MainLoop = CheckSP_MainLoop;
 }
#endif

    }
}

/******************************************************************************
*  Interrupt routine
******************************************************************************/
void main_TickFunctions(void)
{
            Adc_TaskTick1mS();
            Comm_TaskTick1mS();
            Keys_1mS_tick();
            IO_1mS_tick();
            #ifndef SW_TICK_100US
             Motor_TaskTick();
            #endif
            App_TaskTick1mS();
            #ifdef SW_TIP_MEM
             TipMem_TaskTick1mS();
            #endif
            #ifdef SW_VSUP_GEN
             Revive_Vsup_1mS_tick();
            #endif
            Tick100mSCounter--;
            if (Tick100mSCounter == 0)
            {                           //100mS
                Tick100mSCounter = TICK_100MS_COUNT;
                #ifdef SW_TIP_MEM
                 TipMem_TaskTick100mS();
                #endif
                Led_TaskTick100mS();
            }
}
    

#ifdef SW_UC_PIC16F
    #pragma interrupt InterruptHandlerLow
#endif
#ifdef SW_UC_PIC18F
    #pragma interruptlow InterruptHandlerLow
#endif
void InterruptHandlerLow(void)
{
    if(PIR1bits.TMR1IF)
    {                               //1mS/100uS system tick
        TMR1H = TMR1H_BYTE; //Count 1000
        TMR1L = TMR1L_BYTE;
        #ifdef SW_TICK_100US
            main_obj.Tick100uSCounter++;
            Tick100uSCounter--;
            if (Tick100uSCounter == 0)
            {                           //1mS
                Tick100uSCounter = TICK_100US_COUNT;
                main_obj.Status.Flags.Tick = 1;
            }
        #else
            main_obj.Status.Flags.Tick = 1;
        #endif
        PIR1bits.TMR1IF = 0;
    }
}    

#pragma interrupt InterruptHandlerHigh
void InterruptHandlerHigh(void)
{
    if (PIR2bits.C1IF)
    {
        T5CONbits.TMR5ON = 0;
        main_obj.Status.Flags.C1IF = 1;
        PIR2bits.C1IF = 0;
        PIE2bits.C1IE = 0;
    }    
    if (PIR1bits.RC1IF)
    {                               //UART RX interrupt
        Comm_RxInterrupt();
        PIR1bits.RC1IF = 0;
    }    
    if (PIR1bits.TX1IF)
    {                               //UART TX interrupt
        Comm_TxInterrupt();
        PIR1bits.TX1IF = 0;
    }
//Check stack pointer
#ifdef SW_DEBUG_LOG_STACK
 CheckSP_Int = STKPTR & 0x1f;
 if (CheckSP_Int > MaxSP_Int)
 {
     MaxSP_Int = CheckSP_Int;
 }    
#endif
}    

#ifdef SW_UC_PIC16F
    #pragma code low_vector_section=0x18
#endif
#ifdef SW_UC_PIC18F
//    #pragma code low_vector=0x18
#endif
//void low_vector (void)
//{
//  _asm GOTO InterruptHandlerLow _endasm
//}

#ifdef SW_UC_PIC16F
    #pragma code
    #pragma code high_vector_section=0x08
#endif
#ifdef SW_UC_PIC18F
//    #pragma code high_vector=0x08
#endif
//void high_vector (void)
//{
//  _asm GOTO InterruptHandlerHigh _endasm
//}

#ifdef SW_ROM_CHECK
    #ifdef SW_UC_PIC16F
        #pragma code
        #pragma code checksum_byte_section=SW_ROM_ADDR_END
        const uint8 RomChecksumByte = SW_ROM_CHECKSUM;
    #endif
    #ifdef SW_UC_PIC18F
        #pragma code checksum_byte=SW_ROM_ADDR_END
        const uint8 RomChecksumByte = SW_ROM_CHECKSUM;
    #endif
#endif


/**
 End of File
*/