/**
 * @addtogroup PBA_EVENTS
 * @{
 *******************************************************************************
 * @file            PBA_events.h
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
#ifndef _PBA_EVENTS_
#define _PBA_EVENTS_

/**
*@brief Bitfeld zum speichern von Taster-Events
*/
typedef struct
{
    unsigned switch0: 1;
    unsigned switch1: 1;
    unsigned switch2: 1;
    unsigned switch3: 1;
    unsigned switch4: 1;
    unsigned switch5: 1;
    unsigned switch6: 1;
    unsigned switch7: 1;
}tasterByte_t;


/**
*@brief Events
*/
typedef struct
{
    union
    {
        uint16_t valueEdge;
        struct
        {
            tasterByte_t posEdge;       /**< Flags Taster positive Flanken*/
            tasterByte_t negEdge;       /**< Flags Taster negative Flanken*/
        };
    };
    union
    {
        uint16_t valuePressed;
        struct
        {
            tasterByte_t pressed500ms;  /**< Flags Taster min. 500ms gedrückt*/
            tasterByte_t pressed1000ms; /**< Flags Taster min. 1s gedrückt*/
        };
    };
    uint8_t (* TimeoutMS)(uint16_t);    /**< Starten/Abrufen Timeout-Events*/
    uint8_t (* ActiveUntilMS)(uint16_t);/**< Starten/Abrufen Active-Until-Events*/
    uint8_t (* ResetTimeoutMS)(uint16_t); /**< Zurücksetzen eines Timeout- oder Active-Until-Events */
} events_t;


inline void  EVENTS_TimerISR(void);

void EVENTS_Init(void *p_state,events_t *p_events);

void EVENTS_Update(void);

#endif
/**
 * @}
 */