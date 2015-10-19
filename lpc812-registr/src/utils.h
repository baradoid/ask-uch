#pragma once

#include "chip.h"

void delayMs(uint16_t msec);

#define ASSERT(x) if((x) == 0){debugPrintf("ASSERTION!!\r\n"); while(1);}

#define MEMCMPx(a, b) memcmp(a, b, strlen(b)-1)

void vHttpServerTask ();

typedef enum {
	ready,
	wifi_discon,
	wifi_conn,
	wifi_gotip,
	IPD,
	CIFSR_APIP,
	CIFSR_APMAC,
	CIFSR_STAIP,
	CIFSR_STAMAC,
	CIPSTO,
	CMD_OK,
	RESP_OK,
	RESP_ERROR,
	CMD_CONNECT,
	CMD_CLOSED,
	TEXT,
	RESP_SEND_OK,
	SEND_READY,
	busy_s,
	ready_to_send,
	recv_bytes_report,
	UNKNWON
} TCmdType;

typedef enum{
	GET_ROOT,
	GET_FAVICON,
	POST
} THtmlReqType;



typedef struct {
	TCmdType type;
	union{
		struct{
			uint8_t curConnInd;
			THtmlReqType htmlReqType;
		};
	};

} TCmd;


#define INFINITY -1
//void parseCommand(TCmdType &cmdType, char *wifiMsg);
TCmd getNextWifiCmd(int16_t to_msec, bool debugPrintf);
void processMsg();
void scanWiFiAp();

/* SysTick constants */
//#define SYSTICK_ENABLE              0                                          /* Config-Bit to start or stop the SysTick Timer                         */
//#define SYSTICK_TICKINT             1                                          /* Config-Bit to enable or disable the SysTick interrupt                 */
//#define SYSTICK_CLKSOURCE           2                                          /* Clocksource has the offset 2 in SysTick Control and Status Register   */
//#define SYSTICK_MAXCOUNT       ((1<<24) -1)                                    /* SysTick MaxCount */
//uint32_t SysTick_Config(uint32_t ticks);


typedef struct {
	uint8_t secureType;
	int8_t	rssi;
	char name[10];
} TWifiAp;

#define WIFI_APLISTMAX 10
extern TWifiAp wifiApList[WIFI_APLISTMAX];

void waitForRespOK();
void waitForRespOKorRespError();

void resetApList();

TCmdType blockWaitCmd(bool debugPrintf);

