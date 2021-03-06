#include <stdlib.h>
#include "esp8266.h"
#include "uartUtils.h"
#include "utils.h"

#include <cr_section_macros.h>
#include <iap.h>

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

__RODATA(text) char htmlPart3[] = "<p>PSK Password: <input type=\"text\" name=\"pswd\" /></p>"
"<p><input type=\"submit\" name=\"submit\" value=\"submit\">"
"   <input type=\"submit\" name=\"submit\" value=\"update\"></p>"
"</form>"
"<form enctype=\"multipart/form-data\" method=\"post\">"
"<p>Select files</p>"
"<p><input type=\"file\" name=\"firmware\" accept=\".hex\">"
"<input type=\"submit\" value=\"Upload\"></p>"
"</form>"
//addToHtml("<form  method=\"get\"><input type=\"submit\" value=\"update\"></form>");
"</section></div>";


#define addToHtml(a) strcat (bufStr, a)

void prepareHtmlData(uint8_t curConn)
{
	uint8_t i;
	char bufStr[250], numToStr[10];
	strcpy(bufStr, " ");
	//itoa(xTaskGetTickCount(), numToStr, 10);
	//strcat (htmlBody,numToStr);

	addToHtml(" select WiFi AP ");

	//itoa(xPortGetFreeHeapSize(), numToStr, 10);
	//addToHtml(numToStr);

	addToHtml("<form method=\"get\">");

	addToHtml("<table>");
//	for(i=0;i<WIFI_APLISTMAX;i++){
//		if(wifiApList[i].rssi != 0){
//
//			addToHtml("<tr><td>");
//
//			addToHtml("<input name=\"wifiAp\" type=\"radio\" value= ");
//			addToHtml(wifiApList[i].name);
//			addToHtml( "> ");
//			addToHtml(wifiApList[i].name);
//
//
//
//			addToHtml("</td><td>");
//			uint8_t progressValue = 100 - abs(wifiApList[i].rssi);
//			itoa(progressValue, numToStr, 10);
//
//			addToHtml("        <progress value=\"");
//			addToHtml(numToStr);
//			addToHtml("\" max=\"100\">");
//			//addToHtml("<br>");
//			//addToHtml( " ");
//			itoa(wifiApList[i].rssi, numToStr, 10);
//			addToHtml(numToStr);
//			addToHtml( "dB</progress> ");
//			addToHtml(numToStr);
//			addToHtml( "dB");
//
//			addToHtml("</td></tr>");
//			//	"<input name=\"mycolor\" type=\"radio\" value=\"white\"> white <br>"
//
//
//		}
//	}
	addToHtml("</table>");

	sendWifiDataToBuf(bufStr, curConn);

	//debugPrintf("prepareHtmlData st2 \r\n");

//	strcpy(bufStr, "<p>PSK Password: <input type=\"text\" name=\"pswd\" /></p>");
//	//addToHtml("<p>PSK Password: <input type=\"text\" name=\"pswd\" /></p>");
//	addToHtml("<p><input type=\"submit\" name=\"submit\" value=\"submit\">");
//	addToHtml("   <input type=\"submit\" name=\"submit\" value=\"update\"></p>");
//	addToHtml("</form>");
//
//	addToHtml("<form enctype=\"multipart/form-data\" method=\"post\">");
//	addToHtml("<p>Select files</p>");
//	addToHtml("<p><input type=\"file\" name=\"firmware\" accept=\".hex\">");
//	addToHtml("<input type=\"submit\" value=\"Upload\"></p>");
//	addToHtml("</form>");
//
//	//addToHtml("<form  method=\"get\"><input type=\"submit\" value=\"update\"></form>");
//	addToHtml("</section></div>");


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
	sendWifiDataToBuf(htmlPart3, curConn);
}


__RODATA(text) char svgStr1[] = "<svg height=\"160\"  width=\"400\">\r\n"
		"<g style=\"stroke-dasharray: 5 6; stroke: black;\">\r\n";

