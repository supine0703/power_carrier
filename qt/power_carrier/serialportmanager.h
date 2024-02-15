#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QSerialPortInfo>
#include <QTimer>

PROJECT_BEGIN_NAMESPACE

class SerialPortManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortManager(QObject *parent = nullptr);

    void updateAvailablePorts();

    void startTimingUpdate(int msec = 1000);

    void stopTimingUpdate();

    QStringList availablePorts() const
    {
        return m_availablePorts;
    }

private:
    QStringList m_availablePorts;
    QTimer m_timer;

signals:
    void availablePortsChanged();
};

PROJECT_END_NAMESPACE

#endif // SERIALPORTMANAGER_H
