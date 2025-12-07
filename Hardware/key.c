/**
	************************************************************
	************************************************************
	************************************************************
	*文件名：key.c
	*
	*作者：ZRGBM
	*
	*最后修改日期：2025年12月6日
	*
	*版本：6.0.0
	*
	*说明：按键检测
	************************************************************
	************************************************************
	************************************************************
**/

#include "key.h"
#include "delay.h"

// ZRGBM
void KEY_Init(void)												// 初始化按键 PB12, PB13
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 				// 上拉输入，默认高电平
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

u8 KEY_Scan(void)
{
    static u8 key_up = 1;
    if(key_up && (KEY1 == 0 || KEY2 == 0))
    {
        delay_ms(10);
        key_up = 0;
        if(KEY1 == 0) return 1;
        if(KEY2 == 0) return 2;
    }
    else if(KEY1 == 1 && KEY2 == 1)
    {
        key_up = 1;
    }
    return 0;
}
