#ifndef MYPOOLINGCLASS_H
#define MYPOOLINGCLASS_H

#include <QThread>

class MyPoolingClass
{
public:
    MyPoolingClass();
    void initPooling();
    bool poolingPerformed;
private:
    QThread poolingThread;
    void performPooling();
};

#endif // MYPOOLINGCLASS_H
