PBA BSP Library
===================

##Lizenzierung

Diese Software kann unter den Bestimmungen der MIT-Lizenz verwendet werden.

Copyright � 2016 ICT Berufsbildungscenter AG

###The MIT Licence (MIT)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the �Software�), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.


##Einbinden des BSP in ein MPLABX Projekt
1. Neues MPLABX Projekt erstellen
2. Die beiden Ordner "src" und "inc" in den Projektordner kopieren
3. Im Projekt Rechtsklick auf Source Files, danach "Add existing Item"
4. Im Browse-Dialog in den Source Ordner navigieren, alle C-Files anw�hlen und diese dem Projekt hinzuzuf�gen
   **WICHTIG!! Die Auswahl "Store path as:" muss auf "Relative" eingestellt sein**
5. Schritte 3&4 wiederholen f�r die Include Files
6. Neues C-File erstellen und im Ordner "src" abspeichern
7. Zur Verwendung der Library muss folgende Code-Zeile im neu erstellten C-File hinzugef�gt werden:

  ```C
  #include "../inc/PBA_config.h"
  ```
  
##BSP-Konfiguration
Zur Konfiguration muss die Header-Datei ```PBA_config.h ``` editiert werden.
Durch ein- oder auskommentieren der defines im Abschnitt "BSP-Konfiguration" l�sst sich
das BSP auf die Bed�rfnisse anpassen.
  ```C
/******************** BSP-Konfiguration ***********************************************/

#define PBA6/**<Auskommentieren, falls nicht PBA6 verwendet wird*/

#define	USE_ADC_LIBRARY         /**< ADC-Support Funktionen*/

#define	USE_LCD_LIBRARY         /**< LCD DOGM162 Funktionen*/

#define	USE_UART_LIBRARY        /**< UART Funktionen*/

#define	USE_I2C_LIBRARY         /**< I2C Funktionen*/

#define	USE_LM75_LIBRARY        /**< Ansteuerung I2C-Temperatursensor*/

#define USE_EEPROM_LIBRARY      /**< Verwendung internes EEPROM*/

#define USE_HELPERS_LIBRARY     /**< N�tzliche Makros und Funktionen*/

#define USE_INTERRUPT_LIBRARY	/**< Globaler Interrupt-Handler*/

#define USE_LOOPDELAY_LIBRARY   /**< Timerbasiertes Delay*/

#define USE_EVENTS_LIBRARY      /**< Zeit- und Taster-Events*/

#define USE_MENU_LIBRARY        /**< Funktionen Erstellung und Darstellung eines Menus auf LCD*/

/*Auswahl Standard-Output f�r printf-Funktion*/
#define STDOUT_LCD	    true    /**< Ausgabe printf auf LCD*/
//#define STDOUT_UART	true    /**< Ausgabe printf auf UART-Schnittstelle*/
 ```
##Hardware initialisieren
Zum Initialisieren der Hardware muss die Funktion ```PBA_Init();``` aufgerufen werden.

```C
#include "../inc/PBA_config.h"
void main(void)
{
    PBA_Init();		/* Initialisieren der Hardware */
    while(1)
    {
					/*My Code*/
    }
}
  ```
##Verwendung der loopdelay-Library
**Wichtig: Um die loopdelay-Library zu verwenden, muss die Interrupt-Library aktiviert sein!**
Nach der Initialisierung der Hardware muss das loopdelay initialisiert werden.
Daf�r wird die Funktion ```void LOOPDELAY_Init(uint16_t loopDelayTimeMS);``` aufgerufen,
wobei der �bergabeparameter dem gew�nschten loopdelay in Millisekunden (ms) entspricht.
Ausgef�hrt wird das delay durch die Funktion ```void LOOPDELAY_Wait(void);```.
Der Benutzer ist selber verantwortlich, dass der gesamte Programmdurchlauf nicht l�nger als
die angegebene Zykluszeit dauert.

```C
#include "../inc/PBA_config.h"
void main(void)
{
    PBA_Init();				/*Initialisieren der Hardware*/
    LOOPDELAY_Init(10);		/*Loop-delay Init, loop-time=10ms*/
    while(1)
    {
        LOOPDELAY_Wait();	/*Zykluszeit 10ms*/
        LED0^=1;			/*LED0 blinkt mit einer Frequenz von 50Hz*/
    }
}
  ```
  
##Verwendung der Event-Library
**Wichtig: Wird die Event-Library verwendet, muss die loopdelay-Library gleichzeitig aktiviert sein!**
Die Event-Library bietet die M�glichkeit mit verschiedenen Events zu arbeiten.
Folgende Events sind definiert:

Eventname				| Funktion
----------------------	| ---------------------------------------
posEdge					| Abfragen einer positiven Tasterflanke
negEdge					| Abfragen einer negativen Tasterflanke
ActiveUntilMS(time) 	| Nach dem ersten Aufruf nur w�hrend der angegebenen Zeit aktiv
TimeoutMS(time) 		| Nach dem ersten Aufruf immer nach der angegebenen Zeit aktiv

Zur Initialisierung der Bibliothek wird die Funktion ```void EVENTS_Init(void *p_state,events_t *p_events);``` aufgerufen. Der �bergabeparameter ```void *p_state``` ist der Pointer auf die in einer Aufz�hlung (enum) definierten states.
Der zweite Parameter ist der Pointer auf eine Struktur vom Type events_t. �ber diese Struktur k�nnen Events erstellt und abgefragt werden.
Das loop-delay wird �ber die bekannte Funktion ```void LOOPDELAY_Wait(void);```aufgerufen.
Zum detektieren der Tasterflanken muss die Funktion ```EVENTS_Update(void);``` zyklisch ausgef�hrt werden.

```C
#include "../inc/PBA_config.h"


#define AUS			0
#define EIN			1

#define LEDANZEIGE		LATD

enum {ST_TIMEOUT,ACTIVE_UNTIL, ST_POSEDGE_NEGEDGE}state=ST_TIMEOUT;	/*Aufz�hlung aller Zust�nde*/

events_t events;					/*Event-Struktur*/

void main(void)
{

	PBA_Init();						/*Initialisierung Hardware*/
	LOOPDELAY_Init(10);				/*Initialsierung loop-delay*/
	EVENTS_Init(&state,&events); 	/*Initialisierung Events*/

	LED0=EIN;
	while(1)
	{
		LOOPDELAY_Wait();			/*Zykluszeit 10ms*/
		EVENTS_Update();			/*Events aktualisieren*/
		switch(state)
		{
			case ST_TIMEOUT:
				if(events.TimeoutMS(3000))	/*LED0 leuchtet w�hrend 3 Sekunden*/
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
						LED0^=1;			/*LED0 blinkt mit 5Hz f�r 2s*/
					}
				}
				else
				{
					LED0=1;
					state=ST_POSEDGE_NEGEDGE;
				}
				break;
			case ST_POSEDGE_NEGEDGE:
				if(events.posEdge.switch0)	/*Bei jeder Tasterbet�tigung wird die */
				{							/*Ledanzeige um 1 inkrementiert*/
					LEDANZEIGE++;
				}

				if(events.negEdge.switch7)	/*Beim loslassen des Tasters */
				{
					LEDANZEIGE=1;			/*zur�cksetzen der Anzeige*/
					state=ST_TIMEOUT;		/*R�ckkehr zum 1. Zustand*/
				}
				break;
		}
	}
}
```

**Um genaue Werte bei Timeouts zu erreichen, d�rfen nur Werte die das Ein- oder Mehrfache der Zykluszeit betragen verwendet werden!**

