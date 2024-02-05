// #define USE_LCD1602_ACTION
#include "__config__.h"
#include "serialport.h"
#include "lcd1602.h"
#include "utility.h"

#define uchar unsigned char
#define uint unsigned int

uchar KeyValue(void);
void Delay1ms(uint t);
void LCD1602_Action(void);
void LCD1602_ShowString(uchar* s, uint t);

uchar lcd_action_line1[] = "ABCDEFGHIJKLMNOP";
uchar lcd_action_line2[] = "abcdefghijklmnop";

extern uchar baudRateT;

void init(void)
{
    // 串口初始化
    SCON = 0xd0; // 设置串口控制寄存器SCON=1101 0000
    PCON = 0x00; // 设定电源控制寄存器PCON，这里表示波特率不加倍
    TMOD = 0x20;           // 定时器T1的工作方式2
    TH1 = TL1 = baudRateT; // 波特率 9600bps
    TR1 = 1;               // 启动定时器
}

void main(void)
{
    bit c = 0;
    uchar key, count = 0;
    uchar num[] = {0, 0};

    LCD1602_Action();
    init();

    while (1)
    {
        // 接收按键
        key = KeyValue();
        while (KEY_MATRIX != 0x0f)
            ; // 松开按键后

        if (key != 0xff)
        {
            count++;
            if (count == 9)
            {
                LCD1602_WriteCmd(Move_Cursor_Row2_Col(0));
                LCD1602_ShowString("                ", 0);
                LCD1602_WriteCmd(Move_Cursor_Row2_Col(0));
            }
            else if (count == 17)
            {
                count = 1;
                LCD1602_WriteCmd(Move_Cursor_Row1_Col(0));
                LCD1602_ShowString("                ", 0);
                LCD1602_WriteCmd(Move_Cursor_Row1_Col(0));
            }

            UInt8ToString(key, 16, num, 1);
            LCD1602_ShowString(num, 0);
            
            SP_QTransmitAD((key >> 2) + 1, num, 1);
            if (SP_QReceiveD(2, 0, 0))
            {
                num[0] = SBUF;
                LCD1602_ShowString(num, 0);
            }
            else
                LCD1602_ShowString("!", 0);
        }
    }
}

uchar KeyValue(void)
{
    char i;
    uchar col, row;
    uchar cv[] = {0x07, 0x0b, 0x0d, 0x0e};
    uchar rv[] = {0x70, 0xb0, 0xd0, 0xe0};
    KEY_MATRIX = 0x0f; // 列检测
    if (KEY_MATRIX != 0x0f)
    {
        col = KEY_MATRIX;
        Delay1ms(5); // 按键消抖
        if (col == KEY_MATRIX)
        {
            KEY_MATRIX = 0xf0; // 行检测
            if (KEY_MATRIX != 0xf0)
            {
                row = KEY_MATRIX;
                Delay1ms(5); // 按键消抖
                if (row == KEY_MATRIX)
                {
                    for (i = 3; i >= 0; --i)
                    {
                        if (cv[i] == col)
                            col = i;
                        if (rv[i] == row)
                            row = i << 2;
                    }
                    KEY_MATRIX = 0x0f;
                    return (col | row);
                }
            }
        }
    }
    KEY_MATRIX = 0x0f;
    return 0xff;
}

void Delay1ms(uint t) // 12MHz
{                     // 软件延迟 参数用uchar 比uint 精准
    uchar i;
    while (t--) // mov dec mov jnz orl jz : 9us
    {
        i = 123;    // mov : 1us
        while (i--) // mov dec mov jz : 6us * 124
            ;       // jmp : 2us * 123
    }               // 每当低位为 0 会多1us处理高位(dec) 忽略
} // (9+1+8*124-2=1000)us * t + ((t/256)+10+6)us 约 t ms

// ============== LCD1602 ==============

void LCD1602_ShowString(uchar* s, uint t)
{
    while (*s)
    {
        LCD1602_WriteData(*s++);
        Delay1ms(t);
    }
}

void LCD1602_Action(void)
{
#ifdef USE_LCD1602_ACTION
    uchar i;
#endif
    // 开启 LCD1602 显示 (initial)
    LCD1602_WriteCmd(Set_8bit_2line_5x7);   // 命令6
    LCD1602_WriteCmd(Show_CursorFlicker);   // 命令4
    LCD1602_WriteCmd(Mode_CursorRightMove); // 命令3
    LCD1602_WriteCmd(Clear_Screen);         // 命令1

#ifdef USE_LCD1602_ACTION
    // 开机界面
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(16)); // 命令8 设置光标在显示屏之外
    LCD1602_ShowString(lcd_action_line1, 0);
    LCD1602_WriteCmd(Move_Cursor_Row2_Col(16)); // 命令8
    LCD1602_WriteCmd(Mode_ScreenRightMove);     // 命令3
    LCD1602_ShowString(lcd_action_line2, 200); // 一边输出第二行 一边移动屏幕
    LCD1602_WriteCmd(Mode_CursorRightMove); // 命令3  恢复光标自增

    i = 3;
    do // 闪烁三次
    {
        LCD1602_WriteCmd(Show_ScreenOff); // 命令4
        Delay1ms(400);
        LCD1602_WriteCmd(Show_CursorOff); // 命令4
        Delay1ms(400);
    } while (--i);
    i = 16;
    do
    {
        LCD1602_WriteCmd(Shift_ScreenRight); // 命令5
        Delay1ms(200);
    } while (--i);                  // 字体移出屏幕
    LCD1602_WriteCmd(Clear_Screen); // 命令1 清屏
#endif
} // LCD1602 Action
