#pragma once


#include "chip.h"


void uartPrintf(LPC_USART_T *pUART, const char *str);

#define debugPrintf(b) uartPrintf(LPC_USART0, b)
#define wifiPrintf(b) uartPrintf(LPC_USART1, b)

#define ASSERT(x) if((x) == 0){debugPrintf("ASSERTION!!\r\n"); while(1);}

#define MEMCMPx(a, b) memcmp(a, b, strlen(b)-1)

void vUartTask ();

typedef enum {
	ready,
	wifi_discon,
	wifi_conn,
	wifi_gotip,
	IPD,
	CIFSR_APIP,
	CIFSR_APMAC,
	CIFSR_STAIP,
	CIFSR_STAMAC,
	CMD_OK,
	CMD_CONNECT,
	CMD_CLOSED,
	TEXT,
	SEND_OK,
	SEND_READY,
	busy_s,
	ready_to_send,
	recv_bytes_report,
	UNKNWON
} TCmdType;

void parseCommand(TCmdType &cmdType, char *wifiMsg);
void processMsg(TCmdType cmdType, uint16_t wifiMsgLen);


static __INLINE uint32_t SysTick_Config(uint32_t ticks)
{
  if (ticks > SYSTICK_MAXCOUNT)  return (1);                                             /* Reload value impossible */

  SysTick->LOAD  =  (ticks & SYSTICK_MAXCOUNT) - 1;                                      /* set reload register */
  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);                            /* set Priority for Cortex-M0 System Interrupts */
  SysTick->VAL   =  (0x00);                                                              /* Load the SysTick Counter Value */
  SysTick->CTRL = (1 << SYSTICK_CLKSOURCE) | (1<<SYSTICK_ENABLE) | (1<<SYSTICK_TICKINT); /* Enable SysTick IRQ and SysTick Timer */
  return (0);                                                                            /* Function successful */
}

