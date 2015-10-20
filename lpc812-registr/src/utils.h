#pragma once
#include <string.h>
#include "chip.h"


void delayMs(uint16_t msec);

#define ASSERT(x) if((x) == 0){debugPrintf("ASSERTION!!\r\n"); while(1);}

#define MEMCMPx(a, b) memcmp(a, b, strlen(b)-1)



#define INFINITY -1

/* SysTick constants */
//#define SYSTICK_ENABLE              0                                          /* Config-Bit to start or stop the SysTick Timer                         */
//#define SYSTICK_TICKINT             1                                          /* Config-Bit to enable or disable the SysTick interrupt                 */
//#define SYSTICK_CLKSOURCE           2                                          /* Clocksource has the offset 2 in SysTick Control and Status Register   */
//#define SYSTICK_MAXCOUNT       ((1<<24) -1)                                    /* SysTick MaxCount */
//uint32_t SysTick_Config(uint32_t ticks);


//
//void waitForRespOKorRespError();

//void resetApList();


