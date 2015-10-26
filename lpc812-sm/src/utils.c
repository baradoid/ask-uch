
#include "chip.h"

#include <string.h>
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>


#include "uartUtils.h"

#include "main.h"
#include <cr_section_macros.h>

const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

#define RINGBUF_FULL 0
#define RINGBUF_FULL 0


/* Sets up system hardware */
void prvSetupHardware(void)
{
	/* Turn on the IRC by clearing the power down bit */
	Chip_SYSCTL_PowerUp(SYSCTL_SLPWAKE_IRC_PD);
	Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_IRC);

	/* Select the PLL input in the IRC */
	Chip_Clock_SetSystemPLLSource(SYSCTL_PLLCLKSRC_IRC);

	/* Setup FLASH access to 2 clocks (up to 30MHz) */
	Chip_FMC_SetFLASHAccess(FLASHTIM_30MHZ_CPU);
	/* Power down PLL to change the PLL divider ratio */
	Chip_SYSCTL_PowerDown(SYSCTL_SLPWAKE_SYSPLL_PD);

	/* Configure the PLL M and P dividers */
	/* Setup PLL for main oscillator rate (FCLKIN = 12MHz) * 2 = 24MHz
	   MSEL = 1 (this is pre-decremented), PSEL = 2 (for P = 4)
	   FCLKOUT = FCLKIN * (MSEL + 1) = 12MHz * 2 = 24MHz
	   FCCO = FCLKOUT * 2 * P = 24MHz * 2 * 4 = 192MHz (within FCCO range) */
	Chip_Clock_SetupSystemPLL(4, 1);

	/* Turn on the PLL by clearing the power down bit */
	Chip_SYSCTL_PowerUp(SYSCTL_SLPWAKE_SYSPLL_PD);

	/* Wait for PLL to lock */
	while (!Chip_Clock_IsSystemPLLLocked()) {}

	/* Set system clock divider to 1 */
	Chip_Clock_SetSysClockDiv(1);

	/* Set main clock source to the system PLL. This will drive 24MHz
	   for the main clock and 24MHz for the system clock */
	Chip_Clock_SetMainClockSource(SYSCTL_MAINCLKSRC_PLLOUT);

	SystemCoreClockUpdate();

	/* Initialize GPIO */
	Chip_GPIO_Init(LPC_GPIO_PORT);

	/* Initialize the LEDs */
	//Board_LED_Init();
	//int i;

//	for (i = 0; i < 2; i++) {
//		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, ledBits[i]);
//		Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, ledBits[i], true);
//	}

	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 13);
	//Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 13, true);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 17);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 17, true);


	/* Enable the clock to the Switch Matrix */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

	/* Divided by 1 */
	Chip_Clock_SetUARTClockDiv(1);

	Chip_UART_Init(LPC_USART0);
	Chip_UART_ConfigData(LPC_USART0, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
	Chip_Clock_SetUSARTNBaseClockRate((115200 * 16), true);
	Chip_UART_SetBaud(LPC_USART0, 115200);
	Chip_UART_Enable(LPC_USART0);
	Chip_UART_TXEnable(LPC_USART0);

	/* Connect the U1_TXD_O and U1_RXD_I signals to port pins(P0.12, P0.13) */
	Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I1);
	Chip_SWM_DisableFixedPin(SWM_FIXED_SWDIO);

	Chip_SWM_MovablePinAssign(SWM_U0_TXD_O, 11);
	Chip_SWM_MovablePinAssign(SWM_U0_RXD_I, 2);

	Chip_UART_Init(LPC_USART1);
	Chip_UART_ConfigData(LPC_USART1, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1 | UART_CFG_CTSEN);
	Chip_Clock_SetUSARTNBaseClockRate((115200 * 16), true);
	Chip_UART_SetBaud(LPC_USART1, 115200);
	Chip_UART_Enable(LPC_USART1);
	Chip_UART_TXEnable(LPC_USART1);

	/* Connect the U1_TXD_O and U1_RXD_I signals to port pins(P0.12, P0.13) */
	Chip_SWM_MovablePinAssign(SWM_U1_TXD_O, 12);
	Chip_SWM_MovablePinAssign(SWM_U1_RXD_I, 13);

	Chip_SWM_MovablePinAssign(SWM_U1_RTS_O, 7);

	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_USART1, UART_INTEN_RXRDY);
	/* preemption = 1, sub-priority = 1 */
	NVIC_EnableIRQ(UART1_IRQn);

	/* preemption = 1, sub-priority = 1 */
	NVIC_EnableIRQ(SysTick_IRQn);

	debugPrintf("starting u1\r\n");
	//char tmptmp2[50] = "starting u2\n";
	//Chip_UART_SendBlocking(LPC_USART1, tmptmp2, strlen(tmptmp2));

}

