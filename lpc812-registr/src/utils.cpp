
#include "chip.h"

#include <string.h>
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <cr_section_macros.h>

#include "uartUtils.h"

const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

#define RINGBUF_FULL 0
#define RINGBUF_FULL 0
//#define UART1_RECV_BUF_NUM 5
//#define UART1_RECV_BUF_LENGTH 170
//__NOINIT_DEF char uartRecvBuf[UART1_RECV_BUF_NUM][UART1_RECV_BUF_LENGTH];


//__RODATA(text) char htmlSimple[] = "HTTP/1.1 200 OK\r\n"
//		"Date: Wed, 11 Feb 2009 11:20:59 GMT\r\n"
//		"Content-Type: text/html;charset=utf-8\r\n"
//		"Content-Length: 77\r\n"
//		"Connection: close\r\n\r\n"
//		"<html> <head><title>AIIS ASKUE</title></head><body>simple html</body></html>";

__RODATA(text) char html404[] = "HTTP/1.1 404 Not Found\r\n";

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
		    "<label for=\"tab1\" title=\"Status\">Status</label>"
		    "<input id=\"tab2\" type=\"radio\" name=\"tabs\">"
		    "<label for=\"tab2\" title=\"History\">History</label>"
		    "<input id=\"tab3\" type=\"radio\" name=\"tabs\">"
		    "<label for=\"tab3\" title=\"Settings\">Settings</label>"
		    "<section id=\"content1\">"
		    "Counter statistics<table border=\"3\">"
			"<tr align=\"center\"><td>Num</td><td>Value</td><td>today, KWh</td><td>today</br>max per h, KWh</td><td>today</br>min per h, KWh</td><td>yesterday, KWh</td></tr>"
		    "<tr align=\"center\"><td>1</td><td>0000</td><td>0000</td><td>0000</td><td>0000</td><td>0000</td></tr>"
			"<tr align=\"center\"><td>2</td><td>0000</td><td>0000</td><td>0000</td><td>0000</td><td>0000</td></tr>"
			"<tr align=\"center\"><td>3</td><td>0000</td><td>0000</td><td>0000</td><td>0000</td><td>0000</td></tr>"
			"<tr align=\"center\"><td>4</td><td>0000</td><td>0000</td><td>0000</td><td>0000</td><td>0000</td></tr>"
		    "</table></br>"
		    "<hr size=\"2\" color=\" #ddd \" />"
		    "</br>";


__RODATA(text) char htmlPartStartSec2[] =  "</section><section id=\"content2\">";
__RODATA(text) char htmlPartStartSec3[] =  "</section><section id=\"content3\">";


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


TWifiAp wifiApList[WIFI_APLISTMAX];

char APIP[20], STAPIP[20];

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

void delayMs(uint16_t msec)
{
	extern volatile uint64_t SysTickCnt;
	uint64_t startTicks = SysTickCnt;
	while((SysTickCnt - startTicks) < msec) ;
}


void sendCipClose(uint8_t id)
{
	char numToStr[10];
	wifiPrintf("AT+CIPCLOSE=");
	itoa(id, numToStr, 10);
	wifiPrintf(numToStr);
	wifiPrintf("\r\n");
}


#define addToHtml(a) strcat (bufStr, a)