__RODATA(text) char svgStr2[] = "</g>\r\n"
		"<g font-size=\"12\" font-weight=\"bold\">\r\n";

void sendSvgData(char *bufStr, uint8_t curConnInd/*, uint32_t *vals*/)
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

	//static uint8_t sendBufIdx = 1;

	char numToStr[5];
	//startSendWifiDataWithLen(1908, curConnInd, sendBufIdx++);

	sendWifiDataToBuf(svgStr1, curConnInd);
	//wifiPrintf(svgStr1);

	strcpy(bufStr, "<line x1=\"0\" y1=\"   \" x2=\"400\" y2=\"   \"/>\r\n");
	int i,j, k;
	for(i=0, j=20; i<7; i++, j+=20){
		itoa(j, numToStr, 10);
		uint8_t sl = strlen(numToStr);
		memcpy(&(bufStr[17]), numToStr, sl);
		memcpy(&(bufStr[35]), numToStr, sl);
		sendWifiDataToBuf(bufStr, curConnInd);
		//wifiPrintf(bufStr);
	}

	sendWifiDataToBuf(svgStr2, curConnInd);
	//wifiPrintf(svgStr2);

	strcpy(bufStr, "<text x=\"2\" y=\"   \">   </text>\r\n");
	for(i=0, j=155, k=(minVal-gap); i<8; i++, j-=20, k+=delta){
		itoa(j, numToStr, 10);
		uint8_t sl = strlen(numToStr);
		bufStr[15+2]=' ';
		memcpy(&(bufStr[15]), numToStr, sl);

		itoa(k, numToStr, 10);
		sl = strlen(numToStr);
		//bufStr[20+2]=' ';
		memcpy(&(bufStr[20]), numToStr, sl);

		sendWifiDataToBuf(bufStr, curConnInd);
		//wifiPrintf(bufStr);
	}
	sendWifiDataToBuf("</g>\r\n", curConnInd);
	//wifiPrintf("</g>\r\n");

	sendWifiDataToBuf("<g style=\"fill: rgb(254,254,127); stroke: black;\">\r\n", curConnInd);
	//wifiPrintf("<g style=\"fill: rgb(254,254,127); stroke: black;\">\r\n");
	strcpy(bufStr, "<rect x=\"25 \" y=\"110\" width=\"25\" height=\"350\"/>\r\n");
	for(i=0, j=25; i<12; i++, j+=30){

		itoa(j, numToStr, 10);
		uint8_t sl = strlen(numToStr);
		memcpy(&(bufStr[9]), numToStr, sl);

		uint8_t y = 40+rand() % 100;
		itoa(y, numToStr, 10);
		sl = strlen(numToStr);
		bufStr[19] = ' ';
		memcpy(&(bufStr[17]), numToStr, sl);

		sendWifiDataToBuf(bufStr, curConnInd);
		//wifiPrintf(bufStr);
	}
	//sendWifiDataToBuf("</g>\r\n", curConnInd, true);

	sendWifiDataToBuf("</g>\r\n<g font-size=\"10\" font-weight=\"bold\" text-anchor=\"middle\">\r\n", curConnInd);

	//wifiPrintf("</g>\r\n<g font-size=\"10\" font-weight=\"bold\" text-anchor=\"middle\">\r\n");

	strcpy(bufStr, "<text x=\"37 \" y=\"155\">-11h</text>\r\n");
	for(i=0, j=-11, k=37; i<12; i++, j++, k+=30){

		bufStr[24] = ' ';
		bufStr[25] = ' ';
		itoa(j, numToStr, 10);
		strcat(numToStr, "h");
		uint8_t sl = strlen(numToStr);
		memcpy(&(bufStr[22]), numToStr, sl);

		itoa(k, numToStr, 10);
		sl = strlen(numToStr);
		memcpy(&(bufStr[9]), numToStr, sl);

		sendWifiDataToBuf(bufStr, curConnInd);
		//wifiPrintf(bufStr);
		//wifiPrintf("<text x=\"37 \" y=\"155\">-11h</text>\r\n");
	}
	//sendWifiDataToBuf("</g>\r\n", curConnInd, true);

	//sendWifiDataToBuf("<rect height=\"160\" width=\"400\" style=\"stroke:black; fill:none\"/>\r\n", curConnInd, true);
	//sendWifiDataToBuf("</svg></br></br>\r\n", curConnInd, true);

	wifiPrintf("</g>\r\n<rect height=\"160\" width=\"400\" style=\"stroke:black; fill:none\"/>\r\n</svg></br></br>\r\n");
	sendWifiDataToBuf("</g>\r\n<rect height=\"160\" width=\"400\" style=\"stroke:black; fill:none\"/>\r\n</svg></br></br>\r\n", curConnInd);
	//sendWifiDataToBuf("</svg></br></br>\r\n", curConnInd, true);

	//blockWaitSendOK();
	//blockWaitRecvBytesReport(true);
}

