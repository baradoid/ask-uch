
#include "chip.h"

#include <string.h>
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <cr_section_macros.h>

const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

#define RINGBUF_FULL 0
#define RINGBUF_FULL 0
//#define UART1_RECV_BUF_NUM 5
//#define UART1_RECV_BUF_LENGTH 170
//__NOINIT_DEF char uartRecvBuf[UART1_RECV_BUF_NUM][UART1_RECV_BUF_LENGTH];

#define htmlBodyLen 1000
__NOINIT_DEF char htmlBody[htmlBodyLen];

__RODATA(text) char htmlPart1[] =  "<html> <head><title>AIIS ASKUE</title>"
"<style>\r\n"
		".tabs { min-width: 320px; max-width: 800px; padding: 0px; margin: 0 auto; }\r\n"
		"section { display: none; padding: 15px; background: #fff; border: 1px solid #ddd;}\r\n"
		"#tab1, #tab2, #tab3, #tab4 {display: none;}\r\n"
		".tabs label { display: inline-block; margin: 0 0 -1px; padding: 15px 25px; font-weight: 600; text-align: center; color: #aaa; border: 1px solid #ddd; background: #f1f1f1; border-radius: 3px 3px 0 0; }"
		".tabs label:before { font-family: fontawesome; font-weight: normal; margin-right: 10px;}"
		".tabs label:hover {color: #888;cursor: pointer;}"
		".tabs input:checked + label { color: #555; border: 1px solid #ddd; border-top: 1px solid #009933; border-bottom: 1px solid #fff; background: #fff;}"
		"#tab1:checked ~ #content1, #tab2:checked ~ #content2, #tab3:checked ~ #content3, #tab4:checked ~ #content4 {display: block;}"
		"@media screen and (max-width: 400px) {.tabs label {padding: 15px;}}\r\n"
"</style>\r\n\r\n"
		"</head>"
		" <body>"
		"<div class=\"tabs\">"
		    "<input id=\"tab1\" type=\"radio\" name=\"tabs\" checked>"
		    "<label for=\"tab1\" title=\"info\">info</label>"
		    "<input id=\"tab2\" type=\"radio\" name=\"tabs\">"
		    "<label for=\"tab2\" title=\"status\">status</label>"
		    "<input id=\"tab3\" type=\"radio\" name=\"tabs\">"
		    "<label for=\"tab3\" title=\"settings\">settings</label>"
		    "<section id=\"content1\"><p>content1</p></section>"
		    "<section id=\"content2\"><p>content2</p></section>"
		    "<section id=\"content3\"><p>content3</p>"
		;
__RODATA(text) char htmlPart2[] =
//	"<form method=\"get\">"
//		"<fieldset>"
//			"<select name=\"securType\" size = 1> "
//			"<option value=\"val1\">valText1</option>"
//			"<option value=\"val2\">valText2</option>"
//			"<option value=\"val3\">valText3</option>"
//			"</select><br>"
//			"<input name=\"mycolor\" type=\"radio\" value=\"white\"> white <br>"
//			"<input name=\"mycolor\" type=\"radio\" value=\"green\"> green<br>"
//			"<input name=\"mycolor\" type=\"radio\" value=\"blue\"> blue<br>"
//			"<input type=\"submit\" value=\"but text\">"
//		"</fieldset>"
//	"</form>"
	"</body>"
	"</html>\r\n";


typedef struct {
	uint8_t secureType;
	int8_t	rssi;
	char name[40];
} TWifiAp;

#define WIFI_APLISTMAX 15
TWifiAp wifiApList[WIFI_APLISTMAX];

char APIP[20], STAPIP[20];

void uartPrintf(LPC_USART_T *pUART, const char *str)
{
	Chip_UART_SendBlocking(pUART, str, strlen(str));
}

void uartSimplePrintf(const char *str)
{
	Chip_UART_SendBlocking(LPC_USART0, str, strlen(str));

}


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
	Chip_UART_ConfigData(LPC_USART1, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
	Chip_Clock_SetUSARTNBaseClockRate((115200 * 16), true);
	Chip_UART_SetBaud(LPC_USART1, 115200);
	Chip_UART_Enable(LPC_USART1);
	Chip_UART_TXEnable(LPC_USART1);

	/* Connect the U1_TXD_O and U1_RXD_I signals to port pins(P0.12, P0.13) */
	Chip_SWM_MovablePinAssign(SWM_U1_TXD_O, 12);
	Chip_SWM_MovablePinAssign(SWM_U1_RXD_I, 13);

	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_USART1, UART_INTEN_RXRDY);
	/* preemption = 1, sub-priority = 1 */
	NVIC_EnableIRQ(UART1_IRQn);

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




