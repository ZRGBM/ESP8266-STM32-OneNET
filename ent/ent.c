/**
	************************************************************
	************************************************************
	************************************************************
	*文件名：ent.c
	*
	*作者：ZRGBM
	*
	*最后修改日期：2025年12月6日
	*
	*版本：6.0.0
	*
	*说明：接收数据与发送数据的云平台回复
	************************************************************
	************************************************************
	************************************************************
**/

#include "ent.h"
#include "usart.h"
#include "stdio.h"

// ZRGBM
void MQTT_Publish_Data(u8 temp, u8 humi, u8 door, u8 led, char* prod_id, char* dev_name)
{
    char mqtt_cmd_buf[512]; 
    sprintf(mqtt_cmd_buf, 
        "AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"Temp\\\":{\\\"value\\\":%d.0}\\,\\\"Humi\\\":{\\\"value\\\":%d.0}\\,\\\"Door\\\":{\\\"value\\\":%s}\\,\\\"Led\\\":{\\\"value\\\":%s}}}\",0,0\r\n", 
        prod_id,
        dev_name,
        temp, 
        humi,
        door ? "true" : "false", 
        led  ? "true" : "false"
    );

    USART2_SendString(mqtt_cmd_buf);
}

void MQTT_Send_Reply(char *msg_id, char* prod_id, char* dev_name)
{
    char reply_buf[256];
    sprintf(reply_buf, 
        "AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/set_reply\",\"{\\\"id\\\":\\\"%s\\\"\\,\\\"code\\\":200\\,\\\"msg\\\":\\\"success\\\"}\",0,0\r\n", 
        prod_id,
        dev_name,
        msg_id);
    USART2_SendString(reply_buf);
}
