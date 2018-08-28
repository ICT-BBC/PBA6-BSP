/**
 * @defgroup PBA_HARDWARETEST
 * @{
 *******************************************************************************
 * @file        PBA6_hardwaretest.c
 * @brief       Hardwaretestsoftware für das PIC Board Advanced 6
 * @author      ICT Berufsbildungscenter AG
 * @version     2.1.1
 * @date        23.01.2017: Komplette Überarbeitung, Umsetzung als FSM
 *                          Implementierung aller Library-Features
 * @date        25.04.2047: SFR Zugriffe geändert (Bitfelder), fix Intro
 * @date        26.06.2017: Anpassungen auf neue Interruptumsetzung BSP V1.3.0
 * @date        28.08.2018: Anpassung für XC8 Compiler V2.00
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
#include "../../inc/PBA_config.h"


/********************* Defines / Makros / Konstanten ****************************************/
#define UART_BUFFERSIZE         (16)            /**< Anzahl Zeichen die maximal eingegeben werden können*/

const uint8_t EEADDR_CHECKVALID = 0x00;         /**< EEPROM-Adresse, Überprüfung ob bereits Daten geschrieben*/
const uint8_t EEADDR_INTRO      = 0x01;         /**< EEPROM-Adresse, Intro an/aus*/
const uint8_t EEADDR_SUMMER     = 0x02;         /**< EEPROM-Adresse, Summer Frequenz*/

const uint8_t EE_VALID          = 0x00;         /**< EEPROM-Daten bestehend*/
const uint8_t EE_DEFAULT_INTRO  = 0x01;         /**< EEPROM-Standard-Wert Intro an/aus. Standard: Intro an*/
const uint8_t EE_DEFAULT_SUMMER = 500;          /**< EEPROM-Standard-Wert Summerfrequenz. Standard: 500Hz*/

const uint8_t FREQ_MAX          = 500;          /**< Maximale Summerfrequenz*/
const uint8_t FREQ_STEP         = 100;          /**< Summer: 100Hz Schritt*/
const uint8_t FREQ_MIN          = 100;          /**< Minimale Summerfrequenz*/

/********************* Datentypen ***********************************************/

/**
 *@brief Aufzählung für Zustandsmaschine
 */
typedef enum
{
    ST_AUSWAHL, /**<Auswahl eines Tests*/
    ST_LED,     /**<Test LEDs*/
    ST_SUMMER,  /**<Test Summer*/
    ST_SEG,     /**<Test 7-Segment-Anzeigen*/
    ST_ADC,     /**<Test Potentiometer*/
    ST_UART,    /**<Test UART*/
    ST_TEMP,    /**<Test Temperatursensor*/
    ST_MENU     /**<Anzeige Demomenu*/
} state_t;

/********************* Funktionsprototypen **************************************/

uint8_t IntroEdit(uint8_t firstCall);           /*Menufunktion, Intro an/aus*/
uint8_t SummerEdit(uint8_t firstCall);          /*Menufunktion, Summerfrequenz*/
uint8_t BspMenuFunction(uint8_t firstCall);     /*Menufunktion, Beispiel*/
inline void ISR_Summer(void);
inline void ISR_UartRx(void);
/********************* Globale Daten ********************************************/
volatile uint8_t uartHasReceived_g = 0;             /**< Flag UART-String empfangen*/
volatile uint8_t uartBuffer_g[UART_BUFFERSIZE];     /**< UART-String*/
volatile uint8_t summerCnt_g = 0;                   /**< Zähler für Summer*/
uint8_t  playIntro_g = 0;                           /**< Flag, speichert ob Intro ausgeführt wird beim Start*/
uint16_t summerFreqHZ_g = 0;                        /**< Eingestellte Summerfrequenz */


events_t events_g;  /**< Event-Struktur für Timing- und Tasterevents*/

/*Beispielmenu*/
/**
 * @brief Array mit Menueinträgen
 */
