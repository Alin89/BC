#include "BC26_MQTT.h"

MQTT mqtt;
char resultvlu[50];

	/*------------------------------------------------------------------
	*****************************设备上线*******************************
	------------------------------------------------------------------*/

void Aliyunonline(UART_HandleTypeDef* huart)
{
	unsigned char message[200];
	
	/*---------------------------适配波特率-----------------------------*/
	int i=0;
	while(NULL==strstr((const char*)u3_rx_buf,"OK"))
	{
		HAL_UART_Transmit(huart,"AT\r\n",5,0xFFFF);
		HAL_Delay(2000);
		i++;
		if(i>10)
		{
			uart2out("AT timeout!\r\n\r\n");i=0;break;
		}	
	}
	
	/*----------------------------MQTT配置------------------------------*/
	a:
	i=0;
	memset(message,0,sizeof(message));
	memset(u3_rx_buf,0,sizeof(u3_rx_buf));u3_rx_len=0;
	sprintf((char*)message,"AT+QMTCFG=\"ALIAUTH\",%d,\"%s\",\"%s\",\"%s\"\r\n",mqtt.MQTTID,Product_Key,Device_Name,Device_Secret);
	HAL_UART_Transmit(huart,message,strlen((char*)message),0xFFFF);

	while(1)
	{
		if(NULL!=strstr((const char*)u3_rx_buf,"OK"))
		{
			uart2out("MQTT配置成功!\r\n\r\n");
			break;
		}
		else if(NULL!=strstr((const char*)u3_rx_buf,"ERROR"))
		{
			uart2out("ERROR!\r\n");break;
		}
		else if(i>10)
		{
			uart2out("MQTT配置超时！\r\n\r\n");i=0;break;
		}
		HAL_Delay(1000);i++;
		
	}
	
	/*----------------------------打开网络连接------------------------------*/
	i=0;
	memset(message,0,sizeof(message));
	memset(u3_rx_buf,0,sizeof(u3_rx_buf));u3_rx_len=0;
	memset(resultvlu,0,sizeof(resultvlu));
	sprintf(resultvlu,"+QMTOPEN: %d,0",mqtt.MQTTID);
	sprintf((char*)message,"AT+QMTOPEN=%d,\"%s\",%s\r\n",mqtt.MQTTID,Host_Name,PORT);
	HAL_UART_Transmit(huart,message,strlen((char*)message),0xFFFF);
	
	while(1)
	{
		if(NULL!=strstr((const char*)u3_rx_buf,resultvlu))
		{
			uart2out("打开网络成功!\r\n\r\n");
			break;
		}
		else if(NULL!=strstr((const char*)u3_rx_buf,"-1"))
		{
			uart2out("打开网络失败!\r\n\r\n");goto a;
		}
		else if(NULL!=strstr((const char*)u3_rx_buf,"ERROR"))
		{
			uart2out("fail!\r\n");break;
		}
		else if(i>100)
		{
			uart2out("打开网络超时！\r\n\r\n");i=0;break;
		}
		HAL_Delay(200);i++;
	}
	
	/*-----------------------------连接服务器-------------------------------*/
	i=0;
	memset(resultvlu,0,sizeof(resultvlu));
	memset(message,0,sizeof(message));
	memset(u3_rx_buf,0,sizeof(u3_rx_buf));u3_rx_len=0;	
	sprintf(resultvlu,"+QMTCONN: %d,0,0",mqtt.MQTTID);
	sprintf((char*)message,"AT+QMTCONN=%d,\"%s\"\r\n",mqtt.MQTTID,mqtt.clientID);
	HAL_UART_Transmit(huart,message,strlen((char*)message),0xFFFF);
	
	while(1)
	{
		if(NULL!=strstr((const char*)u3_rx_buf,resultvlu))
		{
			uart2out("连接成功!\r\n\r\n");
			break;
		}
		else if(NULL!=strstr((const char*)u3_rx_buf,"ERROR"))
		{
			uart2out("connect fail!\r\n");break;
		}
		else if(i>100)
		{
			uart2out("连接服务器超时！\r\n\r\n");i=0;break;
		}
		HAL_Delay(200);i++;
	}
	

}

	/*---------------------------------------------------------------------
	*******************************订阅Topic*******************************
	---------------------------------------------------------------------*/
