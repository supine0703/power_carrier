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
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

extern void SP_QTransmitByte(unsigned char byte);

extern void SP_QTransmitData(unsigned char* buf, unsigned char n);

extern bit SP_QReceiveByte(unsigned char* buf, unsigned char t);

extern bit SP_QReceiveData(
    unsigned char* buf, unsigned char* len, unsigned char t
);

// 早期通用版
// // ----------------------------------------------------------------------------

// /**
//  * @def USE_QUERY_TRANSMIT_B: 定义后会编译 SP_QReceiveByte
//  * @def USE_QUERY_TRANSMIT_D: 定义后会编译 SP_QReceiveStr 及之前
//  * @def USE_QUERY_TRANSMIT_A_D: 定义后会编译 SP_QReceiveAD 及之前
//  * @param addr 从机地址码
//  * @param str 发送的字符串
//  * @param wait 发送信息间隔等待时长
//  * @return void
//  */
// extern void SP_QTransmitByte(unsigned char byte);
// extern void SP_QTransmitStr(unsigned char* str, unsigned int wait);
// extern void SP_QTransmitAD(
//     unsigned char addr, unsigned char* str, unsigned int wait
// );

// // ----------------------------------------------------------------------------

// /**
//  * @def USE_INTERRUPT_RECEIVE: 定义后会编译 SP_IReceiveAD
//  * @def USE_INTERRUPT_RECEIVE_A_D: 定义后会编译 SP_IReceiveAD
//  * @param addr 从机地址码
//  * @return 0: 不是串口接收允许中断; 成功建立信道; 关闭信道
//  * @return 1: 接收有效信息
//  * @return 0->1: 成功建立信道并收到有效信息
//  * @return 1->0: 接收信息完毕 关闭信道
//  */
// extern bit SP_IReceiveAD(
//     unsigned char addr, void (*func)(void), void (*answer)(void)
// );

// // ----------------------------------------------------------------------------

// /**
//  * @def USE_QUERY_RECEIVER_D: 定义后会编译 SP_QReceiveD 双机(直接)通信
//  * @def USE_QUERY_RECEIVER_A_D: 定义后会编译 SP_QReceiveAD 多机通信 及之前
//  * @param addr 从机地址码
//  * @param wait 接收信息等待响应时长
//  * @param func 对接收到的信息进行处理的函数指针
//  * @param answer 接收完所有信息后对主机应答的函数指针
//  * @return 0: 对应地址的从机没有接收到信息
//  * @return 1: 有接收信息
//  */
// extern bit SP_QReceiveD(
//     void (*func)(void), void (*answer)(void), unsigned int wait
// ); // 接收数据
// extern bit SP_QReceiveAD(
//     unsigned char addr,
//     void (*func)(void),
//     void (*answer)(void),
//     unsigned int wait
// ); // 接收地址匹配后接收数据

// // ----------------------------------------------------------------------------

#endif // COMMUNICATION_H