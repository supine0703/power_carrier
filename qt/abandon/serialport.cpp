#include "serialport.h"

#include "crc16.h"
#include <QMessageBox>
#include <QMetaEnum>



SerialPort::SerialPort(QObject* parent) : QObject {parent}
{
    m_serial.setFlowControl(QSerialPort::NoFlowControl);
    m_serial.setReadBufferSize(35);
    connect(&m_serial, &QSerialPort::readyRead, this, &SerialPort::receiveData);
}

SerialPort::~SerialPort()
{
}

void SerialPort::receiveData()
{
    // auto b = m_serial.readAll();
    receivedData.append(m_serial.readAll());
    emit receiveStr(receivedData.toHex(' '));
}

void SerialPort::transmitData(const QString &data)
{
    auto ds = data.split(" ");
    QByteArray b;
    for (auto& d : ds)
    {
        b.append(d.toInt(nullptr, 16));
    }
    CRC16::ADD_XMODEM(b);
    m_serial.write(b);
}


bool SerialPort::openSerialPort(QString port)
{
    m_serial.setPortName(port); //设置串口号
    if(m_serial.open(QIODevice::ReadWrite))
    {
        return true;
    }
    else
    {
        QMessageBox::about(nullptr, "提示", "串口无法打开 不存在或已被占用");
        return false;
    }
}


void SerialPort::closeSerialPort()
{
    if(m_serial.isOpen())
        m_serial.close();
}


void SerialPort::setBaudRate(int baudRateIndex)
{
    m_serial.setBaudRate(getEnumValue<QSerialPort::BaudRate>(baudRateIndex));
}

void SerialPort::setDataBits(int dataBitsIndex)
{
    m_serial.setDataBits(getEnumValue<QSerialPort::DataBits>(dataBitsIndex));
}

void SerialPort::setStopBits(int stopBitsIndex)
{
    m_serial.setStopBits(getEnumValue<QSerialPort::StopBits>(stopBitsIndex));
}

void SerialPort::setParity(int parityIndex)
{
    m_serial.setParity(getEnumValue<QSerialPort::Parity>(parityIndex));
}

void SerialPort::clearReceive()
{
    receivedData.clear();
}

template<class T>
T SerialPort::getEnumValue(int index)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<T>();
    Q_ASSERT(0 <= index && index < metaEnum.keyCount());
    return static_cast<T>(metaEnum.value(index));
}
