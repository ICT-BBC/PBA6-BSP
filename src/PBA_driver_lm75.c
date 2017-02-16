/**
 * @defgroup PBA_LM75
 * @{
 *******************************************************************************
 * @file			PBA_driver_lm75.c
 * @brief			LM75-Library für PBA4/5/6
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
#ifdef USE_LM75_LIBRARY

#define TCN75_ADDRESS	(0x90)	/**< Adresse des Temperatursensors TCN75*/
#define I2C_READ		(0x01)

/**
 * @brief Initialisiert die I2C-Schnittstelle und den Temperatursensor.
 */
void LM75_Init(void) 
{
	I2C_Init();								/*I2C-Bus initialisieren*/
	I2C_Start();							/*Bus übernehmen*/
	I2C_WriteByte(TCN75_ADDRESS);			/*Adresse (+ write-Anweisung)*/
	I2C_WriteByte(0x00);					/*Pointer auf Temperaturregister*/
	I2C_Stop();								/*Bus freigeben*/
}
	
/**
 * @brief Auslesen der Temperatur. Bedingung: Temperatursensor muss initialisert sein.
 * @return Temperaturwert*10, 12Bit
 */
int16_t LM75_ReadTemp(void) 
{
	uint8_t dataL;
	int16_t dataH;
	int16_t tempMult10;
	DelayMS(105);							/*LM75 Temp.-konvertierung benötigt min 100ms*/
	I2C_Start();							/*Bus übernehmen*/
	I2C_WriteByte(TCN75_ADDRESS);			/*Adresse (+ write-Anweisung)*/
	I2C_WriteByte(0x00);					/*Pointer auf Temperaturregister*/
	I2C_Restart();							/*Bus erneut übernemen (Restart)*/
	I2C_WriteByte(TCN75_ADDRESS+I2C_READ);	/*Adresse (+ read-Anweisung)*/
	dataH=I2C_ReadByte(1);					/*1. Byte lesen (Temperatur in °C), ACK*/
	dataL=I2C_ReadByte(0);					/*2. Byte lesen (MSB = +/- 0.5°C), NAK*/
	I2C_Stop();								/*Bus freigeben*/
	tempMult10=(int16_t)dataH*10;			/*Umrechnen Temperatur*/
	if(BIT_TEST(dataL,7))					/*+/-0.5°C ?*/
	{
		if(tempMult10 < 0) tempMult10 -=5;	/*Wenn Temperatur negativ -> -0.5°C*/
		else tempMult10 += 5;				/*Wenn Temperatur positiv -> +0.5°C*/
	}
	return(tempMult10);						/*Temperaturwert x 10 zurückgeben*/
 }
#endif
/**
 * @}
 */