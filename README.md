# PBA6-BSP
## Download des PBA6 BSP
Der Sourcecode des PBA6-BSP wird auf Github verwaltet: <a href="https://github.com/ICT-BBC/PBA6-BSP" target="_blank"> https://github.com/ICT-BBC/PBA6-BSP </a>

1. Öffne die <a href="https://github.com/ICT-BBC/PBA6-BSP" target="_blank"> Github-Seite des PBA6-BSP</a>
2. Klicke auf den Button "Clone or Download" auf der rechten Seite, über der Ordnerübersicht
3. Lade das Projekt als Zip-Datei herunter
4. Entpacke die Zip-Datei


## Inhalt BSP
Der entpackte Ordner enthält nun folgende Unterordner und Dateien:

Ordner / Datei  | Inhalt
--------------- | ---------------------------------------------------------------------------------------------------------------------
docs            | Ordner mit der HTML-Dokumentation des Sourcecodes
doxygen         | Ordner mit allen benötigten Dateien für Doxygen --> kann gelöscht werden
example         | MPLABX-Projekt Hardwaretestsoftware
inc             | Include-Files des BSP
src             | Source-Files des BSP
.gitignore      | Ignorierte Files der Versionsverwaltung --> kann gelöscht werden
LICENSE.md      | Lizenz --> Bitte durchlesen
README.md       | Rohdaten der Anleitung --> kann gelöscht werden


## BSP-Module
Modul           | Zweck
--------------- | ---------------------------------------------------------------------------------------------------------------------
ADC             | Funktionen zur Verwendung des PIC-internen Analog-Digital-Konverters.
LCD             | Ansteuerung des DOGM162-LCDs. Mit kleinen Anpassungen auch für andere Controller und Displays verwendbar.
UART            | Funktionen fürs Lesen und Schreiben über die UART-Schnittstelle,
I2C             | Lese- und Schreibfunktionen für die interne I2C-Schnittstelle.
LM75            | Ansteuerung des Temperatursensors TCN75, welcher protokollkompatibel zum Modell LM75 ist.
EEPROM          | Verwendung des PIC-internen EEPROMS.
HELPERS         | Hilfs-Makros und Funktionen wie Bit setzen / löschen, Bit toggle.
INTERRUPT       | Globale Interruptroutine. Möglichkeit zum Hinzufügen von Callbackfunktionen für Timer-, UART- und externe Interrupts.
LOOPDELAY       | Timer-basiertes Delay. Verwendung in Zustandsmaschinen.
EVENTS          | Funktionen zur Verwendung von zeitlichen- und Tasterevents. Anwendung in Zustandsmaschinen.
MENU            | Funktionen zur Darstellung eines Menüs auf zeichenbasierten LCDs. Verwendbar in Zustandsmaschinen.


## Einbinden des BSP in ein MPLABX Projekt
1. Neues MPLABX Projekt erstellen
2. Die beiden Ordner "src" und "inc" in den Projektordner kopieren
3. Im Projekt Rechtsklick auf Source Files, danach "Add existing Item"
4. Im Browse-Dialog in den Source Ordner navigieren, alle C-Files anwählen und diese dem Projekt hinzuzufügen
   **WICHTIG!! Die Auswahl "Store path as:" muss auf "Relative" eingestellt sein**
5. Schritte 3&4 wiederholen für die Include Files
6. Neues C-File erstellen und im Ordner "src" abspeichern
7. Zur Verwendung der Library muss folgende Code-Zeile im neu erstellten C-File hinzugefügt werden:

  ```C
  #include "../inc/PBA_config.h"
  ```
  
  
## BSP-Konfiguration
Zur Konfiguration muss die Header-Datei ```PBA_config.h ``` editiert werden.
Durch ein- oder auskommentieren der defines im Abschnitt "BSP-Konfiguration" lässt sich
das BSP auf die Bedürfnisse anpassen.
  ```C
/******************** BSP-Konfiguration ***********************************************/

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
 ```
 
