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

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "portable.h"

/* Demo app includes. */
#include "partest.h"

/* Hardware specific definitions. */
#include "AT91R40008.h"
#include "pio.h"
#include "aic.h"

#define partstNUM_LEDS			( 8 )
#define partstALL_OUTPUTS_OFF	( ( unsigned long ) ~(0xFFFFFFFF << partstNUM_LEDS) )

static unsigned long ulLEDReg;

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

static void SetLeds (unsigned int leds)
{
unsigned long ulPIOSetReg, ulPIOClearReg;

	/* LEDs are grouped in different port bits: P3-P6 and P16-P19.
	A port bit set to '0' turns an LED on, '1' turns it off. */

	ulPIOSetReg = ( (leds & 0xF) << 16 ) | ( (leds & 0xF0) >> 1 );
	ulPIOClearReg = (~ulPIOSetReg) & 0x000F0078;

	AT91C_BASE_PIO->PIO_SODR = ulPIOSetReg;
	AT91C_BASE_PIO->PIO_CODR = ulPIOClearReg;
}
/*-----------------------------------------------------------*/

void vParTestInitialise( void )
{
	/* This is performed from main() as the io bits are shared with other setup
	functions.  Ensure the outputs are off to start. */
	ulLEDReg = partstALL_OUTPUTS_OFF;  

	/* Enable clock to PIO... */
	AT91C_BASE_PS->PS_PCER = AT91C_PS_PIO;

	/* Enable all 8 LEDs and the four switches to be controlled by PIO... */
	AT91C_BASE_PIO->PIO_PER = P3 | P4 | P5 | P6 | P16 | P17 | P18 | P19 | P1 | P2 | P9 | P12;

	/* Configure all LED PIO lines for output... */
	AT91C_BASE_PIO->PIO_OER = P3 | P4 | P5 | P6 | P16 | P17 | P18 | P19;

	/* Configure all switch PIO lines for input... */
	AT91C_BASE_PIO->PIO_ODR = P1 | P2 | P9 | P12;

	/* Set initial state of LEDs. */
	SetLeds( ulLEDReg );
}
/*-----------------------------------------------------------*/

void vParTestSetLED( unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue )
{
	/* Switch an LED on or off as requested. */
	if (uxLED < partstNUM_LEDS)
	{
		if( xValue )
		{
			ulLEDReg &= ~(1 << uxLED);
		}
		else
		{
			ulLEDReg |= (1 << uxLED);
		}

		SetLeds( ulLEDReg );
	}
}
/*-----------------------------------------------------------*/

void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
	/* Toggle the state of the requested LED. */
	if (uxLED < partstNUM_LEDS)
	{
		ulLEDReg ^= ( 1 << uxLED );
		SetLeds( ulLEDReg );
	}
}

