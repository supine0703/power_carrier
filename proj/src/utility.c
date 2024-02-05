/**
 * 作者：李宗霖 日期：2023/12/01
 * CSDN昵称：Leisure_水中鱼
 * CSDN: https://blog.csdn.net/Supine_0?type=blog
 * ----------------------------------------------
 * 不加入结束符就可以插入别的字符串中间
 * 但是对于单独开空间存储的来说 要么不同长度各自开空间 要么手动加结束符
 * 所以出于妥协 添加一个宏来作为编译的这一份代码是否需要自动加入结束符
 */
#define uchar unsigned char
#define uint unsigned int

#define NO_SIGNED_INT

/**
 * @param num 需要转换的数字
 * @param sys 转为几进制字符串 2, 8, 10, 16 否则默认10进制
 * @param str 存储字符串的首地址
 * @param length 转换后数字可以存放的空间长度(不包含'\0')
 * @param decimal 小数占多少位
 */
void UInt8ToString(uchar num, uchar sys, uchar* str, uchar length)
{
    uchar i;
    str[length] = 0; // 字符串结束标志位

    if (!length)
        return; // 如果空间为0直接返回

    i = length; // 备份 length

    if (sys != 2 && sys != 8 && sys != 10 && sys != 16)
        sys = 10; // 2, 8, 10, 16 否则默认10进制

    str[--length] = num % sys + '0'; // 将数从低位开始转成对应ascii
    while (length)
    {
        num /= sys; // 按进制右移一位继续转
        if (!num)
            break; // 如果全部转成字符串空间没用完 退出循环
        str[--length] = num % sys + '0';
    }

    if (sys == 16)
        while (i-- > length)
        {
            if (str[i] > '9')
                str[i] += 7; // 十六进制需要将超出'9'的部分转为'A'-'F'
        }

    while (length)
        str[--length] = ' '; // 前面多余的空间用空白补全
}

#ifndef NO_SIGNED_INT
void Int8ToString(char num, uchar sys, uchar* str, uchar length)
{
    uchar n;

    if (!length)
        return; // 如果空间为0直接返回

    if (num < 0)
    {
        if (!--length)
            return; // 如果是负数应该至少有一个负号和一个数的空间
        *str = ' '; // 为后面添加负号统一格式
        ++str;      // 将第一个空间预留给负号
        n = -num;   // num取补码(取负数)
    }
    else
        n = num;

    UInt8ToString(n, sys, str, length);

    if (num < 0)
    { // 负数添加负号
        --str;
        while (str[--length] != ' ')
            ; // 排除前面的数字直到找到负号
        str[length] = '-';
    }
}
#endif
