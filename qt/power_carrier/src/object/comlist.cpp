#include "comlist.h"

#include "serialportmanager.h"
#include "configurations.h"

#include <QComboBox>
#include <QThread>


PROJECT_USING_NAMESPACE;


const QString ComList::comX("COM%1");

ComList::ComList(QObject* parent, QComboBox* comsBox)
    : QObject{parent}
    , comsBox(comsBox)
    , subThread(new QThread)
    , spManager(new SerialPortManager)
{
    this->comPortNum = SETTINGS().value(_COM_PORT_NUM_).toInt();
    this->spManager->moveToThread(subThread);
    this->subThread->start();
    connect(this, &ComList::start_timer, spManager, &SerialPortManager::startTimer);
    connect(this, &ComList::stop_timer, spManager, &SerialPortManager::stopTimer);
    connect(spManager, &SerialPortManager::availablePortsChanged,
            this, &ComList::scanComPortPush);
}


ComList::~ComList()
{
    this->subThread->quit();
    this->subThread->wait();
    this->subThread->deleteLater();
    this->spManager->deleteLater();
}


void ComList::updateComBox(int min, int max)
{
    this->comMin = min;
    if (this->comsBox == nullptr)
        return;
    // stop update available ports
    bool ts = this->spManager->isStart();
    if (ts)
        emit this->stop_timer();

    // reset com status's flg
    auto len = max - min + 1;
    this->comsFlg.clear();
    this->comsFlg.resize(len, 0);

    // set options
    QStringList comList;
    for (int i = min; i <= max; i++)
        comList << comX.arg(i);

    // update serialPortComboBox and repos current index
    auto n = this->comPortNum - min;
    this->comsBox->clear();
    this->comsBox->addItems(comList);
    this->comsBox->setCurrentIndex(
        n <= 0 ? 0 : (n < len ? n : len - 1)
        // (0 < n && n < len) ? n : 0
);

    // re start update available ports
    if (ts)
        emit this->start_timer();
}


void ComList::setComsBox(QComboBox* comsBox)
{
    this->comsBox = comsBox;
    if (comsBox)
    {
        this->updateComBox(
            SETTINGS().value(_COM_RANGE_MIN_).toInt(),
            SETTINGS().value(_COM_RANGE_MAX_).toInt()
    );
        emit this->start_timer();
    }
    else
        emit this->stop_timer();
}


void power_carrier::ComList::setComPortNum(int num)
{
    this->comPortNum = num;
}


void ComList::scanComPortPush()
{
    // matching com's num through regular expression
    static const QRegularExpression re(R"(\d+)");
    static QRegularExpressionMatch match;

    if (this->comsBox == nullptr)
        return;


    auto currentCom = this->comsBox->currentText();
    auto len = this->comsFlg.length();

    // update options which scanned serial port
    for (const auto& info : spManager->availablePorts())
    {
        match = re.match(info);
        auto index = match.captured().toInt() - this->comMin;
        if (0 <= index && index < len)
        {
            this->comsFlg[index] = 1;
            this->comsBox->setItemText(index, info);
        }
    }

    // update com status flg: 0-not 1-have 2-have->not default-illegal
    for (int i = 0; i < len; i++)
    {
        switch (this->comsFlg[i])
        {
        case 0:
            break;
        case 1:
            this->comsFlg[i] = 2;
            break;
        case 2:
            this->comsFlg[i] = 0;
            this->comsBox->setItemText(i, comX.arg(i + this->comMin));
            break;
        default:
            Q_ASSERT_X(false, "ComList::scanComPortPush", "coms flg illegal");
        }
    }

    if (currentCom != this->comsBox->currentText())
        emit currentComChanged();
}

