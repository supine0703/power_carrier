#ifndef CRC16_H
#define CRC16_H

#include <QByteArray>

class CRC16
{
private:
    CRC16();

public:
    static quint16 XMODEM(const QByteArray& bytes, int len = 0);
    static quint16 ADD_XMODEM(QByteArray& bytes);
    static bool CHECK_XMODEM(const QByteArray& bytes);

private:
    static quint16* t1021;
};

 

#endif // CRC16_H
