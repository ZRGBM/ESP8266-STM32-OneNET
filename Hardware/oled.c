/**
	************************************************************
	************************************************************
	************************************************************
	*文件名：oled.c
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

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"

//OLED显存数组，144列8页（每页8行）
u8 OLED_GRAM[144][8];

//OLED显示颜色反转
//i: 0-正常显示 1-颜色反转
void OLED_ColorTurn(u8 i)
{
    if(i == 0)
    {
        OLED_WR_Byte(0xA6, OLED_CMD);  //正常显示
    }
    if(i == 1)
    {
        OLED_WR_Byte(0xA7, OLED_CMD);  //颜色反转显示
    }
}

//OLED屏幕旋转180度
//i: 0-正常显示 1-旋转180度显示
void OLED_DisplayTurn(u8 i)
{
    if(i == 0)
    {
        OLED_WR_Byte(0xC8, OLED_CMD);  //正常显示
        OLED_WR_Byte(0xA1, OLED_CMD);
    }
    if(i == 1)
    {
        OLED_WR_Byte(0xC0, OLED_CMD);  //旋转显示
        OLED_WR_Byte(0xA0, OLED_CMD);
    }
}

//I2C起始信号
void I2C_Start(void)
{
    OLED_SDIN_Set();
    OLED_SCLK_Set();
    OLED_SDIN_Clr();
    OLED_SCLK_Clr();
}

//I2C停止信号
void I2C_Stop(void)
{
    OLED_SCLK_Set();
    OLED_SDIN_Clr();
    OLED_SDIN_Set();
}

//等待应答信号
void I2C_WaitAck(void)  //等待应答信号的低电平
{
    OLED_SCLK_Set();
    OLED_SCLK_Clr();
}

//发送一个字节
void Send_Byte(u8 dat)
{
    u8 i;
    for(i = 0; i < 8; i++)
    {
        OLED_SCLK_Clr();  //时钟线拉低，准备发送数据
        if(dat & 0x80)    //从dat的最高位开始发送
        {
            OLED_SDIN_Set();
        }
        else
        {
            OLED_SDIN_Clr();
        }
        OLED_SCLK_Set();  //时钟线拉高，数据有效
        OLED_SCLK_Clr();  //时钟线拉低，准备下一位数据
        dat <<= 1;
    }
}

//向SSD1306写入一个字节
//mode: 数据/命令标志 0-命令 1-数据
void OLED_WR_Byte(u8 dat, u8 mode)
{
    I2C_Start();
    Send_Byte(0x78);     //发送设备地址
    I2C_WaitAck();
    if(mode) 
    {
        Send_Byte(0x40); //发送数据命令
    }
    else 
    {
        Send_Byte(0x00); //发送命令命令
    }
    I2C_WaitAck();
    Send_Byte(dat);      //发送数据/命令
    I2C_WaitAck();
    I2C_Stop();
}

//开启OLED显示
void OLED_DisPlay_On(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);  //电荷泵设置
    OLED_WR_Byte(0x14, OLED_CMD);  //开启电荷泵
    OLED_WR_Byte(0xAF, OLED_CMD);  //开启显示
}

//关闭OLED显示
void OLED_DisPlay_Off(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);  //电荷泵设置
    OLED_WR_Byte(0x10, OLED_CMD);  //关闭电荷泵
    OLED_WR_Byte(0xAF, OLED_CMD);  //关闭显示
}

//将显存数据刷新到OLED
void OLED_Refresh(void)
{
    u8 i, n;
    for(i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + i, OLED_CMD);  //设置页起始地址
        OLED_WR_Byte(0x00, OLED_CMD);      //设置列起始地址低4位
        OLED_WR_Byte(0x10, OLED_CMD);      //设置列起始地址高4位
        for(n = 0; n < 128; n++)
        {
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
        }
    }
}

//清屏函数
void OLED_Clear(void)
{
    u8 i, n;
    for(i = 0; i < 8; i++)
    {
        for(n = 0; n < 128; n++)
        {
            OLED_GRAM[n][i] = 0;  //清空显存
        }
    }
    OLED_Refresh();  //刷新显示
}

//画点函数
//x: 0~127 列坐标
//y: 0~63 行坐标
void OLED_DrawPoint(u8 x, u8 y)
{
    u8 i, m, n;
    i = y / 8;       //计算所在页
    m = y % 8;       //计算在页内的行号
    n = 1 << m;      //计算位掩码
    OLED_GRAM[x][i] |= n;  //设置该点
}

//清除一个点
//x: 0~127 列坐标
//y: 0~63 行坐标
void OLED_ClearPoint(u8 x, u8 y)
{
    u8 i, m, n;
    i = y / 8;       //计算所在页
    m = y % 8;       //计算在页内的行号
    n = 1 << m;      //计算位掩码
    OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
    OLED_GRAM[x][i] |= n;
    OLED_GRAM[x][i] = ~OLED_GRAM[x][i];  //清除该点
}

//画线函数
//x1,y1: 起点坐标
//x2,y2: 终点坐标
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2)
{
    u8 i, k, k1, k2, y0;
    //参数合法性检查
    if((x1 < 0) || (x2 > 128) || (y1 < 0) || (y2 > 64) || (x1 > x2) || (y1 > y2))
        return;
    
    if(x1 == x2)  //垂直线
    {
        for(i = 0; i < (y2 - y1); i++)
        {
            OLED_DrawPoint(x1, y1 + i);
        }
    }
    else if(y1 == y2)  //水平线
    {
        for(i = 0; i < (x2 - x1); i++)
        {
            OLED_DrawPoint(x1 + i, y1);
        }
    }
    else  //斜线
    {
        k1 = y2 - y1;
        k2 = x2 - x1;
        k = k1 * 10 / k2;
        for(i = 0; i < (x2 - x1); i++)
        {
            OLED_DrawPoint(x1 + i, y1 + i * k / 10);
        }
    }
}

//画圆函数
//x,y: 圆心坐标
//r: 圆的半径
void OLED_DrawCircle(u8 x, u8 y, u8 r)
{
    int a, b, num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b);
        OLED_DrawPoint(x - a, y - b);
        OLED_DrawPoint(x - a, y + b);
        OLED_DrawPoint(x + a, y + b);
 
        OLED_DrawPoint(x + b, y + a);
        OLED_DrawPoint(x + b, y - a);
        OLED_DrawPoint(x - b, y - a);
        OLED_DrawPoint(x - b, y + a);
        
        a++;
        num = (a * a + b * b) - r * r;  //计算当前点与圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}

//在指定位置显示一个字符，包括部分字符
//x: 0~127 列坐标
//y: 0~63 行坐标
//size: 选择字体 12/16/24
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1)
{
    u8 i, m, temp, size2, chr1;
    u8 y0 = y;
    //计算一个字符所占的字节数
    size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);
    chr1 = chr - ' ';  //计算偏移量
    
    for(i = 0; i < size2; i++)
    {
        if(size1 == 12)
        {
            temp = asc2_1206[chr1][i];  //1206字体
        }
        else if(size1 == 16)
        {
            temp = asc2_1608[chr1][i];  //1608字体
        }
        else if(size1 == 24)
        {
            temp = asc2_2412[chr1][i];  //2412字体
        }
        else 
            return;
            
        for(m = 0; m < 8; m++)  //写数据
        {
            if(temp & 0x80)
                OLED_DrawPoint(x, y);
            else
                OLED_ClearPoint(x, y);
            temp <<= 1;
            y++;
            if((y - y0) == size1)  //一行写完，移到下一列
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

//显示字符串
//x,y: 起始坐标
//size1: 字体大小
//*chr: 字符串起始地址
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1)
{
    //判断是否为可显示字符
    while((*chr >= ' ') && (*chr <= '~'))
    {
        OLED_ShowChar(x, y, *chr, size1);
        x += size1 / 2;
        //超过屏幕宽度则换行
        if(x > 128 - size1)
        {
            x = 0;
            y += 2;
        }
        chr++;
    }
}

//计算m的n次方
u32 OLED_Pow(u8 m, u8 n)
{
    u32 result = 1;
    while(n--)
    {
        result *= m;
    }
    return result;
}

//显示数字
//x,y: 起始坐标
//len: 数字的位数
//size: 字体大小
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1)
{
    u8 t, temp;
    for(t = 0; t < len; t++)
    {
        temp = (num / OLED_Pow(10, len - t - 1)) % 10;
        if(temp == 0)
        {
            OLED_ShowChar(x + (size1 / 2) * t, y, '0', size1);
        }
        else
        {
            OLED_ShowChar(x + (size1 / 2) * t, y, temp + '0', size1);
        }
    }
}

//显示中文字符
//x,y: 起始坐标
//num: 汉字对应的序号
//size1: 字体大小
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1)
{
    u8 i, m, n = 0, temp, chr1;
    u8 x0 = x, y0 = y;
    u8 size3 = size1 / 8;
    
    while(size3--)
    {
        chr1 = num * size1 / 8 + n;
        n++;
        
        for(i = 0; i < size1; i++)
        {
            if(size1 == 16)
            {
                temp = Hzk1[chr1][i];  //16*16字体
            }
            else if(size1 == 24)
            {
                temp = Hzk2[chr1][i];  //24*24字体
            }
            else if(size1 == 32)
            {
                temp = Hzk3[chr1][i];  //32*32字体
            }
            else if(size1 == 64)
            {
                temp = Hzk4[chr1][i];  //64*64字体
            }
            else 
                return;
                
            for(m = 0; m < 8; m++)
            {
                if(temp & 0x01)
                    OLED_DrawPoint(x, y);
                else
                    OLED_ClearPoint(x, y);
                temp >>= 1;
                y++;
            }
            x++;
            if((x - x0) == size1)  //一列写完，移到下一行
            {
                x = x0;
                y0 = y0 + 8;
            }
            y = y0;
        }
    }
}

//滚动显示汉字
//num 显示汉字的个数
//space 每一个汉字显示的间隔
void OLED_ScrollDisplay(u8 num, u8 space)
{
    u8 i, n, t = 0, m = 0, r;
    while(1)
    {
        if(m == 0)
        {
            //在OLED显存最右侧写一个汉字
            OLED_ShowChinese(128, 24, t, 16);
            t++;
        }
        if(t == num)
        {
            //显示间隔
            for(r = 0; r < 16 * space; r++)
            {
                for(i = 0; i < 144; i++)
                {
                    for(n = 0; n < 8; n++)
                    {
                        OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
                    }
                }
                OLED_Refresh();
            }
            t = 0;
        }
        m++;
        if(m == 16)
        {
            m = 0;
        }
        //实现滚动
        for(i = 0; i < 144; i++)
        {
            for(n = 0; n < 8; n++)
            {
                OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
            }
        }
        OLED_Refresh();
    }
}

//设置写入数据的起始位置
void OLED_WR_BP(u8 x, u8 y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);  //设置页起始地址
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD);
}

//显示图片
//x0,y0: 起始坐标
//x1,y1: 结束坐标
//BMP[]: 要写入的图片数据
void OLED_ShowPicture(u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[])
{
    u32 j = 0;
    u8 x = 0, y = 0;
    if(y % 8 == 0)
        y = 0;
    else
        y += 1;
        
    for(y = y0; y < y1; y++)
    {
        OLED_WR_BP(x0, y);
        for(x = x0; x < x1; x++)
        {
            OLED_WR_Byte(BMP[j], OLED_DATA);
            j++;
        }
    }
}

//OLED初始化函数
void OLED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能A端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化GPIOA
    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_4);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能B端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //速度50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化GPIOB
    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8);
    
    OLED_RST_Set();
    delay_ms(100);
    OLED_RST_Clr();  //复位
    delay_ms(200);
    OLED_RST_Set();
    OLED_DC_Clr();
    
    //SSD1306初始化命令
    OLED_WR_Byte(0xAE, OLED_CMD);  //关闭显示
    OLED_WR_Byte(0x00, OLED_CMD);  //设置低列地址
    OLED_WR_Byte(0x10, OLED_CMD);  //设置高列地址
    OLED_WR_Byte(0x40, OLED_CMD);  //设置起始行地址
    OLED_WR_Byte(0x81, OLED_CMD);  //设置对比度控制寄存器
    OLED_WR_Byte(0xCF, OLED_CMD);  //设置SEG输出电流亮度
    OLED_WR_Byte(0xA1, OLED_CMD);  //设置SEG/列映射
    OLED_WR_Byte(0xC8, OLED_CMD);  //设置COM/行扫描方向
    OLED_WR_Byte(0xA6, OLED_CMD);  //设置正常显示
    OLED_WR_Byte(0xA8, OLED_CMD);  //设置多路复用率(1到64)
    OLED_WR_Byte(0x3f, OLED_CMD);  //1/64占空比
    OLED_WR_Byte(0xD3, OLED_CMD);  //设置显示偏移
    OLED_WR_Byte(0x00, OLED_CMD);  //无偏移
    OLED_WR_Byte(0xd5, OLED_CMD);  //设置显示时钟分频比/振荡器频率
    OLED_WR_Byte(0x80, OLED_CMD);  //设置分频比，时钟为100帧/秒
    OLED_WR_Byte(0xD9, OLED_CMD);  //设置预充电周期
    OLED_WR_Byte(0xF1, OLED_CMD);  //设置预充电为15时钟，放电为1时钟
    OLED_WR_Byte(0xDA, OLED_CMD);  //设置COM引脚硬件配置
    OLED_WR_Byte(0x12, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD);  //设置VCOMH
    OLED_WR_Byte(0x40, OLED_CMD);  //设置VCOM选择级别
    OLED_WR_Byte(0x20, OLED_CMD);  //设置页寻址模式
    OLED_WR_Byte(0x02, OLED_CMD);
    OLED_WR_Byte(0x8D, OLED_CMD);  //设置电荷泵使能/禁用
    OLED_WR_Byte(0x14, OLED_CMD);  //开启电荷泵
    OLED_WR_Byte(0xA4, OLED_CMD);  //禁用整个显示开启
    OLED_WR_Byte(0xA6, OLED_CMD);  //禁用反向显示开启
    OLED_WR_Byte(0xAF, OLED_CMD);  //开启显示
    OLED_Clear();  //清屏
}
