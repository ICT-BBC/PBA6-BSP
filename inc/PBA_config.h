/**
 * @addtogroup PBA_CONFIG
 * @{
 *******************************************************************************
 * @file        PBA_config.h
 * @brief       Konfiguration des Board-Support-Packages für das PBA6
 * @author      ICT Berufsbildungscenter AG
 * @version     1.4.1
 * @date        06.12.2013: Aufbau PBA6-BSP mit Basis PBA5-BSP,
 *                          Unterstützung von PIC16F1787, Anpassungen
 *                          für XC8-Compiler, Interrupt-Library
 * @date        17.02.2016: stdint.h hinzugefügt-->Wechsel auf std-integer Typen
 * @date        04.03.2016: Renamed RS232 to UART
 * @date        06.04.2016: Menu-Bibliothek hinzugefügt, Events erweitert
 * @date        08.04.2016: EEPROM-Bibliothek hinzugefügt
 * @date        15.04.2016: PBA Init-Funktion umbenennt zu pba_init()-->
 *                          alle Funktionen beginnen mit Name der Peripherie
 * @date        25.05.2016: Umstrukturierung statemachine library. loop-delay-
 *                          library wird immer benötigt, Events in eigener
 *                          Bibliothek ohne loop-delay
 * @date        07.02.2017  Renaming, Fehlermeldungen optimiert
 * @date        25.04.2017  Alle SFR-Zugriffe geändert (Bitfelder),
 *                          Support für alle PICs ausser PIC16F1787 entfernt,
 *                          fix Beep-Funktion
 * @date        26.06.2017  Umstrukturierung Interrupts.
 * @date        26.06.2017  Fix LCD Init für f_xtal<20MHz
 * @date        09.03.2018  ResetTimeout Funktion hinzugefügt. Menu-Fix
 * @date        28.08.2018  Anpassungen für XC8-Compiler V2.00
 *
 * @attention   PBA_Init zur Initialiserung und Konfigurations des PBAs.
 *              Um die Libraries zu verwenden, kopieren Sie die beiden Ordner
 *              "Source" und "Include" in Ihren MPLABX-Projektordner. Erstellen
 *              Sie ein Main-Sourcefile, speichern Sie dies im "Source"-Ordner
 *              und fügen Sie die Dateien in den beiden Ordnern ihrem
 *              MPLAB-Projekt hinzu.
 *              Fügen Sie folgende Codezeile ihrem Source-File hinzu:
 *              @code #include "../inc/PBA_config.h" @endcode
 *
 * @attention   Wenn Sie bestimmte Bibliotheken nicht benötigen, kommentieren
 *              Sie die zugehörigen Defines in der Datei @link PBA_config.h @endlink aus.
 *              Entfernen Sie die nicht benötigten Source-Files aus ihrem Projekt.
 *
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
#ifndef _PBA_CONFIG_
#define _PBA_CONFIG_

/********** BSP-Konfiguration *************************************************/


    /**
     * @addtogroup BSP-KONFIGURATION BSP-Konfiguration
     * Nicht benötigte Bibliotheken und Konfigurationen auskommentieren
     * @{
     */
    #define PBA6/**<LCD wird für 3.3V initialisiert. Auskommentieren, falls nicht PBA6 verwendet wird*/

    #define USE_ADC_LIBRARY         /**< ADC-Support Funktionen*/

    #define USE_LCD_LIBRARY         /**< LCD DOGM162 Funktionen*/

    #define USE_UART_LIBRARY        /**< UART Funktionen*/

    #define USE_I2C_LIBRARY         /**< I2C Funktionen*/

    #define USE_LM75_LIBRARY        /**< Ansteuerung I2C-Temperatursensor*/

    #define USE_EEPROM_LIBRARY      /**< Verwendung internes EEPROM*/

    #define USE_HELPERS_LIBRARY     /**< Nützliche Makros und Funktionen*/

    #define USE_INTERRUPT_LIBRARY   /**< Globaler Interrupt-Handler*/

    #define USE_LOOPDELAY_LIBRARY   /**< Timerbasiertes Delay*/

    #define USE_EVENTS_LIBRARY      /**< Zeit- und Taster-Events*/

    #define USE_MENU_LIBRARY        /**< Funktionen Erstellung und Darstellung eines Menus auf LCD*/

/********** Auswahl STDOUT ****************************************************/
    #define STDOUT_LCD              /**< Ausgabe printf auf LCD*/
    //#define STDOUT_UART           /**< Ausgabe printf auf UART-Schnittstelle*/

    /**
     * @}
     */

    /******************** Compiler-Includes, Clock Definiton **********************************/
    #include <xc.h>

    /**
     * @addtogroup DEF_DELAY Taktkonfiguration für Delay
     * Zur Verwendung der Delays muss die Taktfrequenz bekannt sein
     * @{
     */
        #define _XTAL_FREQ 32000000     /**< PIC16F1787--> 8MHz & 4xPLL*/
    /**
     * @}
    */

    #pragma jis                         /* JIS character handling aktivieren (ä,ö,ü) für XC-Compiler*/


    /******************** Schalter/Taster-, LEDs- und Peripherie-Ports **************************/
    /**
    * @addtogroup DEF_TASTER Taster-Bits
    * Ermöglichen Zugriff auf die Taster
    * @{
    */
    #define     TASTER0     PORTBbits.RB0
    #define     TASTER1     PORTBbits.RB1
    #define     TASTER2     PORTBbits.RB2
    #define     TASTER3     PORTBbits.RB3
    #define     TASTER4     PORTBbits.RB4
    #define     TASTER5     PORTBbits.RB5
    #define     TASTER6     PORTBbits.RB6
    #define     TASTER7     PORTBbits.RB7
    /**
     * @}
     */

    /**
    * @addtogroup DEF_LED_SUMMER Definition LED-und Summer-Bits
    * Ermöglichen Zugriff auf LEDs und Summer.
    * PIC16F1787 verfügt über Latch-Register
    * Ausgänge über Latch-Register ansprechen
    * um Fehler bei Bitmanipulationen zu vermeiden
    * @{
    */
    #define     LED0        LATDbits.LATD0
    #define     LED1        LATDbits.LATD1
    #define     LED2        LATDbits.LATD2
    #define     LED3        LATDbits.LATD3
    #define     LED4        LATDbits.LATD4
    #define     LED5        LATDbits.LATD5
    #define     LED6        LATDbits.LATD6
    #define     LED7        LATDbits.LATD7
    #define     LED8        LATEbits.LATE0
    #define     LED9        LATEbits.LATE1

    #define     LEDS_D      LATD
    #define     LEDS_E      LATE

    #define     SUMMER      LATCbits.LATC5
    /**
     * @}
     */

