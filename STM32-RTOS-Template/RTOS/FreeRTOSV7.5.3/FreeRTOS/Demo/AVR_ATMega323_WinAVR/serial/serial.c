/*
    FreeRTOS V7.5.3 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
Changes from V1.2.3

	+ The function xPortInitMinimal() has been renamed to 
	  xSerialPortInitMinimal() and the function xPortInit() has been renamed
	  to xSerialPortInit().

Changes from V2.0.0

	+ Delay periods are now specified using variables and constants of
	  portTickType rather than unsigned long.
	+ xQueueReceiveFromISR() used in place of xQueueReceive() within the ISR.

Changes from V2.6.0

	+ Replaced the inb() and outb() functions with direct memory
	  access.  This allows the port to be built with the 20050414 build of
	  WinAVR.
*/

/* BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER. */


#include <stdlib.h>
#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "serial.h"

#define serBAUD_DIV_CONSTANT			( ( unsigned long ) 16 )

/* Constants for writing to UCSRB. */
#define serRX_INT_ENABLE				( ( unsigned char ) 0x80 )
#define serRX_ENABLE					( ( unsigned char ) 0x10 )
#define serTX_ENABLE					( ( unsigned char ) 0x08 )
#define serTX_INT_ENABLE				( ( unsigned char ) 0x20 )

/* Constants for writing to UCSRC. */
#define serUCSRC_SELECT					( ( unsigned char ) 0x80 )
#define serEIGHT_DATA_BITS				( ( unsigned char ) 0x06 )

static xQueueHandle xRxedChars; 
static xQueueHandle xCharsForTx; 

#define vInterruptOn()										\
{															\
	unsigned char ucByte;								\
															\
	ucByte = UCSRB;											\
	ucByte |= serTX_INT_ENABLE;								\
	UCSRB = ucByte;											\
}																				
/*-----------------------------------------------------------*/

#define vInterruptOff()										\
{															\
	unsigned char ucInByte;								\
															\
	ucInByte = UCSRB;										\
	ucInByte &= ~serTX_INT_ENABLE;							\
	UCSRB = ucInByte;										\
}
/*-----------------------------------------------------------*/

xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
unsigned long ulBaudRateCounter;
unsigned char ucByte;

	portENTER_CRITICAL();
	{
		/* Create the queues used by the com test task. */
		xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
		xCharsForTx = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

		/* Calculate the baud rate register value from the equation in the
		data sheet. */
		ulBaudRateCounter = ( configCPU_CLOCK_HZ / ( serBAUD_DIV_CONSTANT * ulWantedBaud ) ) - ( unsigned long ) 1;

		/* Set the baud rate. */	
		ucByte = ( unsigned char ) ( ulBaudRateCounter & ( unsigned long ) 0xff );	
		UBRRL = ucByte;

		ulBaudRateCounter >>= ( unsigned long ) 8;
		ucByte = ( unsigned char ) ( ulBaudRateCounter & ( unsigned long ) 0xff );	
		UBRRH = ucByte;

		/* Enable the Rx interrupt.  The Tx interrupt will get enabled
		later. Also enable the Rx and Tx. */
		UCSRB = ( serRX_INT_ENABLE | serRX_ENABLE | serTX_ENABLE );

		/* Set the data bits to 8. */
		UCSRC = ( serUCSRC_SELECT | serEIGHT_DATA_BITS );
	}
	portEXIT_CRITICAL();
	
	/* Unlike other ports, this serial code does not allow for more than one
	com port.  We therefore don't return a pointer to a port structure and can
	instead just return NULL. */
	return NULL;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, portTickType xBlockTime )
{
	/* Only one port is supported. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, portTickType xBlockTime )
{
	/* Only one port is supported. */
	( void ) pxPort;

	/* Return false if after the block time there is no room on the Tx queue. */
	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
	{
		return pdFAIL;
	}

	vInterruptOn();

	return pdPASS;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
unsigned char ucByte;

	/* The parameter is not used. */
	( void ) xPort;

	/* Turn off the interrupts.  We may also want to delete the queues and/or
	re-install the original ISR. */

	portENTER_CRITICAL();
	{
		vInterruptOff();
		ucByte = UCSRB;
		ucByte &= ~serRX_INT_ENABLE;
		UCSRB = ucByte;
	}
	portEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

SIGNAL( SIG_UART_RECV )
{
signed char cChar;
signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Get the character and post it on the queue of Rxed characters.
	If the post causes a task to wake force a context switch as the woken task
	may have a higher priority than the task we have interrupted. */
	cChar = UDR;

	xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );

	if( xHigherPriorityTaskWoken != pdFALSE )
	{
		taskYIELD();
	}
}
/*-----------------------------------------------------------*/

SIGNAL( SIG_UART_DATA )
{
signed char cChar, cTaskWoken;

	if( xQueueReceiveFromISR( xCharsForTx, &cChar, &cTaskWoken ) == pdTRUE )
	{
		/* Send the next character queued for Tx. */
		UDR = cChar;
	}
	else
	{
		/* Queue empty, nothing to send. */
		vInterruptOff();
	}
}