menuEntry_t myMenu[] =
{   /* Name             ID  next        prev        submenu     return      function*/
    /*Hauptmenu*/
    {"Einstellungen",   0,  &myMenu[1], &myMenu[3], &myMenu[4], NULL,       NULL},              /*Submenu kann aufgerufen werden*/
    {"foo",             1,  &myMenu[2], &myMenu[0], NULL,       &myMenu[0], BspMenuFunction},   /*Funktion kann ausgeführt werden*/
    {"foo2",            2,  &myMenu[3], &myMenu[1], NULL,       &myMenu[0], BspMenuFunction},   /*Funktion kann ausgeführt werden*/
    {"Exit",            3,  &myMenu[0], &myMenu[2], NULL,       NULL,       NULL},              /*Kein Submenu, keine Funktion, kein Return-->Menu wird beendet*/

    /*Untermenu*/
    {"Intro",           0,  &myMenu[5], &myMenu[6], NULL,       &myMenu[4], IntroEdit},         /*Funktion kann ausgeführt werden*/
    {"Summer",          1,  &myMenu[6], &myMenu[4], NULL,       &myMenu[4], SummerEdit},        /*Funktion kann ausgeführt werden*/
    {"Return",          2,  &myMenu[4], &myMenu[5], NULL,       &myMenu[0], NULL}               /*Return-->Hauptmenu wird aufgerufen*/
};


/********************************************************************************/
/********************* Beginn Hauptprogramm *************************************/

/**
 * @brief Mainroutine Hardware-Testsoftware
 */
