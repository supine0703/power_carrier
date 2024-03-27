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
bit receiveCheck(unsigned char* buf, unsigned char* len);
void addrFunc(unsigned char* buf);
void wordFunc(unsigned char* buf);
void crcErrFunc(void);

void init(void)
{
    extern unsigned char baudRateT;
    // 串口初始化
    SCON = 0x50; // 设置串口控制寄存器SCON=1111 0000
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

void int_4() interrupt 4
{
    bit flg;
    unsigned char len;
    unsigned char buf[10];
    ES = 0; // 禁止串行中断，防止在发送数据器件突然发送中断
    if (RI)
    {
        // RI = 0;
        // SP_QTransmitByte(SBUF);
        if (SM0)
        { // 如果11位通信
            if (RB8)
            {
                SP_QReceiveByte(buf, 0);
                addrFunc(buf);
            }
            else if (receiveCheck(buf, 0))
                wordFunc(buf);
            else
                crcErrFunc();

        }
        else
        { // 如果是10位通信
            flg = receiveCheck(buf, &len);
            if (len == 1)
                addrFunc(buf);
            else if (flg)
                wordFunc(buf);
            else
                crcErrFunc();
        }
    }
    ES = 1;
}

bit receiveCheck(unsigned char* buf, unsigned char* len)
{ // 接收数据并检验真假
    unsigned char length = RECEIVE(buf);
    if (len)
        *len = length;
    return CRC16_CHECK_XMODEM(buf, length);
}

void addrFunc(unsigned char* buf)
{
    switch (buf[0])
    {
    case _REN_ADDR_:
        if (SM2 == 1)
            break;
    case THIS_ADDR: // 建立信道
        SM2 = 0;
        buf[0] = _ACK_WORD_;
        buf[1] = THIS_ADDR;
        SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 2));
        break;
    case _CLOSE_ADDR_:
        SM2 = 1;
        break;
    default:
        break;
    }
}

void wordFunc(unsigned char* buf)
{
    switch (buf[0])
    {
    case _US_WORD_:
        buf[0] = updateState(buf[1]) ? _SSU_WORD_ : _SNS_WORD_;
        buf[2] = buf[1];
        buf[1] = THIS_ADDR;
        SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 3));
        break;
    case _ASK_WORD_:
        // if no
        buf[0] = _NULL_WORD_;
        SP_QTransmitData(buf, CRC16_ADD_XMODEM(buf, 1));
        break;
    default:
        break;
    }
}

void crcErrFunc(void)
{
    unsigned char err[4] = {_REN_WORD_, _ERR_CRC_REN_};
    SP_QReceiveByte(err, CRC16_ADD_XMODEM(err, 3));
}
