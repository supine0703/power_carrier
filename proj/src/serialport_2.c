#include "__config__.h"

#define uchar unsigned char
#define uint unsigned int

// ============================================================================
#ifdef USE_QUERY_TRANSMIT
// void (*SP_QTWait)(uint) = 0;

// void SP_Set_QTWait(void (*waitFn)(uint))
// {
//     SP_QTWait = waitFn;
// }

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

void SP_QTransmitData(uchar* buf, uchar n, bit second)
{
    if (n)
        do
        {
            SP_QTransmitByte(*buf, second);    
            buf++;
        } while (--n);
}

// void SP_QTransmitData(uchar* buf, uchar n, uint cutT, bit second)
// {
//     if (n)
//         do
//         {
//             SP_QTransmitByte(*buf, second);
//             // if (SP_QTWait)
//             //     SP_QTWait(cutT);
//             buf++;
//         } while (--n);
// }
#endif

#ifdef USE_QUERY_RECEIVE
bit SP1_QRWait(uint t) // 11.0592 约1ms
{
    unsigned char i, j;
    if (RI)
    {
        RI = 0;
        return 1;
    }
    while (t)
    {
        t--;
        for (i = 11; i; --i)
            for (j = 124; j; --j)
                if (RI)
                {
                    RI = 0;
                    return 1;
                }
    }
    return 0;
}

bit SP2_QRWait(uint t) // 11.0592 约1ms
{
    unsigned char i, j;
    if (S2_CHECK(S2RI))
    {
        S2_SET_L(S2RI);
        return 1;
    }
    while (t)
    {
        t--;
        for (i = 25; i; --i)
            for (j = 43; j; --j)
                if (S2_CHECK(S2RI))
                {
                    S2_SET_L(S2RI);
                    return 1;
                }
    }
    return 0;
}

bit SP_QReceiveByte(uchar* buf, uint t, bit second)
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

uchar SP_QReceiveData(uchar* buf, uint waitT, uint cutT, bit second)
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
