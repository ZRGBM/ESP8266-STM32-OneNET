/**
	************************************************************
	************************************************************
	************************************************************
	*文件名：pwm.c
	*
	*作者：ZRGBM
	*
	*最后修改日期：2025年12月6日
	*
	*版本：6.0.0
	*
	*说明：舵机PWM输出
	************************************************************
	************************************************************
	************************************************************
**/

#include "pwm.h"

// ZRGBM
void PWM_Init(void)															                    // 初始化 TIM2_CH2 (PA1) 为 PWM 输出
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 				   	// 使能TIM2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);				    // 使能GPIOA时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化 TIM2
    // 周期 = (arr+1) * (psc+1) / 72000000
    // 我们需要 20ms (50Hz) 周期
    // 设置 psc=719, arr=1999 -> 2000 * 720 / 72M = 20ms
    TIM_TimeBaseStructure.TIM_Period = 1999; // ARR
    TIM_TimeBaseStructure.TIM_Prescaler = 719; // PSC
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 初始化 TIM2_CH2 PWM 模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0; // 初始占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_Cmd(TIM2, ENABLE); // 使能 TIM2
}

void SG90_SetAngle(u8 angle)
{
    u16 ccr_val;
    if(angle > 180) angle = 180;
    ccr_val = 50 + (angle * 200 / 180); 
    TIM_SetCompare2(TIM2, ccr_val);
}
