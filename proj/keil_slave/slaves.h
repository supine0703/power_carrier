#ifndef SLAVES_H
#define SLAVES_H

#define MODEL 1

#include "__config__.h"

void FuncInit(void);
void Func(void);
bit updateState(unsigned char word);

#if MODEL == 0
#define THIS_ADDR 0x00

unsigned char byte = 0;

void wait(unsigned int t) // 11.0592 约1ms
{
    unsigned char i;
    while (t)
    {
        --t;
        for (i = (t ? 151 : 149); i; --i)
            ;
    }
}

void FuncInit(void)
{
}
void Func(void)
{
    unsigned char len = 16;
    unsigned int w = (len + 6) * 270;
    EA = 0;
    SP_QTransmitByte(len);
    while (len--)
    {
        // wait(1);
        SP_QTransmitByte(byte++);
    }
    wait(w);
}

bit updateState(unsigned char word)
{
    switch (word)
    {
    case 0x00:
        /* code */
        break;

    default:
        return 0;
    }
    return 1;
}

#elif MODEL == 1
#define THIS_ADDR 0x01

void wait(unsigned int t) // 11.0592 约1ms
{
    unsigned char i;
    while (t)
    {
        --t;
        for (i = (t ? 151 : 149); i; --i)
            ;
    }
}

#include "lcd1602.h"
void FuncInit(void)
{
    unsigned int i = 0;
    unsigned char num;
    LCD1602_WriteCmd(Set_8bit_2line_5x7);   // 命令6
    LCD1602_WriteCmd(Show_CursorOn);        // 命令4
    LCD1602_WriteCmd(Mode_CursorRightMove); // 命令3
    LCD1602_WriteCmd(Clear_Screen);         // 命令1
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(6));
    num = THIS_ADDR >> 4;
    num += ((num >= 10) ? 55 : 48);
    LCD1602_WriteData(num);
    num = THIS_ADDR & 0x0f;
    num += ((num >= 10) ? 55 : 48);
    LCD1602_WriteData(num);
    while (++i != 0)
        ;
    LCD1602_WriteData(':');
}

void show(unsigned char* s)
{
    while (*s)
        LCD1602_WriteData(*s++);
}
#include <stdio.h>
void Func(void)
{
    unsigned char buff[10];
    extern unsigned int count;
    static unsigned int last;
    if (last != count)
    {
        last = count;
        sprintf(buff, "%5d", count);
        LCD1602_WriteCmd(Move_Cursor_Row2_Col(11));
        show(buff);
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
    }
#if 0
    unsigned char i = 0;
    static unsigned char xdata b[64];
    unsigned char xdata bb[17];
    static unsigned char len;
    EA = 0;
    if (SP_QReceiveData(b, &len, 200))
    {
        LCD1602_WriteCmd(Clear_Screen);
        sprintf(bb, "%02X:", (int)len);
        show(bb);
        while (len-- && i < 4)
        {
            sprintf(bb, "%02X,", (int)b[i++]);
            show(bb);
        }
    }
    else
    {
        SP_QTransmitData(b, len);
        wait(len * 200 + 1000);
    }
#endif
}

bit updateState(unsigned char word)
{
    if (word <= 9)
    {
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
        LCD1602_WriteData('0' + word);
        return 1;
    }
    else if (word <= 35)
    {
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
        LCD1602_WriteData(55 + word);
        return 1;
    }
    else
    {
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
        LCD1602_WriteData('!');
        return 0;
    }
}

#elif MODEL == 2
#define THIS_ADDR 0x1f

#include "lcd1602.h"
void FuncInit(void)
{
    unsigned char num;
    LCD1602_WriteCmd(Set_8bit_2line_5x7);   // 命令6
    LCD1602_WriteCmd(Show_CursorOff);       // 命令4
    LCD1602_WriteCmd(Mode_CursorRightMove); // 命令3
    LCD1602_WriteCmd(Clear_Screen);         // 命令1
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(6));
    num = THIS_ADDR >> 4;
    num += ((num >= 10) ? 55 : 48);
    LCD1602_WriteData(num);
    num = THIS_ADDR & 0x0f;
    num += ((num >= 10) ? 55 : 48);
    LCD1602_WriteData(num);
    LCD1602_WriteData(':');
}

unsigned char byte = 0;

void wait(unsigned int t)
{
    unsigned char i;
    while (t)
    {
        t--;
        for (i = 124; i; --i)
            if (i == 200)
                break;
    }
}

void Func(void)
{
    unsigned char len = 16;
    SP_QTransmitByte(len);
    while (len--)
    {
        SP_QTransmitByte(byte++);
        wait(0);
    }
    wait(6000);
}

bit updateState(unsigned char word)
{
    if (word <= 9)
    {
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
        LCD1602_WriteData('0' + word);
        return 1;
    }
    else if (word <= 35)
    {
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
        LCD1602_WriteData(55 + word);
        return 1;
    }
    else
    {
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
        LCD1602_WriteData('!');
        return 0;
    }
}

#elif MODEL == 3
#define THIS_ADDR 0x03

#include "lcd1602.h"
void FuncInit(void)
{
    unsigned char num;
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

void Func(void)
{
}

bit updateState(unsigned char word)
{
    if (word <= 9)
    {
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
        LCD1602_WriteData('0' + word);
        return 1;
    }
    else
    {
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
        LCD1602_WriteData('N');
        return 0;
    }
}
#elif MODEL == 4
#define THIS_ADDR 0x04

#include "lcd1602.h"
void FuncInit(void)
{
    unsigned char num;
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

void Func(void)
{
}

bit updateState(unsigned char word)
{
    if (word <= 14)
    {
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
        LCD1602_WriteData('a' + word);
        return 1;
    }
    else
    {
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
        LCD1602_WriteData('N');
        return 0;
    }
}
#endif

#endif // SLAVES_H