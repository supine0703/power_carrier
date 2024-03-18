/**
 * 1: 将单个按键组值赋给P1
 * 2: 单个将按键组的值显示LCD1602
 */
#include "crc16.h"
#include "instructions.h"
#include "serialport.h"
#include "slaves.h"

#define WAIT_TIME 40
#define CUT_TIME 4
#define RECEIVE(buf_p) SP_QReceiveData(buf_p, WAIT_TIME, CUT_TIME)
bit receiveCheck(unsigned char* buf);
void addrFunc(void);
void wordFunc(void);

extern unsigned char baudRateT;

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

void main(void)
{
    FuncInit();
    init();
    while (1)
        Func();
}

void int_4() interrupt 4 using 3
{
    ES = 0; // 禁止串行中断，防止在发送数据器件突然发送中断
    if (RI)
    {
        if (RB8)
            addrFunc();
        else
            wordFunc();
    }
    ES = 1;
}

bit receiveCheck(unsigned char* buf)
{ // 接收数据并检验真假
    unsigned char length = RECEIVE(buf);
    return CRC16_CHECK_XMODEM(buf, length);
}

void addrFunc(void)
{
    unsigned char buf[10];
    SP_QReceiveByte(buf, 0);
    switch (buf[0])
    {
    case _REN_ADDR_:
        if (SM2 == 1)
            break;
    case THIS_ADDR: // 建立信道
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

void wordFunc(void)
{
    unsigned char buf[10];
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
