/**
 * 1: 将单个按键组值赋给P1
 * 2: 单个将按键组的值显示LCD1602
 */
#define MODEL 4
#include "__config__.h"
#include "communication.h"

#define uchar unsigned char
#define uint unsigned int

#define _FUNC(X) Func##X
#define FUNC(X) _FUNC(X)

extern uchar baudRateT;

typedef void (*v_ptr)(uchar); // 定义一个函数指针类
v_ptr pFunc;

#if MODEL == 0
uchar addr = 0x00;
#define NO_FUNC_INIT
void FUNC(MODEL)(uchar buf)
{
    P1 = buf - (buf >= 65 ? 55 : 48);
}
#elif MODEL == 1
uchar addr = 0x01;
#include "lcd1602.h"
// 开启 LCD1602 显示 (initial)
void FuncInit(void)
{
    uchar num;
    LCD1602_WriteCmd(Set_8bit_2line_5x7);   // 命令6
    LCD1602_WriteCmd(Show_CursorOff);       // 命令4
    LCD1602_WriteCmd(Mode_CursorRightMove); // 命令3
    LCD1602_WriteCmd(Clear_Screen);         // 命令1
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(6));
    num = MODEL >> 4;
    num += num >= 10 ? 55 : 48;
    LCD1602_WriteData(num);
    num = MODEL & 0x0f;
    num += num >= 10 ? 55 : 48;
    LCD1602_WriteData(num);
    LCD1602_WriteData(':');
}

void FUNC(MODEL)(uchar buf)
{
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
    LCD1602_WriteData(buf);
}
#elif MODEL == 2
uchar addr = 0x02;
#include "lcd1602.h"
// 开启 LCD1602 显示 (initial)
void FuncInit(void)
{
    uchar num;
    LCD1602_WriteCmd(Set_8bit_2line_5x7);   // 命令6
    LCD1602_WriteCmd(Show_CursorOff);       // 命令4
    LCD1602_WriteCmd(Mode_CursorRightMove); // 命令3
    LCD1602_WriteCmd(Clear_Screen);         // 命令1
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(6));
    num = MODEL >> 4;
    num += num >= 10 ? 55 : 48;
    LCD1602_WriteData(num);
    num = MODEL & 0x0f;
    num += num >= 10 ? 55 : 48;
    LCD1602_WriteData(num);
    LCD1602_WriteData(':');
}

void FUNC(MODEL)(uchar buf)
{
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
    LCD1602_WriteData(buf);
}
#elif MODEL == 3
uchar addr = 0x03;
#include "lcd1602.h"
// 开启 LCD1602 显示 (initial)
void FuncInit(void)
{
    uchar num;
    LCD1602_WriteCmd(Set_8bit_2line_5x7);   // 命令6
    LCD1602_WriteCmd(Show_CursorOff);       // 命令4
    LCD1602_WriteCmd(Mode_CursorRightMove); // 命令3
    LCD1602_WriteCmd(Clear_Screen);         // 命令1
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(6));
    num = MODEL >> 4;
    num += num >= 10 ? 55 : 48;
    LCD1602_WriteData(num);
    num = MODEL & 0x0f;
    num += num >= 10 ? 55 : 48;
    LCD1602_WriteData(num);
    LCD1602_WriteData(':');
}

void FUNC(MODEL)(uchar buf)
{
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
    LCD1602_WriteData(buf);
}
#elif MODEL == 4
uchar addr = 0x04;
#include "lcd1602.h"
// 开启 LCD1602 显示 (initial)
void FuncInit(void)
{
    uchar num;
    LCD1602_WriteCmd(Set_8bit_2line_5x7);   // 命令6
    LCD1602_WriteCmd(Show_CursorOff);       // 命令4
    LCD1602_WriteCmd(Mode_CursorRightMove); // 命令3
    LCD1602_WriteCmd(Clear_Screen);         // 命令1
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(6));
    num = MODEL >> 4;
    num += num >= 10 ? 55 : 48;
    LCD1602_WriteData(num);
    num = MODEL & 0x0f;
    num += num >= 10 ? 55 : 48;
    LCD1602_WriteData(num);
    LCD1602_WriteData(':');
}

void FUNC(MODEL)(uchar buf)
{
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
    LCD1602_WriteData(buf);
}
#endif

void init(void)
{
    // 串口初始化
    SCON = 0xf4; // 设置串口控制寄存器SCON=1111 0100
    PCON = 0x00; // 设定电源控制寄存器PCON，这里表示波特率不加倍
    TMOD = 0x20; // 定时器T1的工作方式2
    TH1 = TL1 = baudRateT;
    TR1 = 1; // 启动定时器

    // 中断初始化
    EA = 1; // 中断允许
    ES = 1; // 串口中断允许
}

void main(void)
{
#ifndef NO_FUNC_INIT
    FuncInit();
#endif
    init();
    while (1)
        ;
}

void int_4() interrupt 4
{
    ES = 0; // 禁止串行中断，放置在发送数据器件突然发送中断
    if (Receive(addr))
    {
        pFunc = FUNC(MODEL);
        pFunc(SBUF);
    }
    ES = 1;
}