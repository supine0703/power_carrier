/**
 * _X_: 封闭 表示不需要回复 即 应答
 * _X:  未封闭 表示需要回复 即 请求
 * 0x00~0x7f:  应答
 * 0x80~0xff:  请求
*/
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define _NULL_  0x00 // 空

#define _ACK_   0x01 // 收到 应答
#define _NACK_  0x02 // 未收到

#define _YES_   0x03 // 是 同意
#define _NO_    0x04 // 否 拒绝

#define _REN    0x80 // 要求重复刚才内容
#define _HELLO  0x81 // 询问是否存在
#define _STATUS 0x82 // 请求现在状态

#endif // INSTRUCTIONS_H