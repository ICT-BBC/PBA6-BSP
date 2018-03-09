/**
 * @defgroup PBA_EVENTS
 * @{
 *******************************************************************************
 * @file            PBA_events.c
 * @brief           Events-Library für das PBA6
 * @author          ICT Berufsbildungscenter AG
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

/**
 *@brief Maximale Anzahl Timeouts welche gleichzeitig aktiv sein können
 */
#define MAXTIMEOUTS 10

/**
 *@brief Speichern von Events
 */
typedef struct
{
    uint8_t active;
    uint8_t eventOccured;
    uint16_t ms;
    uint16_t end;
} timeevent_t;
static timeevent_t timeEvents[MAXTIMEOUTS] = {0};

static events_t *p_toEvents;
static uint8_t *p_toState;
static volatile uint16_t timeoutCounter=0;
static uint8_t EVENTS_TimeoutHandler(uint16_t t);
static uint8_t EVENTS_ActiveUntilHandler(uint16_t t);
static uint8_t EVENTS_ResetTimeoutHandler(uint16_t t);
static  uint16_t loopDelayMS=0; /**< Eingestelltes Loopdelay */


/**
 * @brief Events initialisieren.
 * @param p_state Pointer auf Zustands-Enumeration
 * @param p_events Pointer auf Event-Struktur
 */
void EVENTS_Init(void *p_state,events_t *p_events)
{
    #if defined(LOOPDELAY_USE_TIMER0)
    if(0 != INT_AddTmr0CallbackFnc(EVENTS_TimerISR))
    {
        /* Fehler */
    }
    #endif

    #if defined(LOOPDELAY_USE_TIMER1)
    if(0 != INT_AddTmr1CallbackFnc(EVENTS_TimerISR))
    {
        /* Fehler */
    }
    #endif

    #if defined(LOOPDELAY_USE_TIMER2)
    if(0 != INT_AddTmr2CallbackFnc(EVENTS_TimerISR))
    {
        /* Fehler */
    }
    #endif

    loopDelayMS=LOOPDELAY_GetTime();
    p_toState=(uint8_t *)p_state;
    p_toEvents=p_events;
    p_toEvents->TimeoutMS=EVENTS_TimeoutHandler;
    p_toEvents->ActiveUntilMS=EVENTS_ActiveUntilHandler;
    p_toEvents->ResetTimeoutMS=EVENTS_ResetTimeoutHandler;
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
                if(switchCount[i]>=oneSec)      /*1 Sekunde Taster gedrückt*/
                {
                    p_toEvents->valuePressed|=((uint16_t)1<<(8+i));//Bit setzen
                }

                else
                {
                    if(switchCount[i]>=o_5Sec)  /*0.5 Sekunden Taster gedrückt*/
                    {
                        p_toEvents->valuePressed|=((uint16_t)1<<i); /*Bit setzen*/
                    }
                }
            }
        }
        else    /*Falls Taster nicht gedrückt ist*/
        {
            switchCount[i]=0;   /*Entsprechenden Zähler zurücksetzen*/
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
    if(stateOld!=*p_toState)                    /*Bei Zustandswechsel alle Timeouts zurücksetzen*/
        memset(timeEvents,0,sizeof(timeEvents));
    else
        for(i=0;i<MAXTIMEOUTS;i++)              /*Timoutereignisse prüfen*/
            if((timeEvents[i].active==1) && (timeoutCounter==timeEvents[i].end))
                timeEvents[i].eventOccured=1;
    stateOld=*p_toState;
    ++timeoutCounter;
}

/**
 * @brief Einrichten/Überprüfen von Timeout-Events.
 * @param timeoutMS Gewünschtes Timeout in ms
 * @return  1: Timeout eingetreten 0: Timeout nicht eingetreten, 0xFF: Timeout nicht erstellt
 */
