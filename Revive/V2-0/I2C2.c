/* ===================================================================================================================
File Name:   I2C2.c
Description: I2C handler
Based on Microchip functions
====================================================================================================================*/

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


#include <p18cxxx.h>
#include "TipMem_Pub.h"
#include "i2c2.h"

#ifdef SW_TIP_TIMEOUT
    #define IIC_NOT_TIMOUT (TMR3H>=TMR3H_BYTE)
    #define IIC_TIMOUT (TMR3H<TMR3H_BYTE)
#endif

/********************************************************************
*     Function Name:    getsI2C2                                    *
*     Return Value:     error condition status                      *
*     Parameters:       address of read string storage location     *
*                       length of string bytes to read              *
*     Description:      This routine reads a predetemined string    *
*                       length in from the I2C2 bus. The routine is *
*                       developed for the Master mode. The bus ACK  *
*                       condition is generated within this routine. *
********************************************************************/
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
signed char getsI2C2( unsigned char *rdptr, unsigned char length )
{
    while ( length -- )            // perform getcI2C2() for 'length' number of bytes
    {
      *rdptr++ = getcI2C2();       // save byte received
      #ifdef SW_TIP_TIMEOUT
       while ( SSP2CON2bits.RCEN  && IIC_NOT_TIMOUT); // check that receive sequence is over    
      #else
       while ( SSP2CON2bits.RCEN ); // check that receive sequence is over    
      #endif

#if defined (I2C_V3) || defined (I2C_V6)
  if ( PIR3bits.BCL2IF )           // test for bus collision
#elif defined (I2C_V6_1)
  if ( PIR2bits.BCL2IF )           // test for bus collision
#endif  
      {
        return ( -1 );             // return with Bus Collision error 
      }

	if( ((SSP2CON1&0x0F)==0x08) || ((SSP2CON1&0x0F)==0x0B) )	//master mode only
	{	
      if ( length )               // test if 'length' bytes have been read
      {
        SSP2CON2bits.ACKDT = 0;    // set acknowledge bit state for ACK        
        SSP2CON2bits.ACKEN = 1;    // initiate bus acknowledge sequence
        #ifdef SW_TIP_TIMEOUT
         while ( SSP2CON2bits.ACKEN && IIC_NOT_TIMOUT ); // wait until ACK sequence is over 
        #else
         while ( SSP2CON2bits.ACKEN ); // wait until ACK sequence is over 
        #endif
      } 
	} 
	  
    }
    return ( 0 );                  // last byte received so don't send ACK      
}
#endif

/********************************************************************
*     Function Name:    WriteI2C2                                   *
*     Return Value:     Status byte for WCOL detection.             *
*     Parameters:       Single data byte for I2C2 bus.              *
*     Description:      This routine writes a single byte to the    * 
*                       I2C2 bus.                                   *
********************************************************************/
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
signed char WriteI2C2( unsigned char data_out )
{
  SSP2BUF = data_out;           // write single byte to SSP2BUF
  if ( SSP2CON1bits.WCOL )      // test if write collision occurred
    return ( -1 );              // if WCOL bit is set return negative #
  else
  {
	if( ((SSP2CON1&0x0F)!=0x08) && ((SSP2CON1&0x0F)!=0x0B) )	//slave mode only
	{
	 #if defined (I2C_SFR_V1) 
     if ( ( !SSP2STATbits.R_NOT_W ) && ( !SSP2STATbits.BF ) )
	 #else
      if ( ( !SSP2STATbits.R_W ) && ( !SSP2STATbits.BF ) )// if R/W=0 and BF=0, NOT ACK was received
	 #endif 
      {
        return ( -2 );             // return NACK
      }
	  else return(0);				// return ACK
	
	}  
    else if( ((SSP2CON1&0x0F)==0x08) || ((SSP2CON1&0x0F)==0x0B) )	//master mode only
	{
        #ifdef SW_TIP_TIMEOUT
	     while( SSP2STATbits.BF && IIC_NOT_TIMOUT );   // wait until write cycle is complete     
        #else
	     while( SSP2STATbits.BF );   // wait until write cycle is complete     
        #endif
	    IdleI2C2();                  // ensure module is idle
	    if ( SSP2CON2bits.ACKSTAT ) // test for ACK condition received
	    	 return ( -2 );				//return NACK
		else return ( 0 );   			//return ACK
	}
	
  }
}
#endif

