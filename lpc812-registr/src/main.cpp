/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC8xx.h"
#endif


//#include "chip.h"
#include <cr_section_macros.h>

#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "uartUtils.h"

#define SysTick_VALUE (SystemCoreClock/1000 - 1)


void prvSetupHardware();

//#define UART_RB_SIZE 128
/* Transmit and receive ring buffers */
//RINGBUFF_T rxring;
/* Transmit and receive buffers */
//__NOINIT_DEF uint8_t rxbuff[UART_RB_SIZE];

void vUartTask ();

#define RINGBUF_FULL_BITNUM 0
#define UARTTMPBUF_FULL_BITNUM 1
uint8_t errword;

int main(void)
{
	//uint64_t cnt_15s=15000, cnt_1ms=0, cnt_10ms=0, cnt_100ms=0;
	prvSetupHardware();

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 17);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 17, false);

	SysTick_Config(SysTick_VALUE);

	//while(SysTickCnt <2000) ;
	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	//RingBuffer_Init(&rxring, rxbuff, 1, UART_RB_SIZE);

	memset(wifiApList, 0, sizeof(wifiApList));

	//while(1) ;
	debugPrintf("starting main\r\n");


	debugPrintf("core clock ");
	char numToStr[15];
	itoa(SystemCoreClock, &(numToStr[0]), 10);
	debugPrintf("core clock ");
	debugPrintf(numToStr);
	debugPrintf(" Hz\r\n");

	/*debugPrintf("stack top 0x");
	extern void _vStackTop;
	char numToStr[10];
	itoa(_vStackTop, &(numToStr[0]), 10);
	debugPrintf(numToStr);
	debugPrintf(" \r\n");*/

	//readUUU("ready\r\n");
	while(1){
		waitWiFiMsg();
		//debugPrintf(" ->");
		//debugPrintf(uart1Buffer);
		if(strcmp(uart1Buffer, "ready\r\n") == 0){
			break;
		}
		enableWiFiMsg();
	}

	debugPrintf("ready detected\r\n");

	Chip_SWM_MovablePinAssign(SWM_U1_CTS_I, 17);

	while(1){
		wifiPrintf("AT+UART_CUR=115200,8,1,0,3\r\n");
		waitWiFiMsg();
		debugPrintf(" ->");
		debugPrintf(uart1Buffer);
		if(strcmp(uart1Buffer, "OK\r\n") == 0){
			break;
		}
		enableWiFiMsg();
	}
	//wifiPrintf("AT+UART_CUR=115200,8,1,0,3\r\n");
	waitForRespOK();

	debugPrintf("send ate0\r\n");
	wifiPrintf("ATE0\r\n");
	waitForRespOK();

	debugPrintf("wait for wifi state\r\n");
	while(1){
		TCmdType cmdType = getNextWifiCmd(INFINITY, true).type;
		if( cmdType == wifi_gotip)
			break;
		if(cmdType == wifi_discon){
			debugPrintf("AT+CWJAP_CUR=\"YOTA\",\"kkkknnnn\"\r\n");
			wifiPrintf("AT+CWJAP_CUR=\"YOTA\",\"kkkknnnn\"\r\n");
			//debugPrintf("AT+CWJAP_DEF=\"TL-WR842ND\",\"kkkknnnn\"\r\n");
			//wifiPrintf("AT+CWJAP_DEF=\"TL-WR842ND\",\"kkkknnnn\"\r\n");
			waitForRespOK();
			break;
		}
	}

	//debugPrintf("delay\r\n");
	//delayMs(1500);

	//debugPrintf("turn off AT+CWAUTOCON\r\n");
	//wifiPrintf("AT+CWAUTOCONN=1\r\n");
	//waitForRespOK();


	//debugPrintf("wait for ok\r\n");
	//waitForRespOK();

	debugPrintf("AT+CIPMUX=1\r\n");
	wifiPrintf("AT+CIPMUX=1\r\n");
	waitForRespOK();

	debugPrintf("AT+CIPSERVER=1\r\n");
	wifiPrintf("AT+CIPSERVER=1,80\r\n");
	waitForRespOK();

	//wifiPrintf("AT+CIFSR\r\n");
	//readLLL("OK\r\n");

	//wifiPrintf("AT+CWMODE_DEF=3\r\n");
	//readLLL("OK\r\n");

	//wifiPrintf("AT+CWAUTOCONN?\r\n");
	//readLLL("OK\r\n");

	//wifiPrintf("AT+CWDHCP_CUR?\r\n");
	//readLLL("OK\r\n");