void main(void)
{
    state_t state = ST_AUSWAHL;                         /*Zustand der Zustandsmaschine*/

    PBA_Init();                                         /*PBA initialisieren*/


    if (EE_ReadByte(EEADDR_CHECKVALID) != EE_VALID)     /*Keine EEPROM-Daten vorhanden*/
    {
        EE_WriteByte(EEADDR_INTRO, EE_DEFAULT_INTRO);   /*Schreiben Defaultwerte*/
        playIntro_g = EE_DEFAULT_INTRO;

        for (uint8_t i = 0; i<sizeof (summerFreqHZ_g); i++)
        {
            EE_WriteByte((EEADDR_SUMMER + i), (EE_DEFAULT_SUMMER >> (8 * i)));
        }
        summerFreqHZ_g = EE_DEFAULT_SUMMER;
        EE_WriteByte(EEADDR_CHECKVALID, EE_VALID);
    }
    else                                                /*EEPROM-Daten vorhanden*/
    {
        playIntro_g = EE_ReadByte(EEADDR_INTRO);        /*Auslesen der Daten*/
        summerFreqHZ_g = 0;
        for (uint8_t i = 0; i<sizeof (summerFreqHZ_g); i++)
        {
            summerFreqHZ_g |= (uint16_t) (EE_ReadByte((EEADDR_SUMMER + i)) << (8 * i));
        }

    }
    INTCONbits.PEIE = 1;                                /*Peripherie-Interrupts akivieren*/
    INTCONbits.GIE = 1;                                 /*Globale Interrupts akivieren*/

    if (playIntro_g)                                    /*Falls Intro aktiviert*/
    {
        Beep(4000, 40);                                 /*Start-Tonausgabe*/
        DelayMS(50);
        Beep(4000, 80);
        printf("\fPIC Board Adv. 6\n  Hardwaretest  "); /*Start-LCD-Ausgabe während 1s*/
        DelayMS(1000);
    }
    if(0 != INT_AddTmr2CallbackFnc(ISR_Summer))         /*Summerinterruptroutine mit Timer2 abarbeiten*/
    {
        /* Fehler */
    }
    if(0 != INT_AddUartCallbackFnc(ISR_UartRx))         /*UART-Interruptroutine festelegen*/
    {
        /* Fehler */
    }
    LOOPDELAY_Init(10);                                 /*Zyklus-Delay init*/
    EVENTS_Init(&state, &events_g);                     /*Events initialisieren*/
    MENU_Init(&myMenu[0], &events_g);                   /*Menu initialisieren*/
    LCD_Clear();
    printf("  Test wählen   \n(Schalter 0..5) ");       /*Eingabeaufforderung*/
    for (; ;)                                           /*Endlosschleife*/
    {
        LOOPDELAY_Wait();                               /*Zykluszeit 10ms abwarten*/
        EVENTS_Update();                                /*Events aktualisieren*/

        /********************* Beginn Zustandsmaschine **********************************/
        switch (state)
        {
            /********************* Auswahl Test *********************************************/
        case ST_AUSWAHL:
            if (events_g.posEdge.switch0)                           /*Schaltertest*/
            {
                printf("\f Test der LEDs\n und Schalter...");
                state = ST_LED;
            }
            if (events_g.posEdge.switch1)                           /*Summertest*/
            {
                printf("\f Test Summer...");
                summerCnt_g = 1000 / summerFreqHZ_g / 2;            /*Startfrequenz=500Hz*/
                state = ST_SUMMER;

            }
            if (events_g.posEdge.switch2)                           /*7-Segment-Anzeige Test*/
            {
                printf("\f Test 7-Segment-\n  Anzeigen...");
                state = ST_SEG;
            }
            if (events_g.posEdge.switch3)                           /*ADC&Poti Test*/
            {
                printf("\fTest ADC-Poti...");
                state = ST_ADC;
            }
            if (events_g.posEdge.switch4)                           /*UART Test*/
            {
                printf("\f  Test UART-\nSchnittstelle");
                UART_WriteLineRom("\f Test UART mit Terminal...\n");
                UART_WriteLineRom("********************************************");
                UART_WriteLineRom("*       PIC Board Advanced 6               *");
                UART_WriteLineRom("*       ICT Berufsbildungscenter AG        *");
                UART_WriteLineRom("*       Hardware-Testsoftware V2.1.0       *");
                UART_WriteLineRom("********************************************");
                UART_PutsRom("\nBitte Text eingeben und mit Enter bestaetigen: ");
                PIE1bits.RCIE = 1;                                   /*Receive Interrupt einschalten*/
                state = ST_UART;
            }
            if (events_g.posEdge.switch5)                            /*Temperatursensor Test*/
            {
                printf("\fTest Temperatur-\n  sensor (I2C)  ");
                state = ST_TEMP;
            }
            if (events_g.posEdge.switch7)                            /*Beispielmenu*/
            {
                printf("\fBeispielmenu\nT0:%c, T1:%c, T2:%c", 2, 1, 0);
                state = ST_MENU;
            }
            break;

            /********************* LED-Test *************************************************/
        case ST_LED:
            LEDS_D = PORTB;                     /*Tasterwerte an LED0..7 ausgeben*/
            if (events_g.TimeoutMS(100))        /*LED8 & LED9 blinken mit 5Hz*/
            {
                LED8 = LED9 ^= 1;
            }
            if (events_g.negEdge.switch0)
            {
                LEDS_D = 0;
                LED8 = LED9 = 0;
                printf("\f  Test wählen   \n(Schalter 0..5) ");     /*Übersicht*/
                state = ST_AUSWAHL;
            }
            break;
            /********************* Summer-Test **********************************************/
        case ST_SUMMER:
            if (events_g.TimeoutMS(200))
            {
                summerCnt_g = 1000 / summerFreqHZ_g;
            }
            if (events_g.TimeoutMS(400))
            {
                summerCnt_g = 1000 / summerFreqHZ_g / 2;
            }
            if (events_g.negEdge.switch1)
            {
                printf("\f  Test wählen   \n(Schalter 0..5) ");     /*Übersicht*/
                summerCnt_g = 0;
                state = ST_AUSWAHL;
            }
            break;
            /********************* 7-Segment-Test *******************************************/
        case ST_SEG:
        {
            static uint8_t SegCnt = 0;
            if (events_g.TimeoutMS(200))         /*Anzeige alle 200ms um 1 erhöhen*/
            {

                LEDS_D = BinToBcd(SegCnt);
                SegCnt += 11;
                if (110 == SegCnt)
                {
                    SegCnt = 0;
                }
            }

            if (events_g.negEdge.switch2)
            {
                SegCnt = 0;
                LEDS_D = 0;
                printf("\f  Test wählen   \n(Schalter 0..5) "); /*Übersicht*/
                state = ST_AUSWAHL;
            }
        }
            break;
            /********************* ADC-Poti-Test ********************************************/
        case ST_ADC:

            if (!events_g.ActiveUntilMS(1000))      /*Nachdem 1s Starttext angezeigt*/
            {
                uint32_t result;                    /*Eingelesenes ADC-Resultat*/
                uint8_t adc_dec;                    /*Wert vor Komma*/
                uint16_t adc_frac;                  /*Wert nach Komma*/
                ADC_SetChannel(0);
                result = ADC_Read();
                LCD_GotoXY(0, 0);
                adc_dec = (uint16_t) (result * 3.30 / 4095); /*Umrechnen des AD-Wertes in Volt (abgerundet)*/
                adc_frac = (result * 330 / 4095)-(adc_dec * 100); /*Berechnung Kommastellen*/
                printf(" Uadc0 = %01u.%02uV  \n", adc_dec, adc_frac);
                ADC_SetChannel(1);
                result = ADC_Read();
                adc_dec = (uint16_t) (result * 3.30 / 4095);
                adc_frac = (result * 330 / 4095)-(adc_dec * 100);
                printf(" Uadc1 = %01u.%02uV\n", adc_dec, adc_frac);
            }
            if (events_g.negEdge.switch3)
            {
                printf("\f  Test wählen   \n(Schalter 0..5) "); /*Übersicht*/
                state = ST_AUSWAHL;
            }
            break;
            /********************* UART-Test ************************************************/
        case ST_UART:
            if (uartHasReceived_g)                               /*Text empfangen*/
            {
                UART_PutsRom("\nfolgender Text wurde empfangen: ");
                UART_WriteLine(uartBuffer_g);
                UART_PutsRom("\nBitte Text eingeben und mit Enter bestaetigen: ");
                uartHasReceived_g = 0;
            }
            if (events_g.negEdge.switch4)
            {
                UART_WriteLineRom("\n");
                UART_WriteLineRom("********************************************");
                UART_WriteLineRom("* UART-Test wurde durch Benutzer beendet.  *");
                UART_WriteLineRom("* Auf Wiedersehen!                         *");
                UART_WriteLineRom("********************************************");
                PIE1bits.RCIE = 0;                              /*Receive Interrupt ausschalten*/
                printf("\f  Test wählen   \n(Schalter 0..5) "); /*Übersicht*/
                state = ST_AUSWAHL;
            }
            break;
            /********************* Temperatursensor-Test ************************************/
        case ST_TEMP:
            if (!events_g.ActiveUntilMS(1000))          /*Nachdem 1s Starttext angezeigt*/
            {
                uint16_t result;                        /*Eingelesener Binärwert*/
                uint8_t dec;                            /*Stellen vor Komma*/
                uint8_t frac;                           /*Stellen nach Komma*/

                if (events_g.ActiveUntilMS(10))         /*1x ausgeführt*/
                {
                    LCD_Clear();
                }
                LCD_GotoXY(0, 0);
                printf("Akt. Temperatur=\n");
                result = LM75_ReadTemp();                       /*Temperatur einlesen*/
                dec = result / 10;                              /*Temp. in °C berechenen*/
                frac = result - (dec * 10) != 0 ? 5 : 0;        /*Nachkommastelle berechnen*/
                printf("    %2.1d.%u", (uint8_t) dec, (uint8_t) frac);
                printf("%cC", 223);
            }
            if (events_g.negEdge.switch5)
            {
                printf("\f  Test wählen   \n(Schalter 0..5) "); /*Übersicht*/
                state = ST_AUSWAHL;
            }
            break;
            /********************* Beispiel-Menu ********************************************/
        case ST_MENU:
            if (!events_g.ActiveUntilMS(2500))          /*Nachdem 2.5s Instruktion angezeigt*/
            {
                if (MENU_Call())                         /*Falls Menu beendet*/
                {
                    printf("\f  Test wählen   \n(Schalter 0..5) "); /*Übersicht*/
                    state = ST_AUSWAHL;
                }
            }
            break;
        }
    }
}
/******************** Ende Hauptprogramm ****************************************/
/********************************************************************************/





