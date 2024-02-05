/**
 * 作者：李宗霖 日期：2024/2/3
 */

#include "__config__.h"
/**
 * 可以在 '__config__.h' 中通过宏定义选择不编译发送或者接收
 * 用于方式二或三多机通信
 */

#define uchar unsigned char
#define uint unsigned int

#ifndef END_RECEIVE
#define END_RECEIVE '$'
#endif

#ifndef ALL_RECEIVE
#define IS_ALL_RECEIVE(A) 0
#define NO_ALL_RECEIVE(A) 1
#else
#define IS_ALL_RECEIVE(A) ALL_RECEIVE == (A)
#define NO_ALL_RECEIVE(A) ALL_RECEIVE != (A)
#endif

// ============================================================================

#if defined(USE_QUERY_TRANSMIT_B) || defined(USE_QUERY_TRANSMIT_D) ||          \
    defined(USE_QUERY_TRANSMIT_A_D)
void SP_QTransmitByte(uchar byte)
{
    SBUF = byte;
    while (!TI)
        ; // 判断是否发送成功(发送成功后TI会置1 需手动清0)
    TI = 0;
}
#endif

#if defined(USE_QUERY_TRANSMIT_D) || defined(USE_QUERY_TRANSMIT_A_D)
void SP_QTWait(uint t) // 12MHz 1ms
{
    uchar i;
    while (t--)
    {
        i = 123;
        while (i--)
            ;
    }
}

void SP_QTransmitStr(uchar* str, uint wait)
{
    while (*str)
    {
        SP_QTransmitByte(*str++);
        SP_QTWait(wait);
    }
    SP_QTransmitByte(END_RECEIVE); // 字符串结束标志
}
#endif

#ifdef USE_QUERY_TRANSMIT_A_D
void SP_QTransmitAD(uchar addr, uchar* str, uint wait)
{
    TB8 = 1; // 发送地址
    SP_QTransmitByte(addr);
    TB8 = 0; // 发送数据
    SP_QTransmitStr(str, wait);
}
#endif

// ============================================================================

// ============================================================================

#if defined(USE_INTERRUPT_RECEIVE) || defined(USE_INTERRUPT_RECEIVE_A_D)
bit SP_IReceiveAD(uchar addr, void (*func)(void), void (*answer)(void))
{ // 中断方式进行按地址接收
    if (RI)
    { // 是串口接收允许中断
        RI = 0;
        if (RB8) // 收到地址
        {
            if (SBUF == addr || IS_ALL_RECEIVE(addr))
                SM2 = 0; // 成功建立信道
        }
        else if (SBUF == END_RECEIVE)
        {
            if (answer && NO_ALL_RECEIVE(addr))
                answer();
            SM2 = 1; // 关闭信道 等待下一次接收地址
        }
        else
        {
            if (func)
                func();
            return 1; // 接收到有效信息
        }
    }
    return 0;
}
#endif

// ============================================================================

// ============================================================================

#if defined(USE_QUERY_RECEIVE_D) || defined(USE_QUERY_RECEIVE_A_D)
bit SP_QRWait(uint t) // 12MHz 1ms
{
    uchar i;
    if (RI)
    {
        RI = 0;
        return 1;
    }
    while (t--)
    {
        i = 123;
        while (i--)
            if (RI)
            {
                RI = 0;
                return 1;
            }
    }
    return 0;
}

bit SP_QReceiveD(uint wait, void (*func)(void), void (*answer)(void))
{ // 查询方式进行接收
    SM2 = 0;
    if (SP_QRWait(wait))
    {
        if (SBUF != END_RECEIVE)
            do
            {
                if (func)
                    func();
            } while (SBUF != END_RECEIVE && SP_QRWait(wait));
        if (answer)
            answer();
        SM2 = 1;
        return 1;
    }
    return 0;
}
#endif

#ifdef USE_QUERY_RECEIVE_A_D
bit SP_QReceiveAD(
    uchar addr, uint wait, void (*func)(void), void (*answer)(void)
)
{ // 查询方式进行按地址接收
    SM2 = 1;
    if (SP_QRWait(wait))
    {
        if (RB8 == 1 && (SBUF == addr || IS_ALL_RECEIVE(addr)))
        { // 收到地址 成功建立信道
            if (addr == ALL_RECEIVE)
                answer = 0;
            return SP_QReceiveD(wait, func, answer);
        }
    }
    return 0;
}
#endif

// ============================================================================
