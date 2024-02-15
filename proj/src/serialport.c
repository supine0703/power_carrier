/**
 * 作者：李宗霖 日期：2024/2/3
 * ----------------------------------------------------------------------------
 * 这个头文件对c51系列串口多机通信进行了处理
 *  - 发送:
 *     - 发送是一个主动的过程 中断方式不封装更简单 但是个人觉没必要
 *       因为会导致中断函数复杂膨胀 所以仅封装了查询方式
 *     - 对于双机通信 不需要发送地址 则可以只发送字节 也可以以字符串发送
 *     - 多机通信 需要发送地址 必须以字符串发送 因为发送多字符
 *       接收端不知道什么时候切换SM2
 *  - 接收:
 *     - 接收是一个被动的过程 也可以主动等待接收 所以中断和查询方式都封装了
 *     - 对于双机通信中断方式 反而更复杂了 不如直接写 没有封装
 *     - 对于多机中断同样很简单 但是需要在调用前初始化好 无法自我调节机制
 *     - 对于查询方式 因为主动查询所以需要设置等待时间 否则可能会死等
 *     - 对于双机和多机区别仅有匹配地址 所以多机调用双机的函数
 * ----------------------------------------------------------------------------
 * - 可以在 '__config__.h' 中通过宏定义选择需要编译的内容
 * @def USE_QUERY_TRANSMIT_B:      定义后会编译 SP_QReceiveByte
 * @def USE_QUERY_TRANSMIT_D:      定义后会编译 SP_QReceiveStr 及之前
 * @def USE_QUERY_TRANSMIT_A_D:    定义后会编译 SP_QReceiveAD 及之前
 * @def USE_INTERRUPT_RECEIVE:     定义后会编译 SP_IReceiveAD
 * @def USE_INTERRUPT_RECEIVE_A_D: 定义后会编译 SP_IReceiveAD
 * @def USE_QUERY_RECEIVER_D:      定义后会编译 SP_QReceiveD 双机(直接)通信
 * @def USE_QUERY_RECEIVER_A_D:    定义后会编译 SP_QReceiveAD 多机通信 及之前
 * - 可以选择定义
 * @def AR_ADDRESS: 定义某一个地址为全部内容接收但是不可回复
 * @def PC_ADDRESS: 定义PC上位机的地址码
 * @def BROADCAST_GROUP_END: 定义广播组的结束地址 如果没定义默认没有广播组
 * @def BROADCAST_GROUP_LEN: 定义广播组的地址数量 如果没定义默认为1
 * @def END_RECEIVE: 定义一个字符串结束标志符 默认是'$' 不能是0:'\0'
 */
#include "__config__.h"

#define uchar unsigned char
#define uint unsigned int

// 定义接收结束标志符
#ifndef END_RECEIVE
#define END_RECEIVE '$'
#endif

// 定义全部接收的地址码
#ifndef AR_ADDRESS
#define IS_ALL_RECEIVE(A) 0
#else
#define IS_ALL_RECEIVE(A) AR_ADDRESS == (A)
#endif

// 定义广播分组的地址码
#ifndef BROADCAST_GROUP_END
#define BELONG_BC_GROUP(X) 0
#else
#ifndef BROADCAST_GROUP_LEN
#define BROADCAST_GROUP_LEN 0x01
#endif
#define BELONG_BC_GROUP(X) BROADCAST_GROUP_END - (X) < BROADCAST_GROUP_LEN
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
    SP_QTWait(wait);
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
            if (answer && !IS_ALL_RECEIVE(addr) && !BELONG_BC_GROUP(addr))
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

bit SP_QReceiveD(void (*func)(void), void (*answer)(void), uint wait)
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
    uchar addr, void (*func)(void), void (*answer)(void), uint wait
)
{ // 查询方式进行按地址接收
    SM2 = 1;
    if (SP_QRWait(wait))
    {
        if (RB8 && (SBUF == addr || IS_ALL_RECEIVE(addr)))
        { // 收到地址 成功建立信道
            if (IS_ALL_RECEIVE(addr) || BELONG_BC_GROUP(addr))
                answer = 0;
            return SP_QReceiveD(func, answer, wait);
        }
    }
    return 0;
}
#endif

// ============================================================================
