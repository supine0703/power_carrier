#ifndef SERIALPORTWIDGET_H
#define SERIALPORTWIDGET_H

#include <QWidget>
#include <QVector>

namespace Ui {
class SerialPortWidget;
}

class QMessageBox;

class ComList;
class ComRangeDialog;
class SerialPortWorker;
class SerialPortManager;

class SerialPortWidget : public QWidget
{
    Q_OBJECT

private:
    enum LedColor {
        transparent = 0,
        red,
        green,
        orange,
    };

    enum IconCode {
        lock = 0xe600,
        unlock = 0xe601,

        selectLeft = 0xe60a,
        selectRight = 0xe6a3,

        eyesOff = 0xeb45,
        eyesOn = 0xeb46,

        pageUp = 0xe623,
        pageDown = 0xe624
    };


public:
    explicit SerialPortWidget(QWidget* parent = nullptr);
    ~SerialPortWidget();


private:
    // void loadIcon();
    void loadSetting();

    void hiddenGroupManager();

    void connectSPWorker();
    void setSerialPortStatus(LedColor color);


    QChar lockIcon(bool lock);
    QChar selectorIcon(bool right);
    QChar eyesIcon(bool off);
    QChar pageIcon(bool up);


public slots:
    void checkedForSerialPort();

    // void transmitChangeASlaveState(int addr, int word);


private slots:
    void serialPort_watchdog_wakeUp();


private slots:
    void on_hidePushButton_1_clicked(bool hidden);
    void on_hidePushButton_2_clicked(bool hidden);
    void on_hidePushButton_3_clicked(bool hidden);


    void on_comPortComboBox_currentIndexChanged(int index);
    void on_comPortRangePushButton_clicked();
    void on_connectComPushButton_clicked();
    void on_lockPushButton_clicked(bool lock);


    void on_log1SelectorPushButton_1_clicked(bool right);
    void on_log2SelectorPushButton_1_clicked(bool right);
    void on_log1SelectorPushButton_2_clicked(bool on);
    void on_log2SelectorPushButton_2_clicked(bool on);

    void on_log1UnLookPushButton_clicked(bool off);
    void on_log2UnLookPushButton_clicked(bool off);


private:
    Ui::SerialPortWidget* ui;


    QMessageBox* spWarning;
    QThread* spSubThread;
    QTimer* spWatchdogT;
    ComList* comList;
    ComRangeDialog * crDialog;
    SerialPortWorker* spWorker;


    bool openSP = false;
    bool spAbnormal = false;
    bool multipleClicksConnect = false;

    bool logs1And2Visiable = true;

private:
    static const QString hiddenName;
    static const QString ledColor;


signals:
    void activateWatchdog(int = 400);

    void reSetSerialPort(int, int, int, int);
    void closeSPWarning();

    void workerOpenSerialPort(QString);
    void workerCloseSerialPort();

    void serialPortStatusChanged(int);
    void selectComChanged(int);


    void slaveNoState(quint8, quint8);
    void slaveStateChange(quint8, quint8);
    void slaveStateUpdate(quint8, quint8);

    void waitListRemove(int);
    void waitListAppend(QString);
};


#endif // SERIALPORTWIDGET_H
