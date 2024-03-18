#include "serialportwidget.h"
#include "ui_serialportwidget.h"

#include "configurations.h"
#include "comrangedialog.h"
#include "serialportworker.h"
#include "comlist.h"

#include <QRegularExpression>
#include <QFontDatabase>
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QTime>

PROJECT_USING_NAMESPACE;

const QString SerialPortWidget::hiddenName("> %1");

const QString SerialPortWidget::ledColor(
    "background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, "
    "fx:0.5, fy:0.5, stop:0.333333 rgba(%1), stop:1 rgba(255, 255, 255, 0));"
);

SerialPortWidget::SerialPortWidget(QWidget* parent)
    : QWidget{parent}
    , ui(new Ui::SerialPortWidget)
    , spSubThread(new QThread)
    , spWatchdogT(new QTimer(this))
    , crDialog(new ComRangeDialog(this))
    , spWorker(new SerialPortWorker)
    , comList(new ComList(this))
{
    // qDebug() << "main thread id:" << QThread::currentThreadId();
    ui->setupUi(this);
    ui->widgetLayout->setAlignment(Qt::AlignTop);
    comList->setComsBox(ui->comPortComboBox);

    connect(ui->comPortComboBox, &QComboBox::currentIndexChanged, this,
            [this](int index) {
        emit selectComChanged(index + 1);
    });

    connect(comList, &ComList::currentComChanged, this, [this]() {
        if (this->openSP) {
            on_connectComPushButton_clicked();
            QMessageBox::critical(this, "异常", "串口出现异常 连接中断  ");
        }
    });

    spWorker->moveToThread(spSubThread);
    spSubThread->start();


    this->connectSPWorker();

    connect(spWatchdogT, &QTimer::timeout, spWatchdogT, &QTimer::stop);

    connect(spWatchdogT, &QTimer::timeout,
            this, &SerialPortWidget::serialPort_watchdog_wakeUp);

    connect(this, &SerialPortWidget::activateWatchdog, this, [this](int t) {
        if (this->spAbnormal) {
            this->serialPort_watchdog_wakeUp();
        } else if (this->spWatchdogT->isActive()) {
            this->multipleClicksConnect = true;
        } else {
            this->spWatchdogT->setInterval(t);
            this->spWatchdogT->start();
        }
    });


    // loadIcon();
    loadSetting();

    spWarning = new QMessageBox(
        QMessageBox::Warning,
        "请求",
        "串口未知异常  连接超时  请选择: \r\n重启串口(Y)  继续等待(N)",
        QMessageBox::Yes | QMessageBox::No,
        this
    );
}


SerialPortWidget::~SerialPortWidget()
{
    delete ui;

    this->spSubThread->quit();
    this->spSubThread->wait();
    this->spSubThread->deleteLater();
    this->spWorker->deleteLater();
}


/* ========================================================================== */


// void SerialPortWidget::loadIcon()
// {
//     QFont iconFont = ICON_FONT();

//     QWidgetList wList = {
//         ui->lockPushButton,
//         ui->log1SelectorPushButton,
//         ui->log2SelectorPushButton,
//         ui->log1LookPushButton,
//         ui->log2LookPushButton,
//     };

//     for (auto& w : wList)
//         w->setFont(iconFont);
// }

