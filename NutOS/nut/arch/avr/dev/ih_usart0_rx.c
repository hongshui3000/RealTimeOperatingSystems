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
 * \file arch/avr/dev/ih_usart0_rx.c
 * \brief USART0 receive complete interrupt.
 *
 * \verbatim
 * $Id: ih_usart0_rx.c 4937 2013-01-22 11:38:42Z haraldkipp $
 * \endverbatim
 */

#include <dev/irqreg.h>

/*!
 * \addtogroup xgIrqReg
 */
/*@{*/

static int AvrUart0RxIrqCtl(int cmd, void *param);

IRQ_HANDLER sig_UART0_RECV = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    AvrUart0RxIrqCtl            /* Interrupt control, ir_ctl. */
};

/*!
 * \brief USART0 receive complete interrupt.
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
static int AvrUart0RxIrqCtl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    int_fast8_t enabled = bit_is_set(UCR, RXCIE);

    /* Disable interrupt. */
    cbi(UCR, RXCIE);

    switch (cmd) {
    case NUT_IRQCTL_INIT:
        enabled = 0;
    case NUT_IRQCTL_CLEAR:
        /* Clear any pending interrupt. */
        inb(UDR);
        inb(UDR);
        break;
    case NUT_IRQCTL_STATUS:
        if (bit_is_set(USR, RXC)) {
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
        *ival = 16;
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (unsigned int) sig_UART0_RECV.ir_count;
        sig_UART0_RECV.ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        sbi(UCR, RXCIE);
    }
    return rc;
}

/* avr-libc names the vector as in the datasheets. As Atmel naming is
 * inconsistant, so is the avr-libc naming.
 * Equalize!
 */
#if !defined(USART0_RX_vect) && defined( UART0_RX_vect)
#define USART0_RX_vect  UART0_RX_vect
#elif !defined(USART0_RX_vect) && defined(UART_RX_vect)
#define USART0_RX_vect UART_RX_vect
#endif

/*! \fn  UART0_RX_vect(void)
 * \brief Uart0 receive complete interrupt entry.
 */
#ifdef __IMAGECRAFT__
#if defined(iv_USART0_RX)
#pragma interrupt_handler USART0_RX_vect:iv_USART0_RX
#else
#pragma interrupt_handler USART0_RX_vect:iv_UART_RX
#endif
#endif
NUTSIGNAL(USART0_RX_vect, sig_UART0_RECV)
/*@}*/
