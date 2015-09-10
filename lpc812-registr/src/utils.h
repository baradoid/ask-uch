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
	CIPSTO,
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


