/**
 * @defgroup PBA_CONFIG
 * @{
 *******************************************************************************
 * @file            PBA_config.c
 * @brief           Konfiguration des Board-Support-Packages f�r das PBA6
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

/* CONFIG1*/
#pragma config FOSC = HS        /* Oscillator Selection (HS Oscillator, High-speed crystal/resonator connected between OSC1 and OSC2 pins)*/
#pragma config WDTE = OFF       /* Watchdog Timer Enable (WDT disabled)*/
#pragma config PWRTE = ON       /* Power-up Timer Enable (PWRT enabled)*/
#pragma config MCLRE = ON       /* MCLR Pin Function Select (MCLR/VPP pin function is MCLR)*/
#pragma config CP = OFF         /* Flash Program Memory Code Protection (Program memory code protection is disabled)*/
#pragma config CPD = OFF        /* Data Memory Code Protection (Data memory code protection is disabled)*/
#pragma config BOREN = OFF      /* Brown-out Reset Enable (Brown-out Reset disabled)*/
#pragma config CLKOUTEN = OFF   /* Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)*/
#pragma config IESO = ON        /* Internal/External Switchover (Internal/External Switchover mode is disabled)*/
#pragma config FCMEN = OFF      /* Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)*/

/* CONFIG2*/
#pragma config WRT = OFF        /* Flash Memory Self-Write Protection (Write protection off)*/
#pragma config VCAPEN = OFF     /* Voltage Regulator Capacitor Enable bit (Vcap functionality is disabled on RA6.)*/
#pragma config PLLEN = ON       /* PLL Enable (4x PLL enabled)*/
#pragma config STVREN = ON      /* Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)*/
#pragma config BORV = LO        /* Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)*/
#pragma config LPBOR = OFF      /* Low Power Brown-Out Reset Enable Bit (Low power brown-out is disabled)*/
#pragma config LVP = OFF        /* Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)*/


/**
 * @brief Initialisiert die PBA Hardware und Peripherie.
 */
void PBA_Init(void)
{
    OSCCONbits.SCS0=0;                                  /* Takteinstellung von Config-Word*/
    OSCCONbits.SCS1=0;
    ANSELA=ANSELB=ANSELD=ANSELE=0;                      /* Alle AD-Kan�le als digital I/Os*/
    WPUA=WPUB=WPUC=WPUD=WPUE=0;                         /* Interne Pull-Ups deaktivieren*/
    TRISA=0x03;                                         /* I/Os als Input/Output definieren*/
    TRISB=0xff;                                         /* Taster als Eing�nge*/
    TRISC=0x80;
    TRISD=0x00;                                         /* LEDs als Ausg�nge*/
    TRISE=0x00;
    LATD=LATE=LATC=0;                                   /* Ports zur�cksetzen*/

#if defined(USE_ADC_LIBRARY)
    ADC_SetAnalogANx(AN0|AN1);
    ADC_Init(ADC_CLOCK_DIV_32|VREFL_VSS|VREFH_VDD|NEG_IN_REF|POS_IN_AN0|RESULT_R_JUST|RES_12_BIT);
#endif

#if defined(USE_LCD_LIBRARY)
    #if defined (PBA6)
        LCD_Init(V_3V3);                                                /*LCD initialisieren f�r 3.3V*/
    #else
        LCD_Init(V_5V);                                                 /*LCD initialisieren f�r 5V*/
    #endif
#endif
#if defined(USE_I2C_LIBRARY)
    I2C_Init();                                                         /*I2C-Bus initialisieren*/
#endif
#if defined(USE_LM75_LIBRARY)
    LM75_Init();                                                        /*Temperatursensor initialisieren*/
#endif
#if defined(USE_UART_LIBRARY)
    UART_Init();                                                        /*serielle Schnittstelle initialisieren*/
#endif
}

void putch(char c)
{
#ifdef STDOUT_LCD
    LCD_Putc(c);
#endif
#ifdef STDOUT_UART
    UART_Putc(c);
#endif
}

#if (!defined(USE_LCD_LIBRARY) && defined(STDOUT_LCD)) || (!defined(USE_UART_LIBRARY) && defined(STDOUT_UART))
    #error please define the symbol for standard output functionn in PBA_config.h
#endif

/**
 *@}
 */