uint8_t curUartWriteBuf = 0;
//extern "C" void UART1_IRQHandler( void )
//{
//	//debugPrintf("n");
//	//uint32_t ulStatusRegister;
//	static uint16_t uartRecvBufIdx = 0;
//
//	char ch;
//	while ((Chip_UART_GetStatus(LPC_USART1) & UART_STAT_RXRDY) != 0) {
//			ch = Chip_UART_ReadByte(LPC_USART1);
//			//Chip_UART_SendByte(LPC_USART0, ch);
//			if(uartRecvBufIdx < UART1_RECV_BUF_LENGTH)
//				uartRecvBuf[curUartWriteBuf][uartRecvBufIdx++] = ch;
//			if(ch == '\n'){
//				uartRecvBuf[curUartWriteBuf][uartRecvBufIdx] = 0;
//				uartRecvBufIdx = 0;
////
////				xQueueSendFromISR( xRxQueue, &curUartWriteBuf, &xHigherPriorityTaskWoken );
////
//				curUartWriteBuf++;
//				if(curUartWriteBuf>(UART1_RECV_BUF_NUM-1))
//					curUartWriteBuf = 0;
////
////			    /* Unblock the handling task so the task can perform any processing necessitated
////			    by the interrupt.  xHandlingTask is the task's handle, which was obtained
////			    when the task was created.  The handling task's notification value
////			    is bitwise ORed with the interrupt status - ensuring bits that are already
////			    set are not overwritten. */
////			    //xTaskNotifyFromISR( xTaskToNotify,
////			    //					curUartWriteBuf,
////				//					eSetValueWithOverwrite,
////			    //                    &xHigherPriorityTaskWoken );
////
////
////
////			    /* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE.
////			    The macro used to do this is dependent on the port and may be called
////			    portEND_SWITCHING_ISR. */
////			    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
//			}
//	}
//}

__RAM_FUNC void delayMs(uint16_t msec)
{
	extern volatile uint64_t SysTickCnt;
	uint64_t startTicks = SysTickCnt;
	while((SysTickCnt - startTicks) < msec) ;
}

typedef enum {
	idle,
	init,
	cipserver,
	checkIp,
	checkForNetworks,
	checkForNetworksList,
	checkTO,
	waitForCmd,
	processMsgRecv,
	waitForCipSendOk,
	waitForSendOk,
	waitForCloseOk,
	sendHtml404,
	htmlSendSimple,
	htmlSendTest0,
	htmlSend0,
	htmlSend1,
	htmlSend2,
	htmlSend3,
	htmlSend4,
	htmlSend5,
	htmlClose
} TProcState;


#include <string.h>
#include "esp8266.h"
void setwifiBaudRate(uint32_t baudrate)
{
	char numToStr[10], reqStr[] = "AT+UART_CUR=921600,8,1,0,3\r\n";
	itoa(baudrate, numToStr, 10);

	memcpy(&(reqStr[12]), numToStr, strlen(numToStr));

	debugPrintf(reqStr);
	wifiPrintf(reqStr);
	waitForRespOK();

	Chip_Clock_SetUSARTNBaseClockRate((921600 * 16), true);
	Chip_UART_SetBaud(LPC_USART0, baudrate);
	Chip_UART_SetBaud(LPC_USART1, baudrate);
}


//uint8_t curConnInd = 0;

//void waitForRespText()
//{
//	while(1){
//		TCmdType cmd = blockWaitCmd();
//		if( cmd == TEXT){
//			break;
//		}
//	}
//}


