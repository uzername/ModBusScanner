#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>

#include "dialogcomport.h"
#include "dialogrecord.h"

#include "mymodel.h"
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
};

#endif // MAINWINDOW_H
