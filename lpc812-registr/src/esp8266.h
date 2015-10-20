#pragma once
#include <stdint.h>

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

typedef struct {
	uint8_t secureType;
	int8_t	rssi;
	char name[10];
} TWifiAp;

#define WIFI_APLISTMAX 10
extern TWifiAp wifiApList[WIFI_APLISTMAX];

extern char APIP[20], STAPIP[20];

//TCmdType blockWaitCmd();
void blockWaitSendOK();

TCmd getNextWifiCmd(TCmd &cmd, int16_t to_msec);

void startSendWifiDataWithLen(uint16_t strLen, uint8_t connId, uint8_t segInd);
void sendWifiData(char *str, uint8_t connId);
void sendCipClose(uint8_t id);

void waitForRespOK();

