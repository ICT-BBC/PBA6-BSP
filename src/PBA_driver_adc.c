/**
 * @defgroup PBA_ADC
 * @{
 *******************************************************************************
 * @file			PBA_driver_adc.c
 * @brief			ADC-Library für PBA4/5/6
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
#ifdef USE_ADC_LIBRARY

/**
 * @name Ladezeit Hold-Kondensator
 * @{
 */
#define AQUISITION_TIME_US		10 /**< Ladezeit des Hold-Kondensators */
/**
 *@} 
 */			

#if defined (_16F1787)
/**
 * @brief Setzt die gewünschten Pins als Analogeingänge. 
 * Bsp: \code ADC_SetAnalogANx(AN0|AN3|AN11); \endcode
 * @param pin Pin-Nr: @ref ADC_ANALOG_INPUTS "Analog Eingänge"
 * 
 */
void ADC_SetAnalogANx(uint32_t pin)
{
ANSELA=pin>>24;
ANSELB=pin>>16;
ANSELD=pin>>8;
ANSELE=(uint8_t) pin;
}
#endif

/**
* @brief Initialisiert den AD-Wandler mit den angegebenen Einstellungen.
* Bsp: \code ADC_Init(ADC_CLOCK_DIV_32|VREFL_VSS|NEG_IN_REF|POS_IN_AN0); \endcode
* @param settings @ref ADC_SETTINGS "ADC Einstellungen"
*/
void ADC_Init(uint32_t settings)
{
#if defined (_16F1787)
ADCON0=settings>>16;
ADCON1=settings>>8;
ADCON2=(uint8_t) settings;
#endif
#if defined(_16F877A)||defined(_16F874A)||defined(_16F877)||defined(_16F874)
    ADCON0=settings>>8;
    ADCON1=(uint8_t)settings;
#endif	
#if defined(_16F887)||defined(_16F884)
    ADCON0=settings>>24;							/*Allgemeine Einstellungen ADC*/
    ADCON1=settings>>16;							/*VREF- = VSS, VREF+=VDD*/
    ANSELH= settings>>8;							/*Auswahl Analog/Digital-Ports*/
    ANSEL =(uint8_t)settings;						/*Auswahl Analog/Digital-Ports*/
#endif
}

/**
* @brief Stellt den angegebenen Channel zum Messen mit ADC ein.
* Beispiel: \code ADC_SetChannel(2); \endcode -> AN2 eingestellt
* @param channel Messchannel 
*/
void ADC_SetChannel(uint8_t channel)
{
#if defined(_16F1787)
    ADCON0&=0x83;
    ADCON0|=channel<<2;
#endif
#if defined(_16F877A)||defined(_16F874A)||defined(_16F877)||defined(_16F874)	
    ADCON0 &= 0xC7;
    ADCON0 |= channel<<3;
#endif
#if defined(_16F887)||defined(_16F884)	
    ADCON0 &= 0xC3;									/*Kanalauswahl löschen*/
    ADCON0 |= channel<<2;							/*Kanalauswahl setzen*/
#endif
    DelayUS(AQUISITION_TIME_US);					/*Aquisition-Time abwarten*/
}

/**
* @brief Startet eine AD-Wandlung und retourniert das Resultat.
* @return Ausgelesener AD-Wert, uint16
*/
uint16_t ADC_Read(void)
{
    uint16_t result;

    GO_nDONE = 1;									/*Wandlung starten (one shot)*/
    while(GO_nDONE);								/*Warten bis Wandlung abgeschlossen*/

    result=(uint16_t)ADRESH<<8;					/*Resultat auslesen*/
    result|=ADRESL;
    return result;									/*Rückgabe des AD-Wertes*/
}
#endif
	
/**
 * @}
 */