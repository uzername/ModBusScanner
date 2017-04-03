#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include <QTimer>

#include "dialogcomport.h"
#include "dialogrecord.h"

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
    QModbusDataUnit *constructWriteRequest(QModbusDataUnit::RegisterType inp_regtype);

    MyModel myModel;
    QStringList theReadList; //names of register types which might be read.

    void logToTextBox(QString goodMsgForDisplay);

    bool processingPerformed;
    MyPoolingClass *poolingProcessor;
    /*see sect. 2.3 A Clock inside a Table Cell in http://doc.qt.io/qt-5.8/modelview.html */
    QTimer *poolingTimer;
    unsigned int poolingStdTimeout;


private:
    Ui::MainWindow *ui;
    QModbusClient *modbusDevice;
    DialogCOMPort::Settings storedSettings;

    QVector <DialogCOMPort::knownCOMports> *AllCOMPorts;
    bool recalculateCOMPorts;

    bool deviceConnected;

private slots:
    void openComPortDialog();
    void on_pushButtonRead_clicked();

    void readReady();
    void on_pushButtonWrite_clicked();
    void on_pushButtonAdd_clicked();
    void on_pushButton_clicked();
    void timerHit();
    // these slots process calls from qthread
    void externalLogRequest(QString externalLogLine);
    void processPoolingFinished();
};

#endif // MAINWINDOW_H
