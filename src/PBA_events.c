/**
 * @defgroup PBA_EVENTS
 * @{
 *******************************************************************************
 * @file			PBA_events.c
 * @brief			Events-Library für PBA4/5/6
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

#ifdef USE_EVENTS_LIBRARY
	#define MAXTIMEOUTS	10
	
/**
 *@brief Speichern von Events
 */
static struct
{	
uint8_t active[MAXTIMEOUTS];
	uint8_t eventOccured[MAXTIMEOUTS];
	uint16_t ms[MAXTIMEOUTS];
	uint16_t end[MAXTIMEOUTS];
} timeoutValues = {0};

static events_t *p_toEvents;
static uint8_t *p_toState;
static volatile uint16_t timeoutCounter=0;
static uint8_t EVENTS_TimeoutHandler(uint16_t t);
static uint8_t EVENTS_ActiveUntilHandler(uint16_t t);

static  uint16_t loopDelayMS=0;	/**< Eingestelltes Loopdelay */


/**
 * @brief Events initialisieren.
 * @param p_state Pointer auf Zustands-Enumeration
 * @param p_events Pointer auf Event-Struktur
 */
void EVENTS_Init(void *p_state,events_t *p_events)
{
	loopDelayMS=LOOPDELAY_GetTime();
	p_toState=(uint8_t *)p_state;
    p_toEvents=p_events;
	p_toEvents->TimeoutMS=EVENTS_TimeoutHandler;
	p_toEvents->ActiveUntilMS=EVENTS_ActiveUntilHandler;
}

/**
 * @brief Einlesen der Taster, Flanken Tastendrücke detektieren.
 */
void EVENTS_Update(void)
{
	static uint8_t switchFlags = 0;
	static uint8_t switchCount[8]={0};
	uint16_t oneSec=1000/loopDelayMS;
	uint16_t o_5Sec=500/loopDelayMS;
	uint8_t i=0;
	uint8_t switchValue;
	switchValue = PORTB;
	p_toEvents->valueEdge = ((~switchValue&switchFlags)<<8)|(switchValue&~switchFlags);
	switchFlags = switchValue;
	for(i=0;i<8;i++)
	{
		if(BIT_TEST(switchValue,i))
		{
			if(switchCount[i]<255)
			{
				switchCount[i]++;
				if(switchCount[i]>=oneSec)	/*1 Sekunde Taster gedrückt*/
				{
					p_toEvents->valuePressed|=((uint16_t)1<<(8+i));//Bit setzen
				}

				else
				{
					if(switchCount[i]>=o_5Sec)	/*0.5 Sekunden Taster gedrückt*/
					{
						p_toEvents->valuePressed|=((uint16_t)1<<i); /*Bit setzen*/
					}
				}
			}
		}
		else	/*Falls Taster nicht gedrückt ist*/
		{
			switchCount[i]=0;	/*Entsprechenden Zähler zurücksetzen*/
			p_toEvents->valuePressed&=~(((uint16_t)1<<i)|((uint16_t)1<<(8+i))); /*Beide Bits löschen*/
		}
			
		
	}
}

/**
 * @brief Aktualisieren der zeitlichen Events.
 */
inline void EVENTS_TimerISR(void)
{
	static uint8_t stateOld=0;
    uint8_t i;
	if(stateOld!=*p_toState)							/*Bei Zustandswechsel alle Timeouts zurücksetzen*/
		for(i=0;i<MAXTIMEOUTS;i++)
		{
			timeoutValues.eventOccured[i]=0;
			timeoutValues.active[i]=0;
			timeoutValues.ms[i]=0;
		}
	else
		for(i=0;i<MAXTIMEOUTS;i++)					/*Timoutereignisse prüfen*/
			if((timeoutValues.active[i]==1) && (timeoutCounter==timeoutValues.end[i]))
				timeoutValues.eventOccured[i]=1;
	stateOld=*p_toState;
	++timeoutCounter;
}

/**
 * @brief Einrichten/Überprüfen von Timeout-Events.
 * @param timeoutMS Gewünschtes Timeout in ms
 * @return  1: Timeout eingetreten
 */
