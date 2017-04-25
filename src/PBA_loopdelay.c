/**
 * @defgroup PBA_LOOPDELAY
 * @{
 *******************************************************************************
 * @file			PBA_loopdelay.c
 * @brief			Loopdelay-Library für PBA4/5/6
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
#ifdef USE_LOOPDELAY_LIBRARY

	#if !defined(LOOPDELAY_USE_TIMER0) && !defined(LOOPDELAY_USE_TIMER1) && !defined(LOOPDELAY_USE_TIMER2)
		#error Wählen Sie ein Timer-Modul aus für das Loop-Delay (PBA_loopdelay.h)
	#endif
	
	#if defined(LOOPDELAY_USE_TIMER0)
		#if _XTAL_FREQ <= 8000000
			#define	TIMER0_RELOAD_VALUE	(0xFF-(_XTAL_FREQ/32000UL))+1
		#else
			#error _XTAL_FREQ Wert zu gross für Loop-Delay, wählen Sie ein anderes Timer-Modul oder eine kleinere Frequenz
		#endif
	#endif
	#if defined(LOOPDELAY_USE_TIMER1)
		#if _XTAL_FREQ >= 1000000
			#define	TIMER1_RELOAD_VALUE	(0xFFFF-(_XTAL_FREQ/32000UL))+1
		#else
			# error _XTAL_FREQ Wert zu klein für Loop-Delay, wählen Sie ein anderes Timer-Modul oder eine grössere Frequenz
		#endif
	#endif
	#if defined(LOOPDELAY_USE_TIMER2)
		#if _XTAL_FREQ >= 2000000
			#if _XTAL_FREQ>20000000
				#define	TIMER2_PERIODE	((_XTAL_FREQ/160000UL))-1
			#else
				#define	TIMER2_PERIODE	((_XTAL_FREQ/80000UL))-1
			#endif
		#else
			# error _XTAL_FREQ Wert zu klein für Loop-Delay, wählen Sie ein anderes Timer-Modul oder eine grössere Frequenz
		#endif	
	#endif
	
	#define MAX_LOOP_DELAY_MS	10000
	static volatile uint16_t loopDelayCnt = 0;
	static uint16_t loopDelayMS=0;
	
	/**
	* @brief Interrupt-Routine Loop-Delay-Timer.
	*/
	inline void	 LOOPDELAY_TimerISR(void)
	{
	#if defined(LOOPDELAY_USE_TIMER0)
			/*Timer neu laden um genau 1kHz Interruptfrequenz zu erreichen*/
			TMR0=TIMER0_RELOAD_VALUE;
			/*Loop-Delay-Zähler inkrementieren (x1ms)*/
			if(loopDelayCnt<MAX_LOOP_DELAY_MS)
				loopDelayCnt++;
	#endif
	
	#if defined(LOOPDELAY_USE_TIMER1)
			/*Timer neu laden um genau 1kHz Interruptfrequenz zu erreichen*/
			TMR1H=(TIMER1_RELOAD_VALUE)>>8;
			TMR1L=(TIMER1_RELOAD_VALUE);
			/*Loop-Delay-Zähler inkrementieren (x1ms)*/
			if(loopDelayCnt<MAX_LOOP_DELAY_MS)
				loopDelayCnt++;
	#endif
	
	#if defined(LOOPDELAY_USE_TIMER2)
			/*Loop-Delay-Zähler inkrementieren (x1ms)*/
			if(loopDelayCnt<MAX_LOOP_DELAY_MS)
				loopDelayCnt++;
	#endif
	}
	
	/**
	 * @brief Zykluszeit abwareten
	 */
	void LOOPDELAY_Wait(void)
	{
		while(loopDelayCnt < loopDelayMS);
		loopDelayCnt = 0;
	}
	
	/**
	 * @brief Abfrage der eingestellten Zykluszeit
	 * @return Eingestellte Zykluszeit in ms
	 */
	uint16_t LOOPDELAY_GetTime(void)
	{
		return loopDelayMS;
	}
	
	
	/**
	* @brief Intitialisierung des Loop-Delays
	* @param loopDelayTimeMS Zykluszeit in ms
	*/
	void LOOPDELAY_Init(uint16_t loopDelayTimeMS)
	{
	loopDelayMS=loopDelayTimeMS;
	#if defined(LOOPDELAY_USE_TIMER0)
		OPTION_REG &= 0xC0;							/*TIMER0-Bits im OPTION-Register löschen*/
		OPTION_REG |= 0b00000010;					/*TIMER0 einstellen (OPTION-Register) (Prescaler = 8)*/
		TMR0=TIMER0_RELOAD_VALUE;
		INTCONbits.T0IF = 0;						/*Timer0-Interrupt-Flag löschen*/
		INTCONbits.T0IE	= 1;						/*Timer0-Interrupt einschalten*/
	#endif
	#if defined(LOOPDELAY_USE_TIMER1)
		T1CON	= 0b00110001;						/*Prescaler = 8, Timer on*/
		TMR1H=(TIMER1_RELOAD_VALUE)>>8;
		TMR1L=(TIMER1_RELOAD_VALUE);
		PIR1bits.TMR1IF = 0;						/*Timer1-Interrupt-Flag löschen*/
		PIE1bits.TMR1IE	= 1;						/*Timer1-Interrupt einschalten*/
	#endif
	#if defined(LOOPDELAY_USE_TIMER2)
		#if _XTAL_FREQ>20000000
		T2CON	= 0b01001101;						/*Prescaler =4, Timer on, Postscaler = 10*/
		#else
		T2CON	= 0b00100101;						/*Prescaler = 4, Timer on, Postscaler = 5*/
		#endif
		PR2		= TIMER2_PERIODE;					/*Timer2 Periode festlegen*/
		TMR2	= 0;								/*Timer2-Register löschen*/
		PIR1bits.TMR2IF = 0;						/*Timer2-Interrupt-Flag löschen*/
		PIE1bits.TMR2IE	= 1;						/*Timer2-Interrupt einschalten*/
	#endif
		INTCONbits.PEIE	= 1;						/*Peripherie-Interrupts einschalten*/
		INTCONbits.GIE	= 1;						/*Globale Interrupts einschalten*/
	}
#endif
/**
 * @}
 */