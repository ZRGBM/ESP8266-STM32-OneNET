/**
	************************************************************
	************************************************************
	************************************************************
	*文件名：esp8266.c
	*
	*作者：ZRGBM
	*
	*最后修改日期：2025年12月6日
	*
	*版本：6.0.0
	*
	*说明：重新定义了宏定义的位置，参数自动发送和接收
	************************************************************
	************************************************************
	************************************************************
**/

#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include "stdio.h"

// ZRGBM
void ESP8266_Config(char* ssid, char* pwd, char* dev_name, char* prod_id, char* token)
{
    char cmd_buf[512];
    printf("启动ESP8266自动配置\r\n");
    ESP8266_Silent_Mode = 1; 
	  USART2_SendString("  \r\n");
    delay_ms(100);
    USART2_SendString("AT\r\n");
    delay_ms(500); 
    USART2_SendString("AT+RST\r\n");
    delay_ms(3000);
    sprintf(cmd_buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    USART2_SendString(cmd_buf);
    delay_ms(5000);
    sprintf(cmd_buf, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", dev_name, prod_id, token);
    USART2_SendString(cmd_buf);
    delay_ms(1500); 
    USART2_SendString("AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n");
    delay_ms(1500); 
    sprintf(cmd_buf, "AT+MQTTSUB=0,\"$sys/%s/%s/thing/property/post/reply\",1\r\n", prod_id, dev_name);
    USART2_SendString(cmd_buf);
    delay_ms(1500); 
    sprintf(cmd_buf, "AT+MQTTSUB=0,\"$sys/%s/%s/thing/property/set\",1\r\n", prod_id, dev_name);
    USART2_SendString(cmd_buf);
    ESP8266_Silent_Mode = 0; 
    printf("ESP8266启动成功\r\n");
}