void SerialPortWidget::loadSetting()
{
    if (SETTINGS_CONTAINS({
            _SERIAL_PORT_BRI_,
            _SERIAL_PORT_DBI_,
            _SERIAL_PORT_SBI_,
            _SERIAL_PORT_PI_
        }))
    {
        ui->baudRateComboBox->setCurrentIndex(SETTINGS().value(_SERIAL_PORT_BRI_).toInt());
        ui->dataBitsComboBox->setCurrentIndex(SETTINGS().value(_SERIAL_PORT_DBI_).toInt());
        ui->stopBitsComboBox->setCurrentIndex(SETTINGS().value(_SERIAL_PORT_SBI_).toInt());
        ui->parityComboBox->setCurrentIndex(SETTINGS().value(_SERIAL_PORT_PI_).toInt());
    }

    auto lock = SETTINGS().value(_SERIAL_PORT_LOCK_).toBool();
    ui->lockPushButton->setChecked(lock);
    this->on_lockPushButton_clicked(lock);

    auto log1s = SETTINGS().value(_LOG1_SELECTOR_).toBool();
    ui->log1SelectorPushButton->setChecked(log1s);
    this->on_log1SelectorPushButton_clicked(log1s);
    auto log2s = SETTINGS().value(_LOG2_SELECTOR_).toBool();
    ui->log2SelectorPushButton->setChecked(log2s);
    this->on_log2SelectorPushButton_clicked(log2s);

    auto log1e = SETTINGS().value(_LOG1_EYES_STATE_).toBool();
    ui->log1LookPushButton->setChecked(log1e);
    this->on_log1LookPushButton_clicked(log1e);
    auto log2e = SETTINGS().value(_LOG2_EYES_STATE_).toBool();
    ui->log2LookPushButton->setChecked(log2e);
    this->on_log2LookPushButton_clicked(log2e);

    auto group1h = SETTINGS().value(_GROUP1_HIDDEN_).toBool();
    ui->hidePushButton_1->setChecked(group1h);
    this->on_hidePushButton_1_clicked(group1h);
    auto group2h = SETTINGS().value(_GROUP2_HIDDEN_).toBool();
    ui->hidePushButton_2->setChecked(group2h);
    this->on_hidePushButton_2_clicked(group2h);
    auto group3h = SETTINGS().value(_GROUP3_HIDDEN_).toBool();
    ui->hidePushButton_3->setChecked(group3h);
    this->on_hidePushButton_3_clicked(group3h);
    this->logs1And2Visiable = !group2h && !group3h;

    emit reSetSerialPort(
        ui->baudRateComboBox->currentIndex(),
        ui->dataBitsComboBox->currentIndex(),
        ui->stopBitsComboBox->currentIndex(),
        ui->parityComboBox->currentIndex()
    );
}


void SerialPortWidget::hiddenGroupManager()
{
    auto hidden_2 = ui->hidePushButton_2->isChecked();
    auto hidden_3 = ui->hidePushButton_3->isChecked();
    auto hidden_log = !hidden_2 && !hidden_3;
    if (hidden_log)
    { // use splotter
        // ui->widgetLayout->insertWidget(1, ui->logSplitter);
        ui->logSplitter->addWidget(ui->groupWidget_2);
        ui->logSplitter->addWidget(ui->groupWidget_3);
        ui->logSplitter->restoreState(
            SETTINGS().value(_LOG_SPLITTER_STATE_).toByteArray());
    }
    else
    { // remove splitter
        if (this->logs1And2Visiable)
            SETTINGS().setValue(_LOG_SPLITTER_STATE_, ui->logSplitter->saveState());
        ui->widgetLayout->insertWidget(1, ui->groupWidget_2);
        ui->widgetLayout->insertWidget(2, ui->groupWidget_3);
        // ui->widgetLayout->removeWidget(ui->logSplitter);
    }
    this->logs1And2Visiable = hidden_log;
    ui->logSplitter->setVisible(hidden_log);
}


