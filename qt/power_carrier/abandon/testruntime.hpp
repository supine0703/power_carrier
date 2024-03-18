#ifndef TESTRUNTIME_H
#define TESTRUNTIME_H

#include <QDebug>
#include <QElapsedTimer>

class testRunTime
{
public:
    explicit testRunTime(int times) : times(times) {}

    template <typename Func>
    void test(Func func, const QString& what = "")
    {
        QElapsedTimer t;
        t.start();
        for (int i = 0; i < times; i++)
            func();
        qDebug() << what << "use time:" << t.nsecsElapsed();
    }

    // void test(void (*func)(void))
    // {
    //     QElapsedTimer t;
    //     t.start();
    //     for (int i = 0; i < times; i++)
    //         func();
    //     qDebug() << "use time:" << t.nsecsElapsed();
    // }

private:
    int times;
};

#endif // TESTRUNTIME_H
