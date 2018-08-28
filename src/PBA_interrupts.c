/**
 * @defgroup PBA_INTERRUPTS
 * @{
 *******************************************************************************
 * @file            PBA_interrupts.c
 * @brief           Interrupt-Handler für das PBA6
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
#ifdef USE_INTERRUPT_LIBRARY

/**
 *@brief maximale Anzahl Callbackfunktionen pro Timer-Interrupt
 */
#define MAX_TMR_CALLBACK_FUNCTIONS 3

/**
 *@brief Funktionspointer-Array für Timer0 Callbackfunktionen
 */
static callbackFunction_t tmr0CallbackFunction[MAX_TMR_CALLBACK_FUNCTIONS]={NULL};

/**
 *@brief Funktionspointer-Array für Timer1 Callbackfunktionen
 */
static callbackFunction_t tmr1CallbackFunction[MAX_TMR_CALLBACK_FUNCTIONS]={NULL};

/**
 *@brief Funktionspointer-Array für Timer2 Callbackfunktionen
 */
static callbackFunction_t tmr2CallbackFunction[MAX_TMR_CALLBACK_FUNCTIONS]={NULL};

/**
 *@brief Funktionspointer für UART Callbackfunktion
 */
static callbackFunction_t uartCallbackFunction = NULL;

/**
 *@brief Funktionspointer für Ext. Interrupt Callbackfunktion
 */
static callbackFunction_t extIntCallbackFunction = NULL;

static void UnhandledInterrupt(void);

/**
 * @brief Globale Interruptroutine.
 */
void __interrupt() INT_Manager(void)
{
    /******* Timer 0 Interrupt **************************************************/
    if(INTCONbits.TMR0IF && INTCONbits.TMR0IE)
    {
        INTCONbits.TMR0IF = 0;
        if(NULL != tmr0CallbackFunction[0]) /* Callback-Funktion hinterlegt */
        {
            for(uint8_t cnt=0; (cnt< MAX_TMR_CALLBACK_FUNCTIONS) && (NULL != tmr0CallbackFunction[cnt]); cnt++)
            {
                tmr0CallbackFunction[cnt]();/* Alle vorhandenen Funktionen ausführen */
            }
        }
        else                                /* keine Callback-Funktion hinterlegt */
        {
            UnhandledInterrupt();           /* nicht behandelter Interrupt */
        }

    }
    /******* Timer 1 Interrupt **************************************************/
    if(INTCONbits.PEIE && PIE1bits.TMR1IE && PIR1bits.TMR1IF)
    {
        PIR1bits.TMR1IF=0;
         if(NULL != tmr1CallbackFunction[0]) /* Callback-Funktion hinterlegt */
        {
            for(uint8_t cnt=0; (cnt< MAX_TMR_CALLBACK_FUNCTIONS) && (NULL != tmr1CallbackFunction[cnt]); cnt++)
            {
               tmr1CallbackFunction[cnt]();/* Alle vorhandenen Funktionen ausführen */
            }
        }
        else                                /* keine Callback-Funktion hinterlegt */
        {
            UnhandledInterrupt();           /* nicht behandelter Interrupt */
        }

    }

    /******* Timer 2 Interrupt **************************************************/
    if(INTCONbits.PEIE && PIE1bits.TMR2IE && PIR1bits.TMR2IF)
    {
        PIR1bits.TMR2IF=0;
         if(NULL != tmr2CallbackFunction[0]) /* Callback-Funktion hinterlegt */
        {
            for(uint8_t cnt=0; (cnt< MAX_TMR_CALLBACK_FUNCTIONS) && (NULL != tmr2CallbackFunction[cnt]); cnt++)
            {
                tmr2CallbackFunction[cnt]();/* Alle vorhandenen Funktionen ausführen */
            }
        }
        else                                /* keine Callback-Funktion hinterlegt */
        {
            UnhandledInterrupt();           /* nicht behandelter Interrupt */
        }

    }

    /******* UART-Receive-Interrupt ********************************************/
    if(INTCONbits.PEIE && PIE1bits.RCIE && PIR1bits.RCIF)
    {
        if(NULL != uartCallbackFunction)    /* Callback-Funktion hinterlegt */
        {
            uartCallbackFunction();         /* Funktion ausführen */
        }
        else                                /* keine Callback-Funktion hinterlegt */
        {
            UnhandledInterrupt();           /* nicht behandelter Interrupt */
        }
    }

    /******* Interrupt an ext. Interrupt-Pin (PORTB0) **************************/
    if(INTCONbits.INTE && INTCONbits.INTF)
    {
        INTCONbits.INTF=0;
        if(NULL != extIntCallbackFunction)  /* Callback-Funktion hinterlegt */
        {
            extIntCallbackFunction();       /* Funktion ausführen */
        }
        else                                /* keine Callback-Funktion hinterlegt */
        {
            UnhandledInterrupt();           /* nicht behandelter Interrupt */
        }
    }
}


