/**
 * @defgroup PBA_HELPERS
 * @{
 *******************************************************************************
 * @file            PBA_helpers.c
 * @brief           Helper-Library für PBA4/5/6
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
#ifdef USE_HELPERS_LIBRARY
/**
 * @brief Delayroutine für Delays im Millisekunden-Bereich.
 * Blockierend!
 * @param timeMS Gewünschtes Delay in ms
 */

void DelayMS(uint16_t timeMS)   /*Workaround für grössere Delays bei 20MHz Systemtakt*/
{
    for(;timeMS>=35;timeMS-=35)
        __delay_ms(35);
    for(;timeMS>=1;timeMS--)
        __delay_ms(1);
}

/**
 * @brief Delayroutine für Delays im Mikrosekunden-Bereich.
 * Blockierend!
 * @param timeUS Gewünschtes Delay in us
 */
void DelayUS(uint16_t timeUS)   /*Workaround für grössere Delays bei 20MHz Systemtakt*/
{
    for(;timeUS>=100;timeUS-=100)
        __delay_us(100);
    for(;timeUS>=10;timeUS-=10)
        __delay_us(10);
}


/**
 * @brief Wandelt eine Binärzahl (8-Bit) um in BCD-Code.
 * @param value Zahl die gewandelt werden soll
 * @return BCD-Wert (4-Bit H|4-Bit Z|4-Bit E)LSB
 */
uint16_t BinToBcd(uint8_t value)        /*Umwandlung Binär zu BCD*/
{
    uint8_t E,Z;                        /*dekl. Lokale Variabeln*/
    uint16_t H;                         /*dekl. Lokale Variabeln*/
    H=value/100;                        /*Hunderter definieren*/
    Z=(value-=H*100)/10;                /*Zehner definieren*/
    E=value-Z*10;                       /*Einer definieren*/
    H<<=8;
    Z<<=4;
    return H|Z|E;
}

/**
 * @brief Gibt am Summer für einen Ton mit bestimmter Länge und Frequenz aus.
 * Blockierend!
 * @param freqHZ Gewünschte Frequenz in Hz
 * @param durationMS Länge des Tons in ms
 */
void Beep(uint16_t freqHZ,uint16_t durationMS) /*Ton auf dem Summer ausgeben (Parameter f und Länge in ms)*/
{
    uint16_t impulsTimeCnt;
    uint16_t impulsTimeUS;
    uint32_t durationUS;
    uint32_t periodCnt;
    impulsTimeUS= 472500/freqHZ;                /*Implus-/Pausendauer in us mit Korrekturfaktor*/
    durationUS=durationMS*939;                  /*Tonlänge in us mit Korrekturfaktor*/
    periodCnt=durationUS/(impulsTimeUS*2);      /*Anzahl Perioden ->  Tonlänge/(Impulsdauer*2)*/
    for(;periodCnt>0;periodCnt--)
    {
        impulsTimeCnt=0;
        SUMMER=1;
        #if _XTAL_FREQ==32000000
        while((impulsTimeCnt+=13)<=impulsTimeUS) __delay_us(12);    /*12us Delay, Zeit jeweils um 13 hochzählen (Korrekturfaktor)*/
        #else
        while((impulsTimeCnt+=14)<=impulsTimeUS) __delay_us(12);    /*Bei 20Mhz grössere Korrektur*/
        #endif
        impulsTimeCnt=0;
        SUMMER=0;
        #if _XTAL_FREQ==32000000
        while((impulsTimeCnt+=13)<=impulsTimeUS) __delay_us(12);
        #else
        while((impulsTimeCnt+=14)<=impulsTimeUS) __delay_us(12);
        #endif
    }
}

/**
 * @brief Spiegelt ein Byte.
 * @param value Zu Spiegelndes Byte
 * @return Gespiegelter Wert
 */
uint8_t MirrorByte(uint8_t value)
{
    uint8_t i,rw;
    for(i=rw=0;i<=7;i++)
        if(BIT_TEST(value,i))
            BIT_SET(rw,7-i);
    return rw;
}


/**
 * @brief Zählt die Anzahl gesetzter Bits in einer Variable.
 * @param value Zu prüfender Wert
 * @return Anzahl gesetzter Bits
 */
uint8_t BitSum(uint8_t value)
{
    uint8_t i,Result;
    for(i=Result=0;i<=7;i++)
        Result+=BIT_TEST(value,i);
    return Result;
}

/**
 * @brief Gibt die Bitnummer des letzten gesetzten Bits zurück.
 * @param value Zu prüfendert Wert
 * @return Nummer des letzten gesetzten Bits
 */
uint8_t BitNumb(uint8_t value)
{
    uint8_t i,Result;
    Result=0xFF;
    for(i=0;i<=7;i++)
        Result = BIT_TEST(value,i) ? i : Result;
    return Result;
}
#endif
/**
 * @}
 */