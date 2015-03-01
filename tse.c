/*
 * tse.c
 *
 *  Created on: 28.02.2015
 *      Author: blinch89
 */

#include <XMC1300.h>		// SFR declarations of the selected device
#include "XMC1000_TSE.h"	// TSE library header
#include "tse.h"
#include "GPIO.h"

uint8_t TSE_ISR_HIGH;		// Status for temp. High event
uint8_t TSE_ISR_LOW;		// Status for temp. Low event



uint8_t checkTSE()
{
	uint32_t *CSIDptr;
	uint32_t ID_Value;

	/* Check whether the device with TSE available */
	CSIDptr = (uint32_t *) 0x10000FE8;  // Assign User Config address
	ID_Value = *CSIDptr;				// Get identifier value from the User Config

	if(ID_Value != 0xFFFFFFFF)
		return 1;
	else
		return 0;
}





void initTSE()
{
	/* Enable global interrupt */
	__enable_irq();

	/* Enable SCU.SR1 interrupt */
	NVIC_EnableIRQ(SCU_1_IRQn);

	/* enable TSE */
	SCU_ANALOG->ANATSECTRL = 0x01;
}






void initTSEinterrupt()
{
	uint32_t temp_High_k;		// Temperature High in unit Kelvin
	uint32_t temp_HighVar;		// Temperature High in register counter value
	uint32_t temp_Low_k;		// Temperature Low in unit Kelvin
	uint32_t temp_LowVar;		// Temperature Low in register counter value

	/* Set TSE High interrupt temperature  */
	temp_High_k = 85 + 273;                          	// Convert 85°C to Kelvin
	temp_HighVar = XMC1000_CalcTSEVAR(temp_High_k);  	// Convert from Kelvin to TSE counter value
	SCU_ANALOG->ANATSEIH = temp_HighVar;				// Insert temp High value to register ANATSEIH
	SET_BIT(SCU_INTERRUPT->SRMSK,SCU_INTERRUPT_SRMSK_TSE_HIGH_Pos);		// Enable temp high interrupt

	/* Set TSE Low interrupt temperature  */
	temp_Low_k = 0 + 273;                           	// Convert 0°C to Kelvin
	temp_LowVar = XMC1000_CalcTSEVAR(temp_Low_k);   	// Convert from Kelvin to TSE counter value
	SCU_ANALOG->ANATSEIL = temp_LowVar;					// Insert temp. Low value to register ANATSEIL
	SET_BIT(SCU_INTERRUPT->SRMSK,SCU_INTERRUPT_SRMSK_TSE_LOW_Pos);		// Enable temp Low interrupt
}






/*
 *	This is the SCU.SR1 interrupt for temp. High and temp. Low event
 */
void SCU_1_IRQHandler (void)
{

	  /* TSE Compare High Event */
	  if(RD_REG(SCU_INTERRUPT->SRRAW,SCU_INTERRUPT_SRRAW_TSE_HIGH_Msk, SCU_INTERRUPT_SRRAW_TSE_HIGH_Pos))
	  {
		  /* clear the interrupt */
		  SCU_INTERRUPT->SRCLR |= SCU_INTERRUPT_SRCLR_TSE_HIGH_Msk;
		  /* disable interrupt */
		  CLR_BIT(SCU_INTERRUPT->SRMSK,SCU_INTERRUPT_SRMSK_TSE_HIGH_Pos);
		  TSE_ISR_HIGH = 1;

		  /* user code goes here... */

	  }
	  /* TSE Compare Low Event */
	  else if(RD_REG(SCU_INTERRUPT->SRRAW,SCU_INTERRUPT_SRRAW_TSE_LOW_Msk, SCU_INTERRUPT_SRRAW_TSE_LOW_Pos))
	  {
		  /* clear the interrupt */
		  SCU_INTERRUPT->SRCLR |= SCU_INTERRUPT_SRCLR_TSE_LOW_Msk;
		  /* disable interrupt */
		  CLR_BIT(SCU_INTERRUPT->SRMSK,SCU_INTERRUPT_SRMSK_TSE_LOW_Pos);
		  TSE_ISR_LOW = 1;

		  /* user code goes here... */
	  }
}






int getTemperatureC()
{
	return (int)(XMC1000_CalcTemperature()-273); //-273 for conversion into °C
}






void checkTSEinterruptOccurence(int tempCelsius)
{
	/* Check if temp. Low event has occur and re-enable interrupt if temperature is above the TSE_LOW */
	if(TSE_ISR_LOW == 1 && tempCelsius > 0)
	{
		SET_BIT(SCU_INTERRUPT->SRMSK,SCU_INTERRUPT_SRMSK_TSE_LOW_Pos);
		TSE_ISR_LOW = 0;
	}

	/* Check if temp. High event has occur and re-enable interrupt if temperature is below the TSE_HIGH */
	if(TSE_ISR_HIGH == 1 && tempCelsius < 85)
	{
		SET_BIT(SCU_INTERRUPT->SRMSK,SCU_INTERRUPT_SRMSK_TSE_HIGH_Pos);
		TSE_ISR_HIGH = 0;
	}
}
