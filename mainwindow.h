#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include <QTimer>

#include "dialogcomport.h"
#include "dialogrecord.h"
#include "dialogdisplayparams.h"
#include "dialogwriteitm.h"

#include "displaytypes.h"

#include "mymodel.h"

#include "mypoolingclass.h"
class QModbusClient;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void connectDevice(DialogCOMPort::Settings inp_settings);
    QModbusDataUnit *constructReadRequest(QModbusDataUnit::RegisterType inp_regtype);
    //QModbusDataUnit *constructWriteRequest(QModbusDataUnit::RegisterType inp_regtype);
    QModbusDataUnit* constructWriteRequest(QString regTypeInpValue, quint16 dataRegValue, unsigned int regNumberInp);

    MyModel myModel;
    QStringList theReadList; //names of register types which might be read.

    void logToTextBox(QString goodMsgForDisplay);

    bool processingPerformed;
    MyPoolingClass *poolingProcessor;
    /*see sect. 2.3 A Clock inside a Table Cell in http://doc.qt.io/qt-5.8/modelview.html */
    QTimer *poolingTimer;
    unsigned int poolingStdTimeout;

    bool buttonMode;

    void addValueToTable(QModbusDataUnit inpUnit, int serv);
    QString formalModbusRegTypeToString(QModbusDataUnit::RegisterType inpRegType);
    QString currentDisplayParameter;

    void sendWriteRequestToDevice(QString regTypeInp, quint16 dataRegValue, unsigned int regNumberInp, unsigned int deviceAddrInp);
public slots:
    void on_actionQueryTimer_triggered();
private:
    Ui::MainWindow *ui;
    QModbusClient *modbusDevice;
    DialogCOMPort::Settings storedSettings;

    QVector <DialogCOMPort::knownCOMports> *AllCOMPorts;
    bool recalculateCOMPorts;

    bool deviceConnected;

    bool requestDelayUsed_internal;
    int requestDelay_internal;

    QModelIndex index; //current selected index
private slots:
    // https://forum.qt.io/topic/31233/how-to-create-a-custom-context-menu-for-qtableview/3
    void customMenuRequested(QPoint pos);
    void writeActionProcessor();
    void removeActionProcessor();

    void openComPortDialog();
    void on_pushButtonRead_clicked(); //unused

    void readReady();
    void on_pushButtonWrite_clicked(); //unused
    void on_pushButtonAdd_clicked();
    void on_pushButton_clicked(); //start scan action

    void on_actionDisplay_triggered();
    void on_actionGenTimer_triggered();

    void timerHit();
    // these slots process calls from qthread
    void externalLogRequest(QString externalLogLine);
    void processPoolingFinished();
    void externalReadRequest(QModbusDataUnit theUnit, unsigned int devAddr);
};

#endif // MAINWINDOW_H
