#include <cr_section_macros.h>
#include <string.h>
#include <stdlib.h>
#include "esp8266.h"
#include "uartUtils.h"

extern uint32_t SystemCoreClock;

__RAM_FUNC inline uint8_t convertAddrToSecNum(uint16_t addr)
{
	return addr/0x400;
}

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

__RAM_FUNC inline int8_t convertTwosCompl(int8_t a) {
  //if (a < 0)
//    a = ( ~-a|128 ) + 1;

  return ~a + 1;
}

static char msgPrepSec[] = " getFirmware s=> Chip_IAP_PreSectorForReadWrite ret \r\n";
static char msgErs[] = " getFirmware s=> Chip_IAP_EraseSector ret \r\n";
static char msg3[] = " clear OK\r\n";
__RAM_FUNC void flashClear()
{

	uint8_t i;
	uint8_t ret = 0;
	uint32_t SystemCoreClock1000 = SystemCoreClock / 1000;
	for(i=5; i<10; i++){
//		if( (i != 3) && (i != 2)  && (i != 1) && (i != 0)){
			ret = Chip_IAP_PreSectorForReadWrite(i, i);
			debugPrintf(msgPrepSec);
			ret = Chip_IAP_EraseSectorExt(i, i, SystemCoreClock1000);
			debugPrintf(msgErs);
//		}
//		else{
//			debugPrintf(msg3);
//		}
	}
	debugPrintf(msg3);
}

#define DATA 1
#define NOTDATA 2
#define ESA 3
#define EOF 4

static char msgNotData[] = " not data\r\n";
static char crcFail[] = " crc fail! \r\n";
__RAM_FUNC uint8_t parseData(char *recvBuf, uint8_t *data, uint8_t *dLen, uint16_t *dAddr)
{
	//char numToStr[10];

	if(recvBuf[0] != ':'){
		debugPrintf(msgNotData);
		return NOTDATA;
	}

	uint8_t dType=0, dCrc = 0;

	char *parsePtr = &(recvBuf[1]);
	*dLen = parseInt8(parsePtr);
	parsePtr+=2;
	*dAddr = parseInt16(parsePtr);
	parsePtr+=4;
	dType = parseInt8(parsePtr);
	parsePtr+=2;

	/*debugPrintf("l");
	itoa(*dLen, numToStr, 16);
	debugPrintf(numToStr);
	debugPrintf(".a");
	itoa(*dAddr, numToStr, 16);
	debugPrintf(numToStr);
	debugPrintf(".t");
	itoa(dType, numToStr, 16);
	debugPrintf(numToStr);
	debugPrintf(" ");*/

	uint8_t cc = 0;
	cc+=*dLen;
	cc+=dType;
	cc+=(*dAddr&0xff);
	cc+=((*dAddr>>8)&0xff);

	uint8_t i;
	for(i=0;i<*dLen;i++){
		data[i] = parseInt8(parsePtr);
		cc += data[i];
		parsePtr += 2;

		/*itoa(data[i], numToStr, 16);
		debugPrintf(numToStr);
		debugPrintf(".");*/
	}
	/*debugPrintf("\r\n");*/

	cc = convertTwosCompl(cc);
	/*debugPrintf(" cc");
	itoa(cc, numToStr, 16);
	debugPrintf(numToStr);*/

	dCrc = parseInt8(parsePtr);
	/*debugPrintf(" c");
	itoa(dCrc, numToStr, 16);
	debugPrintf(numToStr);


	debugPrintf(" chs >> ");
	debugPrintf("data:");*/
	if(dCrc != cc){
		debugPrintf(crcFail);
		return NOTDATA;
	}
	if(dType == 3)
		return ESA;
	else if(dType == 1)
		return EOF;

	return DATA;
	//debugPrintflen(recvBuf, wifiMsgLen);
}


//char msg3[] = "skip\r\n";
//char msg4[] = "end\r\n";
static char msg6[] = " getFirmware s=> start \r\n";
static char msg7[] = " getFirmware s=>";
static char flash[] = " flashing\r\n";
static char eraseSec[] = " erase sector\r\n";

static char prepSecErr[] = "PreSectorForReadWrite err\r\n";
static char cpyRamErr[] = "CopyRamToFlash err\r\n";
static char msgErsErr[] = "EraseSector err\r\n";
static char msgReset[] = "system reset\r\n";
static char msgAddrStep[] = " getFirmware s=> adress step\r\n";
static char msgOk[] = " ok\r\n";

#pragma pack(16)
static uint8_t bootLoaderData[256];

