/**
 * @addtogroup PBA_ADC
 * @{
 *******************************************************************************
 * @file            PBA_driver_adc.h
 * @brief           ADC-Library für das PBA6
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
#ifndef _PBA_DRIVER_ADC_
#define _PBA_DRIVER_ADC_

/* Konstanten für ADC_SetAnalogANx() bei Verwendung mit PIC16F1787*/

/**
 * @addtogroup ADC_KONSTANTEN ADC-Konstanten
 * @{
 * @addtogroup ADC_ANALOG_INPUTS Analog Inputs PIC16F1787
 * Die folgenden Makros dienen der Definition der analogen Eingänge für den PIC16F1787
 * @{
 */
    #define NO_ANALOGS              0x00000000  /**< Alle Pins Digital */
    #define ALL_ANALOG              0x2F3F0207  /**< Alle Pins Analog (power up default) */
    /*Analogpins Port A*/
    #define AN0                     0x01000000  /**< RA0 Analog */
    #define AN1                     0x02000000  /**< RA1 Analog */
    #define AN2                     0x04000000  /**< RA2 Analog */
    #define AN3                     0x08000000  /**< RA3 Analog */
    #define AN4                     0x20000000  /**< RA5 Analog */
    /*Analogpins Port E*/
    #define AN5                     0x00000001  /**< RE0 Analog */
    #define AN6                     0x00000002  /**< RE1 Analog */
    #define AN7                     0x00000004  /**< RE2 Analog */
    /*Analogpins Port B*/
    #define AN8                     0x00040000  /**< RB2 Analog */
    #define AN9                     0x00080000  /**< RB3 Analog */
    #define AN10                    0x00020000  /**< RB1 Analog */
    #define AN11                    0x00100000  /**< RB4 Analog */
    #define AN12                    0x00010000  /**< RB0 Analog */
    #define AN13                    0x00200000  /**< RB5 Analog */
    /*Analogpins Port D*/
    #define AN21                    0x00000200  /**< RD1 Analog */
/**
 * @}
 */


/* Konstanten für ADC_Init() bei Verwendung mit PIC16F1787*/
/***************************************************************************************************************
Anwendungsbeispiele:
    ADC_Init(ADC_CLOCK_DIV_32|VREFL_VSS|NEG_IN_REF|POS_IN_AN0);
****************************************************************************************************************/
/**
 * @addtogroup ADC_SETTINGS Einstellungen ADC PIC16F1787
 * Die folgenden Makros dienen zur Konfiguration des ADC.
 * @{
 */
    /**
     * @name  Taktauswahl
     * @{
     */
    #define ADC_OFF                 0x000000    /**< ADC off (power up default) */
    #define ADC_CLOCK_DIV_2         0x010000    /**< fosc/2 + ADC on */
    #define ADC_CLOCK_DIV_4         0x014000    /**< fosc/4 + ADC on */
    #define ADC_CLOCK_DIV_8         0x011000    /**< fosc/8 + ADC on */
    #define ADC_CLOCK_DIV_16        0x015000    /**< fosc/16 + ADC on */
    #define ADC_CLOCK_DIV_32        0x012000    /**< fosc/32 + ADC on */
    #define ADC_CLOCK_DIV_64        0x016000    /**< fosc/64 + ADC on */
    #define ADC_CLOCK_RC            0x013000    /**< Extern RC-OSC + ADC on */
    /**
     * @}
     */

    /**
     * @name  Negative Referenz
     * VREF-
     * @{
     */
    #define VREFL_VSS               0x000000    /**< VREF- an GND (Standard) */
    #define VREFL_VREF              0x000400    /**< VREF- an vref-pin */
    /**
     * @}
     */

    /**
     * @name  Positive Referenz
     * VREF+
     * @{
     */
    #define VREFH_VDD               0x000000    /**< VREF+ an VCC   (Standard) */
    #define VREFH_VREF              0x000100    /**< VREF+ an vref-pin */
    #define VREFH_FVR               0x000300    /**< VREF+ an Fixed Voltage Reference */
    /**
     * @}
     */

    /**
     * @name  Negativer Eingang
     * Standard: VREF-
     * @{
     */
    #define NEG_IN_REF              0x00000F
    #define NEG_IN_AN0              0x00000E
    #define NEG_IN_AN1              0x00000D
    #define NEG_IN_AN2              0x00000C
    #define NEG_IN_AN3              0x00000B
    #define NEG_IN_AN4              0x00000A
    #define NEG_IN_AN5              0x000009
    #define NEG_IN_AN6              0x000008
    #define NEG_IN_AN7              0x000007
    #define NEG_IN_AN8              0x000006
    #define NEG_IN_AN9              0x000005
    #define NEG_IN_AN10             0x000004
    #define NEG_IN_AN11             0x000003
    #define NEG_IN_AN12             0x000002
    #define NEG_IN_AN13             0x000001
    #define NEG_IN_AN21             0x000000
    /**
     * @}
     */

    /**
     * @name  Positiver Eingang
     * (Mess-Eingang)
     * @{
     */
    #define POS_IN_FVR              0x7C0000
    #define POS_IN_DAC              0x780000
    #define POS_IN_TEMP_IND         0x740000
    #define POS_IN_AN0              0x000000
    #define POS_IN_AN1              0x040000
    #define POS_IN_AN2              0x080000
    #define POS_IN_AN3              0x0C0000
    #define POS_IN_AN4              0x100000
    #define POS_IN_AN5              0x140000
    #define POS_IN_AN6              0x180000
    #define POS_IN_AN7              0x1C0000
    #define POS_IN_AN8              0x200000
    #define POS_IN_AN9              0x240000
    #define POS_IN_AN10             0x280000
    #define POS_IN_AN11             0x2C0000
    #define POS_IN_AN12             0x300000
    #define POS_IN_AN13             0x340000
    #define POS_IN_AN21             0x540000
    /**
     * @}
     */

    /**
     * @name  Resultat-Format
     * @{
     */
    #define RESULT_L_JUST           0x000000    /**< Resultat left justified (Standard) */
    #define RESULT_R_JUST           0x008000    /**< Resultat right justified */
    /**
     * @}
     */

    /**
     * @name  Resultat-Modus
     * @{
     */

    #define RES_10_BIT              0x800000    /**< Resultat 10-bit */
    #define RES_12_BIT              0x000000    /**< Resultat 12-bit */
    /**
     * @}
     */
/**
 * @}
 */
/**
 * @}
 */

void ADC_SetAnalogANx(uint32_t pin);


void ADC_Init(uint32_t settings);


void ADC_SetChannel(uint8_t channel);


uint16_t ADC_Read(void);

#endif

/**
 *@}
 */