#include "serialport.h"

#include <QMessageBox>

PROJECT_USING_NAMESPACE;

//计算异或校验位
SerialPort::SerialPort(QObject *parent) : QObject {parent}
{
    // 读取串口数据
    m_serial.setFlowControl(QSerialPort::NoFlowControl);
    connect(&m_serial, &QSerialPort::readyRead, this, &SerialPort::receiveData);
}

SerialPort::~SerialPort()
{

}

//串口发送
void SerialPort::sendData()
{
    // 构造协议数据
    QByteArray protocolData;
    protocolData.append('\x01');
    protocolData.append('\x11');
    protocolData.append('\x02');
    protocolData.append('\x22');
    protocolData.append('\x03');

    // 计算并添加异或校验位
    // quint8 checksum = xorChecksum(protocolData);
    // protocolData.append(checksum);

    // 发送协议数据
    qint64 bytesWritten = m_serial.write(protocolData);
    if (bytesWritten == -1) {
        qDebug() << "Failed to write data to serial port:" << m_serial.errorString();
    }
    qDebug() << "发送字节数:" << bytesWritten;
}

//接收数据
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
    m_serial.write(b);
}


bool SerialPort::openSerialPort(QString port)
{
    m_serial.setPortName(port); //设置串口号
    m_serial.setReadBufferSize(9);
    if(m_serial.open(QIODevice::ReadWrite))
    {
        return true;
    }
    else
    {
        QMessageBox::about(nullptr, "提示", "串口无法打开\r\n不存在或已被占用");
        return false;
    }
}


void SerialPort::closeSerialPort()
{
    if(m_serial.isOpen())
        m_serial.close();
}


void SerialPort::read()
{
    emit receiveStr(m_serial.readAll().toHex().data());
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

#include <QMetaEnum>

template<class T>
T SerialPort::getEnumValue(int index)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<T>();
    Q_ASSERT(0 <= index && index < metaEnum.keyCount());
    return static_cast<T>(metaEnum.value(index));
}
