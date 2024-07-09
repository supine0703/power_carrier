#ifndef SAFETOOLS_H
#define SAFETOOLS_H

#define SAFE_NEW SafeTools::st_new

#include <QVector>

#include <QMutex>

class SafeTools
{
    SafeTools() = delete;
    ~SafeTools() = delete;
    SafeTools(SafeTools&) = delete;
    SafeTools(SafeTools&&) = delete;

public:
    static void mutexsReSize(qsizetype size);

    template<typename T>
    static void st_new(T* &p, qsizetype key = 0);

private:
    static QVector<QMutex*> mutexs;
    static bool reFlg;
};

template<typename T>
void SafeTools::st_new(T* &p, qsizetype key)
{
    Q_ASSERT(0 <= key && key < mutexs.length());
    if (!p)
    {
        while (!reFlg);
        mutexs[key]->lock();
        if (!p)
        {
            p = new T;
        }
    }
}

#endif // SAFETOOLS_H
