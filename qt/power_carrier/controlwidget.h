#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>
#include <QVector>

namespace Ui {
class ControlWidget;
}

PROJECT_BEGIN_NAMESPACE

class SerialPort;
class ComRangeDialog;
class SerialPortManager;

class ControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlWidget(QWidget *parent = nullptr);
    ~ControlWidget();

private:
    void initSerialPortOptions();


    void updateComRange(int min, int max);

private slots:
    void scanComPortPush();

    void on_comPortRangePushButton_clicked();

    void on_comPortComboBox_currentIndexChanged(int index);

    void on_connectSerialPortPushButton_clicked();

    void on_receivePushButton_clicked();

    void on_transmitPushButton_clicked();

private:
    Ui::ControlWidget *ui;
    ComRangeDialog  *comD;
    SerialPortManager *sPM;
    SerialPort *serialPort;
    QVector<qint8> comFlg;
    int comPortNum;
    bool openSP = false;

};

PROJECT_END_NAMESPACE

#endif // CONTROLWIDGET_H
