/**
 * @defgroup PBA_INTERRUPTS
 * @{
 *******************************************************************************
 * @file			PBA_interrupts.c
 * @brief			Interrupt-Handler für PBA4/5/6
 * @author			ICT Berufsbildungscenter AG
 *******************************************************************************
 * 
 * @copyright
 * @{
 * 
 * Diese Software kann unter den Bedingungen der MIT-Lizenz verwendet werden.
 * 
 * Copyright &copy; 2016 ICT Berufsbildungscenter AG
 * 
 * #####MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @}
 */
#include "../inc/PBA_config.h"
#ifdef USE_INTERRUPT_LIBRARY

/**
 * @brief Globale Interruptroutine.
 */
void interrupt ISR_Global(void)
{
	/******* Timer 0 Interrupt **************************************************/
	#if defined (TMR0_INT)||defined(LOOPDELAY_USE_TIMER0)
	if(INTCONbits.T0IF && INTCONbits.T0IE)
	{
		#if defined (TMR0_INT)
			ISR_Timer0();
		#endif
		#if defined (LOOPDELAY_USE_TIMER0)
			LOOPDELAY_TimerISR();
            #if defined (USE_EVENTS_LIBRARY)
            EVENTS_TimerISR();
            #endif
		#endif
		T0IF=0;
	}
	#endif
	/******* Timer 1 Interrupt **************************************************/
	#if defined (TMR1_INT)|| defined (LOOPDELAY_USE_TIMER1)
	if(PIR1bits.TMR1IF && PIE1bits.TMR1IE)
	{
		PIR1bits.TMR1IF=0;
		#if defined (TMR1_INT)
			ISR_Timer1();
		#endif
		#if defined (LOOPDELAY_USE_TIMER1)
			LOOPDELAY_TimerISR();
            #if defined (USE_EVENTS_LIBRARY)
            EVENTS_TimerISR();
            #endif
		#endif
		
	}
	#endif

	/******* Timer 2 Interrupt **************************************************/
	#if defined (TMR2_INT)||defined (LOOPDELAY_USE_TIMER2)
	if(PIR1bits.TMR2IF && PIE1bits.TMR2IE)
	{
		PIR1bits.TMR2IF=0;
		#if defined (TMR2_INT)
			ISR_Timer2();
		#endif
		#if defined (LOOPDELAY_USE_TIMER2)      /*loop-delay lib aktiviert*/
			LOOPDELAY_TimerISR();
            #if defined (USE_EVENTS_LIBRARY)    /*events-lib aktiviert*/
            EVENTS_TimerISR();
            #endif
		#endif
	}
	#endif

	/******* USART-Receive-Interrupt ********************************************/
	#if defined (USART_RC_INT)
	if(PIR1bits.RCIF && PIE1bits.RCIE)
	{
		ISR_UartRx();
	}
	#endif

	/******* Interrupt an ext. Interrupt-Pin (PORTB0) ***************************/
	#if defined (EXT_INT)
	if(INTCONbits.INTF && INTCONbits.INTE)
	{
		ISR_Ext();
		INTCONbits.INTF=0;
	}
	#endif
}

#endif

/**
 *@}
 */
