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
    /**
     * @brief requestDelay . Because modbus requires some delay between queries,
     * we should give it capability to recover between queries. This value represents time delay in ms as int (that's how QT processes it)
     * actually it depends on baud rate (3.5 symbols), but let's keep it simple. Might be important for low timings
     */
    bool requestDelayUsed;
    int requestDelay;
    QModbusDataUnit readRequest(MyModel::rowDataItm modelItmParm);
signals:
    void send();
    void sendStringToLog( QString logLine );
    void handmadeFinished();
    void delegateSendReadRequest(QModbusDataUnit immaterialRequestInp, unsigned int deviceAddress);
private:
    MyModel *modelPtr;
    //we need pointer to modbusdevice to start polling data by given addresses (stored in MyModel instance)
    QModbusClient *modbusDevicePtr;

    void performPooling();

    QString name;
public slots:
    void readReady();
};

#endif // MYPOOLINGCLASS_H