void SerialPortWidget::connectSPWorker()
{
    connect(ui->baudRateComboBox, &QComboBox::currentIndexChanged,
            spWorker, &SerialPortWorker::setBaudRate);
    connect(ui->dataBitsComboBox, &QComboBox::currentIndexChanged,
            spWorker, &SerialPortWorker::setDataBits);
    connect(ui->stopBitsComboBox, &QComboBox::currentIndexChanged,
            spWorker, &SerialPortWorker::setStopBits);
    connect(ui->parityComboBox, &QComboBox::currentIndexChanged,
            spWorker, &SerialPortWorker::setParity);


    connect(this, &SerialPortWidget::reSetSerialPort,
            spWorker, &SerialPortWorker::setAll);


    connect(this, &SerialPortWidget::workerCloseSerialPort,
            spWorker, &SerialPortWorker::closeSerialPort);
    connect(this, &SerialPortWidget::workerOpenSerialPort,
            spWorker, &SerialPortWorker::openSerialPort);


    connect(spWorker, &SerialPortWorker::openSuccess, this, [this](bool success) {
        spWatchdogT->stop();
        if (success) { // close -> open
            this->setSerialPortStatus(LedColor::green);
        } else {       // open -> close
            this->setSerialPortStatus(LedColor::red);
        }
        if (!success) {
            QMessageBox::information(this, "提示", "串口无法打开 不存在或已被占用 ");
        }
    });


    connect(spWorker, &SerialPortWorker::slaveStateUpdate,
            this, &SerialPortWidget::slaveStateUpdate);

    connect(spWorker, &SerialPortWorker::slaveNoState,
            this, &SerialPortWidget::slaveNoState);

    connect(this, &SerialPortWidget::slaveStateChange,
            spWorker, &SerialPortWorker::addTransmitForSlaveStateUpdate);

    connect(spWorker, &SerialPortWorker::spNotOpen, this, [this]() {
        QMessageBox::warning(this, "警告", "串口未启动, 但命令已加入等待队列");
    });

    connect(spWorker, &SerialPortWorker::havenReceive,
            this, [this](bool channel, QByteArray bytes) {
        ui->log2TextEdit->setTextColor(QColor(0, 85, 0));
        ui->log2TextEdit->append(QString("[%1]:RX <- %2").arg(
            QTime::currentTime().toString("HH:mm:ss:zzz"),
            bytes.toHex(' ').trimmed().toUpper()
        ));
        if (channel) {
            ui->log1TextEdit->setTextColor(QColor(0, 85, 0));
            ui->log1TextEdit->append(QString("[%1]:RX <- %2").arg(
                QTime::currentTime().toString("HH:mm:ss:zzz"),
                bytes.toHex(' ').trimmed().toUpper()
            ));
        }
    });

    connect(spWorker, &SerialPortWorker::havenTransmit,
            this, [this](QByteArray bytes) {
        ui->log1TextEdit->setTextColor(QColor(Qt::blue));
        ui->log1TextEdit->append(
            QString("[%1]:TX -> %2").arg(
                QTime::currentTime().toString("HH:mm:ss:zzz"),
                bytes.toHex(' ').trimmed().toUpper()
            )
        );
        ui->log2TextEdit->setTextColor(QColor(Qt::blue));
        ui->log2TextEdit->append(QString("[%1]:TX -> %2").arg(
            QTime::currentTime().toString("HH:mm:ss:zzz"),
            bytes.toHex(' ').trimmed().toUpper()
            ));
    });

    connect(spWorker, &SerialPortWorker::errorLog,
            this, [this](QString str) {
        ui->log2TextEdit->setTextColor(QColor(Qt::red));
        ui->log2TextEdit->append(QString("[%1]: %2").arg(
            QTime::currentTime().toString("HH:mm:ss:zzz"), str
        ));
    });
}

void SerialPortWidget::setSerialPortStatus(LedColor color)
{
    this->multipleClicksConnect = false;
    if (this->spWarning->isVisible() && (color == red || color == green))
        this->spWarning->close();

    switch (color)
    {
    // case LedColor::transparent:
    //     ui->connectLedLabel->setStyleSheet(ledColor.arg("0, 0, 0, 0"));
    //     break;
    case LedColor::red:
        ui->connectLedLabel->setStyleSheet(ledColor.arg("255, 0, 0, 255"));
        ui->connectComPushButton->setText("打开串口");
        ui->comPortRangePushButton->setEnabled(true);
        ui->comPortComboBox->setEnabled(true);
        this->spAbnormal = false;
        this->openSP = false;
        break;
    case LedColor::green:
        ui->connectLedLabel->setStyleSheet(ledColor.arg("85, 255, 0, 255"));
        ui->connectComPushButton->setText("关闭串口");
        ui->comPortRangePushButton->setEnabled(false);
        ui->comPortComboBox->setEnabled(false);
        this->spAbnormal = false;
        this->openSP = true;
        break;
    case LedColor::orange:
        ui->connectLedLabel->setStyleSheet(ledColor.arg("255, 170, 0, 255"));
        ui->connectComPushButton->setText("重启串口");
        ui->comPortRangePushButton->setEnabled(false);
        ui->comPortComboBox->setEnabled(false);
        this->spAbnormal = true;
        this->openSP = false;
        break;
    default:
        Q_ASSERT_X(false, "ControlWidget::setSerialStatusLed", "color invalid");
        break;
    }
    emit serialPortStatusChanged(color - 1);
}


