/**
 * __config__.h
 * 作者：李宗霖 日期：2023/11/16
 * CSDN昵称：Leisure_水中鱼
 * CSDN: https://blog.csdn.net/Supine_0?type=blog
 * ----------------------------------------------
 * 以下头文件对此文件是强依赖：
 *   lcd1602.*  -- 2
 *   ds18b20.*  -- 3
 *   i2c.*      -- 4
 * 也可以自己定义配置文件以减少耦合
 * 将所有定义放与config文件方便修改单片机配置
 * ----------------------------------------------
 * 此头文件可以调整对 c5x 单片机数据端口的定义
 * 需要满足以下要求：
 *   1. 对uint uchar定义
 *   2. out LCD_RS LCD_RW LCD_EN 为了lcd1602定义
 *   3. DQ 为了ds18b20定义
 *   4. SDA SCL 为了i2c而定义
 */
#ifndef __CONFIG___H
#define __CONFIG___H
/**
 * 作者：李宗霖 日期：2023/12/04
 * ----------------------------------------------
 * - 将sbit 从详细定义 改为借助宏进行定义
 * - 从.h文件包含此文件 转为 .c文件包含
 * 这样做好处是:
 * 1. 避免头文件中包含具体变量的定义 虽然keil对于sbit的处理
 *    让其哪怕重复包含头文件也不会出现重定义 但是仍需要保持良好的编程习惯
 * 2. 头文件中包含extern申明的函数 而非函数的申明(定义)似乎更符合 c51 的编程习惯
 * 3. 避免其他模块包含与自己无关的变量
 * ... ...
 */

// -------------------------------------

#include <STC89C5xRC.H>

// ------- define for lcd1602 ----------

// #define LCD1602_USE_DEFAULT // 使用默认配置
#define LCD1602_NO_READDATA      // 不编译LCD1602_ReadData(void)
#define LCD1602_DATA P0          // 数据 to LCD1602
#define LCD1602_DEFINE_RS P3 ^ 5 // 寄存器选择
#define LCD1602_DEFINE_RW P3 ^ 6 // 读/写
#define LCD1602_DEFINE_EN P3 ^ 4 // 使能

// ----- define for communication ------

#define AR_ADDRESS 0x00 // all receive address 全能接收地址
#define PC_ADDRESS 0x01 // pc address PC机的地址

#define BROADCAST_GROUP_END 0xff // 广播分组的尾地址
#define BROADCAST_GROUP_LEN 0x10 // 广播分组的长度

#define END_RECEIVE '$'        // 结束接受符号 0x36
#define USE_QUERY_RECEIVE_A_D  // 使用查询方式接收
#define USE_QUERY_TRANSMIT // 使用查询方式发送

// -------------------------------------

#define KEY_MATRIX P1
#define KEY_SP // 定义按键的引脚P
#define KEY_SB // 定义按键有效的位

// -------------------------------------

#endif // __CONFIG___H