__RODATA(text) char statusText1[] = "Wifi AP status: ON with name \"ESP_9DACCD\" with ip ";
__RODATA(text) char statusText2[] = "</br>Wifi client status: connected on \"TL-WR842ND\" with ip ";
__RODATA(text) char statusText3[] = "</br></br>Uptime: ";
void sendPageStart(uint8_t curConn)
{
	char htmlBody[250];

	debugPrintf("sendWifiDataToBuf\r\n");
	sendWifiDataToBuf(htmlPart1, curConn);
	debugPrintf("sendWifiDataToBuf\r\n");
	strcpy(htmlBody, statusText1);
	strcat(htmlBody, APIP);
	strcat(htmlBody, statusText2);
	strcat(htmlBody, STAPIP);
	strcat(htmlBody, statusText3);

	char numToStr[10];
	extern uint64_t SysTickCnt;
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
	sendWifiDataToBuf(htmlBody, curConn);

	sendWifiDataToBuf(htmlPartStartSec2, curConn);

}


#define DEBUGPRINTF
void vHttpServerTask ()
{
	TCmd cmd;
	{
		char recvBuf[BUF_LEN];

		getNextWifiCmdExtBuf(recvBuf, &cmd, INFINITY);

		debugPrintf("httpTask->");
		debugPrintf(recvBuf);
		debugPrintf("\r\n");
	}

	//debugPrintf("start process \r\n");

	if(cmd.type == IPD){
		//debugPrintf("!!! IPD !!!\r\n");
		if(cmd.htmlReqType == GET_ROOT){
			debugPrintf("httpTask-> !!! GET_ROOT !!!\r\n");
			extern uint64_t SysTickCnt;
			uint32_t startGenPage =  (uint32_t)SysTickCnt;

			sendPageStart(cmd.curConnInd);

			debugPrintf("!!! start send SVG!!!\r\n");
			//uint32_t cntVals[12];int
			int i;
			for(i=0; i<4; i++){
	//					cntVals[0] = rand() % 100;
	//					for(int j=1; j<12; j++){
	//						cntVals[j] += rand() % 100;
	//					}
	//			sendSvgData(htmlBody, cmd.curConnInd/*, cntVals*/);
			}
			debugPrintf("!!! stop send SVG!!!\r\n");
			sendWifiDataToBuf(htmlPartStartSec3, cmd.curConnInd);
			//sendWifiData(htmlPart3, curConnInd);

#ifdef DEBUGPRINTF
			debugPrintf("!!! htmlSend0 SEND_OK detected!!!\r\n");
#endif

			prepareHtmlData(cmd.curConnInd);

#ifdef DEBUGPRINTF
			debugPrintf("!!! htmlBody SEND_OK detected!!!\r\n");
#endif

			sendWifiDataToBuf("<center>Page generate in ", cmd.curConnInd);


			{
				char numToStr[10];
				itoa(SysTickCnt - startGenPage, numToStr, 10);


				//debugPrintf(htmlBody);
				//debugPrintf(" for page generate\r\n");
				sendWifiDataToBuf(numToStr, cmd.curConnInd);
				sendWifiDataToBuf(" ms</center>", cmd.curConnInd);

				sendWifiDataToBuf(htmlPart2, cmd.curConnInd);
			}

#ifdef DEBUGPRINTF
			debugPrintf("!!! htmlClose SEND_OK detected!!!\r\n");
#endif

			//sendCipClose(cmd.curConnInd);

			//waitForRespOK();
		}
		else if(cmd.htmlReqType == POST){
			debugPrintf("httpTask-> !!! POST !!!\r\n");
			getFirmware();
		}
		else{
			debugPrintf("httpTask-> not ROOT. send 404\r\n");
			sendWifiData(html404, cmd.curConnInd);
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
				getNextWifiCmd(&cmd, INFINITY);

				if( cmd.type == ready_to_send){
					break;
				}

				if( cmd.type == CMD_CLOSED){
#ifdef DEBUGPRINTF
					debugPrintf("detect CLOSED \r\n");
#endif
					//eState = waitForCmd;
					break;
				}
			}
#ifdef DEBUGPRINTF
				debugPrintf("!!! htmlSend0 ready_to_send detected!!!\r\n");
#endif
			wifiPrintf(&(html404[0]));

			while(1){
				getNextWifiCmd(&cmd, INFINITY);

				if( cmd.type == RESP_SEND_OK){
					break;
				}
			}
#ifdef DEBUGPRINTF
			debugPrintf("!!! html404 SEND_OK detected!!!\r\n");
#endif
			//eState = waitForCmd;
		}
	}
	return;
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


