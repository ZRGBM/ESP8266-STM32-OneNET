/**
	************************************************************
	************************************************************
	************************************************************
	*文件名：usart.h
	*
	*作者：ZRGBM
	*
	*最后修改日期：2025年12月6日
	*
	*版本：6.0.0
	*
	*说明：串口定义
	************************************************************
	************************************************************
	************************************************************
**/

#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	  // USART1最大接收字节数
#define EN_USART1_RX 			1		  // 使能USART1接收

// ZRGBM
#define USART2_REC_LEN          1024      // USART2接收缓冲区
extern u8  USART2_RX_BUF[USART2_REC_LEN]; // 接收缓冲
extern u16 USART2_RX_STA;                 // 接收状态

// 全局状态变量 (M和L)
extern volatile u8 Door_Status; 
extern volatile u8 Led_Status;  
extern volatile u8 ESP8266_Silent_Mode; 

extern u8  USART_RX_BUF[USART_REC_LEN]; 
extern u16 USART_RX_STA;         		

// 函数声明
void uart_init(u32 bound);          // USART1 初始化
void USART2_Init(u32 bound);        // USART2 初始化
void USART2_SendString(char *str);  // USART2 发送字符串
void USART2_SendByte(u8 Data);      // USART2 发送字节

#endif
