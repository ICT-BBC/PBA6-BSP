/**
 * @addtogroup PBA_INTERRUPTS
 * @{
 *******************************************************************************
 * @file            PBA_interrupts.h
 * @brief           Interrupt-Handler für das PBA6
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

/**
 * @brief Funktionspointer Callbackfunktionen
 */
typedef void (*callbackFunction_t) (void);

uint8_t INT_AddTmr0CallbackFnc (callbackFunction_t p_callBackFunction);
uint8_t INT_AddTmr1CallbackFnc (callbackFunction_t p_callBackFunction);
uint8_t INT_AddTmr2CallbackFnc (callbackFunction_t p_callBackFunction);
uint8_t INT_AddUartCallbackFnc (callbackFunction_t p_callBackFunction);
uint8_t INT_AddExtIntCallbackFnc (callbackFunction_t p_callBackFunction);

#endif

/**
 *@}
 */