QChar SerialPortWidget::lockIcon(bool lock)
{
    return QChar(lock ? IconCode::lock : IconCode::unlock);
}

QChar SerialPortWidget::selectorIcon(bool right)
{
    return QChar(right ? IconCode::selectRight : IconCode::selectLeft);
}

QChar SerialPortWidget::eyesIcon(bool off)
{
    return QChar(off ? IconCode::eyesOff : IconCode::eyesOn);
}

QChar SerialPortWidget::pageIcon(bool up)
{
    return QChar(up ? IconCode::pageUp : IconCode::pageDown);
}


/* ========================================================================== */


void SerialPortWidget::checkedForSerialPort()
{
    this->on_connectComPushButton_clicked();
}

// void SerialPortWidget::transmitChangeASlaveState(int addr, int word)
// {
//     QByteArray buf;
//     buf.append(static_cast<quint8>(0x04));
//     buf.append(static_cast<quint8>(addr));
//     buf.append(static_cast<quint8>(word));
//     emit transmitData(buf);
// }


/* ========================================================================== */


void SerialPortWidget::serialPort_watchdog_wakeUp()
{
    this->setSerialPortStatus(LedColor::orange);

    if (this->spWarning->exec() == QMessageBox::Yes)
    {
        // delete sub thread
        auto spDelWorker = spWorker;
        auto spDelSubThread = spSubThread;
        auto spDelTimer = new QTimer(this);

        emit workerCloseSerialPort();
        spDelWorker->disconnect();
        this->disconnect(spDelWorker);

        connect(spDelWorker, &SerialPortWorker::closeFinished, this,
                [spDelWorker, spDelSubThread, spDelTimer, this]() {
            // qDebug() << "close finished:" << spDelWorker->currentPortName();
            spDelTimer->stop();
            connect(spDelSubThread, &QThread::finished, this,
                    [spDelWorker, spDelSubThread, spDelTimer]() {
                spDelSubThread->deleteLater();
                spDelWorker->deleteLater();
                spDelTimer->deleteLater();
            });
            spDelSubThread->quit();
        });

        connect(spDelTimer, &QTimer::timeout, this,
                [spDelWorker, spDelSubThread, spDelTimer]() {
            // qDebug() << "del time out:" << spDelWorker->currentPortName();
            spDelTimer->stop();
            spDelSubThread->terminate();
            spDelSubThread->wait();
            spDelSubThread->deleteLater();
            spDelWorker->deleteLater();
            spDelTimer->deleteLater();
        });

        spDelTimer->setInterval(30000);
        spDelTimer->start();


        // re set thread
        spSubThread = new QThread;
        spWorker = new SerialPortWorker;
        spWorker->moveToThread(spSubThread);
        spSubThread->start();
        this->connectSPWorker();


        // re set gui
        emit reSetSerialPort(
            ui->baudRateComboBox->currentIndex(),
            ui->dataBitsComboBox->currentIndex(),
            ui->stopBitsComboBox->currentIndex(),
            ui->parityComboBox->currentIndex()
        );
        this->setSerialPortStatus(LedColor::red);
    }
}


/* ========================================================================== */


