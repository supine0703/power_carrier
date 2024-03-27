#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>

class QMutex;
class QTimer;

class SerialPortManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortManager(QObject* parent = nullptr);
    ~SerialPortManager();

    const QStringList& availablePorts() const;
    void updateAvailablePorts();

    bool isStart() const;


public slots:
    void startTimer(int msec = 1000);
    void stopTimer();


private:
    void safeNewTimer();


private:
    QStringList m_availablePorts;
    QMutex* m_mutex;
    QTimer* m_timer;

    bool startFlg = false;


private:
    const static QString infoT;


signals:
    void availablePortsChanged();
};

 

#endif // SERIALPORTMANAGER_H