void SubTpoic(UART_HandleTypeDef* huart,char* topic)
{
	unsigned char message[200];
	int i=0;	
	memset(resultvlu,0,sizeof(resultvlu));
	memset(message,0,sizeof(message));
	memset(u3_rx_buf,0,sizeof(u3_rx_buf));u3_rx_len=0;	
	sprintf(resultvlu,"+QMTSUB: %d,%s,0",mqtt.MQTTID,mqtt.msgid);
	sprintf((char*)message,"AT+QMTSUB=%d,%s,\"%s\",%d\r\n",mqtt.MQTTID,mqtt.msgid,topic,mqtt.qos);
	HAL_UART_Transmit(huart,message,strlen((char*)message),0xFFFF);
	
	while(1)
	{
		if(NULL!=strstr((const char*)u3_rx_buf,resultvlu))
		{
			uart2out("订阅成功!\r\n\r\n");
			break;
		}
		else if(NULL!=strstr((const char*)u3_rx_buf,"ERROR"))
		{
			uart2out("ERROR!\r\n");break;
		}
		else if(i>500)
		{
			i=0;uart2out("timeout!\r\n");break;
		}
		HAL_Delay(200);i++;
	}
}

	/*---------------------------------------------------------------------
	**********************************推送消息*****************************
	----------------------------------------------------------------------*/
void PUBMSG(UART_HandleTypeDef* huart,char* topic,char* msg)
{
	unsigned char message[200];
	int i=0;
	memset(resultvlu,0,sizeof(resultvlu));
	memset(message,0,sizeof(message));
	memset(u3_rx_buf,0,sizeof(u3_rx_buf));u3_rx_len=0;
	sprintf((char*)message,"AT+QMTPUB=%d,%s,%d,%d,\"%s\",\"%s\"\r\n",mqtt.MQTTID,mqtt.msgid,mqtt.qos,mqtt.retain,topic,msg);
	HAL_UART_Transmit(huart,message,strlen((char*)message),0xFFFF);
		while(1)
	{
		if(NULL!=strstr((const char*)u3_rx_buf,"OK"))
		{
			uart2out("publish ok!\r\n");
			break;
		}
		else if(NULL!=strstr((const char*)u3_rx_buf,"ERROR"))
		{
			uart2out("ERROR!\r\n");break;
		}
		else if(i>500)
		{
			i=0;
			uart2out("publish timeout!\r\n");
			break;
		}
		HAL_Delay(200);i++;
	}
}


	/*---------------------------------------------------------------------
	**********************************设备下线*****************************
	----------------------------------------------------------------------*/
void offline(UART_HandleTypeDef* huart)
{
	unsigned char message[20];
	int i=0;
	memset(message,0,sizeof(message));
	memset(u3_rx_buf,0,sizeof(u3_rx_buf));u3_rx_len=0;
	sprintf((char*)message,"AT+QMTCLOSE=%d\r\n",mqtt.MQTTID);
	HAL_UART_Transmit(huart,message,strlen((char*)message),0xFFFF);
	while(1)
	{
		if(NULL!=strstr((const char*)u3_rx_buf,"OK"))
		{
			uart2out("off line success!\r\n");
			break;
		}
		else if(NULL!=strstr((const char*)u3_rx_buf,"ERROR"))
		{
			uart2out("ERROR!\r\n");break;
		}
		else if(i>50)
		{
			i=0;
			uart2out("timeout!\r\n");
			break;
		}
		HAL_Delay(200);i++;
	}
}
	

