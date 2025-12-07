/**
	************************************************************
	************************************************************
	************************************************************
	*文件名：led.c
	*
	*作者：ZRGBM
	*
	*最后修改日期：2025年12月6日
	*
	*版本：6.0.0
	*
	*说明：LED的点亮
	************************************************************
	************************************************************
	************************************************************
**/

#include "led.h"

// ZRGBM
// 初始化板载LED (PC13)
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 开启GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // 配置PC13为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 默认输出高电平 (灭)
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

// 控制LED状态
// status: 1=ON (亮), 0=OFF (灭)
void LED_Switch(u8 status)
{
    if(status == 0)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13); // 低电平点亮
    }
    else
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);   // 高电平熄灭
    }
}
