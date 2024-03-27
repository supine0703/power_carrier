#include "serialportworker.h"

#include <QSerialPort>
#include <QMetaEnum>
#include <QMutex>
#include <QTimer>

#include "configurations.h"
#include "crc16.h"

#include "keywords.h"


SerialPortWorker::SerialPortWorker(QObject* parent)
    : QObject{parent}
    , m_mutex(new QMutex)
    , m_timer(nullptr)
    , m_sp(nullptr)
{}


SerialPortWorker::~SerialPortWorker()
{
    delete m_mutex;
}


const QString &SerialPortWorker::currentPortName() const
{
    return portName;
}

bool SerialPortWorker::isChannel() const
{
    return channelFlg;
}


void SerialPortWorker::setAll(int br, int db, int sb, int p)
{
    this->setBaudRate(br);
    this->setDataBits(db);
    this->setStopBits(sb);
    this->setParity(p);
}


void SerialPortWorker::setBaudRate(int baudRateIndex)
{
    this->SafeNewSerialPort();
    m_sp->setBaudRate(getEnumValue<QSerialPort::BaudRate>(baudRateIndex));
    SETTINGS().setValue(_SERIAL_PORT_BRI_, baudRateIndex);
}


void SerialPortWorker::setDataBits(int dataBitsIndex)
{
    this->SafeNewSerialPort();
    m_sp->setDataBits(getEnumValue<QSerialPort::DataBits>(dataBitsIndex));
    SETTINGS().setValue(_SERIAL_PORT_DBI_, dataBitsIndex);
}


void SerialPortWorker::setStopBits(int stopBitsIndex)
{
    this->SafeNewSerialPort();
    m_sp->setStopBits(getEnumValue<QSerialPort::StopBits>(stopBitsIndex));
    SETTINGS().setValue(_SERIAL_PORT_SBI_, stopBitsIndex);
}


void SerialPortWorker::setParity(int parityIndex)
{
    this->SafeNewSerialPort();
    m_sp->setParity(getEnumValue<QSerialPort::Parity>(parityIndex));
    SETTINGS().setValue(_SERIAL_PORT_PI_, parityIndex);
}


void SerialPortWorker::openSerialPort(const QString &port)
{
    this->SafeNewSerialPort();
    m_sp->setPortName(portName = port);
    emit openSuccess(m_sp->open(QIODevice::ReadWrite));
}


void SerialPortWorker::closeSerialPort()
{
    this->SafeNewSerialPort();
    if(m_sp->isOpen())
        m_sp->close();
    emit closeFinished();
}

// #include <QThread>
void SerialPortWorker::SafeNewSerialPort()
{
    if (!m_sp)
    {
        m_mutex->lock();
        if (!m_sp)
        {
            // qDebug() << "sub thread id:" << QThread::currentThreadId();
            m_sp = new QSerialPort(this);
            // m_sp->setReadBufferSize(10);
            connect(m_sp, &QSerialPort::readyRead,
                    this, &SerialPortWorker::receiveHostOrder);
            m_timer = new QTimer(this);
            connect(m_timer, &QTimer::timeout, this, [this]() {
                m_timer->stop();
                if (channelFlg) {
                    channelFlg = false;
                    emit errorLog("信道10秒无操作，自动关闭");
                }
            });
        }
        m_mutex->unlock();
    }
}


void SerialPortWorker::transmit(QByteArray& bytes)
{
    CRC16::ADD_XMODEM(bytes);
    m_sp->write(bytes);
    emit havenTransmit(bytes);
    // qDebug() << "haven transmit:" << bytes.toHex(' ');
}


void SerialPortWorker::addTransmitWaitList(QByteArray bytes)
{
    if (bytes.isEmpty())
        return;
    if (m_sp == nullptr || !m_sp->isOpen())
        emit spNotOpen();
    waitList.append(bytes);
    QByteArray b = bytes;
    CRC16::ADD_XMODEM(b);
    emit waitListAppend(b);
    // emit havenTransmit(bytes);
    // qDebug() << "wait transmit:" << bytes.toHex(' ');
}


void SerialPortWorker::addTransmitForSlaveStateUpdate(quint8 addr, quint8 word)
{
    QByteArray bytes;
    bytes.append(_SSU_WORD_);
    bytes.append(addr);
    bytes.append(word);
    for (int i = 0, end = waitList.length(); i < end; i++)
    {
        const auto& b(waitList.at(i));
        if (b.at(0) == _SSU_WORD_ && b.at(1) == addr)
        {
            waitList.removeAt(i);
            emit waitListRemove(i);
            break;
        }
    }
    this->addTransmitWaitList(bytes);

    // emit slaveStateUpdate(bytes.at(1), bytes.at(2));
    // qDebug() << waitList;
}


