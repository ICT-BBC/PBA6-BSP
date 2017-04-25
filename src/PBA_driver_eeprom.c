/**
 * @defgroup PBA_EEPROM
 * @{
 *******************************************************************************
 * @file			PBA_driver_eeprom.c
 * @brief			EEPROM-Library für PBA4/5/6
 * @author			ICT Berufsbildungscenter AG, Microchip
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

#ifdef USE_EEPROM_LIBRARY

/**
 * @brief Schreiben eines Bytes ins interne EEPROM.
 * @param eeAddr	EEPROM-Adresse
 * @param eeData	Zu schreibende Daten
 */
void EE_WriteByte(uint8_t eeAddr, uint8_t eeData)
{
	uint8_t GIEBitValue = 0;

	EEADRL = (eeAddr & 0x0ff);    /* Data Memory Address to write*/
	EEDATL = eeData;             /* Data Memory Value to write*/
	EECON1bits.EEPGD = 0;		/* Point to DATA memory*/
	EECON1bits.CFGS = 0;        /* Deselect Configuration space*/
	EECON1bits.WREN = 1;        /* Enable writes*/

	GIEBitValue = INTCONbits.GIE;
	INTCONbits.GIE = 0;			/* Disable INTs*/
	EECON2 = 0x55;
	EECON2 = 0xAA;
	EECON1bits.WR = 1;			/* Set WR bit to begin write*/
	/* Wait for write to complete*/
	while (EECON1bits.WR)
	{
	}

	EECON1bits.WREN = 0;		/* Disable writes*/
	INTCONbits.GIE = GIEBitValue;
}

/**
 * @brief Lesen eines Bytes vom internen EEPROM.
 * @param eeAddr	EEPROM-Adresse
 * @return			Gelesene Daten
 */
uint8_t EE_ReadByte(uint8_t eeAddr)
{
	EEADRL = (eeAddr & 0x0ff);    /* Data Memory Address to read*/
	EECON1bits.CFGS = 0;		/* Deselect Configuration space*/
	EECON1bits.EEPGD = 0;		/* Point to DATA memory*/
	EECON1bits.RD = 1;			/* EE Read*/
	NOP();  /* NOPs may be required for latency at high frequencies*/
	NOP();
	return (EEDATL);
}

#endif
/**
 *@}
 */