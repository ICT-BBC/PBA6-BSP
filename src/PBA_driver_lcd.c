/**
 * @defgroup PBA_LCD
 * @{
 *******************************************************************************
 * @file			PBA_driver_lcd.c
 * @brief			LCD-Library für das PBA6
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
#ifdef USE_LCD_LIBRARY

static uint8_t LCD_GetSpecChar(uint8_t ch);

/**
 * @brief Sendet ein Datenbyte (z.B. ein Zeichen) oder einen Befehl ans LCD.
 *
 * @param value		Das zu übetragende Datenbyte / der zu übetragende Befehl (8Bit)
 */
void LCD_Send(uint8_t value)
{
	LCD_EN=1;						/*Pos. Flanke am Enable-Pin*/
	LCD_D7 = BIT_TEST(value,7);		/*Oberes Daten-Nibble anlegen*/
	LCD_D6 = BIT_TEST(value,6);
	LCD_D5 = BIT_TEST(value,5);
	LCD_D4 = BIT_TEST(value,4);
	LCD_EN=0;						/*Oberes Nibble senden*/
	#if (_XTAL_FREQ>20000000)
	NOP();							/*1 Zyklus warten-> verhindert zu kurzen Puls bei 32MHz (min. 130ns stable)*/
	#endif
	LCD_EN=1;						/*Pos. Flanke am Enable-Pin*/
	LCD_D7 = BIT_TEST(value,3);		/*Unteres Daten-Nibble anlegen*/
	LCD_D6 = BIT_TEST(value,2);
	LCD_D5 = BIT_TEST(value,1);
	LCD_D4 = BIT_TEST(value,0);
	LCD_EN=0;						/*Unteres Nibble senden*/
	__delay_us(30);					/*Maximaler Zeitbedarf einer Übertragung (blocking)*/
 }

/**
 * @brief Display löschen.
 * Das gesamte LCD-Daten-RAM wird mit 0x20 (Leerschlag) überschrieben
 * Da der Vorgang bis zu 2ms dauern kann muss der Programmablauf nach dem Senden des
 * Befehls entsprechend lang blockiert werden (delay).
 */
void LCD_Clear(void)
{
	LCD_SEND_COMMAND(0b00000001);				/*clear display*/
	DelayMS(2);									/*max. Zeitbedarf des LCD-Clear-Befehls*/
}

/**
 * @brief Rurücksetzen Anzeigeberich und Cursor.
 * Display-RAM zurückschieben (Ausgangsposition). Die Funktion setzt den Anzeigebereich
 * des LCDs und den Cursor zurück auf die Anfangsposition (Adresse 0).
 */
void LCD_Home(void)
 {
	LCD_SEND_COMMAND(0b00000010);				/*return home*/
	DelayMS(2);
 }

/**
 * @brief Display-RAM im Anzeigebereich nach links schieben.
 *
 * @param numPos		Anzahl Verschiebungsschritte des Display-RAMs
 */
void LCD_ShiftLeft(uint8_t numPos)
{
	for(;numPos!=0;numPos--)
		LCD_SEND_COMMAND(0b00011000);			/*display shift left*/
}

/**
 * @brief Display-RAM im Anzeigebereich nach rechts schieben.
 *
 * @param numPos		Anzahl Verschiebungsschritte des Display-RAMs
 */
void LCD_ShiftRight(uint8_t numPos)
{
	for(;numPos!=0;numPos--)
		LCD_SEND_COMMAND(0b00011100);			/*display shift right */
}

/**
 * @brief Setzt die Cursor-Position für ein 2 Zeilen-Display.
 *
 * @param posX		X-Koordinate (0-15) resp. Spalte auf die der Cursor gesetzt werden soll
 * @param posY		Y-Koordinate (0-1) resp. Zeile auf die der Cursor gesetzt werden soll
 */
void LCD_GotoXY(uint8_t posX, uint8_t posY)
{
	uint8_t dispAddress;
	if(0 == posY) dispAddress = 0x00;						/*Basisadresse 1. Zeile*/
	if(1 == posY) dispAddress = 0x40;						/*Basisadresse 2. Zeile*/
	dispAddress += posX;									/*Basisadresse + Spaltenadresse*/
	LCD_SEND_COMMAND((0b10000000 | dispAddress));			/*Cursor-Adresse an LCD senden*/
}

/**
 * @brief Umschreiben der Umlaute für japanischen Zeichensatz.
 * Zeichen im erweiterten Zeichensatz (ä,ö,ü) für den japanischen Zeichensatz ermitteln. Falls
 * Das übergebene Zeichen kein Umlaut ist wird das Originalzeichen zurück gegeben ansonsten
 * der für den dog-m-Zeichensatz korrekte Zeichenwert.
 *
 * @param ch		Zeichen das geprüft und falls Umlaut korrigiert werden soll
 * @return			Originalzeichen oder gemäss dog-m-Zeichensatz korrigiertes Zeichen
 */
static uint8_t LCD_GetSpecChar(uint8_t ch)
{
	switch (ch)
	{
		case 0xE4: return 0x84;		/*'ä'*/
		case 0xC4: return 0x8E;		/*'Ä'*/
		case 0xF6: return 0x94;		/*'ö'*/
		case 0xD6: return 0x99;		/*'Ö'*/
		case 0xFC: return 0x81;		/*'ü'*/
		case 0xDC: return 0x9A;		/*'Ü'*/
		default:  return ch;
	}
}

