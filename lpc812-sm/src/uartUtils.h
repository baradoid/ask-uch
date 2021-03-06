#pragma once
#include "chip.h"

void uartPrintf(LPC_USART_T *pUART, const char *str);


void Chip_UART_SendBlockingString(LPC_USART_T *pUART, const char *data);

#define debugPrintf(b) Chip_UART_SendBlockingString(LPC_USART0, b)
#define debugPrintflen(b, l) Chip_UART_SendBlocking(LPC_USART0, b, l)
#define wifiPrintf(b) Chip_UART_SendBlockingString(LPC_USART1, b)

bool isWiFiMsgRecvd();
void waitWiFiMsg();
int16_t waitWiFiMsgTO(int16_t to_msec);
void enableWiFiMsg();
uint16_t recvWifiMsg(char *rcvbf);
int16_t recvWifiMsgTO(char *rcvbf, int16_t to_msec);
uint16_t recvWifiMsgBlocking(char *rcvbf, uint16_t bufLen);

int16_t waitWifiMsgAndStartRecv();
int16_t waitWifiMsgAndStartRecvTO(int16_t to_msec);

//void readUUU(const char *strToWa,  int16_t to_msec = -1);
//void readLLL(const char *strToWa,  int16_t to_msec = -1);


#define BUF_LEN  400
extern char uart1Buffer[BUF_LEN];


inline uint16_t getUartIrqMsgLength();