//void commonInit()
//{
//	wifiPrintf("AT+CIPMUX=1\r\n");
//	waitForRespOK();
//
//	wifiPrintf("AT+CIPSERVER=1,80\r\n");
//	waitForRespOK();
//
//	wifiPrintf("AT+CIFSR\r\n");
//	waitForRespOK();
//
//	wifiPrintf("AT+CWMODE_CUR=3\r\n");
//	waitForRespOKorRespError();
//
//	wifiPrintf("AT+CWLAP\r\n");
//	waitForRespOKorRespError();
//
//	debugPrintf("Wait AT+CWLAP resp OK! \r\n");
//
//
//
////	char numToStr[10];
////	for(uint8_t i=0; i<WIFI_APLISTMAX; i++){
////		if(wifiApList[i].rssi != 0){
////			debugPrintf(&(wifiApList[i].name[0]));
////			debugPrintf("   ==>  ");
////
////			itoa(wifiApList[i].rssi, numToStr, 10);
////			debugPrintf(numToStr);
////
////			debugPrintf("\r\n");
////		}
////	}
//}



//void processMsg()
//{
//	TCmd cmd;
//	TCmdType cmdType = UNKNWON; // getNextWifiCmd(true);
//	static TProcState eState = init;
//
//	char numToStr[10];
//
//	//char *curStr;
//	//uint8_t curStrLen;
//	//curStr = tempRecvBuf; //&(uartRecvBuf[curInd][0]);
//	//curStrLen = strlen(curStr);
//	//Chip_UART_SendBlocking(LPC_USART0, curStr, curStrLen);
//
////	if(cmdType == IPD)
////		eState = processMsgRecv;
//	if(cmdType == CMD_CLOSED){
//		//debugPrintf("!!! detected connection closed !!!\r\n");
//		eState = waitForCmd;
//	}
//	/////
//	switch(eState){
//		case init:
////			switch(cmdType){
////				case ready:
////					wifiPrintf("ATE0\r\n");
////					waitForRespOK();
////					wifiPrintf("AT+UART_DEF?\r\n");
////					waitForRespOK();
////					wifiPrintf("AT+GMR\r\n");
////					waitForRespOK();
////
////					commonInit();
////
////					debugPrintf(" trying to connect\r\n");
////
////					wifiPrintf("AT+CWQAP\r\n");
////					waitForRespOKorRespError();
////
////					debugPrintf("AT+CWJAP_CUR=\"TL-WR842ND\",\"kkkknnnn\"\r\n");
////					wifiPrintf("AT+CWJAP_CUR=\"TL-WR842ND\",\"kkkknnnn\"\r\n");
////					waitForRespOKorRespError();
////
////					wifiPrintf("AT+CIFSR\r\n");
////					waitForRespOK();
////
////					eState = waitForCmd;
////
////					break;
////				case wifi_discon:
////					commonInit();
////					eState = waitForCmd;
////					break;
////				case wifi_conn:
////					break;
////				case wifi_gotip:
////					commonInit();
////					eState = waitForCmd;
////					break;
////				default:
////					break;
////			}
//			break;
//
//		case waitForCmd:
//			if(cmdType == IPD){
//				//eState = processMsgRecv;
//				//debugPrintf(" processMsgRecv IPD type\r\n");
//			}
//			break;
//		case processMsgRecv:
//			if(cmdType == TEXT){
//				debugPrintf(" processMsgRecv TEXT type\r\n");
//				if(msgLength > 0){
//					//msgLength -= wifiMsgLen;
//
//					if(msgLength == 0){
//						debugPrintf(" message recvd! \r\n");
//
//
//						/*wifiPrintf("AT+CIPSEND=");
//						debugPrintf("AT+CIPSEND=");
//						itoa(curConnInd, numToStr, 10);
//						wifiPrintf(numToStr);
//						debugPrintf(numToStr);
//						wifiPrintf(",");
//						debugPrintf(",");
//						itoa(strlen(&(htmlPart1[0])), numToStr, 10);
//						wifiPrintf(numToStr);
//						debugPrintf(numToStr);
//						wifiPrintf("\r\n");
//						debugPrintf("\r\n");*/
//
//						//if(0){
//							if(htmlReqType == GET_ROOT){
//								/*debugPrintf("send ROOT\r\n");
//								wifiPrintf("AT+CIPSEND=");
//								debugPrintf("AT+CIPSEND=");
//								itoa(curConnInd, numToStr, 10);
//								wifiPrintf(numToStr);
//								debugPrintf(numToStr);
//								wifiPrintf(",");
//								debugPrintf(",");
//								itoa(strlen(&(htmlPart1[0])), numToStr, 10);
//								wifiPrintf(numToStr);
//								debugPrintf(numToStr);
//								wifiPrintf("\r\n");
//								debugPrintf("\r\n");
//
//								blockWaitForReadyToSend();
//
//								wifiPrintf(&(htmlPart1[0]));
//
//								blockWaitSendOK();
//
//								debugPrintf("!!! htmlSend0 SEND_OK detected!!!\r\n");
//								prepareHtmlData();
//								wifiPrintf("AT+CIPSEND=");
//								itoa(curConnInd, numToStr, 10);
//								wifiPrintf(numToStr);
//								wifiPrintf(",");
//								itoa(strlen(&(htmlBody[0])), numToStr, 10);
//								wifiPrintf(numToStr);
//								wifiPrintf("\r\n");
//
//								blockWaitForReadyToSend();
//
//								wifiPrintf(&(htmlBody[0]));
//
//								blockWaitSendOK();
//
//								debugPrintf("!!! htmlBody SEND_OK detected!!!\r\n");
//								wifiPrintf("AT+CIPSEND=");
//								itoa(curConnInd, numToStr, 10);
//								wifiPrintf(numToStr);
//								wifiPrintf(",");
//								itoa(strlen(&(htmlPart2[0])), numToStr, 10);
//								wifiPrintf(numToStr);
//								wifiPrintf("\r\n");
//
//								blockWaitForReadyToSend();
//
//								wifiPrintf(&(htmlPart2[0]));
//
//								blockWaitSendOK();
//
//								debugPrintf("!!! htmlClose SEND_OK detected!!!\r\n");
//
//								sendCipClose(curConnInd);*/
//
//								/*wifiPrintf("AT+CIPSEND=");
//								debugPrintf("AT+CIPSEND=");
//								itoa(curConnInd, numToStr, 10);
//								wifiPrintf(numToStr);
//								debugPrintf(numToStr);
//								wifiPrintf(",");
//								debugPrintf(",");
//								itoa(strlen(&(htmlSimple[0])), numToStr, 10);
//								wifiPrintf(numToStr);
//								debugPrintf(numToStr);
//								wifiPrintf("\r\n");
//								debugPrintf("\r\n");
//								blockWaitForReadyToSend();
//								debugPrintf("ready_to_send \r\n");
//								wifiPrintf(&(htmlSimple[0]));*/
//
//								//wifiPrintf("AT+CIPSEND=?\r\n");
//								//debugPrintf("AT+CIPSEND=?\r\n");
//
//								/*debugPrintf("send simple html\r\n");
//								wifiPrintf("AT+CIPSENDEX=");
//								debugPrintf("AT+CIPSENDEX=");
//								itoa(curConnInd, numToStr, 10);
//								wifiPrintf(numToStr);
//								debugPrintf(numToStr);
//								wifiPrintf(",2048\r\n");
//								debugPrintf(",2048\r\n");*/
//								eState = waitForCmd;
//							}
//							else {
//								debugPrintf("not ROOT. send 404\r\n");
////								wifiPrintf("AT+CIPCLOSE=");
////								itoa(curConnInd, numToStr, 10);
////								wifiPrintf(numToStr);
////								wifiPrintf("\r\n");
////								eState = waitForSendOk;
//
//								wifiPrintf("AT+CIPSEND=");
//								debugPrintf("AT+CIPSEND=");
//								itoa(curConnInd, numToStr, 10);
//								wifiPrintf(numToStr);
//								debugPrintf(numToStr);
//								wifiPrintf(",");
//								debugPrintf(",");
//								itoa(strlen(&(html404[0])), numToStr, 10);
//								wifiPrintf(numToStr);
//								debugPrintf(numToStr);
//								wifiPrintf("\r\n");
//								debugPrintf("\r\n");
//
//								while(1){
//									TCmdType cmd = blockWaitCmd();
//									if( cmd == ready_to_send){
//										break;
//									}
//
//									if( cmd == CMD_CLOSED){
//										debugPrintf("detect CLOSED \r\n");
//										eState = waitForCmd;
//										return;
//									}
//								}
//								debugPrintf("!!! htmlSend0 ready_to_send detected!!!\r\n");
//								wifiPrintf(&(html404[0]));
//								eState = waitForCmd;
//							}
//						//}
//					}
//					else if(msgLength < 0){
//						debugPrintf("ERROR RECV MSG!\r\n");
//						eState = htmlSend0;
//					}
//					else{
//						debugPrintf(" => ");
//						//itoa(wifiMsgLen, numToStr, 10);
//						//debugPrintf(numToStr);
//						debugPrintf(" rn ");
//						itoa(msgLength, numToStr, 10);
//						debugPrintf(numToStr);
//						debugPrintf(" chs\r\n");
//					}
//				}
//			}
//			else{
//				debugPrintf(" not TEXT type\r\n");
//			}
//			break;
//
//		case sendHtml404:
//			if(cmdType == ready_to_send){
////				debugPrintf("!!! htmlSend0 ready_to_send detected!!!\r\n");
////				wifiPrintf(&(html404[0]));
//			}
//			else if(cmdType == RESP_SEND_OK){
////				debugPrintf("!!!! sendHtml404 SEND_OK\r\n");
//				eState = waitForCmd;
//			}
//			break;
//
//		case htmlSendSimple:
//			if(cmdType == ready_to_send){
////				debugPrintf("!!! htmlSend0 ready_to_send detected!!!\r\n");
////				wifiPrintf(&(htmlSimple[0]));
//				eState = waitForSendOk;
//			}
//			break;
//		case htmlSendTest0:
//			if(cmdType == CMD_OK){
////				debugPrintf("!!! htmlSendTest0 CMD_OK detected!!!\r\n");
////				wifiPrintf("AT+CIPSEND=");
////				debugPrintf("AT+CIPSEND=");
////				itoa(curConnInd, numToStr, 10);
////				wifiPrintf(numToStr);
////				debugPrintf(numToStr);
////				wifiPrintf(",");
////				debugPrintf(",");
////				itoa(strlen(&(htmlPart1[0])), numToStr, 10);
////				wifiPrintf(numToStr);
////				debugPrintf(numToStr);
////				wifiPrintf("\r\n");
////				debugPrintf("\r\n");
//				eState = htmlSend0;
//			}
//			else if(cmdType == busy_s){
////				debugPrintf("!!! htmlSendTest0 busy_s detected!!!\r\n");
////				wifiPrintf("AT+CIPCLOSE=");
////				itoa(curConnInd, numToStr, 10);
////				wifiPrintf(numToStr);
////				wifiPrintf("\r\n");
//				eState = waitForSendOk;
//				//wifiPrintf("AT+CIPSEND=?\r\n");
//				//debugPrintf("AT+CIPSEND=?\r\n");
//			}
//			else if(cmdType == RESP_SEND_OK){
//				debugPrintf("!!! htmlSendTest0 SEND_OK detected!!!\r\n");
//			}
//			break;
//
//		case htmlSend0:
//			if(cmdType == CMD_OK){
//				debugPrintf("!!! htmlSend0 OK detected!!!\r\n");
//				//wifiPrintf(&(htmlPart1[0]));
//
//			}
//			else if(cmdType == ready_to_send){
//				debugPrintf("!!! htmlSend0 ready_to_send detected!!!\r\n");
//				wifiPrintf(&(htmlPart1[0]));
//			}
//			else if(cmdType == busy_s){
//				debugPrintf("!!! busy detected!!!\r\n");
//			}
//			else if(cmdType == RESP_SEND_OK){
//				debugPrintf("!!! htmlSend0 SEND_OK detected!!!\r\n");
//				eState = htmlSend1;
//			}
//			break;
//
//		case htmlSend1:
//			if(cmdType == CMD_OK){
//				debugPrintf("!!! htmlSend1 CMD_OK detected!!!\r\n");
//			}
//			else if(cmdType == ready_to_send){
//				debugPrintf("!!! htmlSend1 ready_to_send detected!!!\r\n");
//				//wifiPrintf(&(htmlBody[0]));
//			}
//			else if(cmdType == RESP_SEND_OK){
//				debugPrintf("!!! htmlSend1 SEND_OK detected!!!\r\n");
//				eState = htmlSend2;
//			}
//			break;
//		case htmlSend2:
//			if(cmdType == CMD_OK){
//				debugPrintf("!!! htmlSend2 CMD_OK detected!!!\r\n");
//
//			}
//			else if(cmdType == ready_to_send){
//				debugPrintf("!!! htmlSend2 ready_to_send detected!!!\r\n");
//				wifiPrintf(&(htmlPart2[0]));
//				eState = htmlClose;
//			}
//			break;
//		case htmlSend3:
//			break;
//		case htmlClose:
//			if(cmdType == RESP_SEND_OK){
//				debugPrintf("!!! htmlClose SEND_OK detected!!!\r\n");
//				wifiPrintf("AT+CIPCLOSE=");
//				itoa(curConnInd, numToStr, 10);
//				wifiPrintf(numToStr);
//				wifiPrintf("\r\n");
//				eState = waitForCloseOk;
//			}
//			break;
//
//		case waitForCipSendOk: //wait for "AT+CIPSEND=0," OK
//			if(cmdType == CMD_OK){
//				eState = waitForCmd;
//			}
//			break;
//
//		case waitForSendOk: //wait for "AT+CIPCLOSE=0 OK
//			if(cmdType == RESP_SEND_OK){
//				eState = waitForCmd;
//			}
//			break;
//		case waitForCloseOk:
//			if(cmdType == CMD_OK){
//				eState = waitForCmd;
//			}
//			break;
//
//		default:
//			break;
//	}
//
//}