/**
 * @brief Ein Zeichen an das LCD senden (inkl. Sonderzeichenbearbeitung).
 *
 * @param ch		Zeichen das ans LCD gesendet werden soll
 */
void LCD_Putc(uint8_t ch)
{
	switch (ch)
	{
		case '\r':								/*Wenn Zeilenumbruch oder Zeilenvorschub...*/
		case '\n': LCD_GotoXY(0,1); break;		/*-> Cursor auf 1. Zeichen, 2. Zeile*/
		case '\f': LCD_Clear(); break;			/*Wenn form feed -> LCD löschen*/
		default:
			LCD_SEND_DATA(LCD_GetSpecChar(ch)); /*Zeichen an LCD senden*/
	}
}

/**
 * @brief String an LCD ausgeben mit Verwendung der lcd_putc-Funktion.
 *
 * @param *p_string		Zeiger auf den String mit Null-Terminierung
 */
void LCD_Puts(uint8_t *p_string)
{
	while(*p_string!=0)								/*Solange Nullterminierung noch nicht erreicht*/
		LCD_Putc(*p_string++);						/*Zeichen an LCD senden und Pointer auf nächstes Zeichen*/
}

	/**
 * @brief ROM-String an LCD ausgeben mit Verwendung der lcd_putc-Funktion.
 *
 * @param *p_romString		Zeiger auf den String mit Null-Terminierung
 */
void LCD_PutsRom(const uint8_t *p_romString)
{
	while(*p_romString!=0)							/*Solange Nullterminierung noch nicht erreicht*/
		LCD_Putc(*p_romString++);					/*Zeichen an LCD senden und Pointer auf nächstes Zeichen*/
}

/**
 * @brief Cursor oder blinkender Cursor ein-/ausschalten.
 *
 * @param cursorMode	Der einzustellende Cursor-Modus (CURSOR_ON, CURSOR_OFF, CURSOR_OFF_BLINK oder CURSOR_ON_BLINK)
 */
void LCD_SetCursorMode(uint8_t cursorMode)
{
	LCD_SEND_COMMAND(0b00001000|cursorMode);		/*Cursor-Wert an LCD senden*/
}

/**
 * @brief Initialisiert das dog-m-Text-LCD.
 *
 * Initialiert LCD mit 4-Bit-Modus, Cursor increment, Cursor off, Blink off,
 * Einstellungen des Boost-Converters, ...)
 *
 * Das LCD wird nach dem Initialisierungsvorgang zusätzlich gelöscht.
 * Diese Funktion sollte in einem Projekt nur einmal ausgeführt werden.
 *
 * Die TRIS-Register der LCD-Pins müssen diese als Ausgang definieren!
 * @param displayVoltage	Betriebsspannung des Displays (V_3V3, V_5V)
 */
void LCD_Init(displayVoltage_t displayVoltage)
{
	DelayMS(40);						/*Betriebsspannung am LCD für mind. 40ms vorhanden*/
	LCD_RS=0;							/*LCD interpretiert nächstes Zeichen als Befehl */
	LCD_RW=0;							/*Auf LCD schreiben...*/
	LCD_SEND_COMMAND(0b00000011);		/*Function Set 8-Bit*/
	DelayMS(2);							/*Pause gemäss init-Ablauf (siehe Datenblatt)*/
	LCD_SEND_COMMAND(0b00110011);		/*Function Set 8-Bit (2x)*/
	LCD_SEND_COMMAND(0b00100010);		/*Function Set 4-Bit-Modus*/
	LCD_SEND_COMMAND(0b00101001);		/*Function Set*/
	if(V_3V3==displayVoltage)
	{
		LCD_SEND_COMMAND(0b00010100);	/*Internal OSC frequency/BIAS Set(BS:1/5->3.3V)*/
		LCD_SEND_COMMAND(0b01010101);	/*Power/ICON/Contrast control (Booster on->3.3V)*/
		LCD_SEND_COMMAND(0b01101101);	/*Follower control*/
		LCD_SEND_COMMAND(0b01111000);	/*Contrast Set*/
	}
	else if(V_5V==displayVoltage)
	{
		LCD_SEND_COMMAND(0b00011100);	/*Internal OSC frequency/BIAS Set(BS:1/4->5V)*/
		LCD_SEND_COMMAND(0b01010010);	/*Power/ICON/Contrast control (Booster on->3.3V)*/
		LCD_SEND_COMMAND(0b01101001);	/*Follower control*/
		LCD_SEND_COMMAND(0b01110100);	/*Contrast Set*/
	}
	LCD_SEND_COMMAND(0b00101000);		/*Function Set*/
	LCD_SEND_COMMAND(0b00001100);		/*Display ON/OFF control*/
	LCD_SEND_COMMAND(0b00000001);		/*Display löschen / Cursor home*/
	DelayMS(2);							/*max. Zeitbedarf des LCD-Clear-Befehls*/
	LCD_SEND_COMMAND(0b00000110);		/*Cursor Auto-increment*/
	LCD_SEND_COMMAND(0b00101000);		/*Function Set*/
	LCD_SEND_COMMAND(0b00001100);		/*Display ON/OFF control*/
}
#endif

/**
 * @}
 */