void SerialPortWidget::on_hidePushButton_1_clicked(bool hidden)
{
    SETTINGS().setValue(_GROUP1_HIDDEN_, hidden);
    ui->serialPortGroupBox->setHidden(hidden);
    if (hidden)
        ui->hidePushButton_1->setText(
            hiddenName.arg(ui->serialPortGroupBox->title())
        );
    else
        ui->hidePushButton_1->setText("v");
}

void SerialPortWidget::on_hidePushButton_2_clicked(bool hidden)
{
    SETTINGS().setValue(_GROUP2_HIDDEN_, hidden);
    ui->log1GroupBox->setHidden(hidden);
    if (hidden)
        ui->hidePushButton_2->setText(
            hiddenName.arg(ui->log1GroupBox->title())
        );
    else
        ui->hidePushButton_2->setText("v");
    hiddenGroupManager();
}

void SerialPortWidget::on_hidePushButton_3_clicked(bool hidden)
{
    SETTINGS().setValue(_GROUP3_HIDDEN_, hidden);
    ui->log2GroupBox->setHidden(hidden);
    if (hidden)
        ui->hidePushButton_3->setText(
            hiddenName.arg(ui->log2GroupBox->title())
        );
    else
        ui->hidePushButton_3->setText("v");
    hiddenGroupManager();
}


void SerialPortWidget::on_comPortComboBox_currentIndexChanged(int index)
{
    // save setting about com's num
    auto comPortNum = index + crDialog->comMin();
    SETTINGS().setValue(_COM_PORT_NUM_, comPortNum);
    comList->setComPortNum(comPortNum);
}

void SerialPortWidget::on_comPortRangePushButton_clicked()
{
    if (crDialog->ready(), (crDialog->exec() == QDialog::Accepted))
        comList->updateComBox(crDialog->comMin(), crDialog->comMax());
}

void SerialPortWidget::on_connectComPushButton_clicked()
{
    // have open serial port == need close == next open
    if (this->openSP)
    { // green -> red
        this->setSerialPortStatus(LedColor::red); // gui
        emit workerCloseSerialPort(); // close
    }
    // need open serial port == next close if open sucessfully
    else
    {
        auto abnormal = this->spAbnormal;
        // orange | orange -> red | green  (abnormal)
        emit activateWatchdog();
        // red -> green | orange           (not abnormal)
        if (!abnormal && !this->multipleClicksConnect)
            emit workerOpenSerialPort(
                QString("COM%1").arg(
                    ui->comPortComboBox->currentIndex() + crDialog->comMin()
                )
            );
    }
}

void SerialPortWidget::on_lockPushButton_clicked(bool lock)
{
    SETTINGS().setValue(_SERIAL_PORT_LOCK_, lock);
    ui->lockPushButton->setText(lockIcon(lock));
    ui->baudRateComboBox->setEnabled(!lock);
    ui->dataBitsComboBox->setEnabled(!lock);
    ui->stopBitsComboBox->setEnabled(!lock);
    ui->parityComboBox->setEnabled(!lock);
}



void SerialPortWidget::on_log1SelectorPushButton_clicked(bool left)
{
    SETTINGS().setValue(_LOG1_SELECTOR_, left);
    ui->log1SelectorPushButton->setText(selectorIcon(left));
}

void SerialPortWidget::on_log2SelectorPushButton_clicked(bool left)
{
    SETTINGS().setValue(_LOG2_SELECTOR_, left);
    ui->log2SelectorPushButton->setText(selectorIcon(left));
}

void SerialPortWidget::on_log1LookPushButton_clicked(bool off)
{
    SETTINGS().setValue(_LOG1_EYES_STATE_, off);
    ui->log1LookPushButton->setText(eyesIcon(off));
}

void SerialPortWidget::on_log2LookPushButton_clicked(bool off)
{
    SETTINGS().setValue(_LOG2_EYES_STATE_, off);
    ui->log2LookPushButton->setText(eyesIcon(off));
}

void SerialPortWidget::on_log1ClearPushButton_clicked()
{

}

void SerialPortWidget::on_log2ClearPushButton_clicked()
{

}



