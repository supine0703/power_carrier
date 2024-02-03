/**
 * 作者：李宗霖 日期：2024/2/3
 */

#include "__config__.h"
/**
 * 可以在 '__config__.h' 中通过宏定义选择不编译发送或者接收
 */

#define uchar unsigned char

#ifndef NO_TRANSMIT
void TransmitByte(uchar byte)
{
    SBUF = byte;
    while (!TI)
        ; // 判断是否发送成功(发送成功后TI会置1 需手动清0)
    TI = 0;
}

void Transmit(uchar addr, uchar* str)
{
    TB8 = 1;
    TransmitByte(addr);
    TB8 = 0; // 发送数据
    while (*str)
    {
        TransmitByte(*str++);
    }
    TransmitByte('$'); // 字符串结束标志
}
#endif

#ifndef NO_RECEIVE
bit Receive(uchar addr)
{
    if (RI)
    { // 是串口接收允许中断
        RI = 0;
        if (RB8 == 1) // 收到地址
        {
            if (SBUF == addr || addr == 0) // 判断主机进行中断的函数
                SM2 = 0; // 成功建立信道
        }
        else if (SBUF == '$')
            SM2 = 1; // 关闭信道 等待下一次接收地址
        else
            return 1; // 接收到有效信息
    }
    return 0;
}
#endif
