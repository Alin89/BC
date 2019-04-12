#include "B8_ONE.h"
char msgidch[6];
char USART3_RX_BUF[RX_LEN];
char usart3_rcv_len;
/*-----------------------设备初始化-------------------------*/
char on_line(UART_HandleTypeDef *huart,char* objid,char* msgid,char* insBitmap,char* vlestr,char attrCount,char actCount)
{
	char flag=0;
	while(1)
	{
		HAL_UART_Transmit(huart,"AT\r\n",5,1000);
		HAL_Delay(6000);
		if(NULL!=strstr((const char*)USART3_RX_BUF,"OK"))
		{
			memset(USART3_RX_BUF,0,strlen((const char*)USART3_RX_BUF));
			usart3_rcv_len=0;
			break;
		}
	}
	
	a:
	if(Create_one(huart,5000))
	{
		//暂不做处理
	}
	else
	{
		flag=101;
		off_line(huart,objid);
		HAL_Delay(500);goto a;
	}
	if(Create_obj(huart,objid,insBitmap,attrCount,actCount,5000))
	{
		//暂不做处理
	}
	else
	{
		flag=102;off_line(huart,objid);
		HAL_Delay(500);;goto a;
	}
	if(0!=login(huart,msgid,vlestr,35))
	{
		//暂不做处理
	}
	else
	{
		flag=103;off_line(huart,objid);
		HAL_Delay(500);;goto a;
	}
	
	return 1;
}

/*------------------创建ONENET通讯实例------------------*/
char Create_one(UART_HandleTypeDef *huart,int timeout)
{
	memset(USART3_RX_BUF,0,strlen((const char*)USART3_RX_BUF));
	usart3_rcv_len=0;
	HAL_UART_Transmit(huart,(u8*)MIPLDELETE,strlen(MIPLDELETE),500);
	
	HAL_Delay(timeout);
		
	if(NULL!=strstr((const char*)USART3_RX_BUF,"OK"))
	{
	//	HAL_UART_Transmit(huart,"Create OneNet Success!\n",24,1000);
		memset(USART3_RX_BUF,0,strlen((const char*)USART3_RX_BUF));
		usart3_rcv_len=0;
		return 1;
	}
	else
	{
	//	HAL_UART_Transmit(huart,"Create OneNet Failed!\n",23,1000);
		memset(USART3_RX_BUF,0,strlen((const char*)USART3_RX_BUF));
		usart3_rcv_len=0;
		return 0;
	}
}