/*********************************************************************
*     Function Name:    putsI2C2                                     *
*     Return Value:     error condition status                       *
*     Parameters:       address of write string storage location     *
*     Description:      This routine writes a string to the I2C2 bus,*
*                       until a null character is reached. If Master *
*                       function putcI2C is called. When trans-      *
*                       mission is complete then test for ack-       *
*                       nowledge bit. If Slave transmitter wait for  *
*                       null character or not ACK received from bus  *
*                       device.                                      *
*     Gal changes:      Add length parmeter, instead of null         *
*                       termination.                                 *
*********************************************************************/
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
//signed char putsI2C2( unsigned char *wrptr )
signed char putsI2C2L( unsigned char *wrptr , unsigned char Length)  //Gal
{
   unsigned char temp;  
	//while ( *wrptr )                 // transmit data until null character 
	while ( Length )                 // Gal 
  {
    if ( SSP2CON1bits.SSPM3 )      // if Master transmitter then execute the following
    {  
	   temp = putcI2C2 ( *wrptr );
	   if (temp ) return ( temp );            // return with write collision error
    }

    else                           // else Slave transmitter
    {
#if defined (I2C_V3) || defined (I2C_V6)
      PIR3bits.SSP2IF = 0;         // reset SSP2IF bit
#elif defined (I2C_V6_1)
	  PIR2bits.SSP2IF = 0;         // reset SSP2IF bit
#endif		
      SSP2BUF = *wrptr;            // load SSP2BUF with new data
      SSP2CON1bits.CKP = 1;        // release clock line 
#if defined (I2C_V3) || defined (I2C_V6)
      #ifdef SW_TIP_TIMEOUT
       while ( !PIR3bits.SSP2IF && IIC_NOT_TIMOUT );  // wait until ninth clock pulse received
      #else
       while ( !PIR3bits.SSP2IF );  // wait until ninth clock pulse received
      #endif
#elif defined (I2C_V6_1)
      #ifdef SW_TIP_TIMEOUT
	   while ( !PIR2bits.SSP2I && IIC_NOT_TIMOUTF );  // wait until ninth clock pulse received
      #else
	   while ( !PIR2bits.SSP2IF );  // wait until ninth clock pulse received
      #endif
#endif	  
	 #if defined (I2C_SFR_V1) 
     if ( ( SSP2CON1bits.CKP ) && ( !SSP2STATbits.BF ) )
	 #else
      if ( ( SSP2CON1bits.CKP ) && ( !SSP2STATbits.BF ) )// if R/W=0 and BF=0, NOT ACK was received
	 #endif 
      {
        return ( -2 );             // terminate PutsI2C2() function
      }
    }

  wrptr ++;                        // increment pointer
  Length--;                        // Gal

  }                                // continue data writes until null character

  return ( 0 );
}
#endif