#define addToHtml(a) strcat (htmlBody, a)
uint32_t prepareHtmlData()
{
	uint8_t i;
	char numToStr[10];
	strcpy(htmlBody, " ");
	//itoa(xTaskGetTickCount(), numToStr, 10);
	//strcat (htmlBody,numToStr);

	addToHtml(" select WiFi AP ");

	//itoa(xPortGetFreeHeapSize(), numToStr, 10);
	//addToHtml(numToStr);

	addToHtml("<form method=\"post\">");

	for(i=0;i<WIFI_APLISTMAX;i++){
		if(wifiApList[i].rssi != 0){

			addToHtml("<input name=\"wifiAp\" type=\"radio\" value= ");
			addToHtml(wifiApList[i].name);
			addToHtml( "> ");
			addToHtml(wifiApList[i].name);

			addToHtml( " (");
			itoa(wifiApList[i].rssi, numToStr, 10);
			addToHtml(numToStr);
			addToHtml( ")");

			addToHtml("<br>");
			//	"<input name=\"mycolor\" type=\"radio\" value=\"white\"> white <br>"


		}
	}
	addToHtml("<p><input type=\"submit\"></p>");
	addToHtml("</form></section></div>");

//	"<select name=\"securType\" size = 1> "
//	"<option value=\"val1\">valText1</option>"
//	"<option value=\"val2\">valText2</option>"
//	"<option value=\"val3\">valText3</option>"
//	"</select><br>"
//	"<input name=\"mycolor\" type=\"radio\" value=\"white\"> white <br>"
//	"<input name=\"mycolor\" type=\"radio\" value=\"green\"> green<br>"
//	"<input name=\"mycolor\" type=\"radio\" value=\"blue\"> blue<br>"
//	"<input type=\"submit\" value=\"but text\">"

	ASSERT(strlen(&(htmlBody[0])) < htmlBodyLen);


	uint32_t htmlLen = strlen(&(htmlPart1[0]));
	htmlLen += strlen(&(htmlPart2[0]));
	htmlLen += strlen(&(htmlBody[0]));
	return htmlLen;
}

typedef enum {
	idle,
	init,
	cipserver,
	checkIp,
	checkForNetworks,
	checkForNetworksList,
	waitForCmd,
	processMsgRecv,
	waitForCipSendOk,
	waitForSendOk,
	waitForCloseOk,
	htmlSend0,
	htmlSend1,
	htmlSend2,
	htmlSend3,
	htmlSend4,
	htmlSend5,
	htmlClose
} TProcState;


void parseIPD(char *str, uint8_t *curConnInd, int16_t *msgLen)
{
	char *pch;
	char numToStr[10];
	uint8_t i;

	uint16_t tailLen = 0;
	//debugPrintf("parseIPD\r\n");
	pch = strtok (str, ",:");
	for(i=0; pch != NULL; i++){
		switch(i){
		case 1:
			*curConnInd = (uint8_t)atoi(pch);
			break;
		case 2:
			*msgLen = atoi(pch);
			//debugPrintf("msg length:");
			//debugPrintf(pch);
			//debugPrintf("\r\n");

			//debugPrintf(pch);
			//debugPrintf("\r\n");
			break;
		case 3:
			tailLen = strlen(pch);
			//itoa(strlen(pch), tailLen, 10);
			//atoi(pch);

			//debugPrintf(pch);
			//debugPrintf(" => ");
			//debugPrintf(numToStr);
			//debugPrintf("\r\n");

			break;
		}
		pch = strtok (NULL, ",:");
	}
	debugPrintf("conNum ");
	itoa(*curConnInd, numToStr, 10);
	debugPrintf(numToStr);

	debugPrintf(" start recv msg with length ");
	itoa(*msgLen, numToStr, 10);
	debugPrintf(numToStr);
	debugPrintf(" and ");
	itoa(tailLen, numToStr, 10);
	debugPrintf(numToStr);
	debugPrintf(" chars already recvd\r\n");
	*msgLen -= tailLen;
}

