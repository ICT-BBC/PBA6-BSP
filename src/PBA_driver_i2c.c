/**
 * @defgroup PBA_I2C
 * @{
 *******************************************************************************
 * @file            PBA_driver_i2c.c
 * @brief           I2C-Library für PBA4/5/6
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
#ifdef USE_I2C_LIBRARY

/**
 * @name Konfiguration I2C Bitrate
 * Anpassen falls nötig
 * @{
 */
#define FSCL_HZ     100000  /**< 100kHz I2C-Mode */
/**
 * @}
 */

/**
 *@brief Berechnung Taktgenerator
 */
#define BRGVAL      _XTAL_FREQ/(4*FSCL_HZ)-1        /**<BRG = (PIC / (4 * SCL)) -1*/

/**
 * @brief Warten auf i2c Interrupt-Flag. Blockierend!
 */
void I2C_WaitIF(void)
{
    while(!PIR1bits.SSP1IF);                /*Warten bis I2C-IF gesetzt (MSSP action done)*/
    PIR1bits.SSP1IF=0;                      /*Interrupt-Flag rücksetzen*/
}

/**
 * @brief Initialisiert i2c Schnittstelle.
 * Takt einstellen mit Define @link FSCL_HZ FSCL_HZ@endlink in Datei @link PBA_driver_i2c.c@endlink.
 */
void I2C_Init(void)
{
    TRISC|=0b00011000;                      /*RC3(SCL), RC4(SDA) als Inputs definieren*/
    SSPADD=BRGVAL;                          /*Takt 100kHz: BRGVAL = (FOSC / (4 * SCL)) -1*/
    SSPCON=0x08;                            /*master mode, clock=Fosc/(4*(SSPADD+1))*/
    SSP1CON1bits.SSPEN = 1;                 /*MSSP-Modul eingeschalten (SSP-Enable)*/
}

/**
 * @brief Bus übernehmen.
 */
void I2C_Start(void)
{
    SSP1CON2bits.SEN = 1;                   /*Bus übernemen*/
    I2C_WaitIF();                           /*Warten bis Bus übernommen */
}

/**
 * @brief Bus erneut übernehmen.
 */
void I2C_Restart(void)
{
    SSP1CON2bits.RSEN = 1;                  /*Bus erneut übernehmen*/
    I2C_WaitIF();                           /*Warten bis Bus übernommen */
}

/**
 * @brief Bus freigeben.
 */
void I2C_Stop(void)
{
    SSP1CON2bits.PEN = 1;                   /*Bus freigeben*/
    I2C_WaitIF();                           /*Warten bis Bus freigegeben*/
}

/**
 * @brief Byte an Slave übertragen.
 * @param data Datenbyte
 */
void I2C_WriteByte(uint8_t data)
{
    SSPBUF=data;                            /*Ein Byte zum I2C-Slave übertragen */
    I2C_WaitIF();                           /*Warten bis Daten geschrieben*/
}

/**
 * @brief Daten-Byte lesen.
 * @param ack 1->ACK, 0->NAK
 * @return Eingelesene Daten
 */
uint8_t I2C_ReadByte(uint8_t ack)
{
    SSP1CON2bits.RCEN = 1;                  /*Daten Empfang einschalten*/
    I2C_WaitIF();                           /*Warten bis Bus übernommen*/
    SSP1CON2bits.ACKDT = !ack;              /*ACK / NoACK von Master*/
    SSP1CON2bits.ACKEN = 1;                 /*Acknowledge Sequenz starten*/
    I2C_WaitIF();                           /*Warten bis Bus übernommen*/
    return SSPBUF;
}
#endif

/**
 * @}
 */