### Modul-Abhängigkeiten
Einige Module verwenden Funktionen anderer Module und setzen deshalb voraus, dass diese ebenfalls aktiviert sind.
Ist dies nicht der Fall, werden passende Fehlermeldungen ausgegeben. Die nachfolgende Tabelle gibt einen Überblick über die Abhängigkeiten.
 
|     Module     |  LCD  |  I2C  | HELPERS | INTERRUPT | LOOPDELAY | EVENTS |
| -------------- | :---: | :---: | :-----: | :-------: | :-------: | :----: | 
| LCD            |       |       |    x    |           |           |        |
| LM75           |       |   x   |         |           |           |        |  
| LOOPDELAY      |       |       |         |     x     |           |        |
| EVENTS         |       |       |    x    |     x     |     x     |        |  
| MENU           |   x   |       |    x    |     x     |     x     |   x    |

### STDOUT
Printf-Ausgaben können entweder ans LCD oder über UART gesendet werden. Mittels Compiler-Schalter
kann zwischen den beiden Varianten umgestellt werden. Zu beachten ist, dass das jeweilige BSP-Modul
aktiviert sein muss. Sind weder UART- noch LCD-Modul aktiviert, müssen beide Compiler-Schalter deaktiviert werden.
  ```C
/********** Auswahl STDOUT ****************************************************/
#define STDOUT_LCD      true    /**< Ausgabe printf auf LCD*/
//#define STDOUT_UART   true    /**< Ausgabe printf auf UART-Schnittstelle*/
 ```
 
 
## Interrupts
Der Mikrocontroller verfügt nur über einen Interrupt-Vektor. Die globale Interrupt-Routine
ist in der Datei ```PBA_Interrupts.c ``` umgesetzt. Für die wichtigsten Peripherie-
Interrupts können Callback-Funktionen hinzugefügt werden. Folgende Funktionen stehen dazu zur Verfügung:
  ```C
uint8_t INT_AddTmr0CallbackFnc   (callbackFunction_t p_callBackFunction);
uint8_t INT_AddTmr1CallbackFnc   (callbackFunction_t p_callBackFunction);
uint8_t INT_AddTmr2CallbackFnc   (callbackFunction_t p_callBackFunction);
uint8_t INT_AddUartCallbackFnc   (callbackFunction_t p_callBackFunction);
uint8_t INT_AddExtIntCallbackFnc (callbackFunction_t p_callBackFunction);
```
Pro Timer können jeweils drei Funktionen hinterlegt werden. Die restlichen Interrupts akzeptieren nur eine
Callbackfunktion. Über den Rückgabeparameter kann überprüft werden, ob die Funktion erfolgreich hinterlegt wurde.

Rückgabewert    | Bedeutung
--------------- | -----------------------------------------------------------
0               | Erfolg
1               | Fehler, die maximale Anzahl Funktionen ist bereits erreicht


## Hardware initialisieren
Zum Initialisieren der Hardware muss die Funktion ```PBA_Init();``` aufgerufen werden.

```C
#include "../inc/PBA_config.h"
void main(void)
{
    PBA_Init();     /* Initialisieren der Hardware */
    while(1)
    {
                    /*My Code*/
    }
}
  ```
  
  
## Verwendung der Loopdelay-Library
Nach der Initialisierung der Hardware muss das Loopdelay initialisiert werden.
Dafür wird die Funktion ```void LOOPDELAY_Init(uint16_t loopDelayTimeMS);``` aufgerufen,
wobei der Übergabeparameter dem gewünschten Loopdelay in Millisekunden (ms) entspricht.
Ausgeführt wird das Delay durch die Funktion ```void LOOPDELAY_Wait(void);```.
Der Benutzer ist selber verantwortlich, dass der gesamte Programmdurchlauf nicht länger als
die angegebene Zykluszeit dauert.

```C
#include "../inc/PBA_config.h"
void main(void)
{
    PBA_Init();             /*Initialisieren der Hardware*/
    LOOPDELAY_Init(10);     /*Loopdelay Init, loop-time=10ms*/
    while(1)
    {
        LOOPDELAY_Wait();   /*Zykluszeit 10ms*/
        LED0^=1;            /*LED0 blinkt mit einer Frequenz von 50Hz*/
    }
}
  ```
 
 
