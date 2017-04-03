#ifndef MYPOOLINGCLASS_H
#define MYPOOLINGCLASS_H

#include <QThread>
#include <QModbusDataUnit>
#include <QModbusRtuSerialMaster>
#include "mymodel.h"

class MyPoolingClass : public QThread
{
Q_OBJECT
public:
    explicit MyPoolingClass(QString s);
    void run();
    /**
     * @brief initPooling . call initPooling and set all the values required for pooling.
     */
    void initPooling(QModbusClient *modbusClientPtrInp, MyModel *modelPtrInp);
    bool event(QEvent* event);
    bool poolingPerformed;
signals:
    void send();
private:
    MyModel *modelPtr;
    //we need pointer to modbusdevice to start polling data by given addresses (stored in MyModel instance)
    QModbusClient *modbusDevicePtr;

    void performPooling();

    QString name;
};

#endif // MYPOOLINGCLASS_H
