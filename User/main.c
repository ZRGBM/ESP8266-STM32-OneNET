/**
	************************************************************
	************************************************************
	************************************************************
	*文件名：main.c
	*
	*作者：ZRGBM
	*
	*最后修改日期：2025年12月6日
	*
	*版本：6.0.0
	*
	*说明：对所有变量的初始化，LED，ESP，舵机，OLED
	************************************************************
	************************************************************
	************************************************************
**/

#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "dht11.h"
#include "usart.h"
#include "pwm.h"
#include "key.h"
#include "esp8266.h" 
#include "ent.h" 
#include "led.h"
#include "stdio.h"
#include "string.h" 
//以下内容必须修改为自己设置的东西！！！！
#define WIFI_SSID     ""             // WIFI名称
#define WIFI_PASS     ""       // WIFI密码
#define DEVICE_NAME   ""         // 设备名称
#define PRODUCT_ID    ""      // 产品ID
#define MQTT_TOKEN    ""					// Token
// ZRGBM
int main(void)
{
    u8 temperature = 0;        // 变量定义
    u8 humidity = 0;
    char display_buf[20];
    u8 key_val = 0;

    u16 time_count = 0;    	  // 计时变量
    u16 rx_timeout = 0;

    char cmd_id[20];          // 用于解析指令ID的变量
    char *p_start, *p_end;

    delay_init();							//硬件初始化
    NVIC_Configuration();
    
    uart_init(115200);     	  // USART1 (PA9/PA10)调试
    USART2_Init(115200);   	  // USART2 (PA2/PA3)ESP8266
    
    PWM_Init();               // 舵机  (PA1)
    KEY_Init();               // 按键  (PB12/PB13)
    LED_Init();               // LED初始化 (PC13)
    OLED_Init();              // OLED  (P05SCL/P07SDA)
    
    OLED_ColorTurn(0);
    OLED_DisplayTurn(0);
    SG90_SetAngle(0);
    LED_Switch(0);            // 默认关灯

    printf("系统上电中.\r\n");
    ESP8266_Config(WIFI_SSID, WIFI_PASS, DEVICE_NAME, PRODUCT_ID, MQTT_TOKEN);

    OLED_Clear();
    OLED_ShowString(28, 0, "Z R G B M", 16); 
    OLED_ShowString(0, 16, "Temp:    C", 16); 
    OLED_ShowString(0, 32, "Humi:    %", 16); 
    OLED_ShowString(0, 48, "M:0      L:0", 16); 
    OLED_Refresh();

    while(1)
    {
        if(USART2_RX_STA & 0x8000)
        {
            USART2_RX_BUF[USART2_RX_STA & 0x3FFF] = 0; 							// 添加结束符

            if(strstr((const char*)USART2_RX_BUF, "+MQTTSUBRECV"))  // 查找关键字
            {
                p_start = strstr((const char*)USART2_RX_BUF, "\"id\":\"");
                if(p_start != NULL)
                {
                    p_start += 6;
                    p_end = strstr(p_start, "\"");
                    if(p_end != NULL)
                    {
                        memset(cmd_id, 0, sizeof(cmd_id));
                        if(p_end - p_start < sizeof(cmd_id))
                        {
                            strncpy(cmd_id, p_start, p_end - p_start);
                            MQTT_Send_Reply(cmd_id, PRODUCT_ID, DEVICE_NAME); 
                        }
                    }
                }
                if(strstr((const char*)USART2_RX_BUF, "\"Door\":true"))       
                {
                    Door_Status = 1;
                    SG90_SetAngle(90); // 开门
                }
                else if(strstr((const char*)USART2_RX_BUF, "\"Door\":false")) 
                {
                    Door_Status = 0;
                    SG90_SetAngle(0);  // 关门
                }
                
                if(strstr((const char*)USART2_RX_BUF, "\"Led\":true"))   			
                {
                    Led_Status = 1;
                }
                else if(strstr((const char*)USART2_RX_BUF, "\"Led\":false"))  
                {
                    Led_Status = 0;
                }
                LED_Switch(Led_Status); 

                sprintf(display_buf, "M:%d      L:%d", Door_Status, Led_Status);
                OLED_ShowString(0, 48, (u8*)display_buf, 16);
                OLED_Refresh();
                printf("Temp: %d C, Humi: %d %%, M: %d, L: %d\r\n", temperature, humidity, Door_Status, Led_Status);                // 串口PA9/PA10打印状态
            }
            USART2_RX_STA = 0;
            rx_timeout = 0;
        }
        else
        {
            if(USART2_RX_STA != 0)
            {
                rx_timeout++;
                if(rx_timeout > 200) { USART2_RX_STA = 0; rx_timeout = 0; }
            }
            else rx_timeout = 0;
        }

        key_val = KEY_Scan();
        if(key_val)
        {
            if(key_val == 1) // 按键1开门
            {
                Door_Status = 1;
                SG90_SetAngle(90); 
                printf("Key1: M=1 (Servo 90)\r\n");
            }
            else if(key_val == 2) // 按键2关门
            {
                Door_Status = 0;
                SG90_SetAngle(0);  
                printf("Key2: M=0 (Servo 0)\r\n");
            }
            sprintf(display_buf, "M:%d      L:%d", Door_Status, Led_Status);
            OLED_ShowString(0, 48, (u8*)display_buf, 16);
            OLED_Refresh();
        }

        time_count++;
        delay_ms(10); 
        
        if(time_count >= 100) 
        {
            time_count = 0;
            DHT11_Read_Data(&temperature, &humidity);
            sprintf(display_buf, "%2d", temperature);
            OLED_ShowString(48, 16, (u8*)display_buf, 16);
            sprintf(display_buf, "%2d", humidity);
            OLED_ShowString(48, 32, (u8*)display_buf, 16); 
            sprintf(display_buf, "M:%d      L:%d", Door_Status, Led_Status);
            OLED_ShowString(0, 48, (u8*)display_buf, 16);          
            OLED_Refresh();
            printf("Temp: %d C, Humi: %d %%, M: %d, L: %d\r\n", temperature, humidity, Door_Status, Led_Status);
            MQTT_Publish_Data(temperature, humidity, Door_Status, Led_Status, PRODUCT_ID, DEVICE_NAME);            
        }
    }
}
