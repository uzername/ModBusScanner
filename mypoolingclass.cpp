#include "mypoolingclass.h"

MyPoolingClass::MyPoolingClass(QString s) : name(s)
{

}

void MyPoolingClass::run()
{
    this->poolingPerformed = true;
    this->performPooling();
    emit handmadeFinished();
}

void MyPoolingClass::initPooling(QModbusClient *modbusClientPtrInp, MyModel *modelPtrInp)  {
    this->modbusDevicePtr = modbusClientPtrInp;
    this->modelPtr = modelPtrInp;
}

void MyPoolingClass::performPooling()
{
    foreach (MyModel::rowDataItm modelItm, this->modelPtr->dataItems) {
        //perform polling for each item in model
        //qDebug("performing polling...");
        emit sendStringToLog(QString("performing polling: %1").arg(MyModel::rowDataItmToString(modelItm)));

        if (!modbusDevicePtr)
            return;
        //see modbus master example from qt examples
        //========= Emitting read signal to device =========

        if (auto *reply = modbusDevicePtr->sendReadRequest(readRequest(modelItm), modelItm.deviceAddress )) {
            if (!reply->isFinished())
                connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
            else
                delete reply; // broadcast replies return immediately
        } else {
            //statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
            QString errline = tr("Read error: ") + modbusDevice->errorString();
            emit sendStringToLog(errline);
        }

        //======================================
        if (poolingPerformed == false) {
            break;
        }
    }

}

QModbusDataUnit MyPoolingClass::readRequest(MyModel::rowDataItm modelItmParm) {
    unsigned int startAddress = modelItmParm.dataStructAddress;
    unsigned int numberOfEntries = 1;
    QModbusDataUnit::RegisterType table;

    if (modelItmParm.dataStructType == "CoilFlag") {
        table = QModbusDataUnit::RegisterType::Coils;
    } else {
        if (modelItmParm.dataStructType == "Discrette Input") {
            table = QModbusDataUnit::RegisterType::DiscreteInputs;
        } else {
            if (modelItmParm.dataStructType == "HoldingRegister") {
                table = QModbusDataUnit::RegisterType::HoldingRegisters;
            } else {
                if (modelItmParm.dataStructType == "InputRegister") {
                    table = QModbusDataUnit::RegisterType::InputRegisters;
                }
            }
        }
    }
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}

bool MyPoolingClass::event(QEvent* event){
    bool parentResult = QThread::event(event);

    return parentResult;
}