/*------------------创建LWM2M对象-----------------------*/
char Create_obj(UART_HandleTypeDef *huart,char* objid,char* insBitmap,u8 attrCount,u8 actCount,int timeout)
{
	u8 message[send_len];
	memset(USART3_RX_BUF,0,strlen((const char*)USART3_RX_BUF));
	usart3_rcv_len=0;
	sprintf((char*)message,"%s%s,%d,\"%s\",%d,%d\r\n",MIPLADDOBJ,objid,strlen(insBitmap),insBitmap,attrCount,actCount);
	
	HAL_UART_Transmit(huart,message,strlen((const char*)message),1000);
	
	HAL_Delay(timeout);
	
	if(NULL!=strstr((const char*)USART3_RX_BUF,"OK"))
	{
	//	HAL_UART_Transmit(huart,"Create object success!\n",24,1000);
		return 1;
	}
	else
	{
	//	HAL_UART_Transmit(huart,"Create object failed!\n",23,1000);
		return 0;
	}
}
/*-------------向ONENET平台发送注册请求-----------------*/
char login(UART_HandleTypeDef *huart,char* msgid,char* vlestr,int timeout)
{
	u8 message[send_len],buffer[send_len],i;
	char* p1;
	char* p2;
	char* msgid2;
	memset(USART3_RX_BUF,0,strlen((const char*)USART3_RX_BUF));usart3_rcv_len=0;
	strcpy((char*)message,MIPLOPEN);
	HAL_UART_Transmit(huart,message,strlen((const char*)message),1000);
	memset(message,0,strlen((const char*)message));
	
	while(1)
	{
		if(NULL!=strstr((const char*)USART3_RX_BUF,"-1"))
			break;
	}
	
	strcpy((char*)buffer,(char*)USART3_RX_BUF);
	memset(USART3_RX_BUF,0,strlen((const char*)USART3_RX_BUF));
	usart3_rcv_len=0;
	if(NULL!=strstr((const char*)buffer,"+MIPLEVENT: 0,6"))
	{
		p1=(char*)buffer;
		p1=strrchr((const char*)buffer,':');
		msgid=strtok(p1,",");
		msgid=strtok(NULL,",");
		strcpy(msgidch,msgid);
		sprintf((char*)message,"AT+MIPLOBSERVERSP=0,%s,1\r\n",msgid);
		
		HAL_UART_Transmit(huart,message,strlen((const char*)message),1000);
	}
	
	memset(buffer,0,sizeof(buffer));
	for(i=0;i<6;i++)
	{
		HAL_Delay(1000);
	}
	
	strcpy((char*)buffer,(char*)USART3_RX_BUF);
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));usart3_rcv_len=0;
	
	if(NULL!=strstr((const char*)buffer,"OK"))
	{
		if(NULL!=strstr((const char*)buffer,"+MIPLDISCOVER:"))
		{
			memset(message,0,strlen((const char*)message));
			p2=(char*)buffer;
			p2=strrchr((const char*)buffer,':');
			msgid2=strtok(p2,",");
			msgid2=strtok(NULL,",");
			sprintf((char*)message,"%s%s,1,%d,\"%s\"\r\n",MIPLDISCOVERRSP,msgid2,strlen(vlestr),vlestr);
			HAL_UART_Transmit(huart,message,strlen((const char*)message),1000);
		}
		else
		{
		//	HAL_UART_Transmit(huart,"login failed!\n",15,1000);
			return 0;
			//return
		}
	}
	memset(buffer,0,sizeof(buffer));
	for(i=0;i<timeout;i++)
	{
		HAL_Delay(1000);
	}
	strcpy((char*)buffer,(char*)USART3_RX_BUF);
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));usart3_rcv_len=0;
	if(NULL!=strstr((char*)buffer,"OK"))
	{
	//	HAL_UART_Transmit(huart,"login success!\n",16,1000);
		return 1;
	}
	else
	//	HAL_UART_Transmit(huart,"login failed!\n",15,1000);
		return 0;
	
	
}
/*---------------------上传数据-------------------------*/
char send_message(UART_HandleTypeDef *huart,char* msgid,char* objid,char insid,char*resid,char vletp,char len,char* value)
{
	u8 message[send_len];
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));usart3_rcv_len=0;
	sprintf((char*)message,"%s,%s,%s,%d,%s,%d,%d,%s,0,0\r\n",MIPLNOTIFY,msgid,objid,insid,resid,vletp,len,value);
	HAL_UART_Transmit(huart,message,strlen((const char*)message),1000);
	
	HAL_Delay(6000);
	
	if(NULL!=strstr((char*)USART3_RX_BUF,"OK"))
	{
	//	HAL_UART_Transmit(huart,"send success!\n",15,1000);
		return 1;
	}
	else
	//	HAL_UART_Transmit(huart,"send failed!\n",14,1000);
		return 0;
}



/*---------------------设备下线-------------------------*/
char off_line(UART_HandleTypeDef *huart,char* obj)
{
	closeone(huart);
  HAL_Delay(500);
	del_obj(huart,obj);
  HAL_Delay(500);
	all_del(huart);
  HAL_Delay(500);
	return 1;
}




/*---------------------注销实例-------------------------*/
char closeone(UART_HandleTypeDef *huart)
{
	u8 message[send_len];
	strcpy((char*)message,MIPLCLOSE);
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));usart3_rcv_len=0;
	HAL_UART_Transmit(huart,message,strlen((const char*)message),1000);
	
	HAL_Delay(3000);
	
	if(NULL!=strstr((char*)USART3_RX_BUF,"OK"))
	{
		memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));usart3_rcv_len=0;
		return 1;
	}
	else
		return 0;
	
}

/*------------------删除LWM2M对象-----------------------*/
char del_obj(UART_HandleTypeDef *huart,char *obj)
{
	u8 message[send_len];
	sprintf((char*)message,"%s%s%s",MIPLDELOBJ,obj,"\r\n");
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));usart3_rcv_len=0;
	HAL_UART_Transmit(huart,message,strlen((const char*)message),1000);
	
	HAL_Delay(3000);
	
	if(NULL!=strstr((char*)USART3_RX_BUF,"OK"))
	{
		memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));usart3_rcv_len=0;
		return 1;
	}
	else
		return 0;
	
}
/*-------------------删除对象--------------------------*/ 
char all_del(UART_HandleTypeDef *huart)
{
	u8 message[send_len];
	strcpy((char*)message,ALL_Del);
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));usart3_rcv_len=0;
	HAL_UART_Transmit(huart,message,strlen((const char*)message),1000);

	HAL_Delay(3000);

	
	if(NULL!=strstr((char*)USART3_RX_BUF,"OK"))
	{
		memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));usart3_rcv_len=0;
		return 1;
	}
	else
		return 0;
}





