#ifndef CRC16_H
#define CRC16_H

extern unsigned int CRC16_XMODEM(unsigned char* buf, unsigned char n);

extern unsigned char CRC16_ADD_XMODEM(unsigned char* buf, unsigned char n);

extern bit CRC16_CHECK_XMODEM(unsigned char* buf, unsigned char n);


#endif // CRC16_H