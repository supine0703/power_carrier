#include "__config__.h"
#include "crc16.h"
#include "instructions.h"
#include "lcd1602.h"
#include "serialport_2.h"

/**
 * 约定:
 *   - 只有host建立信道时发送的地址, 是单字节不带crc;
 *   - 从机(slave and pc)发送的所有信息均带crc 则至少3字节;
 *   - 除了 CLOSE 和 广播 外, 主机有问必有答;
*/

#define WITH_PC 0
#define WITH_SLAVE 1

#define R_PC_WAIT_TIME 20
#define R_PC_CUT_TIME 10

#define R_SLAVE_WAIT_TIME 100
#define R_SLAVE_CUT_TIME 4

unsigned char KeyValue(void);
void Delay1ms(unsigned int t);
void LCD1602_Action(void);
void LCD1602_ShowString(unsigned char* s, unsigned char t);

void init(void)
{
    extern unsigned char baudRateT;
    // 串口1初始化
    SCON = 0x50; // 设置串口控制寄存器SCON=0101 0000
    PCON = 0x00; // 设定电源控制寄存器PCON，这里表示波特率不加倍
    TMOD = 0x20;           // 定时器T1的工作方式2
    TH1 = TL1 = baudRateT; // 波特率 9600bps
    TR1 = 1;               // 启动定时器

    // 串口2初始化
    S2CON = 0x50;    // 串口2 方式1 10位异步收发 S2REN=1允许接收
    BRT = baudRateT; // 独立波特率发生器初值
    AUXR = 0x10;     // BRTR=1 独立波特率发生器开始计数

    // EA = 1;     //开总中断
    // ES = 1;     //开串口1中断
    // IE2 = 0x01; //开串口2中断  ES2=1
}

/* ========================================================================== */
/**
 * 建立信道: 
 *   - 如果接收到信息有误, 则重来: 发送REN接收, 若有误先关闭信道再发送REN接收
 *     - 如果重来任然有误则失败
 *   - 如果接收到正确信息, 则判断地址内容是否正确: _ACK_WORD_ + addr + _NULL_WORD_
 *     - 如果错误, 则重来并重新判断地址内容
 *   - 如果都正确则成功建立信道, 否则失败
*/
bit pcChannel = 0;
bit slaveChannel = 0;

bit receiveCheckWithPC(unsigned char* buf)
{ // 接收数据并检验真假
    unsigned char length =
        SP_QReceiveData(buf, R_PC_WAIT_TIME, R_PC_CUT_TIME, WITH_PC);
    return CRC16_CHECK_XMODEM(buf, length);
}

void closePCChannel()
{
    SP_QTransmitByte(_CLOSE_ADDR_, WITH_PC);
    Delay1ms(R_PC_CUT_TIME);
    pcChannel = 0;
}

void createPCChannel()
{
    unsigned char buf[4];
    if (pcChannel)
        return;
    SP_QTransmitByte(_PC_ADDR_, WITH_PC);
    pcChannel = receiveCheckWithPC(buf);
    if (!pcChannel)
    {
        closePCChannel();
        SP_QTransmitByte(_PC_ADDR_, WITH_PC);
        pcChannel = receiveCheckWithPC(buf);
        if (!pcChannel)
            closePCChannel();
    }
}

bit receiveCheckWithSlave(unsigned char* buf, unsigned char* len)
{ // 接收数据并检验真假
    unsigned char length =
        SP_QReceiveData(buf, R_SLAVE_WAIT_TIME, R_SLAVE_CUT_TIME, WITH_SLAVE);
    if (len)
        *len = length;
    return CRC16_CHECK_XMODEM(buf, length);
}

void closeSlaveChannel()
{
    S2_SET_H(S2TB8);
    SP_QTransmitByte(_CLOSE_ADDR_, WITH_SLAVE);
    Delay1ms(R_SLAVE_CUT_TIME);
    slaveChannel = 0;
}

void resetSlaveChannel(unsigned char addr, unsigned char* buf)
{
    SP_QTransmitByte(_REN_ADDR_, WITH_SLAVE);
    slaveChannel = receiveCheckWithSlave(buf, 0);
    if (!slaveChannel)
    {
        closeSlaveChannel();
        SP_QTransmitByte(addr, WITH_SLAVE);
        slaveChannel = receiveCheckWithSlave(buf, 0);
    }
}