//#define RINGBUF_FULL_BITNUM 0
//#define UARTTMPBUF_FULL_BITNUM 1
//
//extern RINGBUFF_T rxring;
//void sendHtml(char *str)
//{
//	extern uint8_t errword;
//	char ch;
//	uint16_t sendPartLen = 100;
//	uint16_t strSendLen = strlen(str);
//	while(strSendLen>0){
//		if(sendPartLen > strSendLen)
//			sendPartLen = strSendLen;
//
//		Chip_UART_SendBlocking(LPC_USART1, str, sendPartLen);
//
//		strSendLen -= sendPartLen;
//		str += sendPartLen;
//
//    	if(errword){
//    		debugPrintf("err ");
//    		if(errword&(1<<RINGBUF_FULL_BITNUM))
//    			debugPrintf("RINGBUF_FULL!");
//    		if(errword&(1<<UARTTMPBUF_FULL_BITNUM))
//    		    debugPrintf("UARTTMPBUF_FULL!");
//    		debugPrintf("\r\n");
//    		while(1);
//    	}
//
//		while(RingBuffer_Pop(&rxring, &ch))
//			Chip_UART_Send(LPC_USART0, &ch, 1);
//
//		//debugPrintf("HTML ECHO OK 1\r\n");
//	}
//}


