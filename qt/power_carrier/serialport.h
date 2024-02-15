#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QDebug>
#include <QCoreApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDataStream>

PROJECT_BEGIN_NAMESPACE

class SerialPort : public QObject
{
    Q_OBJECT

public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();

    quint8 xorChecksum(QByteArray processedData);
    void sendData();


    bool openSerialPort(QString port);
    void closeSerialPort();
    void read();
    void clearReceive();

public slots:
    void setBaudRate(int baudRateIndex);

    void setDataBits(int dataBitsIndex);

    void setStopBits(int stopBitsIndex);

    void setParity(int parityIndex);

    void receiveData();

    void transmitData(const QString &data);

private:

    template<typename T>
    T getEnumValue(int index);

    QSerialPort m_serial;
    QByteArray receivedData;

signals:
    void receiveStr(QString);
};

PROJECT_END_NAMESPACE

#endif // SERIALPORT_H
