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
 * "Reg test" tasks - These fill the registers with known values, then check
 * that each register maintains its expected value for the lifetime of the
 * task.  Each task uses a different set of values.  The reg test tasks execute
 * with a very low priority, so get preempted very frequently.  A register
 * containing an unexpected value is indicative of an error in the context
 * switching mechanism.
 */
 
	RSEG    CODE:CODE(2)
	thumb


	EXTERN ulRegTest1LoopCounter
	EXTERN ulRegTest2LoopCounter

	PUBLIC vRegTest1Task
	PUBLIC vRegTest2Task

/*-----------------------------------------------------------*/
vRegTest1Task

	/* Fill the core registers with known values.  This is only done once. */
	movs r1, #101
	movs r2, #102
	movs r3, #103
	movs r4, #104
	movs r5, #105
	movs r6, #106
	movs r7, #107
	movs r0, #108
	mov	 r8, r0
	movs r0, #109
	mov  r9, r0
	movs r0, #110
	mov	 r10, r0
	movs r0, #111
	mov	 r11, r0
	movs r0, #112
	mov  r12, r0
	movs r0, #100

reg1_loop
	/* Repeatedly check that each register still contains the value written to
	it when the task started. */
	cmp	r0, #100
	bne	reg1_error_loop
	cmp	r1, #101
	bne	reg1_error_loop
	cmp	r2, #102
	bne	reg1_error_loop
	cmp r3, #103
	bne	reg1_error_loop
	cmp	r4, #104
	bne	reg1_error_loop
	cmp	r5, #105
	bne	reg1_error_loop
	cmp	r6, #106
	bne	reg1_error_loop
	cmp	r7, #107
	bne	reg1_error_loop
	movs r0, #108
	cmp	r8, r0
	bne	reg1_error_loop
	movs r0, #109
	cmp	r9, r0
	bne	reg1_error_loop
	movs r0, #110
	cmp	r10, r0
	bne	reg1_error_loop
	movs r0, #111
	cmp	r11, r0
	bne	reg1_error_loop
	movs r0, #112
	cmp	r12, r0
	bne	reg1_error_loop

	/* Everything passed, increment the loop counter. */
	push { r1 }
	ldr	r0, =ulRegTest1LoopCounter
	ldr r1, [r0]
	adds r1, r1, #1
	str r1, [r0]

	/* Yield to increase test coverage. */
	movs r0, #0x01
	ldr r1, =0xe000ed04 /* NVIC_INT_CTRL */
	lsls r0 ,r0, #28 /* Shift to PendSV bit */
	str r0, [r1]
	dsb
	pop { r1 }

	/* Start again. */
	movs r0, #100
	b reg1_loop

reg1_error_loop
	/* If this line is hit then there was an error in a core register value.
	The loop ensures the loop counter stops incrementing. */
	b reg1_error_loop
	nop



vRegTest2Task

	/* Fill the core registers with known values.  This is only done once. */
	movs r1, #1
	movs r2, #2
	movs r3, #3
	movs r4, #4
	movs r5, #5
	movs r6, #6
	movs r7, #7
	movs r0, #8
	mov	r8, r0
	movs r0, #9
	mov r9, r0
	movs r0, #10
	mov	r10, r0
	movs r0, #11
	mov	r11, r0
	movs r0, #12
	mov r12, r0
	movs r0, #10

reg2_loop
	/* Repeatedly check that each register still contains the value written to
	it when the task started. */
	cmp	r0, #10
	bne	reg2_error_loop
	cmp	r1, #1
	bne	reg2_error_loop
	cmp	r2, #2
	bne	reg2_error_loop
	cmp r3, #3
	bne	reg2_error_loop
	cmp	r4, #4
	bne	reg2_error_loop
	cmp	r5, #5
	bne	reg2_error_loop
	cmp	r6, #6
	bne	reg2_error_loop
	cmp	r7, #7
	bne	reg2_error_loop
	movs r0, #8
	cmp	r8, r0
	bne	reg2_error_loop
	movs r0, #9
	cmp	r9, r0
	bne	reg2_error_loop
	movs r0, #10
	cmp	r10, r0
	bne	reg2_error_loop
	movs r0, #11
	cmp	r11, r0
	bne	reg2_error_loop
	movs r0, #12
	cmp	r12, r0
	bne	reg2_error_loop

	/* Everything passed, increment the loop counter. */
	push { r1 }
	ldr	r0, =ulRegTest2LoopCounter
	ldr r1, [r0]
	adds r1, r1, #1
	str r1, [r0]
	pop { r1 }

	/* Start again. */
	movs r0, #10
	b reg2_loop

reg2_error_loop
	;/* If this line is hit then there was an error in a core register value.
	;The loop ensures the loop counter stops incrementing. */
	b reg2_error_loop
	nop

	END
