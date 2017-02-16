/**
 * @defgroup PBA_UART
 * @{
 *******************************************************************************
 * @file			PBA_driver_uart.c
 * @brief			UART-Library für PBA4/5/6
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
#ifdef USE_UART_LIBRARY

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @name Konfiguration UART Baudarate
 * Anpassen falls nötig
 * @{
 */
#define UART_BAUD 115200     /**< BAUD-Rate 300-115200 (115.2k nur bei FOSC>=3.6864 MHz)*/
/**
 * @}
 */

/**
 *@brief Berechnung Taktgenerator
 */
/*Berechnung Teilerwert für den Baudrate-Generator*/
#if defined(_16F877A)||defined(_16F874A)||defined(_16F877)||defined(_16F874)
	#define SPBRG_VAL ((uint16_t)(_XTAL_FREQ/(64UL * UART_BAUD) -1))	
#elif defined(_16F887)||defined(_16F884)||defined(_16F1787)
	/*Berechnung Teilerwert für den Baudrate-Generator*/
	#define SPBRG_VAL ((uint16_t)(_XTAL_FREQ/(4UL * UART_BAUD) -1)) 
#endif

/**
* @brief Initialisierung der Schnittstelle. 
 * Baudrate definieren mit Define @link UART_BAUD BAUD @endlink in @link PBA_driver_uart.c@endlink.
*/
void UART_Init(void)
{
	TXSTA=0x20;
	RCSTA=0x90;
	SPBRG=(uint8_t)SPBRG_VAL;				/*Baudrate (untere 8Bit)*/
#if defined(_16F877A)||defined(_16F874A)||defined(_16F877)||defined(_16F874)
	BRGH=0;
#elif defined(_16F1787)||defined(_16F887)||defined(_16F884)
	SPBRGH=SPBRG_VAL>>8;				/*Baudrate (obere 8Bit)*/
	BRGH=1;
	BRG16=1;
#endif
}

/**
 * @brief Ein Zeichen empfangen.
 * @return Empfangenes Zeichen
 */
uint8_t UART_Getc(void)
{
	CREN=0; CREN=1;						/*clear OERR-Overflow-Error-Bit*/
	while(!RCIF);						/*Warten auf Empfangs-Interrupt-Flag*/
	return RCREG;						/*Empfangens Byte zurückgeben*/
}

/**
 * @brief Ein Zeichen senden.
 * @param c Zu sendendes Zeichen/Datenbyte.
 */
void UART_Putc(uint8_t c)
{
	if(c=='\n')							/*Wenn Zeilenumbruch gefordert*/
	{
			TXREG='\n'; 				/*Line Feed in Senderegister schreiben*/
			while(!TRMT); 				/*Warten bis Zeichen gesendet*/
			c='\r';						/*nächstes Zeichen Carriage Return */
	}
	TXREG=c;							/*Zeichen in Senderegister schreiben*/
	while(!TRMT);						/*Warten bis Zeichen gesendet*/
}

/**
 * @brief Ein RAM-String mit anschliessendem Zeilenumbruch senden.
 * @param p_string Pointer auf Start der zu sendenden Daten
 */
void UART_WriteLine(uint8_t *p_string)
{
	while(*p_string!=0) 				/*Bis String-Ende*/
		UART_Putc(*p_string++);			/*Aktuelles String-Zeichen senden*/
	UART_Putc('\n');					/*Nach String ein Zeilenumbruch senden*/
}

/**
 * @brief Ein RAM-String ohne anschliessenden Zeilenumbruch senden.
 * @param p_string Pointer auf Start der zu sendenden Daten
 */
void UART_Puts(uint8_t *p_string)
{
	while(*p_string!=0) 				/*Bis String-Ende*/
		UART_Putc(*p_string++);			/*Aktuelles String-Zeichen senden*/
}

/**
 * @brief Ein ROM-String mit anschliessendem Zeilenumbruch senden.
 * @param p_string Pointer auf Start der zu sendenden Daten
 */
void UART_WriteLineRom(const uint8_t *p_string)
{
	while(*p_string!=0)  				/*Bis String-Ende*/
	{
		UART_Putc(*p_string++);			/*Aktuelles String-Zeichen senden*/
	}
	UART_Putc('\n');					/*Nach String ein Zeilenumbruch senden*/
}

