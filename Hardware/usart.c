/**
	************************************************************
	************************************************************
	************************************************************
	*文件名：usart.c
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

#include "sys.h"
#include "usart.h"	  

// ZRGBM
// 全局变量定义
volatile u8 ESP8266_Silent_Mode = 0; // 静默模式标志
volatile u8 Door_Status = 0;         // 门状态
volatile u8 Led_Status = 0;          // 灯状态

// USART2 相关变量
u8 USART2_RX_BUF[USART2_REC_LEN]; 
u16 USART2_RX_STA = 0;

// USART1 相关变量
u8 USART_RX_BUF[USART_REC_LEN];     
u16 USART_RX_STA=0;       
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
   
#if 1
#pragma import(__use_no_semihosting)                             
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
void _sys_exit(int x) 
{ 
	x = x; 
} 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //如果使能了接收
  
//初始化串口1
void uart_init(u32 bound){
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
  
    //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

    USART_Init(USART1, &USART_InitStructure); 
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口 
}

//串口1中断服务程序
void USART1_IRQHandler(void)                	
{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		Res =USART_ReceiveData(USART1); //读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
    } 
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif	

// 初始化串口2
void USART2_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. 开启时钟 (注意USART2是APB1总线)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // 2. GPIO配置
    // PA2 (TX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA3 (RX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. NVIC中断配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 4. USART配置
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启接收中断
    USART_Cmd(USART2, ENABLE); // 使能串口
}

// USART2 发送一个字节
void USART2_SendByte(u8 Data)
{
    USART_SendData(USART2, Data);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 
}

// USART2 发送字符串
void USART2_SendString(char *str)
{
    while(*str)
    {
        USART2_SendByte(*str++);
    }
}

// 串口2 中断服务函数
// 功能：接收数据存入缓冲区，供 main.c 解析
void USART2_IRQHandler(void)
{
    u8 Res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        Res = USART_ReceiveData(USART2); // 读取数据
        
        // 如果不是静默模式，则将数据存入缓冲区
        if(ESP8266_Silent_Mode == 0)
        {
            if((USART2_RX_STA & 0x8000) == 0) // 接收未完成
            {
                if(USART2_RX_STA & 0x4000) // 接收到了0x0d
                {
                    if(Res != 0x0a) USART2_RX_STA = 0; // 接收错误,重新开始
                    else USART2_RX_STA |= 0x8000;	   // 接收完成了 
                }
                else // 还没收到0X0D
                {	
                    if(Res == 0x0d) USART2_RX_STA |= 0x4000;
                    else
                    {
                        USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = Res;
                        USART2_RX_STA++;
                        if(USART2_RX_STA > (USART2_REC_LEN - 1)) USART2_RX_STA = 0; 
                    }		 
                }
            }
        }
        else
        {
            (void)Res;
        }
    }
}
