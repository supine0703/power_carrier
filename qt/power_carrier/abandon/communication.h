#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QtTypes>

PROJECT_BEGIN_NAMESPACE

class Communication
{
public:
    Communication(quint8 pc_address);



private:
    quint8 addr;
};

PROJECT_END_NAMESPACE

#endif // COMMUNICATION_H
