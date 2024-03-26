#include "__config__.h"

#define uchar unsigned char
#define uint unsigned int

// ============================================================================

#ifdef USE_QUERY_TRANSMIT
void SP_QTransmitByte(uchar byte, bit second)
{
    if (second)
    {
        S2BUF = byte;
        while (!S2_CHECK(S2TI))
            ; // 判断是否发送成功(发送成功后TI会置1 需手动清0)
        S2_SET_L(S2TI);
    }
    else
    {
        SBUF = byte;
        while (!TI)
            ; // 判断是否发送成功(发送成功后TI会置1 需手动清0)
        TI = 0;
    }
}

void SP_QTransmitData(uchar* buf, uchar n, uchar cutT, bit second)
{
    uchar t;
    if (n)
        do
        {
            SP_QTransmitByte(*buf, second);
            t = cutT;
            if (t)
                while (--t)
                    ;
            buf++;
        } while (--n);
}
#endif

#ifdef USE_QUERY_RECEIVE
bit SP1_QRWait(uchar t) // 12MHz (t*1000 + 11)us above 1ms
{
    uchar i;
    if (RI)
    {
        RI = 0;
        return 1;
    }
    while (t)
    {
        t--;
        for (i = 124; i; --i)
            if (RI)
            {
                RI = 0;
                return 1;
            }
    }
    return 0;
}

bit SP2_QRWait(uchar t) // 12MHz (t*1000 + 11)us above 1ms
{
    uchar i;
    if (S2_CHECK(S2RI))
    {
        S2_SET_L(S2RI);
        return 1;
    }
    while (t)
    {
        t--;
        for (i = 124; i; --i)
            if (S2_CHECK(S2RI))
            {
                S2_SET_L(S2RI);
                return 1;
            }
    }
    return 0;
}

bit SP_QReceiveByte(uchar* buf, uchar t, bit second)
{
    if (second)
    {
        if (SP2_QRWait(t))
        {
            *buf = S2BUF;
            return 1;
        }
    }
    else if (SP1_QRWait(t))
    {
        *buf = SBUF;
        return 1;
    }
    return 0;
}

uchar SP_QReceiveData(uchar* buf, uchar waitT, uchar cutT, bit second)
{
    uchar count = 0;
    if (SP_QReceiveByte(buf, waitT, second))
    {
        do
        {
            buf++;
            count++;
        } while (SP_QReceiveByte(buf, cutT, second));
    }
    return count;
}
#endif
