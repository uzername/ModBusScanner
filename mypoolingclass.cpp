#include "mypoolingclass.h"

MyPoolingClass::MyPoolingClass(QString s) : name(s)
{
this->requestDelay = 100;
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

        if (!modbusDevicePtr) {
            //modbus device has not been initialized. skipping
            emit sendStringToLog(tr("Пристрій не інціалізовано. Пропускаємо отримання даних"));
            continue;
        }

        //see modbus master example from qt examples
        //========= Emitting read signal to device =========
        //apparently we could write it without threading. It is a weird limitation of QT: we cannot create a thread inside another (current) thread.
        //and sendreadrequest creates a thread. Emit a signal!
        //see http://stackoverflow.com/questions/3268073/qobject-cannot-create-children-for-a-parent-that-is-in-a-different-thread
        QModbusDataUnit immaterialRequest = readRequest(modelItm);
        /*
        if (auto *reply = modbusDevicePtr->sendReadRequest(readRequest(modelItm), modelItm.deviceAddress )) {
            if (!reply->isFinished())
                connect(reply, &QModbusReply::finished, this, &MyPoolingClass::readReady);
            else
                delete reply; // broadcast replies return immediately
        } else {
            //statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
            QString errline = tr("Read error: ") + modbusDevicePtr->errorString();
            emit sendStringToLog(errline);
        }
        */
        emit delegateSendReadRequest(immaterialRequest, modelItm.deviceAddress);
        msleep(requestDelay);
        //======================================
        if (poolingPerformed == false) {
            break;
        }
    }

}

void MyPoolingClass::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++) {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress())
                                     .arg(QString::number(unit.value(i),
                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            emit sendStringToLog(entry);
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        QString errline = tr("Read response error: %1 (Modbus exception: 0x%2)").
                                    arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
        emit sendStringToLog(errline);
    } else {
        QString errline2 = tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).arg(reply->error(), -1, 16);
        emit sendStringToLog(errline2);
    }

    reply->deleteLater();
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

bool MyPoolingClass::event(QEvent* event) {
    bool parentResult = QThread::event(event);

    return parentResult;
}
