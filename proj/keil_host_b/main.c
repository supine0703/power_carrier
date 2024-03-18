#include "__config__.h"
#include "crc16.h"
#include "instructions.h"
#include "serialport.h"

/**
 * 约定:
 *   - 只有host建立信道时发送的地址, 是单字节不带crc;
 *   - 从机(slave and pc)发送的所有信息均带crc 则至少3字节;
 *   - 从机信息为固定帧(3+2)+不定帧(n+2), n_max=32;
 *   - 若需用到不定帧: 固定帧为 _MORE_WORD_ + type + n(len) +crc(2);
 *   - 除了 CLOSE 和 广播 外, 主机有问必有答;
*/

#define WAIT_TIME 40
#define CUT_TIME 4

#define RECEIVE(buf_p) SP_QReceiveData(buf_p, WAIT_TIME, CUT_TIME)

unsigned char KeyValue(void);

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

extern unsigned char baudRateT;
// extern unsigned char buff_arr[];
// extern unsigned char buff_len;

void init(void)
{
    // 串口初始化
    SCON = 0xd0; // 设置串口控制寄存器SCON=1101 0000
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
        resetChannel(addr, buf);
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
    bit flg;
    createChannel(addr);
    if (channel)
    {
        buf[0] = _US_WORD_;
        buf[1] = word;
        SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 2), 0);
        if (!(flg = receiveCheck(buf)))
        {
            SP_QTransmitData(buf, 4, 0);
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
    init();

    while (1)
    {
        // createChannel(_PC_ADDR_);
        // if (channel)
        // {
        //     buf[0] = _ASK_WORD_;
        //     SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 1), 0);
        //     while (receiveCheck(buf) && buf[0] != _NULL_WORD_)
        //     {
        //         switch (buf[0])
        //         {
        //         case _SSU_WORD_:
        //             // SP_QTransmitData(buf, 5, 0);
        //             // continue;
        //             if (!updateSlaveState(buf[1], buf[2], buf))
        //             {
        //                 buf[0] = _SSU_WORD_;
        //                 buf[2] = 0xff;
        //                 CRC16_ADD_XMODEM(buf, 3);
        //             }
        //             createChannel(_PC_ADDR_);
        //             if (channel)
        //             {
        //                 SP_QTransmitData(buf, 5, 0);
        //                 continue;
        //             }
        //             else
        //                 channel = 0;
        //             break;
        //         case _ACK_WORD_:

        //             break;
        //         case _REN_WORD_:

        //             break;
        //         default:
        //             break;
        //         }
        //         if (!channel)
        //             break;
        //         buf[0] = _ASK_WORD_;
        //         SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 1), 0);
        //     }
        //     closeChannel();
        // }
        // else
        {
            // 接收按键
            key = KeyValue();
            while (KEY_MATRIX != 0x0f)
                ; // 松开按键后
            if (key != 0xff)
            {
                updateSlaveState((key >> 2) + 1, key, buf);
            }
        }
        // Delay1ms(5000);
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

// struct SerialPortBuff
// {
//     unsigned char buf[35];
//     unsigned char len;
// } data spb;

// unsigned int crc;
// unsigned int ct;

// bit receive(unsigned char len)
// {
//     unsigned char i;
//     // spb.len = len;
//     // if (parity)
//     //     len += 2;
//     // for (i = 0; i < len; ++i)
//     // {
//     //     while (!RI)
//     //         ;
//     //     RI = 0;
//     //     spb.buf[i] = SBUF;
//     // }
//     // if (parity)
//     //     return CRC16_XMODEM(spb.buf, 3) == *(unsigned int*)(spb.buf +
//     //     spb.len);
//     return 1;
// }

// void analysisData()
// {
//     unsigned char odder, type, bytes, i;
//     unsigned char buf;
//     odder = spb.buf[0];
//     type = spb.buf[1] >> 5;
//     bytes = spb.buf[1] & 0x1f;
//     for (i = 0; i < bytes; ++i)
//     {
//         // buf[i] =
//     }
// }

// bit createChannel(unsigned char addr)
// {
//     unsigned char buf;
//     TB8 = 1;
//     SP_QTransmitByte(addr);
//     while (!RI)
//         ;
//     RI = 0;
// }

// extern void _nop_(void);
// bit wait(unsigned char t) // 12MHz 1ms
// {
//     unsigned char i;
//     if (TB8)
//     {
//         TB8 = 1;
//         return 1;
//     }
//     if (t)
//         do
//         {
//             _nop_();
//             for (i = 24; i; --i)
//             {
//                 if (TB8)
//                 {
//                     TB8 = 1;
//                     return 1;
//                 }
//             }
//         } while (--t);
//     return 0;
// }

// void main(void)
// {
//     init(); // 初始化
//     TB8 = 0;
//     wait(1);
//     wait(10);
//     wait(100);
//     wait(200);

//     // 1. POST读取ROM 获取从机信息 如果存在
//     //
//     // 2. POST询问PC: 是否从PC读取从机信息
//     // 与PC建立信道
//     // TB8 = 1;
//     // SP_QTransmitByte(PC_ADDRESS); // PC只能接收8位 所以连发两次以表示地址
//     // SP_QTransmitByte(PC_ADDRESS); //

//     SM2 = 0;
//     TB8 = 0;
//     SP_QTransmitByte(0x01);
//     SM2 = 0;
//     TB8 = 1;
//     SP_QTransmitByte(0x02);
//     SM2 = 1;
//     TB8 = 0;
//     SP_QTransmitByte(0x03);
//     SM2 = 1;
//     TB8 = 1;
//     SP_QTransmitByte(0x04);
//     // receive(3, 1);
//     // 3.
// }