TCmdType parseCIFSR(char *str)
{
	//debugPrintf("parseCIFSR\r\n");
	TCmdType cmdType = UNKNWON;
	if(MEMCMPx(str, "+CIFSR:APIP") == 0){
		//debugPrintf("!!! +APIP !!! ");
		strcpy(APIP, &(str[12]));
		cmdType = CIFSR_APIP;
	}
	else if(MEMCMPx(str, "+CIFSR:APMAC") == 0){
		//debugPrintf("!!! +CIFSR:APMAC !!! ");
		cmdType = CIFSR_APMAC;
	}
	else if(MEMCMPx(str, "+CIFSR:STAIP") == 0){
		//debugPrintf("!!! +CIFSR:STAIP !!! ");
		strcpy(STAPIP, &(str[13]));
		cmdType = CIFSR_STAIP;
	}
	else if(MEMCMPx(str, "+CIFSR:STAMAC") == 0){
	//	debugPrintf("!!! +CIFSR:STAMAC !!! ");
		cmdType = CIFSR_STAMAC;
	}
	return cmdType;
}

void parseCWLAP(char *str)
{
	//debugPrintf("parseCWLAP\r\n");
	uint8_t i;
	char *pch;
	static uint8_t wifiApInd = 0;
	pch = strtok (str, ":(\"");
	TWifiAp *carWifiAap = &(wifiApList[wifiApInd]);
	for(i=0; pch != NULL; i++){

		switch(i){
		case 2:
			//pch[strlen(pch)]='\0';
			memcpy(&(carWifiAap->name[0]), pch+1, strlen(pch)-2);
			//strcpy(&(carWifiAap->name[0]), pch+1);
			break;
		case 3:
			carWifiAap->rssi = atoi(pch);
			break;
		default:
			break;
		}
		pch = strtok (NULL, ",()");
	}
	wifiApInd++;
}

uint8_t curConnInd = 0;
int16_t msgLength = 0;

TCmdType parseCommand(char *wifiMsg)
{
	TCmdType cmdType = UNKNWON;
	if(wifiMsg[0] == '+'){
		if(MEMCMPx(wifiMsg, "+IPD") == 0){
			//debugPrintf("!!! +IPD !!! ");
			parseIPD(wifiMsg, &curConnInd, &msgLength);
			cmdType = IPD;
		}
		else if(MEMCMPx(wifiMsg, "+CIFSR") == 0){
			cmdType = parseCIFSR(wifiMsg);
		}
		else if(MEMCMPx(wifiMsg, "+CWLAP") == 0){
			parseCWLAP(wifiMsg);
		}
	}
	else if(strlen(wifiMsg)==4){
		if(strcmp(wifiMsg, "OK\r\n") == 0)
			cmdType = CMD_OK;
	}
	else if(strcmp(wifiMsg+1, ",CONNECT\r\n") == 0){
		//debugPrintf("!!!CONNECT!!!\r\n");
		cmdType = CMD_CONNECT;

	}
	else if(strcmp(wifiMsg+1, ",CLOSED\r\n") == 0){
		//debugPrintf("!!!CLOSED!!!\r\n");
		cmdType = CMD_CLOSED;
	}
	else if(strcmp(wifiMsg, "SEND OK\r\n") == 0)
		cmdType = SEND_OK;
	else if(strcmp(wifiMsg, "> \r\n") == 0){
		debugPrintf("!!!SEND_READY!!!\r\n");
		cmdType = SEND_READY;
	}
	else if(strcmp(wifiMsg, "WIFI GOT IP\r\n") == 0)
		cmdType = wifi_gotip;
	else if(strcmp(wifiMsg, "WIFI DISCONNECT\r\n") == 0)
		cmdType = wifi_discon;
	else if(strcmp(wifiMsg, "ready\r\n") == 0)
		cmdType = ready;
	else{
		cmdType = TEXT;
	}
	return cmdType;
}

