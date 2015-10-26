#pragma once
#include <string.h>
#include "chip.h"


void delayMs(uint16_t msec);

#define ASSERT(x) if((x) == 0){debugPrintf("ASSERTION!!\r\n"); while(1);}

#define MEMCMPx(a, b) memcmp(a, b, strlen(b)-1)

#define INFINITY -1

void setwifiBaudRate(uint32_t baudrate);

uint16_t parseInt16(char *str);
uint8_t parseInt8(char *str);
uint8_t parseInt4(char ch);
