#include "controlwidget.h"
#include "ui_controlwidget.h"

#include "configurations.h"
#include "comrangedialog.h"
#include "serialportmanager.h"
#include "serialport.h"

#include <QRegularExpression>

PROJECT_USING_NAMESPACE;

ControlWidget::ControlWidget(QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::ControlWidget)
    , comD(new ComRangeDialog(this))
    , sPM(new SerialPortManager(this))
    , serialPort(new SerialPort(this))
{
    ui->setupUi(this);
    initSerialPortOptions();
    connect(
        sPM, &SerialPortManager::availablePortsChanged,
        this, &ControlWidget::scanComPortPush
    );
    sPM->startTimingUpdate();
    connect(serialPort, &SerialPort::receiveStr, this, [this](auto str) {
        ui->receiveTextEdit->setText(str);
        // static int i = 0;
        // qDebug() << ++i;
    });


    connect(ui->baudRateComboBox, &QComboBox::currentIndexChanged,
            serialPort, &SerialPort::setBaudRate);
    connect(ui->dataBitsComboBox, &QComboBox::currentIndexChanged,
            serialPort, &SerialPort::setDataBits);
    connect(ui->stopBitsComboBox, &QComboBox::currentIndexChanged,
            serialPort, &SerialPort::setStopBits);
    connect(ui->parityComboBox, &QComboBox::currentIndexChanged,
            serialPort, &SerialPort::setParity);
}


ControlWidget::~ControlWidget()
{
    delete ui;
}


void ControlWidget::initSerialPortOptions()
{
    comPortNum = CFGS().value(_COM_PORT_NUM_).toInt() - comD->comMin();
    updateComRange(comD->comMin(), comD->comMax());
}


void ControlWidget::updateComRange(int min, int max)
{
    static const QString comX("COM%1");
    // stop update available ports
    sPM->stopTimingUpdate();

    // reset com status's flg
    auto len = max - min + 1;
    this->comFlg.clear();
    this->comFlg.resize(len, 0);

    // set options
    QStringList comList;
    for (int i = min; i <= max; i++)
        comList << comX.arg(i);

    // update serialPortComboBox and repos current index
    auto n = comPortNum;
    ui->comPortComboBox->clear();
    ui->comPortComboBox->addItems(comList);
    ui->comPortComboBox->setCurrentIndex(
        n <= 0 ? 0 : (n < len ? n : len - 1)
        // (0 < n && n < len) ? n : 0
        );
    // re start update available ports
    sPM->startTimingUpdate();
}


void ControlWidget::scanComPortPush()
{
    // matching com's num through regular expression
    static QRegularExpression re(R"(\d+)");
    static QRegularExpressionMatch match;

    auto cM = comD->comMin();
    auto len = this->comFlg.length();

    // update options which scanned serial port
    for (const auto& info : sPM->availablePorts())
    {
        match = re.match(info);
        auto index = match.captured().toInt() - cM;
        if (0 <= index && index < len)
        {
            this->comFlg[index] = 1;
            ui->comPortComboBox->setItemText(index, info);
        }
    }

    // update com status flg: 0-not 1-have 2-have->not
    for (int i = 0; i < len; i++)
    {
        if (this->comFlg[i] == 1)
        {
            this->comFlg[i] = 2;
        }
        else if (this->comFlg[i] == 2)
        {
            this->comFlg[i] = 0;
            ui->comPortComboBox->setItemText(i, QString("COM%1").arg(i + cM));
        }
    }
}


void ControlWidget::on_comPortRangePushButton_clicked()
{
    comD->ready();
    if (comD->exec() == QDialog::Accepted)
        updateComRange(comD->comMin(), comD->comMax());
}


void ControlWidget::on_comPortComboBox_currentIndexChanged(int index)
{
    // save setting about com's num
    comPortNum = index;
    CFGS().setValue(_COM_PORT_NUM_, comPortNum + comD->comMin());
}


void ControlWidget::on_connectSerialPortPushButton_clicked()
{
    static const QString ledColor(
        "background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, "
        "fx:0.5, fy:0.5, stop:0.333333 %1, stop:1 rgba(255, 255, 255, 0));"
        );
    if (openSP) // have open serial port == need close == next open
    {
        openSP = false;
        ui->connectSerialPortPushButton->setText("打开串口");
        ui->connectLedLabel->setStyleSheet(ledColor.arg("rgba(255, 0, 0, 255)"));
        serialPort->closeSerialPort();
    }
    else // need open serial port == next close if open sucessfully
    {
        if (serialPort->openSerialPort(
                QString("COM%1").arg(ui->comPortComboBox->currentIndex() + comD->comMin())
                )
            )
        {
            openSP = true;
            ui->connectSerialPortPushButton->setText("关闭串口");
            ui->connectLedLabel->setStyleSheet(ledColor.arg("rgba(85, 255, 0, 255)"));
        }
    }
}

void ControlWidget::on_receivePushButton_clicked()
{
    serialPort->clearReceive();
    ui->receiveTextEdit->clear();
}


void ControlWidget::on_transmitPushButton_clicked()
{
    serialPort->transmitData(ui->transmitTextEdit->toPlainText());
}

