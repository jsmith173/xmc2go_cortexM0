/*******************************************************************************
** PLATFORM : Infineon XMC1000 Series                                         **
*******************************************************************************/

/* NOTE: This example is not applicable to device with 'AEE' marking.		   *
 *       The device on the bootkit is not supporting this example code.		   *
 *		 Please replaced the device on the current bootkit with ES or		   *
 *		 qualified sample.													   *
 *																			   *
 *		 This Main.c file is used to demonstrate the use of some			   *
 *		 modules/libraries.									   				   */

#include "GPIO.h"
#include "string.h"
#include "stdio.h"
#include "tse.h"
#include "uartUtH.h"
#include "light_ws2812_cortex.h"

#define TICKS_PER_SECOND 10UL
#define TICKS_UPDATE_EVENT 10UL


const uint8_t rgb[]   = {0x00,0x20,0x00,
						 0x20,0x00,0x00,
						 0x00,0x00,0x20};

const uint8_t red[]   = {0x00,0x20,0x00,
						 0x00,0x20,0x00,
						 0x00,0x20,0x00};

const uint8_t green[] = {0x20,0x00,0x00,
						 0x20,0x00,0x00,
						 0x20,0x00,0x00};

const uint8_t blue[]  = {0x00,0x00,0x20,
						 0x00,0x00,0x20,
						 0x00,0x00,0x20};

const uint8_t white[] = {0x20,0x20,0x20,
						 0x20,0x20,0x20,
						 0x20,0x20,0x20};

const uint8_t off[]   = {0x00,0x00,0x00,
						 0x00,0x00,0x00,
						 0x00,0x00,0x00};


void updateOnboardLEDs(int tempCelsius);
void updateWS2812LEDs(uint8_t state);


int main(void)
{
	/* Setup system frequency */
	SCU_GENERAL->PASSWD = 0x000000C0UL; // disable bit protection
	SCU_CLK->CLKCR = 0x3FF00400UL; // MCLK = 8MHz, PCLK = 8MHz
	while((SCU_CLK->CLKCR & SCU_CLK_CLKCR_VDDC2LOW_Msk));
	SCU_GENERAL->PASSWD = 0x000000C3UL; // enable bit protection
	SystemCoreClockUpdate();

	// Initialize and start UART
	UART_Init();

	// System Timer initialization
	SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);

	P0_6_set_mode(OUTPUT_PP_GP); //WS2812 data pin
	P0_5_set_mode(INPUT_PU);

	if(checkTSE())
	{
		uartWriteLine("TSE available\r\n",16);
		initTSE();
		initTSEinterrupt();
		/* Set onboard LED pins to output mode */
		P1_0_set_mode(OUTPUT_PP_GP);
		P1_1_set_mode(OUTPUT_PP_GP);
	}
	else
		uartWriteLine("TSE not available\r\n",20);

	while(1)
	{}
	return 0;
}




void SysTick_Handler(void)
{
	static uint32_t update = 0;
	static uint8_t ws2812_state = 0;

	if(!P0_5_read())
		uartWriteLine("button pressed!\r\n",20);

	update++;
	if (update == TICKS_UPDATE_EVENT)
	{
		update = 0;
		int temperatureC = getTemperatureC();
		char tempStr[30] = {0};
		sprintf(tempStr,"temperature: %i\r\n",temperatureC);
		uartWriteLine(tempStr,sizeof(tempStr));
		updateOnboardLEDs(temperatureC);
		checkTSEinterruptOccurence(temperatureC);
		updateWS2812LEDs(ws2812_state++);
		if(ws2812_state == 6) ws2812_state = 0;
	}
}





void updateOnboardLEDs(int tempCelsius)
{
	if(tempCelsius == 25)
	{
		P1_0_set();
		P1_1_reset();
	}
	else if (tempCelsius > 25)
	{
		P1_0_reset();
		P1_1_set();
	}
	else if (tempCelsius < 25)
	{
		P1_0_set();
		P1_1_set();
	}
}


void updateWS2812LEDs(uint8_t state)
{
	switch(state)
	{
	case 0:
		ws2812_sendarray((uint8_t*)rgb, sizeof(rgb));
		break;
	case 1:
		ws2812_sendarray((uint8_t*)red, sizeof(red));
		break;
	case 2:
		ws2812_sendarray((uint8_t*)green, sizeof(green));
		break;
	case 3:
		ws2812_sendarray((uint8_t*)blue, sizeof(blue));
		break;
	case 4:
		ws2812_sendarray((uint8_t*)white, sizeof(white));
		break;
	case 5:
		ws2812_sendarray((uint8_t*)off, sizeof(off));
		break;
	default:
		break;
	}
}
