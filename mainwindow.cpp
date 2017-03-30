#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QModbusRtuSerialMaster>
#include <QDateTime>
#include <QFontDatabase>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    AllCOMPorts(new QVector<DialogCOMPort::knownCOMports>()),
    modbusDevice(nullptr),
    deviceConnected(false)
{
    ui->setupUi(this);

    storedSettings.parity = QSerialPort::EvenParity;
    storedSettings.baud = QSerialPort::Baud19200;
    storedSettings.dataBits = QSerialPort::Data8;
    storedSettings.stopBits = QSerialPort::OneStop;
    storedSettings.responseTime = 5000;
    storedSettings.numberOfRetries = 3;
    storedSettings.portAddr = "COM9";
/*
    QStringList theReadList;
    theReadList<<"CoilFlag"<<"Discrette Input"<<"HoldingRegister"<<"InputRegister";
    ui->comboBoxType->addItems(theReadList);
    ui->comboBoxType->setCurrentIndex(2);
    QStringList theWriteList;
    theWriteList<<"CoilFlag"<<"HoldingRegister";
    ui->comboBoxTypeWrite->addItems(theWriteList);
    ui->comboBoxTypeWrite->setCurrentIndex(1);

    ui->lineEditRegAddr->setValidator(new QIntValidator(0,65535) );
    ui->lineEditRegAddrWrite->setValidator(new QIntValidator(0,65535) );
    ui->lineEditData->setValidator(new QIntValidator(0,65535) );
*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * This code opens 'connect' dialog.
 * http://stackoverflow.com/questions/13116863/qt-show-modal-dialog-ui-on-menu-item-click
 *
 * @brief MainWindow::on_actionConnect_triggered
 */
void MainWindow::openComPortDialog()
{
    DialogCOMPort *connectdialog = new DialogCOMPort(this);
    // http://stackoverflow.com/questions/81627/how-can-i-hide-delete-the-help-button-on-the-title-bar-of-a-qt-dialog
    connectdialog->setWindowFlags(connectdialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    //see http://stackoverflow.com/questions/18147038/passing-object-by-reference-in-c and http://stackoverflow.com/a/18147466
    connectdialog->setComPorts(recalculateCOMPorts, (this->AllCOMPorts));
    if (recalculateCOMPorts == true) { recalculateCOMPorts = false; }

    connectdialog->setSettings(storedSettings);
    connectdialog->exec();
    if ( (connectdialog->acceptButtonClicked == true) && (connectdialog->cancelButtonClicked == false) ) {
        statusBar()->showMessage(tr("Підключення до пристрою"));
        logToTextBox(tr("Дали згоду на підключення до пристрою"));
        //grab here settings
        DialogCOMPort::Settings grabbedSettings = connectdialog->getSettings();
        //init here connection to modbus device
        storedSettings = grabbedSettings;
        qDebug("Trying to connect to device");
        logToTextBox(tr("Пробуємо підключитись до пристрою"));
        this->connectDevice(grabbedSettings);
    } else
    if ( (connectdialog->acceptButtonClicked == false) && (connectdialog->cancelButtonClicked == true) ) {
        statusBar()->showMessage("canceled!",5000);
    }
}
/**
 * @brief MainWindow::connectDevice
 * connect to device, initialize modbusDevice variable. We use here only serial connection, TCP not used here
 * @param inp_settings
 */
void MainWindow::connectDevice(DialogCOMPort::Settings inp_settings) {
    if (!modbusDevice) { //basic init of modbus device connection if it has not been done before
        modbusDevice = new QModbusRtuSerialMaster(this);
        qDebug("modbusDevice created at the very beginning");
        logToTextBox(tr("modbusDevice створено на самому початку"));
    }
    //if modbus device has been connected, disconnect it: we'll set the connection from scratch
    if ( (modbusDevice) && (modbusDevice->state() == QModbusDevice::ConnectedState) ) {
        modbusDevice->disconnectDevice();
        qDebug("disconnecting device");
        logToTextBox(tr("Відключення..."));
        delete modbusDevice;
        modbusDevice = nullptr;
        modbusDevice = new QModbusRtuSerialMaster(this);
        qDebug("modbusDevice created after resetting");
        logToTextBox(tr("modbusDevice створено заново"));
    }
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        qDebug("Connecting device");
        //set parameters
        QString portName = inp_settings.portAddr;
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
            portName);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
            inp_settings.parity);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
            inp_settings.baud);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
            inp_settings.dataBits);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
            inp_settings.stopBits);

        modbusDevice->setTimeout(1000);
        modbusDevice->setNumberOfRetries(3);
        //connect device
        if (!modbusDevice->connectDevice()) {
            QString errorLine = tr("Не вийшло: ") + modbusDevice->errorString();
            statusBar()->showMessage(errorLine, 0);
            logToTextBox(errorLine);
            qDebug("Connection failure");
        } else {
            statusBar()->showMessage(tr("Вийшло!"), 0);
            logToTextBox(tr("Виконано підключення до порту: ")+portName);
            qDebug("Connection success");

            deviceConnected = true;
        }
    }
}

QModbusDataUnit* MainWindow::constructReadRequest(QModbusDataUnit::RegisterType inp_regtype)  {
    QModbusDataUnit::RegisterType regType = inp_regtype;
    /*
    bool ok = true;
    int startAddress = this->ui->lineEditRegAddr->text().toInt(&ok, 0);
    if (ok == false) {
        logToTextBox(tr("Адреса даних введена неправильно"));
        return NULL;

    }
    int numberOfEntries = this->ui->spinBoxRecordNum->value();

    return new QModbusDataUnit(regType, startAddress, numberOfEntries);
    */
    return NULL;
}
/**
 * @brief MainWindow::constructWriteRequest
 * construct write request for operating. real values will be defined later, not here
 * @return
 */
