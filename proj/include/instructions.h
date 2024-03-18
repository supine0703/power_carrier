/**
 * _X_: 封闭 表示不需要回复 即 应答
 * _X:  未封闭 表示需要回复 即 请求
 * 0x00~0x7f:  应答
 * 0x80~0xff:  请求
*/
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define _PC_ADDR_    '\xf0'

#define _ACK_ADDR_   '\xf4'
#define _REN_ADDR_   '\xf5'
#define _CLOSE_ADDR_ '\xf6'


#define _NULL_WORD_ '\x00'
#define _ACK_WORD_  '\x01'
#define _REN_WORD_  '\x02'
#define _ASK_WORD_  '\x03'
#define _SSU_WORD_  '\x04' // slave state update
#define _SNS_WORD_  '\x05' // slave no state
#define _MORE_WORD_ '\x10'

#define _US_WORD_   '\x11' // update state


#define _ERR_CRC_REN_ '\x01'



#endif // INSTRUCTIONS_H