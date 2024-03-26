#ifndef SERIAL_PORT_2_H
#define SERIAL_PORT_2_H

#ifdef USE_QUERY_TRANSMIT
extern void SP_QTransmitByte(unsigned char byte, bit second);

extern void SP_QTransmitData(
    unsigned char* buf, unsigned char n, unsigned char cutT, bit second
);

#define SP1_QTransmitByte(byte) SP_QTransmitByte(byte, 0)
#define SP2_QTransmitByte(byte) SP_QTransmitByte(byte, 1)
#define SP1_QTransmitData(buf, n, cutT) SP_QTransmitData(buf, n, 0, 0)
#define SP2_QTransmitData(buf, n, cutT) SP_QTransmitData(buf, n, 0, 1)

#endif

#ifdef USE_QUERY_RECEIVE
extern bit SP_QReceiveByte(
    unsigned char* buf, unsigned char t, bit second
);

extern unsigned char SP_QReceiveData(
    unsigned char* buf, unsigned char waitT, unsigned char cutT, bit second
);

#define SP1_QReceiveByte(buf, t) SP_QReceiveByte(buf, t, 0)
#define SP2_QReceiveByte(buf, t) SP_QReceiveByte(buf, t, 1)
#define SP1_QReceiveData(buf, n, cutT) SP_QReceiveData(buf, n, cutT, 0)
#define SP2_QReceiveData(buf, n, cutT) SP_QReceiveData(buf, n, cutT, 1)

#endif


#endif // SERIAL_PORT_2_H