void processMsg(TCmdType cmdType, char *wifiMsg, uint16_t wifiMsgLen)
{
	static TProcState eState = init;

	char numToStr[10];

	//char *curStr;
	//uint8_t curStrLen;
	//curStr = tempRecvBuf; //&(uartRecvBuf[curInd][0]);
	//curStrLen = strlen(curStr);
	//Chip_UART_SendBlocking(LPC_USART0, curStr, curStrLen);

	if(cmdType == IPD)
		eState = processMsgRecv;
	/////
	switch(eState){
		case init:
			switch(cmdType){
				case ready:
					//debugPrintf(" READY!!!!\r\n");
					wifiPrintf("ATE0\r\n");
					break;
				case wifi_discon:
					break;
				case wifi_conn:
					break;
				case wifi_gotip:
					break;
				default:
					break;
			}
			if(cmdType == CMD_OK){
				wifiPrintf("AT+CIPMUX=1\r\n");
				eState = cipserver;
			}
			break;
		case cipserver:
			switch(cmdType){
				case CMD_OK:
					wifiPrintf("AT+CIPSERVER=1,80\r\n");
					eState = checkIp;
					break;
				default:
					break;
			}
			break;
		case checkIp:
			switch(cmdType){
				case CMD_OK:
					wifiPrintf("AT+CIFSR\r\n");
					break;
				case CIFSR_STAMAC:
					//debugPrintf(APIP);
					//debugPrintf(STAPIP);
					eState = checkForNetworks;
					break;
				default:
					break;
			}
			break;
		case checkForNetworks:
			switch(cmdType){
				case CMD_OK:
					wifiPrintf("AT+CWLAP\r\n");

					eState = checkForNetworksList;
					break;
				default:
					break;
			}
			break;
		case checkForNetworksList:
			if(cmdType == CMD_OK){
				eState = waitForCmd;
//					uint8_t i;
//					for(i=0; i<WIFI_APLISTMAX; i++){
//						if(wifiApList[i].rssi != 0){
//							debugPrintf(&(wifiApList[i].name[0]));
//							debugPrintf("   ==>  ");
//
//							itoa(wifiApList[i].rssi, numToStr, 10);
//							debugPrintf(numToStr);
//
//							debugPrintf("\r\n");
//						}
//					}
			}
			break;
		case waitForCmd:
			if(cmdType == IPD)
				eState = processMsgRecv;
			break;
		case processMsgRecv:
			if(cmdType == TEXT){
				msgLength -= wifiMsgLen;

				if(msgLength == 0){
					debugPrintf(" message recvd! \r\n");

					wifiPrintf("AT+CIPSEND=");
					//debugPrintf("AT+CIPSEND=");
					itoa(curConnInd, numToStr, 10);
					wifiPrintf(numToStr);
					//debugPrintf(numToStr);
					wifiPrintf(",");
					//debugPrintf(",");
					itoa(strlen(&(htmlPart1[0])), numToStr, 10);
					wifiPrintf(numToStr);
					//debugPrintf(numToStr);
					wifiPrintf("\r\n");
					//debugPrintf("\r\n");

					eState = htmlSend0;
				}
				else if(msgLength < 0){
					debugPrintf("ERROR RECV MSG!\r\n");
					eState = htmlSend0;
				}
				else{
					debugPrintf(" => ");
					itoa(wifiMsgLen, numToStr, 10);
					debugPrintf(numToStr);
					debugPrintf(" rn ");
					itoa(msgLength, numToStr, 10);
					debugPrintf(numToStr);
					debugPrintf(" chs\r\n");
				}
			}
			else{
				debugPrintf(" not TEXT type\r\n");
			}
			break;
		case htmlSend0:
			if(cmdType == CMD_OK){
				//wifiPrintf(&(htmlPart1[0]));
				wifiPrintf(&(htmlPart1[0]));
			}
			else if(cmdType == SEND_OK){
				prepareHtmlData();
				wifiPrintf("AT+CIPSEND=");
				itoa(curConnInd, numToStr, 10);
				wifiPrintf(numToStr);
				wifiPrintf(",");
				itoa(strlen(&(htmlBody[0])), numToStr, 10);
				wifiPrintf(numToStr);
				wifiPrintf("\r\n");
				eState = htmlSend1;
			}
			break;

		case htmlSend1:
			if(cmdType == CMD_OK){
				wifiPrintf(&(htmlBody[0]));
			}
			else if(cmdType == SEND_OK){
				wifiPrintf("AT+CIPSEND=");
				itoa(curConnInd, numToStr, 10);
				wifiPrintf(numToStr);
				wifiPrintf(",");
				itoa(strlen(&(htmlPart2[0])), numToStr, 10);
				wifiPrintf(numToStr);
				wifiPrintf("\r\n");
				eState = htmlSend2;
			}
			break;
		case htmlSend2:
			if(cmdType == CMD_OK){
				wifiPrintf(&(htmlPart2[0]));
				eState = htmlClose;
			}
			break;
		case htmlSend3:
			break;
		case htmlClose:
			if(cmdType == SEND_OK){
				wifiPrintf("AT+CIPCLOSE=");
				itoa(curConnInd, numToStr, 10);
				wifiPrintf(numToStr);
				wifiPrintf("\r\n");
				eState = waitForSendOk;
			}
			break;

		case waitForCipSendOk: //wait for "AT+CIPSEND=0," OK
			if(cmdType == CMD_OK){
				eState = waitForCmd;
			}
			break;

		case waitForSendOk: //wait for "AT+CIPCLOSE=0 OK
			if(cmdType == CMD_OK){
				eState = waitForCmd;
			}
			break;
		case waitForCloseOk:
			if(cmdType == CMD_OK){
				eState = waitForCmd;
			}
			break;

		default:
			break;
	}

}


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

