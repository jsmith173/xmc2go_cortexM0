/*
 * uartUtH.h
 *
 *  Created on: 28.02.2015
 *      Author: blinch89
 * Description: This module is based on INFINEONs example Apps for the xmc2go kit
                and the source code is almost unchanged.
                It enables the use of UART communication between xmc2go and PC
                via USB (virtual COM port).
       version: 1.0
                - no receive functions implemented, only send 
   tested with: DAVE Version: 3.1.10
 */

#include <stdint.h>

// UART baud rate constants for 115.2kbps @ MCLK=8MHz
#define FDR_STEP 590UL
#define BRG_PDIV 3UL
#define BRG_DCTQ 9UL
#define BRG_PCTQ 0UL


void UART_Init();
void uartWriteLine(char * str, uint8_t len);
