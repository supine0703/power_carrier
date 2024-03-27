#ifndef SERIAL_PORT_2_H
#define SERIAL_PORT_2_H

#ifdef USE_QUERY_TRANSMIT
// extern void SP_Set_QTWait(void (*waitFn)(unsigned int));

extern void SP_QTransmitByte(unsigned char byte, bit second);

extern void SP_QTransmitData(unsigned char* buf, unsigned char n, bit second);
#endif

#ifdef USE_QUERY_RECEIVE
extern bit SP_QReceiveByte(unsigned char* buf, unsigned int t, bit second);

extern unsigned char SP_QReceiveData(
    unsigned char* buf, unsigned int waitT, unsigned char cutT, bit second
);
#endif

#endif // SERIAL_PORT_2_H