//uint32_t SysTick_Config(uint32_t ticks)
//{
// if (ticks > SYSTICK_MAXCOUNT)  return (1);                                             /* Reload value impossible */
//
// SysTick->LOAD  =  (ticks & SYSTICK_MAXCOUNT) - 1;                                      /* set reload register */
// //NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);                            /* set Priority for Cortex-M0 System Interrupts */
// SysTick->VAL   =  (0x00);                                                              /* Load the SysTick Counter Value */
// SysTick->CTRL = (1 << SYSTICK_CLKSOURCE) | (1<<SYSTICK_ENABLE) | (1<<SYSTICK_TICKINT); /* Enable SysTick IRQ and SysTick Timer */
// return (0);                                                                            /* Function successful */
//}

__RAM_FUNC uint8_t parseInt4(char ch) //отладить
{
	uint8_t shift = 0;

	if( (ch>=0x30) && (ch<=0x39) )
		shift = 0x30;
	else if( (ch>=0x41) && (ch<=0x46) )
		shift = 0x37;
	else if( (ch>=0x61) && (ch<=0x66) )
		shift = 0x57;

	return ch - shift;
}

__RAM_FUNC uint8_t parseInt8(char *str) //отладить
{
	return (parseInt4(str[0])<<4)|(parseInt4(str[1]));
}

__RAM_FUNC uint16_t parseInt16(char *str)
{
	return (parseInt8(str)<<8)|parseInt8(str+2);
}