/********************************************************************************
 ********************* Unterprogramme *******************************************
 ********************************************************************************/

/**
 * @brief UART Empfangsinterrupt
 * Liest Zeichen ein, bis Carriage return empfangen
 */
inline void  ISR_UartRx(void)
{
    static uint8_t len = 0;                 /*Anzahl empfangene Zeichen*/
    uint8_t c;                              /*Aktuell eingelesenes Zeichen*/
    while (PIR1bits.RCIF)                   /*Zeichen im Buffer*/
    {
        c = UART_Getc();                    /*Zeichen einlesen*/
        if (8 == c)                         /*Wenn ein Backspace empfangen wurde*/
        {
            if (len > 0)                    /*Wenn schon Zeichen empfangen wurden*/
            {
                len--;                      /*Zeichen dekrementieren*/
                UART_Putc(c);               /*Backspace zurücksenden*/
                UART_Putc(' ');             /*gelöschtes Zeichen überschreiben*/
                UART_Putc(c);               /*Backspace zurücksenden*/
            }
        }
        else if (13 == c)                   /*Carriage Return empfangen*/
        {
            uartBuffer_g[len] = 0;          /*Nullterminierung*/
            len = 0;                        /*Zähler zurücksetzen*/
            uartHasReceived_g = 1;          /*Flag setzen, Kompletter Text empfangen*/
        }
        else if ((c >= 32)&&(c <= 126))     /*Wenn druckbares Zeichen empfangen wurde*/
        {
            if (len < (UART_BUFFERSIZE - 1))/*Wenn maximale Stringlänge noch nicht erreicht*/
            {
                uartBuffer_g[len++] = c;    /*Empfangenes Zeichen in String ablegen*/
                UART_Putc(c);               /*Empfangenes Zeichen zurücksenden*/
            }
        }
    }
}

