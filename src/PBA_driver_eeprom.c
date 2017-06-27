/**
 * @defgroup PBA_EEPROM
 * @{
 *******************************************************************************
 * @file            PBA_driver_eeprom.c
 * @brief           EEPROM-Library für das PBA6
 * @author          ICT Berufsbildungscenter AG, Microchip
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
 * @param eeAddr    EEPROM-Adresse
 * @param eeData    Zu schreibende Daten
 */
void EE_WriteByte(uint8_t eeAddr, uint8_t eeData)
{
    uint8_t GIEBitValue = 0;

    EEADRL = (eeAddr & 0x0ff);      /* Zu schreibende Adresse festlegen*/
    EEDATL = eeData;                /* Zu schreibende Daten festlegen*/
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;            /* EEPROM-Speicher auswählen*/
    EECON1bits.WREN = 1;            /* Schreiben aktivieren*/

    GIEBitValue = INTCONbits.GIE;
    INTCONbits.GIE = 0;             /* Alle Interrupts deaktivieren*/
    EECON2 = 0x55;                  /* Freischaltsequenz*/
    EECON2 = 0xAA;
    EECON1bits.WR = 1;              /* Schreibvorgang starten*/
    while (EECON1bits.WR);          /* Warten auf Abschluss Schreibvorgang*/
    EECON1bits.WREN = 0;            /* Schreiben deaktivieren*/
    INTCONbits.GIE = GIEBitValue;   /* Interrupt-Enable zurücksetzen*/
}

/**
 * @brief Lesen eines Bytes vom internen EEPROM.
 * @param eeAddr    EEPROM-Adresse
 * @return          Gelesene Daten
 */
uint8_t EE_ReadByte(uint8_t eeAddr)
{
    EEADRL = (eeAddr & 0x0ff);  /* Zu lesende Adresse festlegen*/
    EECON1bits.CFGS = 0;
    EECON1bits.EEPGD = 0;       /* EEPROM-Speicher auswählen*/
    EECON1bits.RD = 1;          /* Lesen*/
    NOP();
    NOP();
    return (EEDATL);
}

#endif
/**
 *@}
 */