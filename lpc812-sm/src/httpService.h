#pragma once
void parseHttpReq(char *str, THtmlReqType *htmlReqType);
void parsePostReqHead(char *str, TCmd *cmd, uint16_t msgLen, uint8_t strInd);
void parsePostReq(char *str, uint16_t msgLen, uint8_t strInd);
void vHttpServerTask ();
