/**
 * @addtogroup PBA_LCD
 * @{
 *******************************************************************************
 * @file			PBA_driver_lcd.h
 * @brief			LCD-Library für PBA4/5/6
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
#ifndef _PBA_DRIVER_LCD_
#define _PBA_DRIVER_LCD_

/*Display-Pins*/

#if defined (__16F1787)
/**
 * @name Display Pins
 * Zuordnung der Pins zur Ansteuerung des LCD
 * @{
 */
#define 	LCD_D4 			LATE2
#define 	LCD_D5 			LATA2
#define 	LCD_D6 			LATA4
#define 	LCD_D7 			LATA5
#define 	LCD_RW 			LATC0
#define 	LCD_EN 			LATC1
#define 	LCD_RS 			LATC2
/**
 * @}
 */
#else
#define 	LCD_D4 			RE2
#define 	LCD_D5 			RA2
#define 	LCD_D6 			RA4
#define 	LCD_D7 			RA5
#define 	LCD_RW 			RC0
#define 	LCD_EN 			RC1
#define 	LCD_RS 			RC2
#endif

/*Defines für Curson-Funktion (dispcurs(mode);)*/
/**
 * @name Cursor Modi
 * Defintion der verschiedenen Cursor-Modi
 * @see lcd_curs
 * @{
 */
#define CURSOR_ON 			0x06
#define CURSOR_OFF 			0x04
#define CURSOR_OFF_BLINK 	0x05
#define CURSOR_ON_BLINK 	0x07
/**
 * @}
 */

/**
 * @brief	Betriebsspannung LCD
 */
typedef enum  
{
	V_3V3,	/**< 3.3V--> PBA6*/
	V_5V	/**< 5V --> ältere Boards*/
}displayVoltage_t;

/*Makros für das Senden eines Befehl*/
#define LCD_SEND_COMMAND(Command)	LCD_Send(Command)
/*Makros für das Senden eines Datenbytes (Zeichen)*/
#define LCD_SEND_DATA(value)		LCD_RS=1;LCD_Send(value);LCD_RS=0

void LCD_Send(uint8_t value);

void LCD_Clear(void);

void LCD_Home(void);

void LCD_ShiftLeft(uint8_t numPos);

void LCD_ShiftRight(uint8_t numPos);

void LCD_GotoXY(uint8_t posX, uint8_t posY);

void LCD_Putc(uint8_t ch);

void LCD_Puts(uint8_t *p_string);

void LCD_PutsRom(const uint8_t *p_romString);

void LCD_SetCursorMode(uint8_t cursorMode);


void LCD_Init(displayVoltage_t displayVoltage);

#endif

/**
 * @}
 */


