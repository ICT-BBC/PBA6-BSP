/**
 * @addtogroup PBA_MENU
 * @{
 *******************************************************************************
 * @file            PBA_menu.h
 * @brief           Menu-Library für PBA4/5/6
 * @author          ICT Berufsbildungscenter AG
 *******************************************************************************
 * 
 * @copyright
 * @{
 * 
 * Diese Software kann unter den Bedingungen der MIT-Lizenz verwendet werden.
 * 
 * MIT License
 *
 * Copyright &copy; 2016 ICT Berufsbildungscenter AG
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
#ifndef _MENU_
#define _MENU_

/*----- Typdefinitionen für das Menu -----------------------------------------*/

/**
* @brief Typ einer Funktion welche vom Menu aus aufgerufen wird.
*/
typedef uint8_t (menuFunc_t)(uint8_t);  

/**
* @brief Menueintrag.
*/
typedef struct s_menuEntry                  /**< Menueintrag */
{
    const unsigned  char *name;             /**< Menuname */
    uint8_t  id;                            /**< ID des Menupunkts */
    struct s_menuEntry  *next,              /**< Nächste Menupunkt, Taste ab */
                        *prev,              /**< Vorheriger Menupunkt, Taste auf */
                        *submenu,           /**< Untermenu */
                        *ret;               /**< Rückkehrpunkt */
    menuFunc_t          *function;          /**< Aufzurufende Funktion */
}menuEntry_t;


unsigned char MENU_Call(void);


void MENU_Init(menuEntry_t *p_menuStart, events_t *ptr_events);

#endif /* MENU_H_ */

/**
 * @}
 */
