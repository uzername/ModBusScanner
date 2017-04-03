#include "mypoolingclass.h"

MyPoolingClass::MyPoolingClass(QString s) : name(s)
{

}

void MyPoolingClass::run()
{

}

void MyPoolingClass::initPooling(QModbusClient *modbusClientPtrInp, MyModel *modelPtrInp)  {
    this->modbusDevicePtr = modbusClientPtrInp;
    this->modelPtr = modelPtrInp;
}

void MyPoolingClass::performPooling()
{

}
bool MyPoolingClass::event(QEvent* event){
    bool parentResult = QThread::event(event);

    return parentResult;
}
