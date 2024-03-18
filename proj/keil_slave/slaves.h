#ifndef SLAVES_H
#define SLAVES_H

#define MODEL 3

#include "__config__.h"

void FuncInit(void);
void Func(void);
bit updateState(unsigned char word);

#if MODEL == 0
#define THIS_ADDR 0x00

void FuncInit(void) {}
void Func(void) {}

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

void Func(void) {}

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

#elif MODEL == 2
#define THIS_ADDR 0x02

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

void Func(void) {}

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

void Func(void) {}

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

void Func(void) {}

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