/**
 * @brief Ein ROM-String ohne anschliessendem Zeilenumbruch senden.
 * @param p_string Pointer auf Start der zu sendenden Daten
 */
void UART_PutsRom(const uint8_t *p_string)
{
	while(*p_string!=0)  					/*Bis String-Ende */
	{
		UART_Putc(*p_string++);				/*Aktuelles String-Zeichen senden*/
	}
}

/**
 * @brief String mit definierter max. Länge empfangen und ablegen.
 * @param p_string Pointer auf Start des Speicherbereichs
 * @param maxLen Maximale Länge des Strings
 */
void UART_Gets(uint8_t *p_string, uint16_t maxLen)
{
	uint16_t len=0;						/*Anzahl empfangener Zeichen*/
	uint8_t c;							/*Empfangenes Zeichen*/
	maxLen--;							/*Nullterminierung abziehen von maximal möglicher Länge*/
	do 
	{
		c=UART_Getc();				 	/*Einzelnes Zeichen empfangen*/
		if(c==8)					 	/*Wenn ein Backspace empfangen wurde*/
		{
			if(len>0)				 	/*Wenn schon Zeichen empfangen wurden*/
				{
				len--;				 	/*Zeichen dekrementieren*/
				UART_Putc(c);		 	/*Backspace zurücksenden*/
				UART_Putc(' ');			/*gelöschtes Zeichen überschreiben*/
				UART_Putc(c);		 	/*Backspace zurücksenden*/
				}
		}
		else if ((c>=32)&&(c<=126))	 	/*Wenn druckbares Zeichen empfangen wurde*/
		{
			if(len<maxLen) 			 	/*Wenn maximale Stringlänge noch nicht erreicht*/
			{
				p_string[len++]=c;		/*Empfangenes Zeichen in String ablegen*/
				UART_Putc(c);			/*Empfangenes Zeichen zurücksenden*/
			}
		}
	} while(c!=13);						/*Zeichenempfange wiederholen bis CR empfangen*/
	p_string[len]=0;					/*Nullterminierung anhängen*/
}

/**
 * @brief Ein Hex-Digit (ASCII) empfangen und zurückgeben.
 * @return Hex-Digit als Zahlenwert
 */
uint8_t UART_GetHexDigit(void) 
{
	uint8_t digit;				
	digit = UART_Getc();
	UART_Putc(digit);
	if(digit<='9')
		return(digit-'0');
	else
		return((toupper(digit)-'A')+10);
}

/**
 * @brief Ein Hex-Byte (ASCII) empfangen und zurückgeben.
 * @return Hex-Byte als Zahlenwert
 */
uint16_t UART_GetHexByte(void)
{
	uint16_t lo,hi;
	hi = UART_GetHexDigit();
	lo = UART_GetHexDigit();
	if(lo==0xdd)
		return hi;
	else
		return hi*16+lo;
}

/**
 * @brief 1 Byte emfangen und zurückgeben.
 * @return Empfangener Zahlenwert
 */
int8_t UART_GetByte(void)
{
	uint8_t recBuffer[5];
	int8_t byteVal;
	UART_Gets(recBuffer, 5);
	byteVal=(int8_t)atoi(recBuffer);
	return byteVal;
}	

/**
 * @brief Einen Integer emfpangen und zurückgeben.
 * @return Empfangener Integer-Zahlenwert
 */
int16_t UART_GetInt(void) 
{
	uint8_t recBuffer[7];
	int16_t intVal;
	UART_Gets(recBuffer, 7);
	intVal=atoi(recBuffer);
	return intVal;
}

/**
 * @brief Einen Long-Integer empfangen und zurückgeben.
 * @return Empfangener Zahlenwert
 */
int32_t UART_GetLong(void) 
{
	uint8_t recBuffer[9];
	int32_t longVal;
	UART_Gets(recBuffer, 9);
	longVal=atol(recBuffer);
	return longVal;
}

/**
 * @brief Eine Floatingpoint-Zahl empfangen und zurückgeben
 * @return Empfangener Zahlenwert
 */
float UART_GetFloat(void) 
{
	uint8_t recBuffer[20];
	float floatVal;
	UART_Gets(recBuffer, 20);
	floatVal = atof(recBuffer);
	return(floatVal);
}
#endif

/**
 * @}
 */