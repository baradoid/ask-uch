#include <string.h>
#include "chip.h"
#include "uartUtils.h"

extern uint64_t SysTickCnt;
//#define DEBUGPRINTF

bool isWiFiMsgRecvd()
{
	return (Chip_UART_GetIntsEnabled(LPC_USART1)&UART_INTEN_RXRDY) == 0;
}

void waitWiFiMsg()
{
	while((Chip_UART_GetIntsEnabled(LPC_USART1)&UART_INTEN_RXRDY) != 0) ;
}

int16_t waitWiFiMsgTO(int16_t to_msec)
{
	int16_t ret = 0;
	uint64_t startTicks = SysTickCnt;
	while((Chip_UART_GetIntsEnabled(LPC_USART1)&UART_INTEN_RXRDY) != 0) {
		if((SysTickCnt - startTicks) > (uint64_t)to_msec){
			ret = -1;
			break;
		}
	}
	return ret;
}

uint16_t recvWifiMsg(char *rcvbf)
{
	waitWiFiMsg();
	uint16_t msgLen = getUartIrqMsgLength();
	memcpy(rcvbf, uart1Buffer, BUF_LEN);
	enableWiFiMsg();
	return msgLen;
}

int16_t recvWifiMsgTO(char *rcvbf, int16_t to_msec)
{
	int16_t ret = 0;
	uint64_t startTicks = SysTickCnt;
//	//waitWiFiMsg();
	while(1){
		if(isWiFiMsgRecvd() == true){
			ret = getUartIrqMsgLength();
			memcpy(rcvbf, uart1Buffer, BUF_LEN);
			enableWiFiMsg();
			break;
		}
		else if(to_msec != -1){
			if((SysTickCnt - startTicks) > (uint64_t)to_msec){
				ret = -1;
				break;
			}
		}
	}
	return ret;
}

int16_t waitWifiMsgAndStartRecv()
{
	waitWiFiMsg();
	uint16_t msgLen = getUartIrqMsgLength();
#ifdef DEBUGPRINTF
	debugPrintf(uart1Buffer);
#endif
	enableWiFiMsg();
	return msgLen;
}

int16_t waitWifiMsgAndStartRecvTO(int16_t to_msec)
{
	int16_t ret = 0;
	uint64_t startTicks = SysTickCnt;
//	//waitWiFiMsg();
	while(1){
		if(isWiFiMsgRecvd() == true){
			ret = getUartIrqMsgLength();
			debugPrintf(" -> ");
			debugPrintf(uart1Buffer);
			//debugPrintf(uart1Buffer);
			//debugPrintflen(uart1Buffer, ret+);
			enableWiFiMsg();
			break;
		}
		if((SysTickCnt - startTicks) > (uint64_t)to_msec){
			ret = -1;
			break;
		}
	}
	return ret;
}

void enableWiFiMsg()
{
	Chip_UART_IntEnable(LPC_USART1, UART_INTEN_RXRDY);
}

void uartPrintf(LPC_USART_T *pUART, const char *str)
{
	Chip_UART_SendBlocking(pUART, str, strlen(str));
}

void uartSimplePrintf(const char *str)
{
	Chip_UART_SendBlocking(LPC_USART0, str, strlen(str));
}

//void readUUU(const char *strToWa,  int16_t to_msec)
//{
//	uint64_t startTicks = SysTickCnt;
//	while(1){
//		waitWiFiMsg();
//		//debugPrintf(uart1Buffer);
//		if(strcmp(uart1Buffer, strToWa) == 0){
//			break;
//		}
//		enableWiFiMsg();
//
//		if(to_msec != -1){
//			if((SysTickCnt - startTicks) > (uint64_t)to_msec){
//				debugPrintf("timeout!\r\n");
//				//enableWiFiMsg();
//				break;
//			}
//		}
//	}
//	enableWiFiMsg();
//}

//void readLLL(const char *strToWa,  int16_t to_msec)
//{
//	uint64_t startTicks = SysTickCnt;
//	while(1){
//		if(isWiFiMsgRecvd() == true){
//			debugPrintf(" ->");
//			debugPrintf(uart1Buffer);
//			if(strcmp(uart1Buffer, strToWa) == 0){
//				break;
//			}
//			enableWiFiMsg();
//		}
//
//		if(to_msec != -1){
//			if((SysTickCnt - startTicks) > (uint64_t)to_msec){
//				debugPrintf("timeout!\r\n");
//				enableWiFiMsg();
//				return;
//			}
//		}
//	}
//	enableWiFiMsg();
//}


uint16_t uart1MsgLen = 0;
char uart1Buffer[BUF_LEN];

extern "C" void UART1_IRQHandler( void )
{
	static uint16_t recvBufCurInd = 0;

	while ((Chip_UART_GetStatus(LPC_USART1) & UART_STAT_RXRDY) != 0) {
		char ch = Chip_UART_ReadByte(LPC_USART1);
		uart1Buffer[recvBufCurInd++] = ch;
		if( (ch == '\n') || ((uart1Buffer[0] == '>') && (recvBufCurInd==2)) ){
			//debugPrintf("d");
			uart1Buffer[recvBufCurInd] = '\0';
			uart1MsgLen = recvBufCurInd;
			recvBufCurInd = 0;
			Chip_UART_IntDisable(LPC_USART1, UART_INTEN_RXRDY);
			//debugPrintf("<<");
			//debugPrintf(uart1Buffer);
			break;
		}
		if(recvBufCurInd >= BUF_LEN){
			debugPrintf("buffer ovfl!\r\n");
			while(1);
		}
	}

}