void createSlaveChannel(unsigned char addr)
{
    unsigned char buf[4];
    S2_SET_H(S2TB8);
    if (slaveChannel)
        closeSlaveChannel();
    SP_QTransmitByte(addr, WITH_SLAVE);
    slaveChannel = receiveCheckWithSlave(buf, 0);

    if (!slaveChannel)
        resetSlaveChannel(addr, buf);
    if (slaveChannel)
    {
        slaveChannel = (buf[0] == _ACK_WORD_) && (buf[1] == addr);
        if (!slaveChannel)
        {
            resetSlaveChannel(addr, buf);
            slaveChannel = (buf[0] == _ACK_WORD_) && (buf[1] == addr);
        }
    }
    if (!slaveChannel)
        closeSlaveChannel();
    else
        S2_SET_L(S2TB8);
}

bit updateSlaveState(unsigned char addr, unsigned char word, unsigned char* buf)
{
    bit flg;
    unsigned char len;
    unsigned char rBuf[4];
    closePCChannel();
    SP_QTransmitByte(addr, WITH_PC);
    Delay1ms(R_PC_CUT_TIME);
    createSlaveChannel(addr);
    if (slaveChannel)
    {
        rBuf[0] = _US_WORD_;
        rBuf[1] = word;
        len = CRC16_ADD_XMODEM(rBuf, 2);
        SP_QTransmitData(rBuf, len, WITH_PC);
        Delay1ms(R_PC_CUT_TIME);
        SP_QTransmitData(rBuf, len, WITH_SLAVE);
        if (!(flg = receiveCheckWithSlave(buf, &len)))
        {
            SP_QTransmitData(rBuf, len, WITH_SLAVE);
            flg = receiveCheckWithSlave(buf, &len);
        }
        closeSlaveChannel();
        if (len)
        {
            SP_QTransmitData(buf, len, WITH_PC);
            Delay1ms(R_PC_CUT_TIME);
        }
    }
    else
    {
        rBuf[0] = _NULL_WORD_;
        SP_QTransmitData(rBuf, CRC16_ADD_XMODEM(rBuf, 1), WITH_PC);
        Delay1ms(R_PC_CUT_TIME);
    }
    if (!flg)
        buf[1] = word;
    createPCChannel();
    return flg;
}

void main(void)
{
    bit flg;
    unsigned char buf[16];
    unsigned char len;
    // LCD1602_Action();
    init();

    // SP_Set_QTWait(Delay1ms);

    SP_QTransmitByte(_ACTION_, WITH_PC);
    Delay1ms(R_PC_CUT_TIME);

    while (1)
    {
        if (!pcChannel)
            createPCChannel();
        if (pcChannel)
        {
            buf[0] = _ASK_WORD_;
            SP_QTransmitData(buf, len = CRC16_ADD_XMODEM(buf, 1), WITH_PC);
            while ((flg = receiveCheckWithPC(buf)) && buf[0] != _NULL_WORD_)
            {
                switch (buf[0])
                {
                case _SSU_WORD_:
                    // SP_QTransmitData(buf, 5, 0, WITH_PC);
                    // continue;
                    if (!updateSlaveState(buf[1], buf[2], buf))
                    {
                        buf[0] = _SSU_WORD_;
                        buf[2] = 0xff;
                        len = CRC16_ADD_XMODEM(buf, 3);
                    }
                    if (pcChannel)
                    {
                        SP_QTransmitData(buf, len, WITH_PC);
                        Delay1ms(R_PC_CUT_TIME);
                        continue;
                    }
                    break;
                case _ACK_WORD_:

                    break;
                case _REN_WORD_:

                    break;
                default:
                    break;
                }
                if (!pcChannel)
                    break;
                buf[0] = _ASK_WORD_;
                SP_QTransmitData(buf, len = CRC16_ADD_XMODEM(buf, 1), WITH_PC);
                Delay1ms(1000);
            }
            if (flg)
                closePCChannel();
        }
    }
}