/******************** Einbinden der Bibliotheken*******************************/
    #include <stdio.h>              /**<IO-functions (printf, scanf,...)*/
    #include <string.h>             /**<String-functions*/
    #include <stdint.h>             /**<standard integer types*/

    /**
    * @name Libraries einbinden
    * Auskommentieren, falls Library nicht benötigt
    * @{
    */
    #ifdef  USE_HELPERS_LIBRARY
        #include "PBA_helpers.h"
    #endif
    #ifdef  USE_I2C_LIBRARY
        #include "PBA_driver_i2c.h"
    #endif
    #ifdef  USE_LCD_LIBRARY
        #include "PBA_driver_lcd.h"
    #endif
    #ifdef  USE_ADC_LIBRARY
        #include "PBA_driver_adc.h"
    #endif
    #ifdef  USE_LM75_LIBRARY
        #include "PBA_driver_lm75.h"
    #endif
    #ifdef  USE_UART_LIBRARY
        #include "PBA_driver_uart.h"
    #endif
    #ifdef  USE_LOOPDELAY_LIBRARY
        #include "PBA_loopdelay.h"
    #endif
    #ifdef  USE_EVENTS_LIBRARY
        #include "PBA_events.h"
    #endif
    #ifdef USE_INTERRUPT_LIBRARY
        #include "PBA_interrupts.h"
    #endif
    #ifdef USE_MENU_LIBRARY
        #include "PBA_menu.h"
    #endif
    #ifdef USE_EEPROM_LIBRARY
        #include "PBA_driver_eeprom.h"
    #endif
    /**
     * @}
     */
    /******************** Prototypen für Board-support Library Funktionen ***********************/
    void PBA_Init(void);

    /************** Überprüfen der Bibliothek-Abhängigkeiten ************************************/
    #if defined(USE_LM75_LIBRARY) && !defined(USE_I2C_LIBRARY)
        #error Um die LM75-Library zu nutzen, muss die I2C-Library eingebunden sein (PBA_config.h)
    #endif
    #if defined(STDOUT_LCD) && !defined(USE_LCD_LIBRARY)
        #error Um das LCD als Standard-Output (printf) zu nutzen muss die LCD-Bibliothek eingebunden sein (PBA_config.h)
    #endif
    #if defined(STDOUT_UART) && !defined(USE_UART_LIBRARY)
        #error Um UART als Standard-Output (printf) zu nutzen muss die UART-Bibliothek eingebunden sein (PBA_config.h)
    #endif
    #if defined(USE_EVENTS_LIBRARY)
        #if !defined(USE_LOOPDELAY_LIBRARY)
        #error Zur Verwendung der Events-Bibliothek muss die Loopdelay-Bibliothek eingebunden sein (PBA_config.h)
        #endif
        #if !defined(USE_HELPERS_LIBRARY)
        #error Zur Verwendung der Events-Bibliothek muss die Helpers-Bibliothek eingebunden sein (PBA_config.h)
        #endif
    #endif
    #if defined(USE_MENU_LIBRARY)
        #if !defined(USE_LCD_LIBRARY)
        #error Zur Verwendung der Menu-Bibliothek muss die LCD-Bibliothek eingebunden sein (PBA_config.h)
        #endif
        #if !defined(USE_EVENTS_LIBRARY)
        #error Zur Verwendung der Menu-Bibliothek muss die Events-Bibliothek eingebunden sein (PBA_config.h)
        #endif
    #endif
    #if defined (USE_LOOPDELAY_LIBRARY) && !defined (USE_INTERRUPT_LIBRARY)
        #error Um die Loopdelay-Bibliothek zu verwenden muss die Interrupt-Bibliothek eingebunden sein (PBA_config.h)
    #endif
    #if defined (USE_LCD_LIBRARY) && !defined (USE_HELPERS_LIBRARY)
        #error Zur Verwendung der LCD-Bibliothek muss die Helpers-Bibliothek eingebunden sein (PBA_config.h
    #endif
    /************** Überprüfung ob der ausgewählte PIC unterstützt wird vom Board-support Package **********/
    #if !defined(_16F1787)
        /*Kommentieren sie die Fehlermeldung aus, um das Board-Support Package mit einem nicht unterstützten*/
        /*Mikrocontroller zu nutzen.*/
        /*Vorsicht: Einige Bibliotheken funktionieren unter Umständen nicht!!*/
        #error Der ausgewählte PIC wird vom PBA Board-support Package nicht unterstützt!
    #endif

    /************** Deaktivieren der stack overflow Warnungen, Problem von Compiler gelöst **************/
    #pragma warning disable 1393,1090

#endif
/**
 *@}
 */
