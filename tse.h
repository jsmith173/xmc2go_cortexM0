/*
 * tse.h
 *
 *  Created on: 28.02.2015
 *      Author: blinch89
 * Description: This module is based on INFINEONs example Apps and the source code is almost
                unchanged.
                It enables the use of the internal temperature sensor of XMC1000 AES based/like
                microcontrollers.
       version: 1.0
   tested with: DAVE Version: 3.1.10 
 */

#include <stdint.h>

uint8_t checkTSE();         //check the uC ID to ensure TSE is supported
void initTSE();             //enable the use of TSE
void initTSEinterrupt();    //activate low/high temperature triggered interrupts
int getTemperatureC();      //read the internal temperature converted from k into °Celsius
void checkTSEinterruptOccurence(int tempCelsius); //check if an high/low temp. interrupt occured