// void closeChannel()
// {
//     TB8 = 1;
//     SP_QTransmitByte(_CLOSE_ADDR_);
//     Delay1ms(CUT_TIME);
//     channel = 0;
// }

// void resetChannel(unsigned char addr, unsigned char* buf)
// {
//     SP_QTransmitByte(_REN_ADDR_);
//     channel = receiveCheck(buf);
//     if (!channel)
//     {
//         closeChannel();
//         SP_QTransmitByte(addr);
//         channel = receiveCheck(buf);
//     }
// }

// void createChannel(unsigned char addr)
// {
//     unsigned char buf[4];
//     TB8 = 1;
//     if (channel)
//         closeChannel();
//     SP_QTransmitByte(addr);
//     channel = receiveCheck(buf);

//     if (!channel)
//     {
//         SP_QTransmitData(buf, 4, 0);
//         // resetChannel(addr, buf);
//     }
//     if (channel)
//     {
//         channel = (buf[0] == _ACK_WORD_) && (buf[1] == addr);
//         if (!channel)
//         {
//             resetChannel(addr, buf);
//             channel = (buf[0] == _ACK_WORD_) && (buf[1] == addr);
//         }
//     }
//     if (!channel)
//         closeChannel();
//     TB8 = !channel;
// }

// /* ========================================================================== */

// bit updateSlaveState(unsigned char addr, unsigned char word, unsigned char* buf)
// {
//     bit flg;
//     createChannel(addr);
//     if (channel)
//     {
//         buf[0] = _US_WORD_;
//         buf[1] = word;
//         SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 2), 1);
//         if (!(flg = receiveCheck(buf)))
//         {
//             SP_QTransmitData(buf, 4, 1);
//             flg = receiveCheck(buf);
//         }
//     }
//     if (!flg)
//         buf[1] = word;
//     return flg;
// }

// void main(void)
// {
//     unsigned char buf[10];
//     unsigned char key;

//     LCD1602_Action();
//     init();

//     SP_QTransmitByte(_ACTION_);
//     Delay1ms(CUT_TIME);

//     LCD1602_ShowString(":", 0);

//     while (1)
//     {
//         createChannel(_PC_ADDR_);
//         if (channel)
//         {
//             buf[0] = _ASK_WORD_;
//             SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 1), 0);
//             while (receiveCheck(buf) && buf[0] != _NULL_WORD_)
//             {
//                 switch (buf[0])
//                 {
//                 case _SSU_WORD_:
//                     // SP_QTransmitData(buf, 5, 0);
//                     // continue;
//                     if (!updateSlaveState(buf[1], buf[2], buf))
//                     {
//                         buf[0] = _SSU_WORD_;
//                         buf[2] = 0xff;
//                         CRC16_ADD_XMODEM(buf, 3);
//                     }
//                     createChannel(_PC_ADDR_);
//                     if (channel)
//                     {
//                         SP_QTransmitData(buf, 5, 0);
//                         continue;
//                     }
//                     else
//                         channel = 0;
//                     break;
//                 case _ACK_WORD_:

//                     break;
//                 case _REN_WORD_:

//                     break;
//                 default:
//                     break;
//                 }
//                 if (!channel)
//                     break;
//                 buf[0] = _ASK_WORD_;
//                 SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 1), 0);
//             }
//             closeChannel();
//         }
//         // else
//         // {
//         //     // 接收按键
//         //     key = KeyValue();
//         //     while (KEY_MATRIX != 0x0f)
//         //         ; // 松开按键后
//         //     if (key != 0xff)
//         //     {
//         //         updateSlaveState((key >> 2) + 1, key, buf);
//         //     }

//         // }
//         Delay1ms(5000);
//     }
// }

void Delay1ms(unsigned int t) // 11.0592 约1ms
{
    unsigned char i, j;
    while (t)
    {
        t--;
        for (i = 11; i; --i)
            for (j = 248; j; --j)
                ;
    }
}

