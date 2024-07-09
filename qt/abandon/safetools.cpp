#include "safetools.h"

#include <QMutex>

bool SafeTools::reFlg(false);
QVector<QMutex*> SafeTools::mutexs(1, new QMutex);


void SafeTools::mutexsReSize(qsizetype size)
{
    static QMutex M;
    Q_ASSERT(size > 0);
    if (!reFlg)
    {
        if (M.tryLock())
        {
            reFlg = true;
            for (auto& m : mutexs)
            {
                while (!m->tryLock())
                    ;
                m->unlock();
                delete m;
            }
            mutexs.clear();
            mutexs.resize(size);
            for (auto& m : mutexs)
                m = new QMutex;
            reFlg = false;
        }
    }
}

