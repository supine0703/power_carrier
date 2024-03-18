/**
 * 1: 将单个按键组值赋给P1
 * 2: 单个将按键组的值显示LCD1602
 */
#define MODEL 3
#include "__config__.h"
#include "instructions.h"
#include "serialport.h"
#include "crc16.h"

#define uchar unsigned char
#define uint unsigned int

#define WAIT_TIME 40
#define CUT_TIME 4

#define RECEIVE(buf_p) SP_QReceiveData(buf_p, WAIT_TIME, CUT_TIME)

#define CONNECT(X, Y) X##Y
#define CONNECT__(X, Y) CONNECT(X, Y)
#define CONNECT___(X, Y, Z) CONNECT__(X, CONNECT(Y, Z))

extern uchar baudRateT;

typedef void (*v_p_v)(void); // 定义一个函数指针类
v_p_v pFunc, pAnswer;

bit updateState(unsigned char word);

#if MODEL == 0
#define THIS_ADDR 0x00
uchar addr = 0x00;
#define NO_FUNC_INIT

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

void CONNECT___(Func, MODEL, _1)(void)
{
    P1 = SBUF - (SBUF >= 65 ? 55 : 48);
}
#elif MODEL == 1
#define THIS_ADDR 0x01
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

void CONNECT___(Func, MODEL, _1)(void)
{
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
    LCD1602_WriteData('Y');
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

#elif MODEL == 2
#define THIS_ADDR 0x02
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

void CONNECT___(Func, MODEL, _1)(void)
{
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
    LCD1602_WriteData(SBUF);
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
#elif MODEL == 3
#define THIS_ADDR 0x03
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

void CONNECT___(Func, MODEL, _1)(void)
{
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
    LCD1602_WriteData(SBUF);
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

void CONNECT___(Func, MODEL, _1)(void)
{
    LCD1602_WriteCmd(Move_Cursor_Row1_Col(9));
    LCD1602_WriteData(SBUF);
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
#endif

void Answer()
{
    SP_QTransmitByte(MODEL + '0');
}

void init(void)
{
    // 串口初始化
    SCON = 0xf0; // 设置串口控制寄存器SCON=1111 0000
    PCON = 0x00; // 设定电源控制寄存器PCON，这里表示波特率不加倍
    TMOD = 0x20; // 定时器T1的工作方式2
    TH1 = TL1 = baudRateT;
    TR1 = 1; // 启动定时器

    // 中断初始化
    EA = 1; // 中断允许
    ES = 1; // 串口中断允许
}

bit receiveCheck(unsigned char* buf)
{ // 接收数据并检验真假
    unsigned char length = RECEIVE(buf);
    return CRC16_CHECK_XMODEM(buf, length);
}

void main(void)
{
#ifndef NO_FUNC_INIT
    FuncInit();
#endif
    init();
    pFunc = CONNECT___(Func, MODEL, _1);
    pAnswer = Answer;
    while (1)
        ;
}

void int_4() interrupt 4
{
    unsigned char buf[10];
    ES = 0; // 禁止串行中断，防止在发送数据器件突然发送中断
    if (RI)
    {
        if (RB8)
        { // 建立信道
            RI = 0;
            buf[0] = SBUF;
            switch (buf[0])
            {
            case _REN_ADDR_:
                if (SM2 == 1)
                    break;
            case THIS_ADDR:
                SM2 = 0;
                buf[0] = _ACK_WORD_;
                buf[1] = THIS_ADDR;
                SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 2), 0);
                break;
            case _CLOSE_ADDR_:
                SM2 = 1;
                break;
            default:
                break;
            }
        }
        else
        { // 通信
            if (receiveCheck(buf))
            {
                switch (buf[0])
                {
                case _US_WORD_:
                    buf[2] = updateState(buf[1]) ? buf[1] : 0xff;
                    buf[0] = _SSU_WORD_;
                    buf[1] = THIS_ADDR;
                    SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 3), 0);
                    break;
                case _ASK_WORD_:

                    // if no
                    buf[0] = _NULL_WORD_;
                    SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 1), 0);
                    break;
                default:
                    break;
                }
            }
        }
    }
    // SP_IReceiveAD(addr, pFunc, pAnswer);
    // SP_QReceiveAD(addr, 1, FUNC(MODEL), Answer);
    ES = 1;
}