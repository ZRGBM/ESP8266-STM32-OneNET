/**
	************************************************************
	************************************************************
	************************************************************
	*文件名：oled.h
	*
	*作者：ZRGBM
	*
	*最后修改日期：2025年12月6日
	*
	*版本：6.0.0
	*
	*说明：显示屏的定义，对1315驱动的显示屏
	************************************************************
	************************************************************
	************************************************************
**/

#ifndef __OLED_H
#define __OLED_H 

#include "sys.h"
#include "stdlib.h"	

//-----------------LED引脚定义---------------- 
#define LED_ON  GPIO_ResetBits(GPIOB, GPIO_Pin_8)  //LED点亮（拉低电平）
#define LED_OFF GPIO_SetBits(GPIOB, GPIO_Pin_8)    //LED关闭（拉高电平）

//-----------------OLED引脚定义---------------- 
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOA, GPIO_Pin_5)  //SCL时钟线拉低
#define OLED_SCLK_Set() GPIO_SetBits(GPIOA, GPIO_Pin_5)     //SCL时钟线拉高（SCL：串行时钟线）

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOA, GPIO_Pin_7)  //DIN数据线拉低（DIN：串行数据线）
#define OLED_SDIN_Set() GPIO_SetBits(GPIOA, GPIO_Pin_7)     //DIN数据线拉高

#define OLED_RST_Clr()  GPIO_ResetBits(GPIOB, GPIO_Pin_0)  //RES复位线拉低（RES：复位信号）
#define OLED_RST_Set()  GPIO_SetBits(GPIOB, GPIO_Pin_0)    //RES复位线拉高

#define OLED_DC_Clr()   GPIO_ResetBits(GPIOB, GPIO_Pin_1)  //DC控制线拉低（DC：数据/命令选择）

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
#define u8 unsigned char  //无符号8位字符型
#define u32 unsigned int  //无符号32位整型

//清除指定坐标的点
void OLED_ClearPoint(u8 x, u8 y);
//OLED颜色反转控制（0：正常，1：反转）
void OLED_ColorTurn(u8 i);
//OLED显示方向翻转（0：正常，1：翻转）
void OLED_DisplayTurn(u8 i);
//I2C通信开始信号
void I2C_Start(void);
//I2C通信停止信号
void I2C_Stop(void);
//I2C等待应答
void I2C_WaitAck(void);
//发送一个字节数据
void Send_Byte(u8 dat);
//向OLED写入数据或命令
//dat：要写入的数据/命令
//mode：写入模式（OLED_CMD：命令，OLED_DATA：数据）
void OLED_WR_Byte(u8 dat, u8 mode);
//开启OLED显示
void OLED_DisPlay_On(void);
//关闭OLED显示
void OLED_DisPlay_Off(void);
//刷新OLED显示（将缓存数据显示到屏幕）
void OLED_Refresh(void);
//清屏（清除所有显示内容）
void OLED_Clear(void);
//在指定坐标画点
//x：x坐标
//y：y坐标
void OLED_DrawPoint(u8 x, u8 y);
//绘制直线
//x1,y1：起点坐标
//x2,y2：终点坐标
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2);
//绘制圆形
//x,y：圆心坐标
//r：半径
void OLED_DrawCircle(u8 x, u8 y, u8 r);
//显示单个字符
//x,y：显示坐标
//chr：要显示的字符
//size1：字符大小（如8x16,16x16等）
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1);
//显示字符串
//x,y：显示坐标
//chr：字符串指针
//size1：字符大小
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1);
//显示数字
//x,y：显示坐标
//num：要显示的数字
//len：数字位数
//size1：数字大小
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1);
//显示中文字符
//x,y：显示坐标
//num：中文字符在字库中的索引
//size1：字体大小
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1);
//滚动显示
//num：滚动次数
//space：滚动间隔
void OLED_ScrollDisplay(u8 num, u8 space);
//设置画笔位置（内部函数）
void OLED_WR_BP(u8 x, u8 y);
//显示图片
//x0,y0：图片左上角坐标
//x1,y1：图片右下角坐标
//BMP[]：图片数据数组
void OLED_ShowPicture(u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[]);
//OLED初始化函数
void OLED_Init(void);

#endif
