#pragma once
#include <string.h>
#include "chip.h"


void delayMs(uint16_t msec);

#define ASSERT(x) if((x) == 0){debugPrintf("ASSERTION!!\r\n"); while(1);}

#define MEMCMPx(a, b) memcmp(a, b, strlen(b)-1)

#define INFINITY -1

void setwifiBaudRate(uint32_t baudrate);
