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


void prvSetupHardware();

#define UART_RB_SIZE 1024
/* Transmit and receive ring buffers */
RINGBUFF_T rxring;
/* Transmit and receive buffers */
__NOINIT_DEF uint8_t rxbuff[UART_RB_SIZE];

void vUartTask ();

#define RINGBUF_FULL_BITNUM 0
#define UARTTMPBUF_FULL_BITNUM 1
uint8_t errword;

int main(void)
{
	prvSetupHardware();

	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RB_SIZE);

	debugPrintf("starting main\r\n");
    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
    	vUartTask();
    	//debugPrintf("s\r\n");
    	if(errword){
    		debugPrintf("err ");
    		if(errword&(1<<RINGBUF_FULL_BITNUM))
    			debugPrintf("RINGBUF_FULL!");
    		if(errword&(1<<UARTTMPBUF_FULL_BITNUM))
    		    debugPrintf("UARTTMPBUF_FULL!");
    		debugPrintf("\r\n");
    		while(1);
    	}

        i++ ;
    }
    return 0 ;
}

#define TEMPRECVBUF_SIZE 256
extern int16_t msgLength;
void vUartTask ()
{
	static char tempRecvBuf[TEMPRECVBUF_SIZE];
	static uint8_t tempRecvBufInd = 0;

	while(RingBuffer_Pop(&rxring, &(tempRecvBuf[tempRecvBufInd]))){
		if( (tempRecvBuf[tempRecvBufInd] == '\n') ||
			((tempRecvBuf[0] == '>') && (tempRecvBufInd==1))
		  )
		{
			tempRecvBufInd++;
			tempRecvBuf[tempRecvBufInd] = '\0';

			if(tempRecvBufInd >= TEMPRECVBUF_SIZE)
				errword |= (1<<UARTTMPBUF_FULL_BITNUM);
			//tempRecvBufInd++;
			//uint16_t tempRecvBufLen = sizeof(tempRecvBuf);
			Chip_UART_SendBlocking(LPC_USART0, tempRecvBuf, tempRecvBufInd);
			TCmdType cmdType = UNKNWON;
			parseCommand(cmdType, tempRecvBuf);
			processMsg(cmdType, tempRecvBufInd);
			tempRecvBufInd = 0 ;
		}
		else{
			if((msgLength > 0) && (tempRecvBufInd == (msgLength-1))){
				tempRecvBufInd++;
				tempRecvBuf[tempRecvBufInd] = '\0';
				debugPrintf("!!! affected !!! \r\n");
				Chip_UART_SendBlocking(LPC_USART0, tempRecvBuf, tempRecvBufInd);
				TCmdType cmdType = TEXT;
				processMsg(cmdType, tempRecvBufInd);
				tempRecvBufInd = 0 ;
			}
			else
				tempRecvBufInd++;
		}
	}
}

//void LPC_UARTHNDLR(void)
extern "C" void UART1_IRQHandler( void )
{
	/* New data will be ignored if data not popped in time */
	while ((Chip_UART_GetStatus(LPC_USART1) & UART_STAT_RXRDY) != 0) {
		uint8_t ch = Chip_UART_ReadByte(LPC_USART1);
		if(RingBuffer_Insert(&rxring, &ch) == 0)
			errword |= (1<<RINGBUF_FULL_BITNUM);

	}
}
