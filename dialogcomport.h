#ifndef DIALOGCOMPORT_H
#define DIALOGCOMPORT_H

#include <QDialog>
#include <QAbstractButton>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class DialogCOMPort;
}

class DialogCOMPort : public QDialog
{
    Q_OBJECT

public:
    struct knownCOMports {
        QString portName = "";
        QString portInfo = "";
    };
    struct Settings {
        QString portAddr = "";
        QSerialPort::Parity parity = QSerialPort::EvenParity;
        QSerialPort::BaudRate baud = QSerialPort::Baud19200;
        QSerialPort::DataBits dataBits = QSerialPort::Data8;
        QSerialPort::StopBits stopBits = QSerialPort::OneStop;
        int responseTime = 1000;
        int numberOfRetries = 3;
    };
    Settings getSettings();
    void setSettings(Settings inp_settings);
    explicit DialogCOMPort(QWidget *parent = 0);
    ~DialogCOMPort();
    void fillDataOnCreation();
    bool acceptButtonClicked;
    bool cancelButtonClicked;
    void setComPorts(bool inp_recalculate, QVector <DialogCOMPort::knownCOMports> *inp_allCOMports);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    DialogCOMPort::Settings m_settings;
    Ui::DialogCOMPort *ui;
    void defineSettingsFromFields();
};

#endif // DIALOGCOMPORT_H
