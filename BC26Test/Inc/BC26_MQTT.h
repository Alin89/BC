#ifndef __BC26_MQTT_H
#define __BC26_MQTT_H

#include "stm32l4xx_hal.h"
#include "string.h"
#include "main.h"

#define Product_Key			"a10chKupWsh"
#define Device_Name			"bc26test"
#define Device_Secret		"cdDirLNp6mibV17FvBUVRNNYd8fJzetW"

#define Topic						"/a10chKupWsh/bc26test/user/upload"
#define Host_Name				"iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define PORT						"1883"

typedef struct
{
	char MQTTID;
	char *msgid;
	char qos;
	char retain;
	char* result;
	char* clientID;
}MQTT;

extern MQTT mqtt;

extern unsigned char u2_rx_buf[255];
extern unsigned char u2_rx_len;
extern unsigned char u3_rx_buf[255];
extern unsigned char u3_rx_len;


void Aliyunonline(UART_HandleTypeDef* huart);
void SubTpoic(UART_HandleTypeDef* huart,char* topic);
void PUBMSG(UART_HandleTypeDef* huart,char* topic,char* msg);
void offline(UART_HandleTypeDef* huart);

#endif