## Verwendung der Event-Library
Die Event-Library bietet die Möglichkeit mit verschiedenen Events zu arbeiten.
Folgende Events sind definiert:

Eventname               | Funktion
----------------------  | -------------------------------------------------------------
posEdge                 | Abfragen einer positiven Tasterflanke
negEdge                 | Abfragen einer negativen Tasterflanke
ActiveUntilMS(time)     | Nach dem ersten Aufruf nur während der angegebenen Zeit aktiv
TimeoutMS(time)         | Nach dem ersten Aufruf immer nach der angegebenen Zeit aktiv

Zur Initialisierung der Bibliothek wird die Funktion ```void EVENTS_Init(void *p_state,events_t *p_events);``` aufgerufen. Der Übergabeparameter ```void *p_state``` ist der Pointer auf die in einer Aufzählung (enum) definierten states.
Der zweite Parameter ist der Pointer auf eine Struktur vom Type events_t. Über diese Struktur können Events erstellt und abgefragt werden.
Das Loopdelay wird über die bekannte Funktion ```void LOOPDELAY_Wait(void);```aufgerufen.
Zum Detektieren der Tasterflanken muss die Funktion ```EVENTS_Update(void);``` zyklisch ausgeführt werden.  <br>
**Für zeitliche Events dürfen nur Werte die das Ein- oder Mehrfache der Zykluszeit betragen verwendet werden!**

```C
#include "../inc/PBA_config.h"

#define AUS         0
#define EIN         1

#define LEDANZEIGE      LATD

enum {ST_TIMEOUT,ACTIVE_UNTIL, ST_POSEDGE_NEGEDGE}state=ST_TIMEOUT; /*Aufzählung aller Zustände*/

events_t events;                    /*Event-Struktur*/

void main(void)
{

    PBA_Init();                     /*Initialisierung Hardware*/
    LOOPDELAY_Init(10);             /*Initialisierung Loopdelay*/
    EVENTS_Init(&state,&events);    /*Initialisierung Events*/

    LED0=EIN;
    while(1)
    {
        LOOPDELAY_Wait();           /*Zykluszeit 10ms*/
        EVENTS_Update();            /*Events aktualisieren*/
        switch(state)
        {
            case ST_TIMEOUT:
                if(events.TimeoutMS(3000))  /*LED0 leuchtet während 3 Sekunden*/
                {
                    LED0=0;
                    state=ST_ACTIVE_UNTIL;
                }
                break;
            case ST_ACTIVE_UNTIL:
                if(events.ActiveUntilMS(2000))
                {
                    if(events.TimeoutMS(100))
                    {
                        LED0^=1;            /*LED0 blinkt mit 5Hz für 2s*/
                    }
                }
                else
                {
                    LED0=1;
                    state=ST_POSEDGE_NEGEDGE;
                }
                break;
            case ST_POSEDGE_NEGEDGE:
                if(events.posEdge.switch0)  /*Bei jeder Tasterbetätigung wird die */
                {                           /*Ledanzeige um 1 inkrementiert*/
                    LEDANZEIGE++;
                }

                if(events.negEdge.switch7)  /*Beim loslassen des Tasters */
                {
                    LEDANZEIGE=1;           /*zurücksetzen der Anzeige*/
                    state=ST_TIMEOUT;       /*Rückkehr zum 1. Zustand*/
                }
                break;
        }
    }
}
```
## COM-Port Treiber
Je nach Betriebssystem und Konfiguration wird der Treiber für den USB-Seriell-Wandler FT232R nicht automatisch installiert.
Die Virtual Com Port (VCP) Treiber können im <a href="http://www.ftdichip.com/Drivers/VCP.htm" target="_blank">Downloadbereich</a> von FTDI Chip heruntergeladen werden. <br>
Für ungeübte Benutzer stellt der Hersteller <a href="http://www.ftdichip.com/Support/Documents/InstallGuides.htm" target="_blank">Installationsanleitungen</a> für alle gängigen Betriebssysteme zur Verfügung.