unsigned char KeyValue(void)
{
    char i;
    unsigned char col, row;
    unsigned char cv[] = {0x07, 0x0b, 0x0d, 0x0e};
    unsigned char rv[] = {0x70, 0xb0, 0xd0, 0xe0};
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

void LCD1602_ShowString(unsigned char* s, unsigned int t)
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
    LCD1602_WriteCmd(Show_CursorOn);        // 命令4
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

/* //测试双串口通信程序
#define WAIT_TIME 50
#define CUT_TIME 40

#define S1_RECEIVE(buf_p) SP1_QReceiveData(buf_p, WAIT_TIME, CUT_TIME)

#define S2_RECEIVE(buf_p) SP2_QReceiveData(buf_p, WAIT_TIME, CUT_TIME)

unsigned char KeyValue(void);
void Delay1ms(unsigned int t);
void LCD1602_Action(void);
void LCD1602_ShowString(unsigned char* s, unsigned char t);

void init(void)
{
    extern unsigned char baudRateT;
    // 串口1初始化
    SCON = 0x50; // 设置串口控制寄存器SCON=0101 0000
    PCON = 0x00; // 设定电源控制寄存器PCON，这里表示波特率不加倍
    TMOD = 0x20;           // 定时器T1的工作方式2
    TH1 = TL1 = baudRateT; // 波特率 9600bps
    TR1 = 1;               // 启动定时器

    // 串口2初始化
    S2CON = 0x50;    // 串口2 方式1 10位异步收发 S2REN=1允许接收
    BRT = baudRateT; // 独立波特率发生器初值
    AUXR = 0x10;     // BRTR=1 独立波特率发生器开始计数

    EA = 1;     //开总中断
    ES = 1;     //开串口1中断
    IE2 = 0x01; //开串口2中断  ES2=1
}

unsigned char num[2] = {0, 0};
unsigned char str1[] = "1 to 2:";
unsigned char str2[] = "2 to 1:";
unsigned char send[][5] = {"123$", "abcd", "ZYX5"};
unsigned char buf[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char len, c = 0;
// test
void main(void)
{
    
    init();
    LCD1602_Action();

    LCD1602_WriteCmd(Move_Cursor_Row1_Col(0));
    LCD1602_ShowString(str1, 4000);
    LCD1602_WriteCmd(Move_Cursor_Row2_Col(0));
    LCD1602_ShowString(str2, 4000);

    while (1)
    {
        // 1 -> 2
        Delay1ms(6000);
        LCD1602_WriteCmd(Clear_Screen);
        len = 0;
        SP1_QTransmitData(send[c++ % 3], 5, 10);
        // len = S2_RECEIVE(buf);
        SP1_QTransmitData(buf, len, 10);
        buf[len] = 0;
        num[0] = len + '0';

        // LCD1602_WriteCmd(Move_Cursor_Row1_Col(0));
        // LCD1602_ShowString("                ", 1);
        LCD1602_WriteCmd(Move_Cursor_Row1_Col(0));
        LCD1602_ShowString(str1, 4000);
        if (*buf == 0)
            LCD1602_ShowString("N", 4000);
        else
            LCD1602_ShowString(buf, 4000);

        // 2 -> 1
        Delay1ms(6000);
        len = 0;
        SP2_QTransmitData(send[c++ % 3], 5, 10);
        // len = S1_RECEIVE(buf);
        SP1_QTransmitData(buf, len, 10);
        buf[len] = 0;
        num[0] = len + '0';
        // LCD1602_WriteCmd(Move_Cursor_Row2_Col(0));
        // LCD1602_ShowString("                ", 1);
        LCD1602_WriteCmd(Move_Cursor_Row2_Col(0));
        LCD1602_ShowString(str2, 4000);
        if (*buf == 0)
            LCD1602_ShowString("N", 4000);
        else
            LCD1602_ShowString(buf, 4000);
    }
}

// 串行口1中断处理函数
void UART_1Interrupt(void) interrupt 4
{
	if(RI==1)
	{
		RI=0;
		buf[len] = SBUF;
        len++;
	}
}
// 串行口2中断处理函数
void UART_2Interrupt(void) interrupt 8
{
	if(S2_CHECK(S2RI))
	{
		S2_SET_L(S2RI);
		buf[len] = S2BUF;
        len++;
	} 
}
*/