/**
 * @brief Wird im Falle eines nicht behantelten Interrupts ausgeführt. Blockierend!
 */
static void UnhandledInterrupt(void)
{
    while(1)                                /* Endlosschleife */
    {
        LEDS_D^=1;                          /* LED0 blinkt */
        DelayMS(50);                        /* mit 10Hz */
    }
}

/**
 * @brief Hinzufügen von Callbackfunktionen für Timer0 Interrupt
 * @param p_callBackFunction Pointer auf Callbackfunktion
 * @return 0: Funktion erfolgreich hinzugefügt
 */
uint8_t INT_AddTmr0CallbackFnc (callbackFunction_t p_callBackFunction)
{
    for(uint8_t cnt=0; cnt<MAX_TMR_CALLBACK_FUNCTIONS; cnt++)    /* Suche in Array */
    {
        if(tmr0CallbackFunction[cnt]==NULL)                 /* ersten freien Platz */
        {
            tmr0CallbackFunction[cnt]=p_callBackFunction;   /* füge Funktion hinzu */
            return 0;                                       /* erfolgreich */
        }
    }
    return 1;                                               /* keine freie Stelle */
}

/**
 * @brief Hinzufügen von Callbackfunktionen für Timer1 Interrupt
 * @param p_callBackFunction Pointer auf Callbackfunktion
 * @return 0: Funktion erfolgreich hinzugefügt
 */
uint8_t INT_AddTmr1CallbackFnc (callbackFunction_t p_callBackFunction)
{
    for(uint8_t cnt=0; cnt<MAX_TMR_CALLBACK_FUNCTIONS; cnt++)    /* Suche in Array */
    {
        if(tmr1CallbackFunction[cnt]==NULL)                 /* ersten freien Platz */
        {
            tmr1CallbackFunction[cnt]=p_callBackFunction;   /* füge Funktion hinzu */
            return 0;                                       /* erfolgreich */
        }
    }
    return 1;                                               /* keine freie Stelle */
}

/**
 * @brief Hinzufügen von Callbackfunktionen für Timer2 Interrupt
 * @param p_callBackFunction Pointer auf Callbackfunktion
 * @return 0: Funktion erfolgreich hinzugefügt
 */
uint8_t INT_AddTmr2CallbackFnc (callbackFunction_t p_callBackFunction)
{
    for(uint8_t cnt=0; cnt<MAX_TMR_CALLBACK_FUNCTIONS; cnt++)    /* Suche in Array */
    {
        if(tmr2CallbackFunction[cnt]==NULL)                 /* ersten freien Platz */
        {
            tmr2CallbackFunction[cnt]=p_callBackFunction;   /* füge Funktion hinzu */
            return 0;                                       /* erfolgreich */
        }
    }
    return 1;                                               /* keine freie Stelle */
}

/**
 * @brief Hinzufügen einer Callbackfunktion für UART-Empfangsinterrupt
 * @param p_callBackFunction Pointer auf Callbackfunktion
 * @return 0: Funktion erfolgreich hinzugefügt
 */
uint8_t INT_AddUartCallbackFnc (callbackFunction_t p_callBackFunction)
{
    if(uartCallbackFunction==NULL)                      /* noch keine Funktion hinterlegt */
    {
        uartCallbackFunction=p_callBackFunction;        /* füge Funktion hinzu */
        return 0;                                       /* erfolgreich */
    }
    return 1;                                           /* keine freie Stelle */
}

/**
 * @brief Hinzufügen einer Callbackfunktion externen Interrupt
 * @param p_callBackFunction Pointer auf Callbackfunktion
 * @return 0: Funktion erfolgreich hinzugefügt
 */
uint8_t INT_AddExtIntCallbackFnc (callbackFunction_t p_callBackFunction)
{
    if(extIntCallbackFunction==NULL)                    /* noch keine Funktion hinterlegt */
    {
        extIntCallbackFunction=p_callBackFunction;      /* füge Funktion hinzu */
        return 0;                                       /* erfolgreich */
    }
    return 1;                                           /* keine freie Stelle */
}


#endif

/**
 *@}
 */