__RAM_FUNC void getFirmware()
{
	char recvBuf[100];
	//char numToStr[10];
	int16_t wifiMsgLen = 0;
	//int32_t contentLength = cmd->contentLength;
	//uint8_t presSecRet1 = 0, eraseSecRet = 0, presecSecRet2 = 0, ret;
	uint8_t ret = 0;
	 uint32_t SystemCoreClock1000 = SystemCoreClock / 1000;

	__disable_irq();

	NVIC_DisableIRQ(UART1_IRQn);
	NVIC_DisableIRQ(SysTick_IRQn);


	//flashClear();
//	debugPrintf(msg3);
//	for(i=0; i<16; i++){
////		if( (i != 3) && (i != 2)  && (i != 1) && (i != 0)){
//			ret = Chip_IAP_PreSectorForReadWrite(i, i);
//			debugPrintf(msg1);
//			ret = Chip_IAP_EraseSectorExt(i, i, SystemCoreClock1000);
//			debugPrintf(msg2);
////		}
////		else{
////			debugPrintf(msg3);
////		}
//	}
	//debugPrintf(msg4);

	//__disable_irq();
	//ret = Chip_IAP_PreSectorForReadWrite(0, 15);

	//__enable_irq();

	//itoa(ret, numToStr, 10);
	//debugPrintf(msg1);
	//debugPrintflen(numToStr);
	//debugPrintf("\r\n");

	//ret = Chip_IAP_PreSectorForReadWrite(0, 15);
	//ret = Chip_IAP_EraseSectorExt(0, 15, SystemCoreClock1000);

	/*debugPrintf(" == > ");
	itoa(presSecRet1, numToStr, 10);
	debugPrintf(numToStr);
	debugPrintf(" == > ");
	itoa(eraseSecRet, numToStr, 10);
	debugPrintf(numToStr);
	debugPrintf(" == > ");
	itoa(presecSecRet2, numToStr, 10);
	debugPrintf(numToStr);
	debugPrintf(" \r\n ");*/

	debugPrintf(msg6);

	//NVIC_SystemReset();
	uint8_t curMsgData[16], *pPtr = bootLoaderData;
	int16_t dstartAddr = -1, blockLen=0;
	int16_t curPacketLenLeft = 0;
	uint8_t curSec = 0xff;
	uint16_t lastAddr = 0;
	uint8_t needFlash = 0;
	while(1){
		wifiMsgLen = getWifiNextString(recvBuf, &curPacketLenLeft);
		//wifiMsgLen = recvWifiMsgBlocking(recvBuf);


		debugPrintf(msg7);
		debugPrintflen(recvBuf, wifiMsgLen);

		if(dstartAddr == -1){
			pPtr = bootLoaderData;
			blockLen = 0;
			memset(bootLoaderData, 0, 256);
		}
		uint8_t dLen;
		uint16_t dAddr = 0;
		ret = parseData(recvBuf, &(curMsgData[0]), &dLen, &dAddr);
		if(ret == DATA){

			if( (dstartAddr == -1) || ((lastAddr + dLen) == dAddr) ){
				memcpy(pPtr,  &(curMsgData[0]), dLen);
				pPtr += dLen;
				blockLen += dLen;
//				if(blockLen >= 128){
//					uint8_t secNum = convertAddrToSecNum(dstartAddr);
//					if(secNum != curSec){
//						curSec = secNum;
//						debugPrintf(eraseSec);
//						//Chip_IAP_PreSectorForReadWrite(secNum, secNum);
//						//Chip_IAP_EraseSectorExt(secNum, secNum, SystemCoreClock1000);
//					}
//					debugPrintf(flash);
//					dstartAddr = -1;
//				}

			}
			else{
				debugPrintf(msgAddrStep);
				needFlash = 1;
			}
			lastAddr = dAddr;
			if(dstartAddr == -1){
				dstartAddr = dAddr;
			}
			/*debugPrintf(" getFirmware s=>d:");
			uint8_t i=0;
			for(i=0; i<dLen; i++){
				itoa(curMsgData[i], numToStr, 10);
				debugPrintf(numToStr);
				debugPrintf(" ");
			}
			debugPrintf("\r\n");*/

			/*itoa(blockLen, numToStr, 10);
			debugPrintf(" getFirmware s=>");
			debugPrintf(numToStr);
			debugPrintf("\r\n");*/


		}
		if((ret == EOF) || (blockLen >= 128) || (needFlash == 1)){
			uint8_t secNum = convertAddrToSecNum(dstartAddr);
			uint8_t r = 0;
			if(secNum != curSec){
				curSec = secNum;
				debugPrintf(eraseSec);
				Chip_IAP_PreSectorForReadWrite(secNum, secNum);
				if(r != IAP_CMD_SUCCESS)
					debugPrintf(prepSecErr);
				else
					debugPrintf(msgOk);

				r = Chip_IAP_EraseSectorExt(secNum, secNum, SystemCoreClock1000);
				if(r != IAP_CMD_SUCCESS)
					debugPrintf(msgErsErr);
				else
					debugPrintf(msgOk);

			}
			debugPrintf(flash);

			r = Chip_IAP_PreSectorForReadWrite(secNum, secNum);
			if(r != IAP_CMD_SUCCESS)
				debugPrintf(prepSecErr);
			else
				debugPrintf(msgOk);

			r = Chip_IAP_CopyRamToFlashExt(dstartAddr, (uint32_t*)bootLoaderData, blockLen, SystemCoreClock1000);
			if(r != IAP_CMD_SUCCESS)
				debugPrintf(cpyRamErr);
			else
				debugPrintf(msgOk);

			//itoa(ret, numToStr, 10);
			//debugPrintf(" getFirmware s=> Chip_IAP_PreSectorForReadWrite ret ");
			//debugPrintf(numToStr);
			//debugPrintf("\r\n");

			dstartAddr = -1;
			needFlash = 0;
		}
		if(ret == EOF){
			debugPrintf(msgReset);
			break;
		}
	}
	NVIC_SystemReset();
	return;
//
//	debugPrintf(" getFirmware s=> wait for IPD \r\n");
//	while(1){
//		wifiMsgLen = recvWifiMsg(recvBuf);
//		debugPrintf(" getFirmware s=>");
//		debugPrintflen(recvBuf, wifiMsgLen);
//		if(recvBuf[0] == '+'){
//			char *pch = strstr(recvBuf, "+IPD");
//			if(pch != NULL){
//				debugPrintf(" getFirmware s=> IPD detected. \r\n");
//				//char *msg;
//				parseIPD(recvBuf, &curConnInd, &curPacketLenLeft);
//				debugPrintf(" getFirmware s=> msg:");
//				debugPrintf(recvBuf);
//				debugPrintf("\r\n");
//				break;
//			}
//		}
//	}
//
//	debugPrintf(" getFirmware s=> wait for data \r\n");
//	while(1){
//		wifiMsgLen = recvWifiMsg(recvBuf);
//		curPacketLenLeft -= wifiMsgLen;
//
//		debugPrintf(" getFirmware s=>");
//		debugPrintflen(recvBuf, wifiMsgLen);
//		if(recvBuf[0] == ':'){
//			debugPrintf(" getFirmware s=>Data start detected\r\n");
//			break;
//		}
//	}
//	debugPrintf(" getFirmware s=>Start Data recv ==== \r\n");
//	debugPrintflen(recvBuf, wifiMsgLen);
//	char dataBuffer[50];
//	while(1){
//		wifiMsgLen = recvWifiMsg(recvBuf);
//		debugPrintf(" getFirmware s=>");
//		debugPrintflen(recvBuf, wifiMsgLen);
//
//		itoa(wifiMsgLen, numToStr, 10);
//		debugPrintf(" getFirmware s=>");
//		debugPrintf(numToStr);
//		debugPrintf("\r\n");
//
//		if(curPacketLenLeft > 0){
//			curPacketLenLeft -= wifiMsgLen;
//
//			if((wifiMsgLen-2) >= 3){  //key and Len exists
//				uint8_t dLen = 0;
//				char *parsePtr = &(recvBuf[1]);
//				dLen = parseInt8(parsePtr);
//
//				itoa(dLen, numToStr, 10);
//				debugPrintf(" getFirmware s=>dLen=");
//				debugPrintf(numToStr);
//				debugPrintf("  ");
//				itoa((1+2+4+2+dLen*2+2), numToStr, 10);
//				debugPrintf(numToStr);
//				debugPrintf("\r\n");
//				if((wifiMsgLen-2) >= (1+2+4+2+dLen*2+2)){
//
//				}
//				else{
//					debugPrintf(" getFirmware s=>not enough data\r\n");
//				}
//			}
//			else{
//				memcpy(dataBuffer, recvBuf, (wifiMsgLen-2));
//				dataBuffer[(wifiMsgLen-2)] = '\0';
//				debugPrintf(" getFirmware s=>start fo msg saved");
//				debugPrintf(dataBuffer);
//				debugPrintf("\r\n");
//			}
//
//		}
//		else{
//			parseIPD(recvBuf, &curConnInd, &curPacketLenLeft);
//			debugPrintf(" getFirmware s=> msg:");
//			debugPrintf(recvBuf);
//			debugPrintf("\r\n");
//		}
//
//	}
//	return;
//
//	while(contentLength > 0){
//		//waitWifiMsgAndStartRecv();
//		wifiMsgLen = recvWifiMsg(recvBuf);
//		debugPrintf(" getFirmware s=> recvBuf:");
//		//debugPrintf(recvBuf);
//		debugPrintflen(recvBuf, wifiMsgLen);
//		debugPrintf("\r\n");
//		//char *msg;
//
//		parseIPD(recvBuf, &curConnInd, &curPacketLenLeft);
//
//		debugPrintf(" getFirmware s=> msg:");
//		debugPrintf(recvBuf);
//		debugPrintf("\r\n");
//
//		contentLength -= strlen(recvBuf);
//		contentLength -= curPacketLenLeft;
//
//		/*while(msgLength > 0){
//			uint16_t wifiMsgLen = waitWifiMsgAndStartRecv();
//			debugPrintf(" httpServ => ");
//			itoa(msgLength, numToStr, 10);
//			debugPrintf(numToStr);
//			debugPrintf(" | ");
//			itoa(wifiMsgLen, numToStr, 10);
//			debugPrintf(numToStr);
//			msgLength -= wifiMsgLen;
//			debugPrintf("\r\n");
//		}*/
//
//		//uint8_t strInd = 0;
//		while(curPacketLenLeft > 0){
//			wifiMsgLen = recvWifiMsg(recvBuf);
//			debugPrintf(" getFirmware->");
//			debugPrintflen(recvBuf, wifiMsgLen);
//			curPacketLenLeft -= wifiMsgLen;
//
//			/*debugPrintf(" => ");
//			itoa(wifiMsgLen, numToStr, 10);
//			debugPrintf(numToStr);
//			debugPrintf(" rn ");
//			itoa(curPacketLenLeft, numToStr, 10);
//			debugPrintf(numToStr);
//			debugPrintf(" chs >> ");
//			itoa(msgLength, numToStr, 10);
//			debugPrintf(numToStr);
//			debugPrintf(" chs >> ");*/
//
//			if(recvBuf[0] != ':'){
//				debugPrintf(" not data\r\n");
//				continue;
//			}
//
//
//			uint8_t dLen = 0, dType=0, dData[16], dCrc = 0;
//			uint16_t dAddr = 0;
//
//			char *parsePtr = &(recvBuf[1]);
//			dLen = parseInt8(parsePtr);
//			parsePtr+=2;
//			dAddr = parseInt16(parsePtr);
//			parsePtr+=4;
//			dType = parseInt8(parsePtr);
//			parsePtr+=2;
//
//			/*debugPrintf("l");
//			itoa(dLen, numToStr, 16);
//			debugPrintf(numToStr);
//			debugPrintf(".a");
//			itoa(dAddr, numToStr, 16);
//			debugPrintf(numToStr);
//			debugPrintf(".t");
//			itoa(dType, numToStr, 16);
//			debugPrintf(numToStr);
//			debugPrintf(" ");*/
//
//			uint8_t cc = 0;
//			cc+=dLen;
//			cc+=dType;
//			cc+=(dAddr&0xff);
//			cc+=((dAddr>>8)&0xff);
//			uint8_t i;
//			for( i=0;i<dLen;i++){
//				dData[i] = parseInt8(parsePtr);
//				cc += dData[i];
//				parsePtr += 2;
//
//				/*itoa(dData[i], numToStr, 16);
//				debugPrintf(numToStr);
//				debugPrintf(".");*/
//			}
//
//			cc = convertTwosCompl(cc);
//			/*debugPrintf(" cc");
//			itoa(cc, numToStr, 16);
//			debugPrintf(numToStr);*/
//
//			dCrc = parseInt8(parsePtr);
//			/*debugPrintf(" c");
//			itoa(dCrc, numToStr, 16);
//			debugPrintf(numToStr);
//
//
//			debugPrintf(" chs >> ");
//			debugPrintf("data:");*/
//			if(dCrc != cc)
//				debugPrintf(" crc fail! \r\n");
//			//debugPrintflen(recvBuf, wifiMsgLen);
//
//		}
//
//		itoa(contentLength, numToStr, 10);
//		debugPrintf("-----> ");
//		debugPrintf(numToStr);
//		debugPrintf("\r\n");
//	}
//	debugPrintf(" ---- firmware end ----  \r\n");
}

