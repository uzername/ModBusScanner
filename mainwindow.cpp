#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mymodel.h"

#include <QModbusRtuSerialMaster>
#include <QDateTime>
#include <QFontDatabase>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    AllCOMPorts(new QVector<DialogCOMPort::knownCOMports>()),
    modbusDevice(nullptr),
    deviceConnected(false),
    myModel(0),

    currentDisplayParameter("UnsignedDec"),
    buttonMode(true),  /*make button to work as for start*/

    requestDelayUsed_internal(true),
    requestDelay_internal(10)
{
    ui->setupUi(this);

    storedSettings.parity = QSerialPort::EvenParity;
    storedSettings.baud = QSerialPort::Baud19200;
    storedSettings.dataBits = QSerialPort::Data8;
    storedSettings.stopBits = QSerialPort::OneStop;
    storedSettings.responseTime = 5000;
    storedSettings.numberOfRetries = 3;
    storedSettings.portAddr = "COM9";
    //setting table
    //myModel = new MyModel(0);
    myModel.columnHeaders.append(tr("Пристрій \n адреса"));
    myModel.columnHeaders.append(tr("Тип \n структури даних"));
    myModel.columnHeaders.append(tr("Адреса \n структури даних"));
    myModel.columnHeaders.append(tr("Дані"));

    myModel.currentNumberDisplaySetting="UnsignedDec";

    ui->tableView->setModel( &myModel );

    processingPerformed = false;
    poolingStdTimeout = 4000;
    poolingTimer = new QTimer();
    poolingTimer->setInterval(poolingStdTimeout);
    connect(poolingTimer, SIGNAL(timeout()) , this, SLOT(timerHit()));
    poolingProcessor = new MyPoolingClass("A");
    //declare accepting data from MyPoolingClass which runs as thread
    qRegisterMetaType<QModbusDataUnit>("QModbusDataUnit");

    //MyPoolingClass has internal init of these params in constructor
    poolingProcessor->requestDelay = this->requestDelay_internal;
    poolingProcessor->requestDelayUsed = this->requestDelayUsed_internal;

    connect(poolingProcessor, SIGNAL(sendStringToLog(QString)), this, SLOT(externalLogRequest(QString)) );
    connect(poolingProcessor, SIGNAL(handmadeFinished()), this, SLOT(processPoolingFinished() ) );
    connect(poolingProcessor, SIGNAL(delegateSendReadRequest(QModbusDataUnit,uint)), this, SLOT(externalReadRequest(QModbusDataUnit,uint)) );
    //this list is also used in MyPoolingClass
    theReadList = QStringList();
    theReadList<<"CoilFlag"<<"Discrette Input"<<"HoldingRegister"<<"InputRegister";
    /*
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
    connect(this->ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequested(QPoint)));
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
    connectdialog->setWindowFlags(connectdialog->windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);

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

        modbusDevice->setTimeout(inp_settings.responseTime);
        modbusDevice->setNumberOfRetries(inp_settings.numberOfRetries);

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
/*
QModbusDataUnit* MainWindow::constructReadRequest(QModbusDataUnit::RegisterType inp_regtype)  {
    QModbusDataUnit::RegisterType regType = inp_regtype;
    bool ok = true;
    int startAddress = this->ui->lineEditRegAddr->text().toInt(&ok, 0);
    if (ok == false) {
        logToTextBox(tr("Адреса даних введена неправильно"));
        return NULL;

    }
    int numberOfEntries = this->ui->spinBoxRecordNum->value();

    return new QModbusDataUnit(regType, startAddress, numberOfEntries);

    return NULL;
}
*/
/**
 * @brief MainWindow::constructWriteRequest
 * construct write request for operating. real values will be defined later, not here
 * @return
 */
QModbusDataUnit* MainWindow::constructWriteRequest(QString regTypeInp, quint16 dataRegValue, unsigned int regNumberInp){
    QModbusDataUnit::RegisterType regTypeInternal;
    //theReadList<<"CoilFlag"<<"Discrette Input"<<"HoldingRegister"<<"InputRegister";
    if (regTypeInp == "CoilFlag") {
        regTypeInternal = QModbusDataUnit::RegisterType::Coils;
    } else {
        if (regTypeInp == "Discrette Input") {
            regTypeInternal = QModbusDataUnit::RegisterType::DiscreteInputs;
        } else {
            if (regTypeInp == "HoldingRegister") {
                regTypeInternal = QModbusDataUnit::RegisterType::HoldingRegisters;
            } else {
                if (regTypeInp  == "InputRegister") {
                    regTypeInternal = QModbusDataUnit::RegisterType::InputRegisters;
                } else {
                    regTypeInternal = QModbusDataUnit::RegisterType::Invalid;
                }
            }
        }
    }
    //QVector<quint16> newData = new QVector(); newData.append(dataRegValue);
    QModbusDataUnit * retValue = new QModbusDataUnit(regTypeInternal,regNumberInp,1);
    retValue->setValue(0,dataRegValue);
}
/*
QModbusDataUnit* MainWindow::constructWriteRequest(QModbusDataUnit::RegisterType inp_regtype)
{
    QModbusDataUnit::RegisterType regType = inp_regtype;
    bool ok = true;
    int startAddress = this->ui->lineEditRegAddrWrite->text().toInt(&ok, 0);
    if (ok == false) {
        logToTextBox(tr("Адреса даних введена неправильно"));
        return NULL;

    }
    int numberOfEntries = this->ui->spinBoxRecordNum_2->value();

    return new QModbusDataUnit(regType, startAddress, numberOfEntries);
}
*/
/**
 * @brief MainWindow::sendWriteRequest - send write request wrapper.
 * @param regTypeInp
 * @param dataRegValue
 * @param regNumberInp
 * @param deviceAddrInp
 */
void MainWindow::sendWriteRequest(QString regTypeInp, quint16 dataRegValue, unsigned int regNumberInp, unsigned int deviceAddrInp) {
    //pause execution before calling
    bool scanningWasRunningBefore = this->processingPerformed;
    //stop the scan here
    if (scanningWasRunningBefore == true) {
        on_pushButton_clicked();
    }


    if (deviceConnected == false) {
        QMessageBox msgBox;
        msgBox.setText("Підключення НЕ виконано.\n Потрібно підключитись до пристрою");
        msgBox.exec();
        return;
    }
    logToTextBox(tr("Запуск запиту на запис"));
    /*
    QModbusDataUnit* writeUnit = constructWriteRequset
    if (writeUnit == NULL) {
        logToTextBox(tr("Запит на запис НЕ було відправлено"));
        return;
    }
*/
/*
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

    //restore scan process if it was done before
    if (scanningWasRunningBefore == true) {
        on_pushButton_clicked();
    }
}

void MainWindow::logToTextBox(QString goodMsgForDisplay)
{
    //qDebug(goodMsgForDisplay.toStdString().c_str());
    QString dateStamp="["+QDateTime::currentDateTime().toString(Qt::ISODateWithMs)+"]:";
    QTextCharFormat format;
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    format.setFont(fixedFont);
    this->ui->textEditMain->setCurrentCharFormat(format);
    this->ui->textEditMain->append(dateStamp+goodMsgForDisplay);
    //this->ui->textEditMain->append("<font face=\"Courier\">"+dateStamp+goodMsgForDisplay+"</font>");
}

/*be careful, set only one value at a time*/
void MainWindow::addValueToTable(QModbusDataUnit inpUnit, int serv)
{
    bool dataItmFound = false;
    unsigned int rowIndex = /*NAN*/0;
    foreach (MyModel::rowDataItm theItem, this->myModel.dataItems) {

        if (serv == theItem.deviceAddress
                && inpUnit.startAddress() == theItem.dataStructAddress
                && (this->formalModbusRegTypeToString( inpUnit.registerType() ) == theItem.dataStructType) ) {
            dataItmFound = true;
            break;
        }
        rowIndex++;
    }
    if (dataItmFound == false) {
        qDebug("data itm not found (heuristics), returning");
        return;
    }
    QModelIndex theIndex = myModel.index(rowIndex,3);

    myModel.setData(theIndex, QVariant(inpUnit.value(0)), Qt::DisplayRole);
}

void MainWindow::timerHit()  {
    //start processing. It may take long time, so a thread might be required
    logToTextBox(tr("Настав час таймера."));
    if (processingPerformed == true) {
        logToTextBox(tr("Обробка комірок ще не завершена. Сигнал таймера пропущено"));
    } else {
        //good time to recall scanning
        poolingProcessor->start();
    }
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
    qDebug("READREADY! (in main thread)");
    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++) {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress())
                                     .arg(QString::number(unit.value(i),
                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            logToTextBox(entry);

            //ui->readValue->addItem(entry);
            //value has been returned; add here data to table
            addValueToTable(unit, reply->serverAddress());
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
//append item to table and listener
void MainWindow::on_pushButtonAdd_clicked()  {
    //if the scanning has been started before then stop it. Actually this should be done by user, but let's do it here
    bool scanningWasRunningBefore = this->processingPerformed;
    //stop the scan here
    if (scanningWasRunningBefore == true) {
        on_pushButton_clicked();
    }
    //open dialog
    DialogRecord* newRecordDialog = new DialogRecord(this);
    newRecordDialog->setWindowFlags(newRecordDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
    newRecordDialog->exec();
    if ( (newRecordDialog->acceptButtonClicked == true)&&(newRecordDialog->cancelButtonClicked == false) ) {
        //this->myModel.insertRow(this->myModel.rowCount());
        //int lastRow = this->myModel.rowCount();
        //this->myModel.setData(/*this->myModel.index(lastRow,1)*/new QModelIndex(lastRow, 0),"",Qt::DisplayRole);

        unsigned int deviceAddrLocal =newRecordDialog->deviceAddress;
        QString regType = newRecordDialog->dataStructType;
        unsigned int dataStructAddrLocal = newRecordDialog->dataStructAddress;

        this->myModel.brandNewAppendData(deviceAddrLocal, regType, dataStructAddrLocal);

    } else {
        if ( (newRecordDialog->acceptButtonClicked == false)&&(newRecordDialog->cancelButtonClicked == true) ) {
            statusBar()->showMessage("record -- canceled",5000);
        }
    }
    //start the scan after adding, if it has been running before adding rcrd
    if (scanningWasRunningBefore == true) {
        on_pushButton_clicked();
    }
}
/*run (or stop) scanning process*/
void MainWindow::on_pushButton_clicked()  {
    if (/* this->processingPerformed == false */ this->buttonMode==true) {
        if (this->myModel.rowCount()==0) {
            QMessageBox msgBox;
            msgBox.setText(tr("Потрібен хоча б один елемент відслідковування"));
            msgBox.exec();
            return;
        }
        //creation of pooling processor has been performed earlier, in constructor. Creating instance here breaks signals and slots of pooling processor
        poolingProcessor->initPooling(this->modbusDevice, &(this->myModel));

        poolingTimer->start();
        poolingProcessor->start();

        this->ui->pushButton->setText("Stop Scanning");
        this->processingPerformed = true;

        this->buttonMode=false;
    } else {
        //delete poolingProcessor;
        //better not to play with deleting QThread
        poolingTimer->stop();
        // https://forum.qt.io/topic/20539/what-is-the-correct-way-to-stop-qthread/3
        poolingProcessor->poolingPerformed = false;
        this->ui->pushButton->setText("Start Scanning");
        logToTextBox(tr("Опитування зупинено користувачем"));
        this->processingPerformed = false;

        this->buttonMode = true;
    }

}
/**
 * @brief MainWindow::on_actionDisplay_triggered . display dialog with params choice (display option)
 */
void MainWindow::on_actionDisplay_triggered()
{
    /*
    //if the scanning has been started before, then stop it. Actually this should be done by user, but let's do it here
    bool scanningWasRunningBefore = this->processingPerformed;
    //stop the scan here
    if (scanningWasRunningBefore == true) {
        on_pushButton_clicked();
    }
    */
    DialogDisplayParams * displParamsDialog = new DialogDisplayParams(this);

    displParamsDialog->setWindowFlags(displParamsDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
    displParamsDialog->setDisplayParam(this->currentDisplayParameter);
    displParamsDialog->exec();

    if (displParamsDialog->acceptButtonClicked && !(displParamsDialog->cancelButtonClicked)) {
        this->currentDisplayParameter = displParamsDialog->returnedParam;
        myModel.currentNumberDisplaySetting = this->currentDisplayParameter;
    }
    /*
    //start the scan after updating settings, if it has been running before adding rcrd
    if (scanningWasRunningBefore == true) {
        on_pushButton_clicked();
    }
    */
}
/**
 * @brief MainWindow::on_actionGenTimer_triggered . opens a polling timer setting dialog window
 */
void MainWindow::on_actionGenTimer_triggered()  {
    qDebug("opening timer polling dialog");
    bool ok;
    int newValueTimer = QInputDialog::getInt(this, tr("Таймер заг. циклу опитування"), tr("Очікувана тривалість одної ітерації (мс)"),
                                             this->poolingStdTimeout,
                                             5,10000,1,&ok);
    if (ok) {
        logToTextBox(QString("Змінюється значення загального таймера. Старе значення - %1 ; Нове значення - %2").arg(this->poolingStdTimeout).arg(newValueTimer));
        //setting timer's value
        //if the scanning has been started before then stop it. Actually this should be done by user, but let's do it here
        bool scanningWasRunningBefore = this->processingPerformed;
        //stop the scan here
        if (scanningWasRunningBefore == true) {
            on_pushButton_clicked();
        }
        this->poolingStdTimeout = newValueTimer;

        this->poolingTimer->setInterval(/*newValueTimer*/this->poolingStdTimeout);

        //start the scan after adding, if it has been running before adding rcrd
        if (scanningWasRunningBefore == true) {
            on_pushButton_clicked();
        }

    }
}
void MainWindow::on_actionQueryTimer_triggered() {
    qDebug("opening interquery timing param dialog");
    bool ok;
    int newValueTimer = QInputDialog::getInt(this, tr("Очікування між запитами"), tr("Тривалість затримки \n між відправкою запитів (мс)"),
                                             this->requestDelay_internal,
                                             0,5000,1,&ok);
    if (ok) {
        bool newValueTimerUsed = (this->requestDelay_internal != 0);
        logToTextBox(QString("Змінюється значення затримки між запитами. Старе значення - %1 ; Нове значення - %2. Використання затримки: %3").arg(this->requestDelay_internal).arg(newValueTimer).arg(newValueTimerUsed) );
        //if the scanning has been started before then stop it. Actually this should be done by user, but let's do it here
        bool scanningWasRunningBefore = this->processingPerformed;
        //stop the scan here
        if (scanningWasRunningBefore == true) {
            on_pushButton_clicked();
        }

        this->requestDelayUsed_internal = newValueTimerUsed;
        this->requestDelay_internal = newValueTimer;
        this->poolingProcessor->requestDelayUsed = this->requestDelayUsed_internal;
        this->poolingProcessor->requestDelay = this->requestDelay_internal;

        //start the scan after adding, if it has been running before adding rcrd
        if (scanningWasRunningBefore == true) {
            on_pushButton_clicked();
        }

    }
}
//display context menu on table view
//https://forum.qt.io/topic/31233/how-to-create-a-custom-context-menu-for-qtableview/3
void MainWindow::customMenuRequested(QPoint pos)
{
    index = this->ui->tableView->indexAt(pos);
    //qDebug(QString("( %1 ; %2 )").arg(index.column()).arg(index.row()) );
    int selectedRow = index.row(); int selectedCol = index.column();
    if ((selectedRow > -1) && (selectedCol > -1)) {
        QMenu *menu=new QMenu(this);
        QAction* writeAction = new QAction("Записати...", this);
        QAction* takeawayAction = new QAction("Прибрати...", this);
        menu->addAction(writeAction);
        menu->addAction(takeawayAction);
        connect(writeAction, &QAction::triggered, this, &MainWindow::writeActionProcessor);

        menu->popup(this->ui->tableView->viewport()->mapToGlobal(pos));
    }
}
//here is performed writing action
void MainWindow::writeActionProcessor()
{
//show dialog
    //open dialog
    DialogWriteItm* writeRequestDialog = new DialogWriteItm(this);
    writeRequestDialog->setWindowFlags(writeRequestDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
        writeRequestDialog->deviceAddress = myModel.dataItems.at(index.row()).deviceAddress;
        writeRequestDialog->dataStructAddress = myModel.dataItems.at(index.row()).dataStructAddress;
        writeRequestDialog->dataValue = myModel.dataItems.at(index.row()).dataStructData;
        writeRequestDialog->dataStructType = myModel.dataItems.at(index.row()).dataStructType;
        writeRequestDialog->dataEntered();
    writeRequestDialog->exec();
    if ((writeRequestDialog->acceptButtonClicked == true) && (writeRequestDialog->cancelButtonClicked == false)) {
        writeRequestDialog->dataEntered();
        //we have retrieved data from dialog, now performing write request
        quint16 dataWrite = writeRequestDialog->dataValue;
        unsigned int deviceAddrWrite = writeRequestDialog->deviceAddress;
        unsigned int registerNumWrite = writeRequestDialog->dataStructAddress;
        QString registerTypeWrite = writeRequestDialog->dataStructType;



    } else {
        if ((writeRequestDialog->acceptButtonClicked == false) && (writeRequestDialog->cancelButtonClicked == true)) {

        }
    }
}

void MainWindow::externalLogRequest(QString externalLogLine) {
    logToTextBox(externalLogLine);
}

void MainWindow::processPoolingFinished()  {
    logToTextBox(tr("Тред оголосив про завершення обробки"));
    processingPerformed = false;
}

void MainWindow::externalReadRequest(QModbusDataUnit theUnit, unsigned int devAddr) {
    logToTextBox("MainWindow: sending request to port");
    if (auto *reply = modbusDevice->sendReadRequest(theUnit, devAddr )) {
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
}

QString MainWindow::formalModbusRegTypeToString(QModbusDataUnit::RegisterType inpRegType ) {
   // see MyPoolingClass::readRequest
    switch (inpRegType) {
        case (QModbusDataUnit::Coils): { return "CoilFlag"; break;}
        case (QModbusDataUnit::DiscreteInputs): { return "Discrete Input"; break;}
        case (QModbusDataUnit::HoldingRegisters): {return "HoldingRegister"; break;}
        case (QModbusDataUnit::InputRegisters): {return "InputRegister"; break; }
        default: {return "Invalid";}
    }
}
