#include "serialportmanager.h"

#include <QSerialPortInfo>
#include <QThread>
#include <QTimer>
#include <QMutex>
// #include "safetools.h"

// #define MASSAGE

#ifdef MASSAGE
#include <QDebug>
#define qMassage(X) qDebug() << X
#else
#define qMassage(X)
#endif


const QString SerialPortManager::infoT("%1 (%2)");

SerialPortManager::SerialPortManager(QObject* parent)
    : QObject{parent}
    , m_mutex(new QMutex)
    , m_timer(nullptr)
{
    qMassage("spManager ID:" << QThread::currentThreadId());
}


SerialPortManager::~SerialPortManager()
{
    delete m_mutex;
}


const QStringList& SerialPortManager::availablePorts() const
{
    return m_availablePorts;
}


void SerialPortManager::updateAvailablePorts()
{
    bool flg = false;
    QStringList ap = m_availablePorts;
    m_availablePorts.clear();
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (auto &portInfo : ports)
    {
        m_availablePorts.append(
            infoT.arg(portInfo.portName(), portInfo.description())
        );
    }

    if (ap.length() != m_availablePorts.length())
    {
        flg = true;
    }
    else
    {
        for (int i = 0, end = ap.length(); i < end; i++)
        {
            if (ap[i] != m_availablePorts[i])
            {
                flg = true;
                break;
            }
        }
    }

    if (flg)
    {
        emit availablePortsChanged();
    }
}


bool SerialPortManager::isStart() const
{
    return startFlg;
}


void SerialPortManager::startTimer(int msec)
{
    this->m_availablePorts.clear();
    this->updateAvailablePorts();
    this->safeNewTimer();
    this->m_timer->setInterval(msec);
    this->m_timer->start();
    this->startFlg = true;
    qMassage("start ID:" << QThread::currentThreadId());
}


void SerialPortManager::stopTimer()
{
    m_timer->stop();
    this->startFlg = false;
    qMassage("stop ID:" << QThread::currentThreadId());
}


void SerialPortManager::safeNewTimer()
{
    // SAFE_NEW<QTimer>(this->m_timer);
    if (!this->m_timer)
    {
        m_mutex->lock();
        if (!this->m_timer)
        {
            this->m_timer = new QTimer(this);
            connect(m_timer, &QTimer::timeout,
                    this, &SerialPortManager::updateAvailablePorts,
                    Qt::DirectConnection);
        }
        m_mutex->unlock();
    }
}

