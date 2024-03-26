#include "__config__.h"
#include "crc16.h"
#include "instructions.h"
#include "serialport.h"
#include "lcd1602.h"

/**
 * 约定:
 *   - 只有host建立信道时发送的地址, 是单字节不带crc;
 *   - 从机(slave and pc)发送的所有信息均带crc 则至少3字节;
 *   - 从机信息为固定帧(3+2)+不定帧(n+2), n_max=32;
 *   - 若需用到不定帧: 固定帧为 _MORE_WORD_ + type + n(len) +crc(2);
 *   - 除了 CLOSE 和 广播 外, 主机有问必有答;
*/

#define WAIT_TIME 255
#define CUT_TIME 127

#define RECEIVE(buf_p) SP_QReceiveData(buf_p, WAIT_TIME, CUT_TIME)

unsigned char KeyValue(void);
void Delay1ms(unsigned int t);
void LCD1602_Action(void);
void LCD1602_ShowString(unsigned char* s, unsigned char t);

extern unsigned char baudRateT;
// extern unsigned char buff_arr[];
// extern unsigned char buff_len;

void init(void)
{
    // 串口初始化
    SCON = 0x50; // 设置串口控制寄存器SCON=0101 0000
    PCON = 0x00; // 设定电源控制寄存器PCON，这里表示波特率不加倍
    TMOD = 0x20;           // 定时器T1的工作方式2
    TH1 = TL1 = baudRateT; // 波特率 9600bps
    TR1 = 1;               // 启动定时器
}

bit receiveCheck(unsigned char* buf)
{ // 接收数据并检验真假
    unsigned char length = RECEIVE(buf);
    return CRC16_CHECK_XMODEM(buf, length);
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
bit channel = 0;

void closeChannel()
{
    TB8 = 1;
    SP_QTransmitByte(_CLOSE_ADDR_);
    Delay1ms(CUT_TIME);
    channel = 0;
}

void resetChannel(unsigned char addr, unsigned char* buf)
{
    SP_QTransmitByte(_REN_ADDR_);
    channel = receiveCheck(buf);
    if (!channel)
    {
        closeChannel();
        SP_QTransmitByte(addr);
        channel = receiveCheck(buf);
    }
}

void createChannel(unsigned char addr)
{
    unsigned char buf[4];
    TB8 = 1;
    if (channel)
        closeChannel();
    SP_QTransmitByte(addr);
    channel = receiveCheck(buf);

    if (!channel)
    {
        // SP_QTransmitData(buf, 4, 0);
        resetChannel(addr, buf);
    }
    if (channel)
    {
        channel = (buf[0] == _ACK_WORD_) && (buf[1] == addr);
        if (!channel)
        {
            resetChannel(addr, buf);
            channel = (buf[0] == _ACK_WORD_) && (buf[1] == addr);
        }
    }
    if (!channel)
        closeChannel();
    TB8 = !channel;
}

/* ========================================================================== */

bit updateSlaveState(unsigned char addr, unsigned char word, unsigned char* buf)
{
    bit flg = 0;
    createChannel(addr);
    if (channel)
    {
        buf[0] = _US_WORD_;
        buf[1] = word;
        SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 2), 1);
        if (!(flg = receiveCheck(buf)))
        {
            SP_QTransmitData(buf, 4, 1);
            flg = receiveCheck(buf);
        }
    }
    if (!flg)
        buf[1] = word;
    return flg;
}

void main(void)
{
    unsigned char buf[10];
    unsigned char key;

    LCD1602_Action();
    init();

    SP_QTransmitByte(_ACTION_);
    Delay1ms(CUT_TIME);

    LCD1602_ShowString(":", 0);

    while (1)
    {
        createChannel(_PC_ADDR_);
        if (channel)
        {
            buf[0] = _ASK_WORD_;
            SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 1), 0);
            while (receiveCheck(buf) && buf[0] != _NULL_WORD_)
            {
                switch (buf[0])
                {
                case _SSU_WORD_:
                    // SP_QTransmitData(buf, 5, 0);
                    // continue;
                    if (!updateSlaveState(buf[1], buf[2], buf))
                    {
                        buf[0] = _SSU_WORD_;
                        buf[2] = 0xff;
                        CRC16_ADD_XMODEM(buf, 3);
                    }
                    else
                    {
                        buf[0] = 0xcc;
                        buf[1] = 0xcc;
                        buf[2] = 0xcc;
                        buf[3] = 0xcc;
                    }
                    createChannel(_PC_ADDR_);
                    if (channel)
                    {
                        SP_QTransmitData(buf, 5, 0);
                        continue;
                    }
                    else
                        channel = 0;
                    break;
                case _ACK_WORD_:

                    break;
                case _REN_WORD_:

                    break;
                default:
                    break;
                }
                if (!channel)
                    break;
                buf[0] = _ASK_WORD_;
                SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 1), 0);
            }
            closeChannel();
        }
        // else
        // {
        //     // 接收按键
        //     key = KeyValue();
        //     while (KEY_MATRIX != 0x0f)
        //         ; // 松开按键后
        //     if (key != 0xff)
        //     {
        //         updateSlaveState((key >> 2) + 1, key, buf);
        //     }
            
        // }
        Delay1ms(5000);
    }
}

void Delay1ms(unsigned int t) // 12MHz
{                             // 软件延迟 参数用uchar 比uint 精准
    unsigned char i;
    while (t--) // mov dec mov jnz orl jz : 9us
    {
        i = 123;    // mov : 1us
        while (i--) // mov dec mov jz : 6us * 124
            ;       // jmp : 2us * 123
    }               // 每当低位为 0 会多1us处理高位(dec) 忽略
} // (9+1+8*124-2=1000)us * t + ((t/256)+10+6)us 约 t ms


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
