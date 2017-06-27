/**
 * @defgroup PBA_HELPERS
 * @{
 *******************************************************************************
 * @file            PBA_helpers.c
 * @brief           Helper-Library f�r das PBA6
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
 * @brief Delayroutine f�r Delays im Millisekunden-Bereich.
 * Blockierend!
 * @param timeMS Gew�nschtes Delay in ms
 */

void DelayMS(uint16_t timeMS)   /*Workaround f�r gr�ssere Delays bei 20MHz Systemtakt*/
{
    for(;timeMS>=35;timeMS-=35)
        __delay_ms(35);
    for(;timeMS>=1;timeMS--)
        __delay_ms(1);
}

/**
 * @brief Delayroutine f�r Delays im Mikrosekunden-Bereich.
 * Blockierend!
 * @param timeUS Gew�nschtes Delay in us
 */
void DelayUS(uint16_t timeUS)   /*Workaround f�r gr�ssere Delays bei 20MHz Systemtakt*/
{
    for(;timeUS>=100;timeUS-=100)
        __delay_us(100);
    for(;timeUS>=10;timeUS-=10)
        __delay_us(10);
}


/**
 * @brief Wandelt eine Bin�rzahl (8-Bit) um in BCD-Code.
 * @param value Zahl die gewandelt werden soll
 * @return BCD-Wert (4-Bit H|4-Bit Z|4-Bit E)LSB
 */
uint16_t BinToBcd(uint8_t value)        /*Umwandlung Bin�r zu BCD*/
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
 * @brief Gibt am Summer f�r einen Ton mit bestimmter L�nge und Frequenz aus.
 * Blockierend!
 * @param freqHZ Gew�nschte Frequenz in Hz
 * @param durationMS L�nge des Tons in ms
 */
void Beep(uint16_t freqHZ,uint16_t durationMS) /*Ton auf dem Summer ausgeben (Parameter f und L�nge in ms)*/
{
    uint16_t impulsTimeCnt;
    uint16_t impulsTimeUS;
    uint32_t durationUS;
    uint32_t periodCnt;
    impulsTimeUS= 472500/freqHZ;                /*Implus-/Pausendauer in us mit Korrekturfaktor*/
    durationUS=(uint32_t) durationMS*939;                  /*Tonl�nge in us mit Korrekturfaktor*/
    periodCnt=durationUS/(impulsTimeUS*2);      /*Anzahl Perioden ->  Tonl�nge/(Impulsdauer*2)*/
    for(;periodCnt>0;periodCnt--)
    {
        impulsTimeCnt=0;
        SUMMER=1;
        while((impulsTimeCnt+=14)<=impulsTimeUS) __delay_us(12);    /*12us Delay, Zeit jeweils um 13 hochz�hlen (Korrekturfaktor)*/
        impulsTimeCnt=0;
        SUMMER=0;
        while((impulsTimeCnt+=14)<=impulsTimeUS) __delay_us(12);
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
 * @brief Z�hlt die Anzahl gesetzter Bits in einer Variable.
 * @param value Zu pr�fender Wert
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
 * @brief Gibt die Bitnummer des letzten gesetzten Bits zur�ck.
 * @param value Zu pr�fendert Wert
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