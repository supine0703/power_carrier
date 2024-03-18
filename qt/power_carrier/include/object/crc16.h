#ifndef CRC16_H
#define CRC16_H

#include <QByteArray>

PROJECT_BEGIN_NAMESPACE

class CRC16
{
private:
    CRC16();

public:
    static quint16 XMODEM(QByteArray& bytes, int len = 0);
    static quint16 ADD_XMODEM(QByteArray& bytes);
    static bool CHECK_XMODEM(QByteArray& bytes);

private:
    static quint16* t1021;
};

PROJECT_END_NAMESPACE

#endif // CRC16_H