static uint8_t EVENTS_TimeoutHandler(uint16_t timeoutMS)
{
    if(timeoutMS<=loopDelayMS)                  /*Wenn gewünschtes timeout kleiner oder gleich eingestelltem Loopdelay*/
    {
        return 1;                               /*Rückgabe: Timeoutereignis eingetreten->timeout=loop_delay*/
    }
    else
    {
        uint8_t i;
        /*Prüfen ob Timeout schon vorhanden*/
        for(i=0; i<MAXTIMEOUTS && timeEvents[i].ms!=timeoutMS; i++);
        if(i==MAXTIMEOUTS)                              /*Timout noch nicht vorhanden*/
        {
            /*Prüfen ob noch Timeouts vorhanden sind*/
            for(i=0; i<MAXTIMEOUTS && timeEvents[i].active>0; i++);
            if(i==MAXTIMEOUTS) return 0xFF;             /*Rückgabe: Fehler keine Timeouts mehr verfügbar*/
            /*neues Timeout speichern*/
            INTCONbits.GIE=0;                           /*Interrupts ausschalten (counter absichern)*/
            timeEvents[i].active = 1;
            timeEvents[i].eventOccured = 0;
            timeEvents[i].ms = timeoutMS;
            timeEvents[i].end = timeoutCounter+timeoutMS-2*loopDelayMS;
            INTCONbits.GIE=1;                           /*Interrupts wieder einschalten*/
            return 0;                                   /*Rückgabe: Kein Timeoutereignis eingetreten*/
        }
        else                                            /*Timeout bereits vorhanden*/
        {
            if(timeEvents[i].active && timeEvents[i].eventOccured)            /*Timeoutereignis eingetreten*/
            {
                /*Timeout zurücksetzen*/
                timeEvents[i].eventOccured = 0;
                timeEvents[i].active = 0;
                timeEvents[i].ms = 0;
                return 1;                               /*Rückgabe: Timeoutereignis eingetreten*/
            }
        }
        return 0;                                       /*Rückgabe: Kein Timeoutereignis eingetreten*/
    }
}

/**
 * @brief Einrichten und überprüfen von Active-Until-Events.
 * @param activeUntilMS Gewünschte Aktivzeit in ms
 * @return  1: Event eingetreten
 */
static uint8_t EVENTS_ActiveUntilHandler(uint16_t activeUntilMS)
{
    uint8_t i;
    /*Prüfen ob Timeout schon vorhanden*/
    for(i=0; i<MAXTIMEOUTS && timeEvents[i].ms != activeUntilMS; i++);
    if(i==MAXTIMEOUTS)                              /*Timout noch nicht vorhanden*/
    {
        /*Prüfen ob noch Timeouts vorhanden sind*/
        for(i=0; i<MAXTIMEOUTS && timeEvents[i].active>0; i++);
        if(i==MAXTIMEOUTS) return 0xFF;             /*Rückgabe: Fehler keine Timeouts mehr verfügbar*/
        /*neues Timeout speichern*/
        INTCONbits.GIE=0;                           /*Interrupts ausschalten (counter absichern)*/
        timeEvents[i].active = 1;
        timeEvents[i].eventOccured = 0;
        timeEvents[i].ms = activeUntilMS;
        timeEvents[i].end = timeoutCounter+activeUntilMS-loopDelayMS;
        INTCONbits.GIE=1;                           /*Interrupts wieder einschalten*/
        return 1;                                   /*Rückgabe: Kein Timeoutereignis eingetreten*/
    }
    else                                            /*Timeout bereits vorhanden*/
    {
        if(timeEvents[i].active == 2)              /*Ontime-Timemout bereits 1x abgelaufen*/
            return 0;
        if(timeEvents[i].active && timeEvents[i].eventOccured)    /*Timeoutereignis eingetreten*/
        {
            /*Timeout zurücksetzen*/
            timeEvents[i].eventOccured = 0;
            timeEvents[i].active = 2;
            return 0;                               /*Rückgabe: Timeoutereignis eingetreten*/
        }
    }
    return 1;                                       /*Rückgabe: Kein Timeoutereignis eingetreten*/
}

/**
 * @brief Zurücksetzen von Timeout- und Active-Until Events
 * @param t Zeit in ms des Events welcher zurückgesetzt werden soll
 * @return 0: Event erfolgreich zurückgesetzt, 1: zurücksetzen nicht möglich (Event nicht vorhanden)
 */
static uint8_t EVENTS_ResetTimeoutHandler(uint16_t t)
{
    uint8_t i;
    for(i=0; (i<MAXTIMEOUTS) && (timeEvents[i].ms != t); i++);  /* Event suchen */
     if( i == MAXTIMEOUTS)                                      /* Event nicht vorhanden */
     {
         return 1;   /* Timeout nicht gefunden, zurücksetzen nicht möglich */
     }
     else                                                       /* Event vorhanden */
     {
         timeEvents[i].active = 0;
         timeEvents[i].end = 0;
         timeEvents[i].eventOccured = 0;
         timeEvents[i].ms = 0;
        return 0;  /* Timeout erfolgreich zurückgesetzt */
     }
}
#endif
/**
 * @}
 */