static uint8_t EVENTS_TimeoutHandler(uint16_t timeoutMS)
{
	if(timeoutMS<=loopDelayMS)                            /*Wenn gewünschtes timeout kleiner oder gleich eingestelltem Loopdelay*/
    {
        return 1;                                   /*Rückgabe: Timeoutereignis eingetreten->timeout=loop_delay*/
    }
	else
	{
		uint8_t i;
		/*Prüfen ob Timeout schon vorhanden*/
		for(i=0; i<MAXTIMEOUTS && timeoutValues.ms[i]!=timeoutMS; i++);
		if(i==MAXTIMEOUTS)								/*Timout noch nicht vorhanden*/
		{
			/*Prüfen ob noch Timeouts vorhanden sind*/
			for(i=0; i<MAXTIMEOUTS && timeoutValues.active[i]>0; i++);
			if(i==MAXTIMEOUTS) return 0xFF;				/*Rückgabe: Fehler keine Timeouts mehr verfügbar*/
			/*neues Timeout speichern*/
			INTCONbits.GIE=0;							/*Interrupts ausschalten (counter absichern)*/
			timeoutValues.active[i] = 1;
			timeoutValues.eventOccured[i]=0;
			timeoutValues.ms[i] = timeoutMS;
			timeoutValues.end[i] = timeoutCounter+timeoutMS-2*loopDelayMS;
			INTCONbits.GIE=1;							/*Interrupts wieder einschalten*/
			return 0;									/*Rückgabe: Kein Timeoutereignis eingetreten*/
		}
		else											/*Timeout bereits vorhanden*/
		{
			if(timeoutValues.active[i] && timeoutValues.eventOccured[i])			/*Timeoutereignis eingetreten*/
			{
				/*Timeout zurücksetzen*/
				timeoutValues.eventOccured[i]=0;
				timeoutValues.active[i]=0;
				timeoutValues.ms[i]=0;
				return 1;								/*Rückgabe: Timeoutereignis eingetreten*/
			}
		}
		return 0;										/*Rückgabe: Kein Timeoutereignis eingetreten*/
	}
}

/**
 * @brief Einrichten und überprüfen von Active-Until-Events.
 * @param activeUntilMS Gewünschte Aktivzeit in ms
 * @return	1: Event eingetreten
 */
static uint8_t EVENTS_ActiveUntilHandler(uint16_t activeUntilMS)
{
	uint8_t i;
	/*Prüfen ob Timeout schon vorhanden*/
	for(i=0; i<MAXTIMEOUTS && timeoutValues.ms[i]!=activeUntilMS; i++);
	if(i==MAXTIMEOUTS)								/*Timout noch nicht vorhanden*/
	{
		/*Prüfen ob noch Timeouts vorhanden sind*/
		for(i=0; i<MAXTIMEOUTS && timeoutValues.active[i]>0; i++);
		if(i==MAXTIMEOUTS) return 0xFF;				/*Rückgabe: Fehler keine Timeouts mehr verfügbar*/
		/*neues Timeout speichern*/
		INTCONbits.GIE=0;							/*Interrupts ausschalten (counter absichern)*/
		timeoutValues.active[i] = 1;
		timeoutValues.eventOccured[i]=0;
		timeoutValues.ms[i] = activeUntilMS;
		timeoutValues.end[i] = timeoutCounter+activeUntilMS-loopDelayMS;
		INTCONbits.GIE=1;							/*Interrupts wieder einschalten*/
		return 1;									/*Rückgabe: Kein Timeoutereignis eingetreten*/
	}
	else											/*Timeout bereits vorhanden*/
	{
		if(timeoutValues.active[i]==2)				/*Ontime-Timemout bereits 1x abgelaufen*/
			return 0;		
		if(timeoutValues.active[i] && timeoutValues.eventOccured[i])			/*Timeoutereignis eingetreten*/
		{
			/*Timeout zurücksetzen*/
			timeoutValues.eventOccured[i]=0;
			timeoutValues.active[i]=2;
			return 0;								/*Rückgabe: Timeoutereignis eingetreten*/
		}
	}
	return 1;										/*Rückgabe: Kein Timeoutereignis eingetreten*/
}

#endif
/**
 * @}
 */