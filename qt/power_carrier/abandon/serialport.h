#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>

 

class SerialPort : public QObject
{
    Q_OBJECT

public:
    explicit SerialPort(QObject* parent = nullptr);
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

    QSerialPort m_serial;
    QByteArray receivedData;


private:
    template<typename T>
    T getEnumValue(int index);

signals:
    void receiveStr(QString);
};

 

#endif // SERIALPORT_H
