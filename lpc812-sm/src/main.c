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


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "uartUtils.h"

#include "esp8266.h"
#include "httpService.h"

#include "utils.h"

#define SysTick_VALUE (SystemCoreClock/1000 - 1)

//#define UART_RB_SIZE 128
/* Transmit and receive ring buffers */
//RINGBUFF_T rxring;
/* Transmit and receive buffers */
//__NOINIT_DEF uint8_t rxbuff[UART_RB_SIZE];

void vUartTask ();

#define RINGBUF_FULL_BITNUM 0
#define UARTTMPBUF_FULL_BITNUM 1
uint8_t errword;

__RAM_FUNC int main(void)
{
	//uint64_t cnt_15s=15000, cnt_1ms=0, cnt_10ms=0, cnt_100ms=0;
	prvSetupHardware();

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 17);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, 17, false);

	//SysTick_Config(SysTick_VALUE);

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
	TCmd cmd;
	while(1){
		getNextWifiCmd(&cmd, INFINITY);
		if( cmd.type == wifi_gotip)
			break;
		if( cmd.type == wifi_discon){
			//debugPrintf("AT+CWJAP_CUR=\"YOTA\",\"kkkknnnn\"\r\n");
			//wifiPrintf("AT+CWJAP_CUR=\"YOTA\",\"kkkknnnn\"\r\n");
			debugPrintf("AT+CWJAP_DEF=\"TL-WR842ND\",\"kkkknnnn\"\r\n");
			wifiPrintf("AT+CWJAP_DEF=\"TL-WR842ND\",\"kkkknnnn\"\r\n");
			waitForRespOK();
			break;
		}
	}

	//setwifiBaudRate(921600);

	//waitForRespOK();

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
	//waitForRespOK();

	//wifiPrintf("AT+CWMODE_DEF=3\r\n");
	//readLLL("OK\r\n");

	//wifiPrintf("AT+CWAUTOCONN?\r\n");
	//readLLL("OK\r\n");

	//wifiPrintf("AT+CWDHCP_CUR?\r\n");
	//readLLL("OK\r\n");

	//debugPrintf("AT+CWLAP=1\r\n");
	//wifiPrintf("AT+CWLAP\r\n");
	//waitForRespOK();

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

//	char dd1[] = "EC";
//	char dd2[] = "BEAF";
//
//	uint8_t u8Parsed = parseInt8(dd1);
//	uint16_t u16Parsed = parseInt16(dd2);
//
//	itoa(u8Parsed, numToStr, 16);
//	debugPrintf(numToStr);
//	debugPrintf("  ");
//	itoa(u16Parsed, numToStr, 16);
//	debugPrintf(numToStr);
//	debugPrintf("\r\n");


	while(1) {
		vHttpServerTask ();
    }
    return 0 ;
}

//#define TEMPRECVBUF_SIZE 256
//extern int16_t msgLength;

volatile uint64_t SysTickCnt = 0;
void SysTick_Handler(void)
{
	SysTickCnt++;
}

void NMI_Handler(void)
{
	debugPrintf("NMI_Handler\r\n");
    while(1)
    {
    }
}

void HardFault_Handler(void)
{
	debugPrintf("HardFault_Handler\r\n");
    while(1)
    {
    }
}

void SVC_Handler(void)
{
	debugPrintf("SVC_Handler\r\n");
    while(1)
    {
    }
}

void PendSV_Handler(void)
{
	debugPrintf("PendSV_Handler\r\n");
    while(1)
    {
    }
}