QModbusDataUnit* MainWindow::constructWriteRequest(QModbusDataUnit::RegisterType inp_regtype)
{
    /*
    QModbusDataUnit::RegisterType regType = inp_regtype;
    bool ok = true;
    int startAddress = this->ui->lineEditRegAddrWrite->text().toInt(&ok, 0);
    if (ok == false) {
        logToTextBox(tr("Адреса даних введена неправильно"));
        return NULL;

    }
    int numberOfEntries = this->ui->spinBoxRecordNum_2->value();

    return new QModbusDataUnit(regType, startAddress, numberOfEntries);
    */
    return NULL;
}

void MainWindow::logToTextBox(QString goodMsgForDisplay)
{

    QString dateStamp="["+QDateTime::currentDateTime().toString(Qt::ISODateWithMs)+"]:";
    QTextCharFormat format;
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    format.setFont(fixedFont);
    this->ui->textEditMain->setCurrentCharFormat(format);
    this->ui->textEditMain->append(dateStamp+goodMsgForDisplay);
    //this->ui->textEditMain->append("<font face=\"Courier\">"+dateStamp+goodMsgForDisplay+"</font>");
}

//call read function
void MainWindow::on_pushButtonRead_clicked()
{
    /*
    if (deviceConnected == false) {
        QMessageBox msgBox;
        msgBox.setText("Підключення НЕ виконано.\n Потрібно підключитись до пристрою");
        msgBox.exec();
        return;
    }
    logToTextBox(tr("Запуск запиту на читання"));
    //find out the register code
    QModbusDataUnit::RegisterType selectedReg;
    int selectedItmList=this->ui->comboBoxType->currentIndex();
    switch (selectedItmList) {
        case 0: selectedReg=QModbusDataUnit::RegisterType::Coils; break;
        case 1: selectedReg=QModbusDataUnit::RegisterType::DiscreteInputs; break;
        case 2: selectedReg=QModbusDataUnit::RegisterType::HoldingRegisters; break;
        case 3: selectedReg=QModbusDataUnit::RegisterType::InputRegisters; break;
        default: break;
    }

    QModbusDataUnit* preparedQuery = constructReadRequest(selectedReg);
    if (preparedQuery == NULL) {
        logToTextBox(tr("Запит на читання НЕ було відправлено"));
        return;
    }
    logToTextBox(tr("Запит на читання було відправлено"));
    if (auto *reply = modbusDevice->sendReadRequest(*preparedQuery, ui->spinBoxDeviceAddr->value() )) {
        if (!reply->isFinished()) {
            logToTextBox(tr("Чекаємо відповіді на запит читання"));
            connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
        } else {
            logToTextBox(tr("Відповідь на запит читання прийшла зразу. Це дивно"));
            delete reply; // broadcast replies return immediately
        }
    } else {
        QString err_line = tr("Read error: ") + modbusDevice->errorString();
        statusBar()->showMessage(err_line, 5000);
        logToTextBox(err_line);
    }
    */
}
/**
 * @brief MainWindow::readReady
 * function for read processing. called when read request returns
 */
void MainWindow::readReady() {
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++) {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress())
                                     .arg(QString::number(unit.value(i),
                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            logToTextBox(entry);
            //ui->readValue->addItem(entry);
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        QString noGoodMessage = tr("Read response error: %1 (Mobus exception: 0x%2)").
                arg(reply->errorString()).
                arg(reply->rawResult().exceptionCode(), -1, 16);
        statusBar()->showMessage(noGoodMessage, 5000);
        logToTextBox(noGoodMessage);
    } else {
        QString noGoodMessage = tr("Read response error: %1 (code: 0x%2)").
                arg(reply->errorString()).
                arg(reply->error(), -1, 16);
        logToTextBox(noGoodMessage);
        statusBar()->showMessage(noGoodMessage , 5000);
    }
    reply->deleteLater();
}

void MainWindow::on_pushButtonWrite_clicked()
{
    /*
    if (deviceConnected == false) {
        QMessageBox msgBox;
        msgBox.setText("Підключення НЕ виконано.\n Потрібно підключитись до пристрою");
        msgBox.exec();
        return;
    }
    logToTextBox(tr("Запуск запиту на запис"));

    QModbusDataUnit::RegisterType selectedReg;
    int selectedItmList=this->ui->comboBoxTypeWrite->currentIndex();
    switch (selectedItmList) {
        case 0: selectedReg=QModbusDataUnit::RegisterType::Coils; break;
    case 1: selectedReg=QModbusDataUnit::RegisterType::HoldingRegisters; break;

        default: break;
    }

    QModbusDataUnit* writeUnit = constructWriteRequest(selectedReg);
    if (writeUnit == NULL) {
        logToTextBox(tr("Запит на запис НЕ було відправлено"));
        return;
    }
    bool ok = true;
    int writeValue = this->ui->lineEditData->text().toInt(&ok, 0);

            if (ok == false) {
                logToTextBox(tr("Дані введено неправильно. Не відправлено"));
                return ;

            }
    //add write data
    writeUnit->setValue(0, writeValue);

    if (auto *reply = modbusDevice->sendWriteRequest(*writeUnit, ui->spinBoxDeviceAddrWrite->value() )) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    QString err_line = tr("Write response error: %1 (Mobus exception: 0x%2)").arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                    statusBar()->showMessage(err_line, 5000);
                    logToTextBox(err_line);
                } else if (reply->error() != QModbusDevice::NoError) {
                    QString err_line = tr("Write response error: %1 (code: 0x%2)").arg(reply->errorString()).arg(reply->error(), -1, 16);
                    statusBar()->showMessage(err_line, 5000);
                    logToTextBox(err_line);
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
*/
}
