/**
 * 作者：李宗霖 日期：2024/2/3
 * 可以在 '__config__.h' 中通过宏定义选择不编译发送或者接收
 */
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

/**
 * @def NO_TRANSMIT: 定义则不让函数被编译
 * @param addr 从机地址码
 * @param str 发送的字符串
 * @return void
 */
extern void Transmit(unsigned char addr, unsigned char* str);

/**
 * @def NO_RECEIVE: 定义则不让函数被编译
 * @param addr 从机地址码
 * @return 0: 不是串口接收允许中断; 成功建立信道; 关闭信道
 * @return 1: 接收有效信息
 * @return 0->1: 成功建立信道并收到有效信息
 * @return 1->0: 接收信息完毕 关闭信道
 */
extern bit Receive(unsigned char addr);

#endif // COMMUNICATION_H