void parseHttpReq(char *str, THtmlReqType *htmlReqType)
{
	char *pch;
	uint8_t argInd = 0;
	debugPrintf(" parseHttpReq->");
	debugPrintf(str);
	debugPrintf("\r\n");

	pch = strtok (str, " ");
	while(pch != NULL){
		//debugPrintf(pch);
		//debugPrintf("\r\n");

		if(argInd == 0){
			if(strcmp(pch, "GET") == 0){
				debugPrintf(" httpReqParsing-> GET_ROOT\r\n");
				*htmlReqType = GET_ROOT;
			}
			if(strcmp(pch, "POST") == 0){
				debugPrintf(" httpReqParsing-> POST\r\n");
				*htmlReqType = POST;
			}
		}
		if(argInd == 1){
			if(strcmp(pch, "/") == 0){
				//debugPrintf(" GET_ROOT detected\r\n");
				//cmd.htmlReqType = GET_ROOT;
			}
			else if(strcmp(pch, "/favicon.ico") == 0){
				//debugPrintf(" GET_FAVICON detected\r\n");
				*htmlReqType = GET_FAVICON;
			}
		}

		pch = strtok (NULL, " ");
		argInd++;
	}

	//debugPrintf(" ! parse http req ! stop\r\n");
}

void parsePostReqHead(char *str, TCmd *cmd, uint16_t msgLen, uint8_t strInd)
{
	char numToStr[10];
	debugPrintf(" postReqhead-> ");
	if(strInd == 2){
		debugPrintf(" conn len! ");
		char *pch = strtok (str, " ");
		pch = strtok (NULL, " ");
		if(pch != NULL){
			debugPrintf(pch);
			char numToStr[10];
			cmd->contentLength = atoi(pch);
		}
		debugPrintf(" \r'n");
	}
	itoa(strInd, numToStr, 10);
	debugPrintf(numToStr);
	debugPrintf(" ");

	debugPrintf(str);


}

void parsePostReq(char *str, uint16_t msgLen, uint8_t strInd)
{
	char numToStr[10];
	debugPrintf(" postReq-> ");
	itoa(strInd, numToStr, 10);
	debugPrintf(numToStr);
	debugPrintf(" ");

	debugPrintf(str);
}