void prepareHtmlData(char *bufStr)
{

	uint8_t i;
	char numToStr[10];
	strcpy(bufStr, " ");
	//itoa(xTaskGetTickCount(), numToStr, 10);
	//strcat (htmlBody,numToStr);

	addToHtml(" select WiFi AP ");

	//itoa(xPortGetFreeHeapSize(), numToStr, 10);
	//addToHtml(numToStr);

	addToHtml("<form method=\"get\">");

	addToHtml("<table>");
	for(i=0;i<WIFI_APLISTMAX;i++){
		if(wifiApList[i].rssi != 0){

			addToHtml("<tr><td>");

			addToHtml("<input name=\"wifiAp\" type=\"radio\" value= ");
			addToHtml(wifiApList[i].name);
			addToHtml( "> ");
			addToHtml(wifiApList[i].name);



			addToHtml("</td><td>");
			uint8_t progressValue = 100 - abs(wifiApList[i].rssi);
			itoa(progressValue, numToStr, 10);

			addToHtml("        <progress value=\"");
			addToHtml(numToStr);
			addToHtml("\" max=\"100\">");
			//addToHtml("<br>");
			//addToHtml( " ");
			itoa(wifiApList[i].rssi, numToStr, 10);
			addToHtml(numToStr);
			addToHtml( "dB<\/progress> ");
			addToHtml(numToStr);
			addToHtml( "dB");

			addToHtml("</td></tr>");
			//	"<input name=\"mycolor\" type=\"radio\" value=\"white\"> white <br>"


		}
	}
	addToHtml("</table>");

	addToHtml("<p>PSK Password: <input type=\"text\" name=\"pswd\" /></p>");
	addToHtml("<p><input type=\"submit\" name=\"submit\" value=\"submit\">");
	addToHtml("   <input type=\"submit\" name=\"submit\" value=\"update\"></p>");
	addToHtml("</form>");
	//addToHtml("<form  method=\"get\"><input type=\"submit\" value=\"update\"></form>");
	addToHtml("</section></div>");


//	"<select name=\"securType\" size = 1> "
//	"<option value=\"val1\">valText1</option>"
//	"<option value=\"val2\">valText2</option>"
//	"<option value=\"val3\">valText3</option>"
//	"</select><br>"
//	"<input name=\"mycolor\" type=\"radio\" value=\"white\"> white <br>"
//	"<input name=\"mycolor\" type=\"radio\" value=\"green\"> green<br>"
//	"<input name=\"mycolor\" type=\"radio\" value=\"blue\"> blue<br>"
//	"<input type=\"submit\" value=\"but text\">"

	//ASSERT(strlen(&(bufStr[0])) < htmlBodyLen);

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

void parseHttpReq(char *str, THtmlReqType &htmlReqType)
{
	char *pch;
	uint8_t argInd = 0;
	//debugPrintf(" ! parse http req ! start\r\n");
	//debugPrintf(str);
	//debugPrintf("\r\n");

	pch = strtok (str, " ");
	while(pch != NULL){
		//debugPrintf(pch);
		//debugPrintf("\r\n");

		if(argInd == 1){
			if(strcmp(pch, "/") == 0){
				//debugPrintf(" GET_ROOT detected\r\n");
				htmlReqType = GET_ROOT;
			}
			else if(strcmp(pch, "/favicon.ico") == 0){
				//debugPrintf(" GET_FAVICON detected\r\n");
				htmlReqType = GET_FAVICON;
			}
		}

		pch = strtok (NULL, " ");
		argInd++;
	}

	//debugPrintf(" ! parse http req ! stop\r\n");
}

void parseIPD(char *str, TCmd &cmd, int16_t *msgLen)
{
	char *pch;
	char numToStr[10];
	char *strHttpReq = NULL;
	uint8_t i;

	uint16_t tailLen = 0;
	//debugPrintf("parseIPD\r\n");
	pch = strtok (str, ",:");
	for(i=0; pch != NULL; i++){
		switch(i){
		case 1:
			cmd.curConnInd = (uint8_t)atoi(pch);
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
			strHttpReq = pch;
			//itoa(strlen(pch), tailLen, 10);
			//atoi(pch);
			//debugPrintf(pch);
			//debugPrintf(" => ");
			///debugPrintf(numToStr);
			//debugPrintf("\r\n");

			break;
		}
		pch = strtok (NULL, ",:");
	}
	parseHttpReq(strHttpReq, cmd.htmlReqType);

//	debugPrintf("conNum ");
//	itoa(*curConnInd, numToStr, 10);
//	debugPrintf(numToStr);

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

uint8_t wifiApInd = 0;
void resetApList()
{
	wifiApInd = 0;
}
void parseCWLAP(char *str)
{
	char *pch = strtok (str, ":(\"");
	TWifiAp *carWifiAap = &(wifiApList[wifiApInd]);
	for(uint8_t i=0; pch != NULL; i++){
		debugPrintf(pch);
		debugPrintf("\r\n");
		switch(i){
		case 2:
			//pch[strlen(pch)]='\0';
			memcpy(&(carWifiAap->name[0]), pch+1, MIN(10, strlen(pch)-2));
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


TCmdType blockWaitCmd(bool debugPrintf = false)
{
	TCmd cmd = getNextWifiCmd(INFINITY, true);
	return cmd.type;
}

//void waitForRespText()
//{
//	while(1){
//		TCmdType cmd = blockWaitCmd();
//		if( cmd == TEXT){
//			break;
//		}
//	}
//}

void waitForRespOK()
{
	while(1){
		TCmdType cmd = blockWaitCmd(true);
		if( cmd == CMD_OK)
			break;
	}
}

void waitForRespOKorRespError()
{
	while(1){
		TCmdType cmd = blockWaitCmd(true);
		if( cmd == CMD_OK)
			break;
		if(cmd == RESP_ERROR)
			break;
	}
}

void waitForCIPSENDResp()
{
	while(1){
		TCmdType cmd = blockWaitCmd();
		if( cmd == CMD_OK)
			break;
		if(cmd == RESP_ERROR)
			break;
	}
}

void scanWiFiAp()
{
	wifiPrintf("AT+CWLAP\r\n");
	waitForRespOK();
}

void blockWaitRecvBytesReport(bool debugPrintf = false)
{
	while(1){
		TCmdType cmd = blockWaitCmd(debugPrintf);
		if(cmd == RESP_SEND_OK)
			break;
		if(cmd == RESP_ERROR)
			break;
		if(cmd == busy_s)
			debugPrintf("busy!!\r\n");
		if(cmd == recv_bytes_report){
			debugPrintf("recv_bytes_report\r\n");
			break;
		}
	}
}


void blockWaitSendOK(bool debugPrintf = false)
{
	while(1){
		TCmdType cmd = blockWaitCmd(debugPrintf);
		if(cmd == RESP_SEND_OK)
			break;
		if(cmd == RESP_ERROR)
			break;
		if(cmd == busy_s)
			debugPrintf("busy!!\r\n");
	}
}

void blockWaitForReadyToSend(bool debugPrintf = false)
{
	while(1){
		TCmdType cmd = blockWaitCmd(debugPrintf);
		if( cmd == ready_to_send){
			return;
		}
		if( cmd == CMD_CLOSED){
			debugPrintf("detect CLOSED \r\n");
			return;
		}
		if(cmd == busy_s){
			//while (1) ;
		}
	}
}

uint16_t blockWaitTEXT()
{
	uint16_t ret = 0;
	waitWiFiMsg();
	ret = getUartIrqMsgLength(); //strlen(uart1Buffer);
	//uint16_t msgLen = uart1MsgLen;
	//memcpy(uart1ProcBuffer, uart1Buffer, BUF_LEN);
	enableWiFiMsg();
	return ret;

//	extern char tempRecvBuf[];
//	uint8_t tempRecvBufInd = 0;
//
//	extern RINGBUFF_T rxring;
//
//	tempRecvBuf[0] = 0;
//
//	while(1){
//		if(RingBuffer_Pop(&rxring, &(tempRecvBuf[tempRecvBufInd]))){
//			if( (tempRecvBuf[tempRecvBufInd] == '\n') ||
//				((tempRecvBuf[0] == '>') && (tempRecvBufInd==1))
//			  )
//			{
//				tempRecvBufInd++;
//				tempRecvBuf[tempRecvBufInd] = '\0';
//
//				uint16_t msgSize = tempRecvBufInd;
//				Chip_UART_SendBlocking(LPC_USART0, tempRecvBuf, tempRecvBufInd);
//				tempRecvBufInd = 0 ;
//				return msgSize;
//			}
//			else{
//				if((msgLength > 0) && (tempRecvBufInd == (msgLength-1))){
//					tempRecvBufInd++;
//					tempRecvBuf[tempRecvBufInd] = '\0';
//					//debugPrintf("!!! affected !!! \r\n");
//					//Chip_UART_SendBlocking(LPC_USART0, tempRecvBuf, tempRecvBufInd);
//					TCmdType cmdType = TEXT;
//					//processMsg(cmdType, tempRecvBufInd);
//					uint16_t msgSize = tempRecvBufInd;
//					tempRecvBufInd = 0 ;
//					return msgSize;
//				}
//				else
//					tempRecvBufInd++;
//			}
//		}
//	}
}

void sendWifiData(char *str, uint8_t connId, bool debugPrintf = false)
{
	char numToStr[5];
	wifiPrintf("AT+CIPSEND=");

	if(debugPrintf == true)
		debugPrintf("AT+CIPSEND=");

	itoa(connId, numToStr, 10);
	wifiPrintf(numToStr);

	if(debugPrintf == true)
		debugPrintf(numToStr);
	wifiPrintf(",");

	if(debugPrintf == true)
		debugPrintf(",");

	itoa(strlen(str), numToStr, 10);

	wifiPrintf(numToStr);

	if(debugPrintf == true)
		debugPrintf(numToStr);

	wifiPrintf("\r\n");

	if(debugPrintf == true)
		debugPrintf("\r\n");

	blockWaitForReadyToSend();
	wifiPrintf(str);
	blockWaitSendOK();
}

void sendWifiDataToBuf(char *str, uint8_t connId, bool debugPrintf = false)
{
	char numToStr[5];
	wifiPrintf("AT+CIPSENDBUF=");

	if(debugPrintf == true)
		debugPrintf("AT+CIPSENDBUF=");

	itoa(connId, numToStr, 10);
	wifiPrintf(numToStr);

	if(debugPrintf == true)
		debugPrintf(numToStr);
	wifiPrintf(",");

	if(debugPrintf == true)
		debugPrintf(",");

	itoa(strlen(str), numToStr, 10);

	wifiPrintf(numToStr);

	if(debugPrintf == true)
		debugPrintf(numToStr);

	wifiPrintf("\r\n");

	if(debugPrintf == true)
		debugPrintf("\r\n");

	blockWaitForReadyToSend(true);
	debugPrintf("sending data\r\n");
	wifiPrintf(str);
	debugPrintf("data send ok\r\n");
	//blockWaitSendOK();
	blockWaitRecvBytesReport(true);
}

void startSendWifiDataWithLen(uint16_t strLen, uint8_t connId, uint8_t segInd, bool debugPrintf = false)
{
	char numToStr[5];
	wifiPrintf("AT+CIPSEND=");

	if(debugPrintf == true)
		debugPrintf("AT+CIPSEND=");

	itoa(connId, numToStr, 10);
	wifiPrintf(numToStr);

	if(debugPrintf == true)
		debugPrintf(numToStr);
//---------------------
//	wifiPrintf(",");
//
//	if(debugPrintf == true)
//		debugPrintf(",");
//
//	itoa(segInd, numToStr, 10);
//	wifiPrintf(numToStr);
//
//	if(debugPrintf == true)
//		debugPrintf(numToStr);

//---------------------
	wifiPrintf(",");

	if(debugPrintf == true)
		debugPrintf(",");

	itoa(strLen, numToStr, 10);

	wifiPrintf(numToStr);

	if(debugPrintf == true)
		debugPrintf(numToStr);


	wifiPrintf("\r\n");

	if(debugPrintf == true)
		debugPrintf("\r\n");

	blockWaitForReadyToSend(debugPrintf);
}

__RODATA(text) char svgStr1[] = "<svg height=\"160\"  width=\"400\">\r\n"
		"<g style=\"stroke-dasharray: 5 6; stroke: black;\">\r\n";

__RODATA(text) char svgStr2[] = "</g>\r\n"
		"<g font-size=\"12\" font-weight=\"bold\">\r\n";

void sendSvgData(char *bufStr/*, uint32_t *vals*/)
{
	uint32_t minVal=20, maxVal=50;
///	uint32_t minVal=vals[0], maxVal=vals[0];
//	for(int i=1; i<12; i++){
//		if(vals[i] < minVal)
//			minVal=vals[i];
//		if(vals[i] > maxVal)
//			maxVal=vals[i];
//	}
//	uint32_t diap = (maxVal - minVal);
//	uint32_t gap = diap/10;
//	uint32_t delta = ((float)(diap+2*gap))/7;
	uint32_t delta = 10;
	uint32_t gap = 10;

	static uint8_t sendBufIdx = 1;

	char numToStr[5];
	startSendWifiDataWithLen(1908, curConnInd, sendBufIdx++, true);

	//sendWifiDataToBuf(svgStr1, curConnInd, true);
	wifiPrintf(svgStr1);

	strcpy(bufStr, "<line x1=\"0\" y1=\"   \" x2=\"400\" y2=\"   \"/>\r\n");
	for(int i=0, j=20; i<7; i++, j+=20){
		itoa(j, numToStr, 10);
		uint8_t sl = strlen(numToStr);
		memcpy(&(bufStr[17]), numToStr, sl);
		memcpy(&(bufStr[35]), numToStr, sl);
		//sendWifiDataToBuf(bufStr, curConnInd, true);
		wifiPrintf(bufStr);
	}

	//sendWifiDataToBuf(svgStr2, curConnInd, true);
	wifiPrintf(svgStr2);

	strcpy(bufStr, "<text x=\"2\" y=\"   \">   </text>\r\n");
	for(int i=0, j=155, k=(minVal-gap); i<8; i++, j-=20, k+=delta){
		itoa(j, numToStr, 10);
		uint8_t sl = strlen(numToStr);
		bufStr[15+2]=' ';
		memcpy(&(bufStr[15]), numToStr, sl);

		itoa(k, numToStr, 10);
		sl = strlen(numToStr);
		//bufStr[20+2]=' ';
		memcpy(&(bufStr[20]), numToStr, sl);

		//sendWifiDataToBuf(bufStr, curConnInd, true);
		wifiPrintf(bufStr);
	}
	//sendWifiDataToBuf("</g>\r\n", curConnInd, true);
	wifiPrintf("</g>\r\n");

	//sendWifiDataToBuf("<g style=\"fill: rgb(254,254,127); stroke: black;\">\r\n", curConnInd, true);
	wifiPrintf("<g style=\"fill: rgb(254,254,127); stroke: black;\">\r\n");
	strcpy(bufStr, "<rect x=\"25 \" y=\"110\" width=\"25\" height=\"350\"/>\r\n");
	for(int i=0, j=25; i<12; i++, j+=30){

		itoa(j, numToStr, 10);
		uint8_t sl = strlen(numToStr);
		memcpy(&(bufStr[9]), numToStr, sl);

		uint8_t y = 40+rand() % 100;
		itoa(y, numToStr, 10);
		sl = strlen(numToStr);
		bufStr[19] = ' ';
		memcpy(&(bufStr[17]), numToStr, sl);

		//sendWifiDataToBuf(bufStr, curConnInd, true);
		wifiPrintf(bufStr);
	}
	//sendWifiDataToBuf("</g>\r\n", curConnInd, true);

	//sendWifiDataToBuf("<g font-size=\"10\" font-weight=\"bold\" text-anchor=\"middle\">\r\n", curConnInd, true);

	wifiPrintf("</g>\r\n<g font-size=\"10\" font-weight=\"bold\" text-anchor=\"middle\">\r\n");

	strcpy(bufStr, "<text x=\"37 \" y=\"155\">-11h</text>\r\n");
	for(int i=0, j=-11, k=37; i<12; i++, j++, k+=30){

		bufStr[24] = ' ';
		bufStr[25] = ' ';
		itoa(j, numToStr, 10);
		strcat(numToStr, "h");
		uint8_t sl = strlen(numToStr);
		memcpy(&(bufStr[22]), numToStr, sl);

		itoa(k, numToStr, 10);
		sl = strlen(numToStr);
		memcpy(&(bufStr[9]), numToStr, sl);

		//sendWifiDataToBuf(bufStr, curConnInd, true);
		wifiPrintf(bufStr);
		//wifiPrintf("<text x=\"37 \" y=\"155\">-11h</text>\r\n");
	}
	//sendWifiDataToBuf("</g>\r\n", curConnInd, true);

	//sendWifiDataToBuf("<rect height=\"160\" width=\"400\" style=\"stroke:black; fill:none\"/>\r\n", curConnInd, true);
	//sendWifiDataToBuf("</svg></br></br>\r\n", curConnInd, true);

	wifiPrintf("</g>\r\n<rect height=\"160\" width=\"400\" style=\"stroke:black; fill:none\"/>\r\n</svg></br></br>\r\n");

	blockWaitSendOK(true);
	//blockWaitRecvBytesReport(true);
}


__RODATA(text) char statusText1[] = "Wifi AP status: ON with name \"ESP_9DACCD\" with ip ";
__RODATA(text) char statusText2[] = "</br>Wifi client status: connected on \"TL-WR842ND\" with ip ";
__RODATA(text) char statusText3[] = "</br></br>Uptime: ";

//TCmdType getNextWifiCmd(bool debugPrintf)
//{
//	TCmdType cmdType = UNKNWON;
//	char recvBuf[BUF_LEN];
//
//	uint16_t msgLen = recvWifiMsg(recvBuf);
//
//	if(recvBuf[0] == '+'){
//		if(MEMCMPx(recvBuf, "+IPD") == 0){
//			char numToStr[10];
//			debugPrintf("!!! +IPD !!! 0 \r\n");
//			parseIPD(recvBuf, &curConnInd, &msgLength);
//
//			while(msgLength > 0){
//				uint16_t wifiMsgLen = blockWaitTEXT();
//				msgLength -= wifiMsgLen;
//
//				debugPrintf(" => ");
//				itoa(wifiMsgLen, numToStr, 10);
//				debugPrintf(numToStr);
//				debugPrintf(" rn ");
//				itoa(msgLength, numToStr, 10);
//				debugPrintf(numToStr);
//				debugPrintf(" chs\r\n");
//			}
//
//			debugPrintf(" message recvd! \r\n");
//		}
//	}
//	debugPrintf("got msg! -> ");
//	debugPrintflen(recvBuf, msgLen);
//
//	return cmdType;
//}

TCmd getNextWifiCmd(int16_t to_msec, bool debugPrintf)
{
	TCmd cmd;
	cmd.type = UNKNWON;
	char recvBuf[BUF_LEN];

	uint16_t msgLen = recvWifiMsgTO(recvBuf, to_msec);
	if(debugPrintf == true){
		if(msgLen == -1)
			debugPrintf("timeout!\r\n");
		else{
			debugPrintf(" ->");
			debugPrintflen(recvBuf, msgLen);
		}
	}

	if(msgLen != -1){
		if(recvBuf[0] == '+'){
			if(MEMCMPx(recvBuf, "+IPD") == 0){
				char numToStr[10];
				debugPrintf("!!! +IPD !!!\r\n");
				int16_t msgLength = 0;
				parseIPD(recvBuf, cmd, &msgLength);

				while(msgLength > 0){
					uint16_t wifiMsgLen = waitWifiMsgAndStartRecv();
					msgLength -= wifiMsgLen;

					debugPrintf(" => ");
					itoa(wifiMsgLen, numToStr, 10);
					debugPrintf(numToStr);
					debugPrintf(" rn ");
					itoa(msgLength, numToStr, 10);
					debugPrintf(numToStr);
					debugPrintf(" chs\r\n");
				}

				debugPrintf(" message recvd! \r\n");

				cmd.type = IPD;
			}
			else if(MEMCMPx(recvBuf, "+CIFSR") == 0){
				cmd.type = parseCIFSR(recvBuf);
			}
			else if(MEMCMPx(recvBuf, "+CWLAP") == 0){
				debugPrintf(" parse CWLAP\r\n");
				parseCWLAP(recvBuf);
				debugPrintf(" parse CWLAP OK\r\n");
			}
			else if(MEMCMPx(recvBuf, "+CIPSTO") == 0)
				cmd.type = CIPSTO;
		}
		else if(strlen(recvBuf)==4){
			if(strcmp(recvBuf, "OK\r\n") == 0)
				cmd.type = CMD_OK;
		}
		else if(strcmp(recvBuf, "ERROR\r\n") == 0)
			cmd.type = RESP_ERROR;
		else if(strlen(recvBuf)==11){
			if(strcmp(recvBuf, "busy s...\r\n") == 0)
				cmd.type = busy_s;
		}
		else if(strcmp(recvBuf+1, ",CONNECT\r\n") == 0){
			//debugPrintf("!!!CONNECT!!!\r\n");
			cmd.type = CMD_CONNECT;

		}
		else if(strcmp(recvBuf+1, ",CLOSED\r\n") == 0){
			//debugPrintf("!!!CLOSED!!!\r\n");
			cmd.type = CMD_CLOSED;
		}
		else if(strcmp(recvBuf, "SEND OK\r\n") == 0)
			cmd.type = RESP_SEND_OK;
		else if(strcmp(recvBuf, "> \r\n") == 0){
			debugPrintf("!!!SEND_READY!!!\r\n");
			cmd.type = SEND_READY;
		}
		else if(strcmp(recvBuf, "WIFI GOT IP\r\n") == 0){
			//debugPrintf("!!!WIFI GOT IP!!!\r\n");
			cmd.type = wifi_gotip;
		}
		else if(strcmp(recvBuf, "WIFI DISCONNECT\r\n") == 0){
			cmd.type = wifi_discon;
		}
		else if(strcmp(recvBuf, "ready\r\n") == 0)
			cmd.type = ready;
		else if(strcmp(recvBuf, "> ") == 0){
			cmd.type = ready_to_send;
			//debugPrintf("!!! ready_to_send detected !!!\r\n");
		}
		else if(MEMCMPx(recvBuf, "Recv") == 0){
			cmd.type = recv_bytes_report;
			//debugPrintf("!!! recv_bytes_report detected !!!\r\n");
		}
		else{
			cmd.type = TEXT;
	//		debugPrintf("!!! TEXT !!! \r\n");
	//		if((msgLength == 0) && (strcmp(wifiMsg, "\r\n") != 0) )
	//			debugPrintf("!!! unknown cmd!!! \r\n");
		}
	}
	return cmd;
}


void commonInit()
{
	wifiPrintf("AT+CIPMUX=1\r\n");
	waitForRespOK();

	wifiPrintf("AT+CIPSERVER=1,80\r\n");
	waitForRespOK();

	wifiPrintf("AT+CIFSR\r\n");
	waitForRespOK();

	wifiPrintf("AT+CWMODE_CUR=3\r\n");
	waitForRespOKorRespError();

	wifiPrintf("AT+CWLAP\r\n");
	waitForRespOKorRespError();

	debugPrintf("Wait AT+CWLAP resp OK! \r\n");



//	char numToStr[10];
//	for(uint8_t i=0; i<WIFI_APLISTMAX; i++){
//		if(wifiApList[i].rssi != 0){
//			debugPrintf(&(wifiApList[i].name[0]));
//			debugPrintf("   ==>  ");
//
//			itoa(wifiApList[i].rssi, numToStr, 10);
//			debugPrintf(numToStr);
//
//			debugPrintf("\r\n");
//		}
//	}
}



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

void vHttpServerTask ()
{
	char htmlBody[1004];

	TCmd cmd = getNextWifiCmd(INFINITY, true);

	bool debugPrintf = true;

	if(cmd.type == IPD){
		debugPrintf("!!! IPD !!!\r\n");
		if(cmd.htmlReqType == GET_ROOT){
			debugPrintf("!!! GET_ROOT !!!\r\n");

		}
		else{
			debugPrintf("!!! not ROOT. send 404 !!!\r\n");
		}
	}
	return;

	if(cmd.type == IPD){
		if(cmd.htmlReqType == GET_ROOT){

//				wifiPrintf("AT+CIPSEND=");
//				debugPrintf("AT+CIPSEND=");
//				itoa(curConnInd, numToStr, 10);
//				wifiPrintf(numToStr);
//				debugPrintf(numToStr);
//				wifiPrintf(",");
//				debugPrintf(",");
//				itoa(strlen(&(htmlSimple[0])), numToStr, 10);
//				wifiPrintf(numToStr);
//				debugPrintf(numToStr);
//				wifiPrintf("\r\n");
//				debugPrintf("\r\n");
//
//				while(1){
//					TCmdType cmd = blockWaitCmd();
//					if( cmd == ready_to_send){
//						break;
//					}
//
//					if( cmd == CMD_CLOSED){
//						debugPrintf("detect CLOSED \r\n");
//						return;
//					}
//					if(cmd == busy_s){
//						//while (1) ;
//					}
//
//				}
//				//debugPrintf("ready_to_send \r\n");
//				wifiPrintf(&(htmlSimple[0]));
//				debugPrintf(&(htmlSimple[0]));
//				while(1){
//					TCmdType cmd = blockWaitCmd();
//					if( cmd == RESP_SEND_OK){
//						break;
//					}
//				}



//				debugPrintf("!!! htmlSimple SEND_OK detected!!!\r\n");
//				for(int i=0; i<100000; i++) ;
//				debugPrintf("!!! htmlSimple SEND_OK detected!!!\r\n");
//				wifiPrintf("AT+CIPCLOSE=");
//				itoa(curConnInd, numToStr, 10);
//				wifiPrintf(numToStr);
//				wifiPrintf("\r\n");
//				while(1){
//					TCmdType cmd = blockWaitCmd();
//					if( cmd == CMD_OK){
//						break;
//					}
//				}
//				debugPrintf("!!! AT+CIPCLOSE CMD_OK detected!!!\r\n");

		extern uint64_t SysTickCnt;
		uint32_t startGenPage =  (uint32_t)SysTickCnt;

		sendWifiData(htmlPart1, curConnInd);
		strcpy(htmlBody, statusText1);
		strcat(htmlBody, APIP);
		strcat(htmlBody, statusText2);
		strcat(htmlBody, STAPIP);
		strcat(htmlBody, statusText3);

		char numToStr[10];
		itoa(SysTickCnt/(24*3600*1000), numToStr, 10);
		strcat(htmlBody, numToStr);
		strcat(htmlBody, "d ");

		itoa(SysTickCnt/(3600*1000), numToStr, 10);
		strcat(htmlBody, numToStr);
		strcat(htmlBody, "h ");

		itoa(SysTickCnt/(60*1000), numToStr, 10);
		strcat(htmlBody, numToStr);
		strcat(htmlBody, "m ");

		itoa(SysTickCnt/1000, numToStr, 10);
		strcat(htmlBody, numToStr);
		strcat(htmlBody, "s ");
		sendWifiData(htmlBody, curConnInd);

		sendWifiData(htmlPartStartSec2, curConnInd);

		//uint32_t cntVals[12];
		for(int i=0; i<4; i++){
//					cntVals[0] = rand() % 100;
//					for(int j=1; j<12; j++){
//						cntVals[j] += rand() % 100;
//					}
			sendSvgData(htmlBody/*, cntVals*/);
		}
		sendWifiData(htmlPartStartSec3, curConnInd);
		//sendWifiData(htmlPart3, curConnInd);

		if(debugPrintf == true)	debugPrintf("!!! htmlSend0 SEND_OK detected!!!\r\n");

		prepareHtmlData(htmlBody);

		sendWifiData(htmlBody, curConnInd);

		if(debugPrintf == true) debugPrintf("!!! htmlBody SEND_OK detected!!!\r\n");

		sendWifiData("<center>Page generate in ", curConnInd);

		itoa(SysTickCnt - startGenPage, htmlBody, 10);
		//debugPrintf(htmlBody);
		//debugPrintf(" for page generate\r\n");
		sendWifiData(htmlBody, curConnInd);
		sendWifiData(" ms</center>", curConnInd);

		sendWifiData(htmlPart2, curConnInd);


		if(debugPrintf == true) debugPrintf("!!! htmlClose SEND_OK detected!!!\r\n");

		sendCipClose(curConnInd);

		waitForRespOK();
	}
	else {
		if(debugPrintf == true) debugPrintf("not ROOT. send 404\r\n");

		sendWifiData(html404, curConnInd, debugPrintf);
//				wifiPrintf("AT+CIPSEND=");
//				debugPrintf("AT+CIPSEND=");
//				itoa(curConnInd, numToStr, 10);
//				wifiPrintf(numToStr);
//				debugPrintf(numToStr);
//				wifiPrintf(",");
//				debugPrintf(",");
//				itoa(strlen(&(html404[0])), numToStr, 10);
//				wifiPrintf(numToStr);
//				debugPrintf(numToStr);
//				wifiPrintf("\r\n");
//				debugPrintf("\r\n");

		while(1){
			TCmdType cmd = blockWaitCmd(debugPrintf);
			if( cmd == ready_to_send){
				break;
			}

			if( cmd == CMD_CLOSED){
				if(debugPrintf == true) debugPrintf("detect CLOSED \r\n");
				//eState = waitForCmd;
				break;
			}
		}
		if(debugPrintf == true) debugPrintf("!!! htmlSend0 ready_to_send detected!!!\r\n");
		wifiPrintf(&(html404[0]));
		while(1){
			TCmdType cmd = blockWaitCmd(debugPrintf);
			if( cmd == RESP_SEND_OK){
				break;
			}
		}
		if(debugPrintf == true) debugPrintf("!!! html404 SEND_OK detected!!!\r\n");
		//eState = waitForCmd;
		}
	}
//}



//	static uint8_t tempRecvBufInd = 0;

//	while(RingBuffer_Pop(&rxring, &(tempRecvBuf[tempRecvBufInd]))){
//		if( (tempRecvBuf[tempRecvBufInd] == '\n') ||
//			((tempRecvBuf[0] == '>') && (tempRecvBufInd==1))
//		  )
//		{
//			tempRecvBufInd++;
//			tempRecvBuf[tempRecvBufInd] = '\0';
//
//			if(tempRecvBufInd >= TEMPRECVBUF_SIZE)
//				errword |= (1<<UARTTMPBUF_FULL_BITNUM);
//			//tempRecvBufInd++;
//			//uint16_t tempRecvBufLen = sizeof(tempRecvBuf);
//			Chip_UART_SendBlocking(LPC_USART0, tempRecvBuf, tempRecvBufInd);
//			TCmdType cmdType = UNKNWON;
//			parseCommand(cmdType, tempRecvBuf);
//			processMsg(cmdType, tempRecvBufInd);
//			tempRecvBufInd = 0 ;
//		}
//		else{
//			if((msgLength > 0) && (tempRecvBufInd == (msgLength-1))){
//				tempRecvBufInd++;
//				tempRecvBuf[tempRecvBufInd] = '\0';
//				debugPrintf("!!! affected !!! \r\n");
//				Chip_UART_SendBlocking(LPC_USART0, tempRecvBuf, tempRecvBufInd);
//				TCmdType cmdType = TEXT;
//				processMsg(cmdType, tempRecvBufInd);
//				tempRecvBufInd = 0 ;
//			}
//			else
//				tempRecvBufInd++;
//		}
//	}
}