/********************************************************************
*     Function Name:    EEPageWrite2                                *
*     Return Value:     error condition status                      *
*     Parameters:       EE memory control, address and pointer 1    *
*     Description:      Writes data string to I2C2 EE memory        *
*                       device. This routine can be used for any I2C*
*                       EE memory device, which only uses 1 byte of *
*                       address data as in the 24LC01B/02B/04B/08B. *
*     Gal changes:      Add length parmeter, instead of null        *
*                       termination.                                *
********************************************************************/
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
//signed char EEPageWrite2( unsigned char control, unsigned char address, unsigned char *wrptr )
signed char EEPageWrite2L( unsigned char control, unsigned char address, unsigned char *wrptr , unsigned char Length) //Gal
{
  IdleI2C2();                      // ensure module is idle
  StartI2C2();                     // initiate START condition
  #ifdef SW_TIP_TIMEOUT
   while ( SSP2CON2bits.SEN && IIC_NOT_TIMOUT );      // wait until start condition is over 
  #else
   while ( SSP2CON2bits.SEN );      // wait until start condition is over 
  #endif
#if defined (I2C_V3) || defined (I2C_V6)
  if ( PIR3bits.BCL2IF )           // test for bus collision
#elif defined (I2C_V6_1)
  if ( PIR2bits.BCL2IF )           // test for bus collision
#endif  
  {
    return ( -1 );                 // return with Bus Collision error 
  }
  else
  {
    if ( WriteI2C2( control ) )    // write 1 byte - R/W bit should be 0
    {
	  StopI2C2(); 
      return ( -3 );               // return with write collision error
    }

    if ( !SSP2CON2bits.ACKSTAT )   // test for ACK condition, if received 
    { 
      if ( WriteI2C2( address ) )  // write address byte to EEPROM
      {
	    StopI2C2(); 
        return ( -3 );             // return with write collision error
      }

      if ( !SSP2CON2bits.ACKSTAT ) // test for ACK condition, if received
      {
        if ( putsI2C2L( wrptr , Length) ) //Gal
        {
		  StopI2C2(); 
          return( -4 );            // bus device responded possible error
        }
      }
      else
      {
	    StopI2C2(); 
        return ( -2 );             // return with Not Ack error
      }
    }
    else
    {
	  StopI2C2(); 
      return ( -2 );               // return with Not Ack error
    }
  }

  StopI2C2();                      // send STOP condition
  #ifdef SW_TIP_TIMEOUT
   while ( SSP2CON2bits.PEN && IIC_NOT_TIMOUT );      // wait until stop condition is over 
  #else
   while ( SSP2CON2bits.PEN );      // wait until stop condition is over 
  #endif
#if defined (I2C_V3) || defined (I2C_V6)
  if ( PIR3bits.BCL2IF )           // test for bus collision
#elif defined (I2C_V6_1)
  if ( PIR2bits.BCL2IF )           // test for bus collision
#endif  
  {
    return ( -1 );                 // return with Bus Collision error 
  }
  #ifdef SW_TIP_TIMEOUT
    if (IIC_TIMOUT)
    {                              //Timeout - return error
        return-1;
    }   
  #endif
  return ( 0 );                    // return with no error
}
#endif

/********************************************************************
*   Function Name:  OpenI2C2                                        *
*   Return Value:   void                                            *
*   Parameters:     SSP2 peripheral setup bytes                     *
*   Description:    This function sets up the SSP2 module on a      * 
*                   PIC18CXXX device for use with a Microchip I2C   *
*                   EEPROM device or I2C bus device.                *
********************************************************************/
#if defined (I2C_V3)
void OpenI2C2( unsigned char sync_mode, unsigned char slew )
{
  SSP2STAT &= 0x3F;                // power on state 
  SSP2CON1 = 0x00;                 // power on state
  SSP2CON2 = 0x00;                 // power on state
  SSP2CON1 |= sync_mode;           // select serial mode 
  SSP2STAT |= slew;                // slew rate on/off 

  I2C2_SCL = 1;
  I2C2_SDA = 1;
  SSP2CON1 |= SSPENB;              // enable synchronous serial port 
}
#endif

#if defined (I2C_V6) || defined (I2C_V6_1)
void OpenI2C2( unsigned char sync_mode, unsigned char slew )
{
  SSP2STAT &= 0x3F;                // power on state 
  SSP2CON1 = 0x00;                 // power on state
  SSP2CON2 = 0x00;                 // power on state
  SSP2CON1 |= sync_mode;           // select serial mode 
  SSP2STAT |= slew;                // slew rate on/off 
 
  SSP2CON1 |= SSPENB;              // enable synchronous serial port 
}
#endif

