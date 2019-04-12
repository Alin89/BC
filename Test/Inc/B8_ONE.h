#ifndef __B8_ONE_H
#define __B8_ONE_H
#include "stm32l4xx_hal.h"
#include "main.h"
#include "string.h"
#define u8			unsigned char
#define u16			unsigned short
#define u32			unsigned int
#define MIPLDELETE    		"AT+MIPLCREATE\r\n"							//����OneNetͨ��ʵ��  
#define MIPLADDOBJ				"AT+MIPLADDOBJ=0,"							//���LWM2M����
#define MIPLOPEN					"AT+MIPLOPEN=0,38400\r\n"				//��OneNet����ע������
#define MIPLDISCOVERRSP		"AT+MIPLDISCOVERRSP=0,"					//ע����Դ
#define MIPLUPDATE				"AT+MIPLUPDATE=0,9600,1\r\n"		//���¶���
#define MIPLREADRSP				"AT+MIPLREADRSP="								//ģ����Ӧ
#define MIPLNOTIFY				"AT+MIPLNOTIFY=0"								//��ƽ̨��������
#define MIPLCLOSE					"AT+MIPLCLOSE=0\r\n"						//ע��ʵ��
#define MIPLDELOBJ				"AT+MIPLDELOBJ=0,"							//ɾ��LWM2M����
#define ALL_Del						"AT+MIPLDELETE=0\r\n"						//ɾ������
#define send_len           256
#define RX_LEN						 256

extern char USART3_RX_BUF[RX_LEN];
extern char usart3_rcv_len;
extern char msgidch[6];

char on_line(UART_HandleTypeDef *huart,char* objid,char* msgid,char* insBitmap,char* vlestr,char attrCount,char actCount);

char Create_one(UART_HandleTypeDef *huart,int timeout);
char Create_obj(UART_HandleTypeDef *huart,char* objid,char* insBitmap,u8 attrCount,u8 actCount,int timeout);
char mipldiscoverrps(UART_HandleTypeDef *huart,char* msgid,char* str);
char login(UART_HandleTypeDef *huart,char* msgid,char* vlestr,int timeout);

char send_message(UART_HandleTypeDef *huart,char* msgid,char* objid,char insid,char*resid,char vletp,char len,char* value);

char closeone(UART_HandleTypeDef *huart);
char del_obj(UART_HandleTypeDef *huart,char *obj);
char all_del(UART_HandleTypeDef *huart);
char off_line(UART_HandleTypeDef *huart,char* obj);
#endif
