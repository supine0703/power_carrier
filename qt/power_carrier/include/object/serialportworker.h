#ifndef SERIALPORTWORKER_H
#define SERIALPORTWORKER_H

#include <QObject>

class QSerialPort;
class QMutex;

PROJECT_BEGIN_NAMESPACE

class SerialPortWorker : public QObject
{
    Q_OBJECT

public:
    explicit SerialPortWorker(QObject* parent = nullptr);
    ~SerialPortWorker();

    const QString &currentPortName() const;


public slots:
    void setAll(int br, int db, int sb, int p);
    void setBaudRate(int baudRateIndex);
    void setDataBits(int dataBitsIndex);
    void setStopBits(int stopBitsIndex);
    void setParity(int parityIndex);

    void openSerialPort(const QString &port);
    void closeSerialPort();


private:
    void SafeNewSerialPort();

    void transmit(QByteArray& bytes);

    void addTransmitWaitList(QByteArray bytes);


public slots:
    void addTransmitForSlaveStateUpdate(quint8 addr, quint8 word);



private slots:
    void receiveHostOrder();


private:
    QSerialPort* m_sp;
    QString portName;
    QMutex* m_mutex;
    QByteArrayList waitList;

    bool channelFlg = false;

private:
    template<typename T>
    static T getEnumValue(int index);


signals:
    void openSuccess(bool);
    void closeFinished();

    void spNotOpen();
    void havenReceive(bool, QByteArray);
    void havenTransmit(QByteArray);

    void slaveStateUpdate(quint8, quint8);
    void slaveNoState(quint8, quint8);
    void errorLog(QString);
};

PROJECT_END_NAMESPACE

#endif // SERIALPORTWORKER_H