/********************************************************************
*     Function Name:    EESequentialRead2                           *
*     Return Value:     error condition status                      *
*     Parameters:       EE memory control, address, pointer and     *
*                       length bytes.                               *
*     Description:      Reads data string from I2C EE memory        *
*                       device. This routine can be used for any I2C*
*                       EE memory device, which only uses 1 byte of *
*                       address data as in the 24LC01B/02B/04B/08B. *
*                                                                   *  
********************************************************************/
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
signed char EESequentialRead2( unsigned char control, unsigned char address, unsigned char *rdptr, unsigned char length )
{
  IdleI2C2();                      // ensure module is idle
  StartI2C2();                     // initiate START condition
  #ifdef SW_TIP_TIMEOUT
   while ( SSP2CON2bits.SEN && IIC_NOT_TIMOUT );      // wait until start condition is over 
  #else
   while ( SSP2CON2bits.SEN );      // wait until start condition is over 
  #endif
  PIR3bits.BCL2IF = 0;              //Gal.
#if defined (I2C_V3) || defined (I2C_V6)
  if ( PIR3bits.BCL2IF )           // test for bus collision
#elif defined (I2C_V6_1)
  if ( PIR2bits.BCL2IF )           // test for bus collision
#endif  
  {
    return ( -1 );                 // return with Bus Collision error 
  }
  else
  {
    if ( WriteI2C2( control ) )    // write 1 byte 
    {
	  StopI2C2(); 
      return ( -3 );               // set error for write collision
    }

    if ( !SSP2CON2bits.ACKSTAT )   // test for ACK condition, if received
    {
      if ( WriteI2C2( address ) )  // WRITE word address to EEPROM
      {
	    StopI2C2(); 
        return ( -3 );             // return with write collision error
      }
 
      if ( !SSP2CON2bits.ACKSTAT ) // test for ACK condition received
      {
        RestartI2C2();             // generate I2C2 bus restart condition
        #ifdef SW_TIP_TIMEOUT
         while ( SSP2CON2bits.RSEN && IIC_NOT_TIMOUT );  // wait until re-start condition is over 
        #else
         while ( SSP2CON2bits.RSEN );  // wait until re-start condition is over 
        #endif
        if ( WriteI2C2( control +1 ) )// WRITE 1 byte - R/W bit should be 1 for read
        {
		  StopI2C2(); 
          return ( -3 );           // set error for write collision
        }
      
        if ( !SSP2CON2bits.ACKSTAT )// test for ACK condition received
        {
          if ( getsI2C2( rdptr, length ) )// read in multiple bytes
          {
		    StopI2C2(); 
            return ( -1 );         // return with Bus Collision error
          }

          NotAckI2C2();            // send not ACK condition
          #ifdef SW_TIP_TIMEOUT
           while ( SSP2CON2bits.ACKEN && IIC_NOT_TIMOUT );// wait until ACK sequence is over 
          #else
           while ( SSP2CON2bits.ACKEN );// wait until ACK sequence is over 
          #endif
          StopI2C2();              // send STOP condition
          #ifdef SW_TIP_TIMEOUT
           while ( SSP2CON2bits.PEN && IIC_NOT_TIMOUT );// wait until stop condition is over 
          #else
           while ( SSP2CON2bits.PEN );// wait until stop condition is over 
          #endif
#if defined (I2C_V3) || defined (I2C_V6)
  if ( PIR3bits.BCL2IF )           // test for bus collision
#elif defined (I2C_V6_1)
  if ( PIR2bits.BCL2IF )           // test for bus collision
#endif  
          {
            return ( -1 );         // return with Bus Collision error 
          }
        }
        else
        {
		  StopI2C2(); 
          return ( -2 );           // return with Not Ack error
        }
      }
      else
      {
	    StopI2C2(); 
        return ( -2 );             // return with Not Ack error
      } 
    }
    else
    {
	  StopI2C2(); 
      return ( -2 );               // return with Not Ack error
    }
  }
  #ifdef SW_TIP_TIMEOUT
    if (IIC_TIMOUT)
    {                              //Timeout - return error
        return-1;
    }   
  #endif
  return ( 0 );                    // return with no error
}
#endif
