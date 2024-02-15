#include "serialportmanager.h"
#include <QSerialPort>

PROJECT_USING_NAMESPACE;

SerialPortManager::SerialPortManager(QObject *parent)
    : QObject{parent}
{
    connect(&m_timer, &QTimer::timeout, this, &SerialPortManager::updateAvailablePorts);
}

void SerialPortManager::updateAvailablePorts()
{
    m_availablePorts.clear();
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (auto &portInfo : ports)
    {
        m_availablePorts.append(
            QString("%1 (%2)").arg(portInfo.portName(), portInfo.description())
        );
    }
    emit availablePortsChanged();
}

void SerialPortManager::startTimingUpdate(int msec)
{
    updateAvailablePorts();
    m_timer.setInterval(msec);
    m_timer.start();
}

void SerialPortManager::stopTimingUpdate()
{
    m_timer.stop();
}

