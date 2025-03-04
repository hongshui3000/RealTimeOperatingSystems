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

#ifndef LED_IO_H
#define LED_IO_H

/* Include the register definition file that is correct for the hardware being
used.  The C and assembler pre-processor must have one of the following board
definitions defined to have the correct register definition header file
included.  Alternatively, just manually include the correct files here. */


	#ifdef YRPBRL78G13
		#include "ior5f100le.h"
		#include "ior5f100le_ext.h"
		#define LED_BIT			( P7_bit.no7 )
		#define LED_INIT()		P7 &= 0x7F; PM7 &= 0x7F
	#endif /* YRPBRL78G13 */

	#ifdef YRDKRL78G14
		#include "ior5f104pj.h"
		#include "ior5f104pj_ext.h"
		#define LED_BIT			( P4_bit.no1 )
		#define LED_INIT() 		LED_BIT = 0
	#endif /* YRDKRL78G14 */

	#ifdef RSKRL78G1C
		#include "ior5f10jgc.h"
		#include "ior5f10jgc_ext.h"
		#define LED_BIT			( P0_bit.no1 )
		#define LED_INIT()		P0 &= 0xFD; PM0 &= 0xFD
	#endif /* RSKRL78G1C */

	#ifdef RSKRL78L13
		#include "ior5f10wmg.h"
		#include "ior5f10wmg_ext.h"
		#define LED_BIT			( P4_bit.no1 )
		#define LED_INIT() 		P4 &= 0xFD; PM4 &= 0xFD
	#endif /* RSKRL78L13 */

	#ifdef RL78_G1A_TB
		#include "ior5f10ele.h"
		#include "ior5f10ele_ext.h"
		#define LED_BIT			( P6_bit.no2 )
		#define LED_INIT() 		P6 &= 0xFB; PM6 &= 0xFB
	#endif /* RL78_G1A_TB */

	#ifndef LED_BIT
		#error The hardware platform is not defined
	#endif

#endif /* LED_IO_H */

