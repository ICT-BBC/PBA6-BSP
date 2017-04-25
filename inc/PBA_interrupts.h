/**
 * @addtogroup PBA_INTERRUPTS
 * @{
 *******************************************************************************
 * @file            PBA_interrupts.h
 * @brief           Interrupt-Handler für PBA4/5/6
 * @author          ICT Berufsbildungscenter AG
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
#ifndef _PBA_INTERRUPTS_
#define _PBA_INTERRUPTS_

/******* Interrupt Konfiguration **********************************************/
/**
 * @name Interrupt-Konfiguration
 * Interrupts- ein oder auskommentieren
 * @{
 */

/*#define TMR0_INT*/
/*#define TMR1_INT*/
#define TMR2_INT
#define USART_RC_INT
/*#define EXT_INT*/

/**
 * @}
 */

/******* Interrupt-Callback-Funktionen *****************************************/
/**
 * @name Interrupt-Callback-Funktionen
 * Implementation in eigenem Code
 */
extern inline void ISR_Timer0(void);
extern inline void ISR_Timer1(void);
extern inline void ISR_Timer2(void);
extern inline void ISR_UartRx(void);
extern inline void ISR_Ext(void);
/**
 *@}
 */

#endif  


/**
 *@}
 */