void SerialPortWorker::receiveHostOrder()
{
    auto buf(m_sp->readAll());
    // qDebug() << "haven receive:" << buf.toHex(' ');
    // return;
    
    emit havenReceive(channelFlg, buf);

    if (channelFlg)
    {
        m_timer->setInterval(10000);
        m_timer->start();
    }

    if (buf.length() == 1)
    {
        switch (buf.at(0))
        {
        case _REN_ADDR_:
            if (!channelFlg)
                break;
        case _PC_ADDR_:
            channelFlg = true;
            buf.clear();
            buf.append(_ACK_WORD_);
            buf.append(_PC_ADDR_);
            transmit(buf);
            break;
        case _CLOSE_ADDR_:
            channelFlg = false;
            m_timer->stop();
            break;
        default:
            // qDebug() << "unknow addr word:" << buf.toHex();
            break;
        }
    }
    else if (buf.length() > 2)
    {
        if (channelFlg)
        {
            if (CRC16::CHECK_XMODEM(buf))
            {
                QByteArray b;
                switch (buf.at(0))
                {
                case _ASK_WORD_:
                    if (waitList.isEmpty()) {
                        b.append(_NULL_WORD_);
                        transmit(b);
                    } else {
                        b = waitList.at(0);
                        waitList.pop_front();
                        emit waitListRemove(0);
                        transmit(b);
                    }
                    break;
                case _SSU_WORD_:
                    b.append(_ACK_WORD_);
                    transmit(b);
                    emit slaveStateUpdate(buf.at(1), buf.at(2));
                    break;
                case _SNS_WORD_:
                    b.append(_ACK_WORD_);
                    transmit(b);
                    emit slaveNoState(buf.at(1), buf.at(2));
                    break;
                default:
                    break;
                }
            }
            else
            {
                QByteArray b;
                b.append(_REN_WORD_);
                b.append(_ERR_CRC_REN_);
                transmit(b);
                emit errorLog("crc error!");
                // qDebug() << "crc error!";
            }
        }
    }
    else if (buf.length() == 2)
        emit errorLog("receive must not 2b!");
        // qDebug() << "receive length: 2 --error!";
    else if (buf.isEmpty())
        emit errorLog("receive null!");
    // qDebug() << "receive null";
}


template<class T>
T SerialPortWorker::getEnumValue(int index)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<T>();
    Q_ASSERT(0 <= index && index < metaEnum.keyCount());
    return static_cast<T>(metaEnum.value(index));
}


QString SerialPortWorker::getKeywordMeaning(const QByteArray& bytes)
{
    if (bytes.length() == 1)
    {
        switch (bytes.at(0))
        {
        case _PC_ADDR_:
            return "请求与PC建立信道";
        case _ACK_ADDR_:
            return "同意";
        case _REN_ADDR_:
            return "请求重新发送";
        case _CLOSE_ADDR_:
            return "关闭信道";
        case _ACTION_:
            return "HOST 开机";
        default:
            return QString("请求与地址为 %1H 的设备建立信道").arg(bytes.toHex());
            break;
        }
    }
    else if(bytes.length() > 2)
    {
        if (!CRC16::CHECK_XMODEM(bytes))
            return "CRC 码出错";
        switch (bytes.at(0))
        {
        case _ASK_WORD_:
            return "询问是否有待处理的对话";
        case _SSU_WORD_:
            if (bytes.at(2) == '\xff')
                return "更新从机未响应";
            return "请求更新从机状态";
        case _SNS_WORD_:
            return "从机没有此状态";
        case _NULL_WORD_:
            return "否定/空";
        case _ACK_WORD_:
            return "应答/同意";
        case _REN_WORD_:
            if (bytes.length() > 3)
            {
                QString s;
                switch (bytes.at(1))
                {
                case _ERR_CRC_REN_:
                    s = "CRC 错误";
                    break;
                default:
                    s = QString::number(bytes.at(1));
                }
                return QString("请求重新发送 原因: %1").arg(s);
            }
            else
                return "请求重新发送";
        case _US_WORD_:
            return "要求从机更新状态";
        default:
            break;
        }
    }
    return "unkonw";
}

bool SerialPortWorker::theInfoIsKey(const QByteArray& bytes)
{
    if (bytes.length() < 2)
    {
        if (bytes.at(0) == _ACTION_)
            return true;
    }
    else if (bytes.length() == 2)
        return true;
    else if (!CRC16::CHECK_XMODEM(bytes))
        return true;
    switch (bytes.at(0))
    {
    case _SSU_WORD_:
        return true;
    case _SNS_WORD_:
        return true;
    case _REN_WORD_:
        if (bytes.length() > 3)
        {
            switch (bytes.at(1))
            {
            case _ERR_CRC_REN_:
                return true;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
    return false;
}
