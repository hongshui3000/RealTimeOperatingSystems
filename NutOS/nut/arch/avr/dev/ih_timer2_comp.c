/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*!
 * \file arch/avr/dev/ih_timer2_comp.c
 * \brief Timer/Counter 2 compare match interrupt.
 *
 * \verbatim
 * $Id: ih_timer2_comp.c 4706 2012-10-06 17:42:01Z haraldkipp $
 * \endverbatim
 */

#include <dev/irqreg.h>

#if defined(MCU_AT90CAN128) || defined(MCU_ATMEGA2560) || defined(MCU_ATMEGA2561) || defined(MCU_AT90USB1287)
#define INT_MASK_REG    TIMSK2
#define INT_STATUS_REG  TIFR2
#define INT_ENABLE_BIT  OCIE2A
#define INT_STATUS_BIT  OCF2A
#define INT_PRIORITY    8
#else
#define INT_MASK_REG    TIMSK
#define INT_STATUS_REG  TIFR
#define INT_ENABLE_BIT  OCIE2
#define INT_STATUS_BIT  OCF2
#define INT_PRIORITY    8
#endif

/*!
 * \addtogroup xgIrqReg
 */
/*@{*/

static int AvrTimer2CompIrqCtl(int cmd, void *param);

IRQ_HANDLER sig_OUTPUT_COMPARE2 = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    AvrTimer2CompIrqCtl         /* Interrupt control, ir_ctl. */
};

/*!
 * \brief Timer/Counter2 compare match interrupt control.
 *
 * \param cmd   Control command.
 *              - NUT_IRQCTL_INIT Initialize and disable interrupt.
 *              - NUT_IRQCTL_CLEAR Clear interrupt.
 *              - NUT_IRQCTL_STATUS Query interrupt status.
 *              - NUT_IRQCTL_ENABLE Enable interrupt.
 *              - NUT_IRQCTL_DISABLE Disable interrupt.
 *              - NUT_IRQCTL_GETPRIO Query interrupt priority.
 *              - NUT_IRQCTL_GETCOUNT Query and clear interrupt counter.
 * \param param Pointer to optional parameter.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AvrTimer2CompIrqCtl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    int_fast8_t enabled = bit_is_set(INT_MASK_REG, INT_ENABLE_BIT);

    /* Disable interrupt. */
    cbi(INT_MASK_REG, INT_ENABLE_BIT);

    switch (cmd) {
    case NUT_IRQCTL_INIT:
        enabled = 0;
    case NUT_IRQCTL_CLEAR:
        /* Clear any pending interrupt. */
        outb(INT_STATUS_REG, _BV(INT_STATUS_BIT));
        break;
    case NUT_IRQCTL_STATUS:
        if (bit_is_set(INT_STATUS_REG, INT_STATUS_BIT)) {
            *ival = 1;
        } else {
            *ival = 0;
        }
        if (enabled) {
            *ival |= 0x80;
        }
        break;
    case NUT_IRQCTL_ENABLE:
        enabled = 1;
        break;
    case NUT_IRQCTL_DISABLE:
        enabled = 0;
        break;
    case NUT_IRQCTL_GETPRIO:
        *ival = INT_PRIORITY;
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (unsigned int) sig_OUTPUT_COMPARE2.ir_count;
        sig_OUTPUT_COMPARE2.ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        sbi(INT_MASK_REG, INT_ENABLE_BIT);
    }
    return rc;
}

/*! \fn TIMER2_COMP_vect(void)
 * \brief Timer 2 output compare interrupt entry.
 */
#ifdef __IMAGECRAFT__
#if defined(ATMega2560) || defined(ATMega2561)
#pragma interrupt_handler TIMER2_COMP_vect:iv_TIMER2_COMPA
#else
#pragma interrupt_handler TIMER2_COMP_vect:iv_TIMER2_COMP
#endif
NUTSIGNAL(TIMER2_COMP_vect, sig_OUTPUT_COMPARE2)
#else
#if defined(MCU_ATMEGA2560) || defined(MCU_ATMEGA2561) || defined(MCU_AT90USB1287)
NUTSIGNAL(TIMER2_COMPA_vect, sig_OUTPUT_COMPARE2)
#else
NUTSIGNAL(TIMER2_COMP_vect, sig_OUTPUT_COMPARE2)
#endif
#endif

/*@}*/