/**
 * @brief Interruptroutine für Summer.
 * Schaltet den Summer-IO mit der gewünschten Frequenz.
 * Alle 1ms aufrufen.
 */
inline void ISR_Summer(void)          /*Interrupt, alle 1ms*/
{
    static uint8_t cnt = 0;           /*Zählervariable*/
    if (0 == summerCnt_g)             /*Summer aus*/
        cnt = 0;                      /*Zähler zurücksetzen*/
    else                              /*Summer ein*/
        if (summerCnt_g == ++cnt)     /*Gewünschte Zeit vorbei*/
    {
        SUMMER ^= 1;                  /*Summer-Ausgang invertieren*/
        cnt = 0;                      /*Zähler zurücksetzen*/
    }

}

/**
 * @brief Funktion zum ein-/ausschalten des Intros
 * @param firstCall gibt an, ob die Funktion zum ersten Mal ausgeführt wird
 * @return 1:Funktion beenden, 0:Funktion erneut ausführen
 */
uint8_t IntroEdit(uint8_t firstCall)
{
    if (firstCall)                      /*Falls die Funktion zum ersten Mal aufgerufen wird*/
    {
        LCD_Clear();                    /*Anzeige löschen*/
        printf("Intro ausführen:\n");   /*Ausgabe Text*/
        if (playIntro_g)                /*Ausgabe ob Intro ein- oder ausgeschaltet*/
        {
            printf("Ja  ");
        }
        else
        {
            printf("Nein");
        }
        LCD_GotoXY(0, 1);                       /*Zweite Zeile auswählen*/
    }

    if (events_g.posEdge.switch1)               /*Wenn Taster 1 Mal gedrückt*/
    {
        playIntro_g ^= 1;
        if (playIntro_g)
        {
            printf("Ja  ");
        }
        else
        {
            printf("Nein");
        }
        LCD_GotoXY(0, 1);

    }
    if (events_g.posEdge.switch2)               /*Wenn Taster 1 Mal gedrückt*/
    {
        playIntro_g ^= 1;
        if (playIntro_g)
        {
            printf("Ja  ");
        }
        else
        {
            printf("Nein");
        }
        LCD_GotoXY(0, 1);
    }
    if (events_g.posEdge.switch0)               /*Falls aktuelle Anzahl bestätigt*/
    {
        EE_WriteByte(EEADDR_INTRO, playIntro_g);
        return 1;                               /*Funktion muss nicht mehr ausgeführt werden->Rückgabewert 1*/
    }

    else                                        /*Falls akuelle Anzahl nicht bestätigt*/
        return 0;                               /*Funktion muss beim nächsten Durchlauf wieder ausgeführt werden->Rückgabewert 0*/
}

/**
 * Funktion zum Einstellen der Summerfrequenz für den Hardwaretest
 * @param firstCall gibt an, ob die Funktion zum ersten Mal ausgeführt wird
 * @return 1:Funktion beenden, 0:erneut ausführen
 */