//	debugPrintf("AT+CWLAP=1\r\n");
//	wifiPrintf("AT+CWLAP\r\n");
//	waitForRespOK();

//	debugPrintf("Wait AT+CWLAP resp OK! \r\n");

	debugPrintf(" trying to connect\r\n");

	//wifiPrintf("AT+CWQAP\r\n");
	//waitForRespOK();

	//debugPrintf("AT+CWJAP_DEF=\"TL-WR842ND\",\"kkkknnnn\"\r\n");
	//wifiPrintf("AT+CWJAP_DEF=\"TL-WR842ND\",\"kkkknnnn\"\r\n");
	//readLLL("OK\r\n");

	wifiPrintf("AT+CIFSR\r\n");
	waitForRespOK();

	debugPrintf("init ok\r\n");

	while(1) {
		vHttpServerTask ();
//		TCmd cmd = getNextWifiCmd(INFINITY, true);
//		waitWiFiMsg();
//		uint16_t msgLen = getUartIrqMsgLength();
//		memcpy(uart1ProcBuffer, uart1Buffer, BUF_LEN);
//		enableWiFiMsg();
//
//		debugPrintf("got msg! -> ");
//		debugPrintflen(uart1ProcBuffer, msgLen);
//
//		TCmdType cmdType = UNKNWON;
//		parseCommand(cmdType, uart1ProcBuffer, true);

		//processMsg();

    	//vUartTask();
    	//debugPrintf("s\r\n");
//    	if(errword){
//    		debugPrintf("err ");
//    		if(errword&(1<<RINGBUF_FULL_BITNUM))
//    			debugPrintf("RINGBUF_FULL!");
//    		if(errword&(1<<UARTTMPBUF_FULL_BITNUM))
//    		    debugPrintf("UARTTMPBUF_FULL!");
//    		debugPrintf("\r\n");
//    		while(1);
//    	}

//        if( SysTickCnt >= cnt_100ms ){
//            cnt_100ms = SysTickCnt+100;
//        }
//        if( SysTickCnt >= cnt_15s ){
//        	cnt_15s = SysTickCnt+15000;
//        	//debugPrintf("time to rescan wifi\r\n");
//        	//scanWiFiAp();
//        }
        //	char iToStr[10];
        //	if(SysTickCnt > 1000){
        //		SysTickCnt = 0;
        		//debugPrintf("sec \r\n");
        //		if(secondsCount++ >= 59){
        //			secondsCount = 0;
        //			minutsCount++;
        //			itoa(minutsCount, iToStr, 10);
        //			debugPrintf("min ");
        //			debugPrintf(iToStr);
        //			debugPrintf("\r\n");
        //		}
        //	}
    }
    return 0 ;
}

//#define TEMPRECVBUF_SIZE 256
//extern int16_t msgLength;

volatile uint64_t SysTickCnt = 0;
extern "C" void SysTick_Handler(void)
{
	SysTickCnt++;
}

extern "C" void NMI_Handler(void)
{
	debugPrintf("NMI_Handler\r\n");
    while(1)
    {
    }
}

extern "C" void HardFault_Handler(void)
{
	debugPrintf("HardFault_Handler\r\n");
    while(1)
    {
    }
}

extern "C" void SVC_Handler(void)
{
	debugPrintf("SVC_Handler\r\n");
    while(1)
    {
    }
}

extern "C" void PendSV_Handler(void)
{
	debugPrintf("PendSV_Handler\r\n");
    while(1)
    {
    }
}
