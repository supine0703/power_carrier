#ifndef COMLIST_H
#define COMLIST_H

#include <QObject>
#include <QVector>

class QThread;
class QComboBox;


PROJECT_BEGIN_NAMESPACE

class SerialPortManager;

class ComList : public QObject
{
    Q_OBJECT

public:
    explicit ComList(QObject* parent = nullptr, QComboBox* comsBox = nullptr);
    ~ComList();

    void updateComBox(int min, int max);
    void setComsBox(QComboBox* comsBox);
    void setComPortNum(int num);


private slots:
    void scanComPortPush();


private:
    QThread* subThread;
    QComboBox* comsBox;
    QVector<qint8> comsFlg;
    SerialPortManager* spManager;

    int comPortNum;
    int comMin;


private:
    static const QString comX;


signals:
    void start_timer(int = 1000);
    void stop_timer();
    void currentComChanged();
};

PROJECT_END_NAMESPACE

#endif // COMLIST_H
