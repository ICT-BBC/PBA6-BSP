/**
 * @addtogroup PBA_HELPERS
 * @{
 *******************************************************************************
 * @file            PBA_helpers.h
 * @brief           Helper-Library für PBA4/5/6
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
#ifndef _PBA_HELPERS_
#define _PBA_HELPERS_

/******************** Helper Makros *******************************************/

/**
 * @brief Löscht ein bestimmtes Bit in einem Byte.
 * @param reg Datenbyte
 * @param bitNumber Bitnummer
 */
    #define BIT_CLEAR( reg, bitNumber ) ((reg) &= ~(1 << (bitNumber)))

/**
 * @brief Setzt ein bestimmtes Bit in einem Byte.
 * @param reg Datenbyte
 * @param bitNumber Bitnummer
 */
    #define BIT_SET( reg, bitNumber )       ((reg) |= (1 << (bitNumber)))

/**
 * @brief Wechselt den Zustand ein bestimmtes Bits in einem Byte.
 * @param reg Datenbyte
 * @param bitNumber Bitnummer
 */
    #define BIT_TOGGLE( reg, bitNumber )    ((reg) ^= (1 << (bitNumber)))

/**
 * @brief Testet ein bestimmtes Bit in einem Byte und retourniert Zustand.
 * @param reg Datenbyte
 * @param bitNumber Bitnummer
 */
    #define BIT_TEST( reg, bitNumber )  (((reg) & (1 << (bitNumber)))>0)


/******************** Prototypen Helper-Funktionen ****************************/

void DelayMS(uint16_t timeMS);


void DelayUS(uint16_t timeUS);


uint16_t BinToBcd(uint8_t value);


void Beep(uint16_t freqHZ, uint16_t durationMS);


uint8_t MirrorByte(uint8_t value);


uint8_t BitSum(uint8_t value);


uint8_t BitNumb(uint8_t value);
#endif
/**
 * @}
 */