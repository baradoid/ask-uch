#pragma once


#include "chip.h"


void uartPrintf(LPC_USART_T *pUART, const char *str);

#define debugPrintf(b) uartPrintf(LPC_USART0, b)
#define wifiPrintf(b) uartPrintf(LPC_USART1, b)

#define ASSERT(x) if((x) == 0){debugPrintf("ASSERTION!!\r\n"); while(1);}

#define MEMCMPx(a, b) memcmp(a, b, strlen(b)-1)

void vUartTask ();