uint8_t SummerEdit(uint8_t firstCall)
{
    static uint8_t count_d = 0, count_u = 0;            /*Zählervariablen*/
    uint8_t i;

    if (firstCall)                                      /*Falls die Funktion zum ersten Mal aufgerufen wird*/
    {
        LCD_Clear();                                    /*Ausgabe löschen*/
        printf("Summer_Freq:\n%u", summerFreqHZ_g);     /*Eingestellte Frequenz ausgeben*/
    }
    if (events_g.pressed1000ms.switch1)                 /*Falls down Taster 1 Sekunde gedrückt ist Zähler erhöhen*/
        count_d++;
    else                                                /*Wenn Taster losegelassen Zähler nullen*/
        count_d = 0;
    if (events_g.pressed1000ms.switch2)                 /*Falls up Taster 1 Sekunde gedrückt ist Zähler erhöhen*/
        count_u++;
    else                                                /*Wenn Taster losegelassen Zähler nullen*/
        count_u = 0;

    if (events_g.posEdge.switch1 || (20 == count_d))    /*Wenn down Taster 1 Mal gedrückt, oder falls dauernd gedrückt (alle 200ms)*/
    {
        count_d = 0;                                 /*Zähler nullen*/
        if (FREQ_MIN == summerFreqHZ_g)              /*Minimale Frequenz bereits eingestellt*/
        {
            summerFreqHZ_g = FREQ_MAX;               /*Maximum einstellen*/
        }
        else
        {
            summerFreqHZ_g -= FREQ_STEP;             /*Frequenz um Schrittgrösse verkleinern*/
        }
        LCD_GotoXY(0, 1);                            /*Zweite Zeile anwählen*/
        printf("%u    ", summerFreqHZ_g);            /*Ausgabe neue Frequenz*/

    }
    if (events_g.posEdge.switch2 || (20 == count_u))    /*Wenn Taster up 1 Mal gedrückt, oder falls dauernd gedrückt (alle 200ms)*/
    {
        count_u = 0;                                    /*Zähler nullen*/
        if (FREQ_MAX == summerFreqHZ_g)                 /*Maximale Frequenz bereits erreicht*/
        {
            summerFreqHZ_g = FREQ_MIN;                  /*Minimale Frequenz einstellen*/
        }
        else
        {
            summerFreqHZ_g += FREQ_STEP;                /*Frequenz um Schrittweite erhöhen*/
        }
        LCD_GotoXY(0, 1);                               /*Zweite Zeile anwählen*/
        printf("%u    ", summerFreqHZ_g);               /*Ausgabe neue Frequenz*/
    }
    if (events_g.posEdge.switch0)                       /*Falls aktuelle Anzahl bestätigt*/
    {
        for (i = 0; i<sizeof (summerFreqHZ_g); i++)     /*Einstellungen in EEPROM speichern*/
        {
            EE_WriteByte((EEADDR_SUMMER + i), (summerFreqHZ_g >> (8 * i)));
        }
        return 1;                               /*Funktion muss nicht mehr ausgeführt werden->Rückgabewert 1*/
    }

    else                                        /*Falls akuelle Anzahl nicht bestätigt*/
        return 0;                               /*Funktion muss weiter ausgeführt werden-->Rückgabewert 0*/
}

/**
 * @brief Beispiel einer Funktion, welche durch das Menu aufgerufen werden kann
 * @param firstCall gibt an, ob die Funktion zum ersten Mal ausgeführt wird
 * @return 1:Funktion beenden, 0:erneut ausführen
 */
uint8_t BspMenuFunction(uint8_t firstCall)      /*Menufunktion muss 1x 8-Bit Rückgabewert und 1x 8-Bit Übergabewert haben*/
{
    if (firstCall)                              /*Der Übergabeparameter wird beim 1. Aufruf der Funktion gesetzt*/
    {                                           /*Ermöglicht Initialisierung*/
        LCD_Clear();
        printf("Beispiel-\nFunktion");
    }
    if (events_g.posEdge.switch0)
    {
        return 1;                               /*Rückgabeparameter 1: Die Funktion ist beendet und wird nicht mehr aufgerufen*/
    }                                           /*Menu wird wieder angezeigt*/
    else
    {
        return 0;                               /*Rückgabeparameter 0: Die Funktion wird beim nächsten Menuaufruf wieder ausgeführt*/
    }
}

/**
 *@}
 */
