/**
 * 作者：李宗霖 日期：2023/12/01
 * CSDN昵称：Leisure_水中鱼
 * CSDN: https://blog.csdn.net/Supine_0?type=blog
 * ----------------------------------------------
 * 这是一个工具类 其中的函数与C51无关 属于通用C函数 
 */
#ifndef UTILITY_H
#define UTILITY_H

/**
 * @param num 需要转换的数字
 * @param str 存储字符串的首地址
 * @param length 转换后数字可以存放的空间长度(不包含'\0')
 * @param decimal 小数占多少位
 */
extern void Int8ToString(char num, unsigned char* str, unsigned char length);

extern void FloatToString(
    float num, unsigned char* str, unsigned char length, unsigned char decimal
);

#endif