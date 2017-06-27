/**
 * @addtogroup PBA_UART
 * @{
 *******************************************************************************
 * @file            PBA_driver_uart.h
 * @brief           UART-Library für das PBA6
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
#ifndef _PBA_DRIVER_UART_
#define _PBA_DRIVER_UART_

void UART_Init(void);


uint8_t UART_Getc(void);


void UART_Putc(uint8_t c);


void UART_WriteLine(uint8_t *p_string);


void UART_Puts(uint8_t *p_string);


void UART_WriteLineRom(const uint8_t *p_string);


void UART_PutsRom(const uint8_t *p_string);


void UART_Gets(uint8_t *p_string, uint16_t maxLen);

uint8_t UART_GetHexDigit(void);

uint16_t UART_GetHexByte(void);

int8_t UART_GetByte(void);

int16_t UART_GetInt(void);

int32_t UART_GetLong(void);

float UART_GetFloat(void);

#endif
/**
 * @}
 */
