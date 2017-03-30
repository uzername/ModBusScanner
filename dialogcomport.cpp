#include "dialogcomport.h"
#include "ui_dialogcomport.h"

#include <QtSerialPort/QSerialPortInfo>

DialogCOMPort::DialogCOMPort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCOMPort)
{
    ui->setupUi(this);
     // perform additional setup here...
     //top fancyness:program should remember com port settings and load these on start
     //if settings file not available then fill the dialog fields with some default settings
    this->fillDataOnCreation();
}
/**
 * @brief DialogCOMPort::fillDataOnCreation
 * upon creation the comboboxes are filled with values. These values go to nowhere by themselves when user exits the dialog.
 * They might be worth saving if they mean something...
 */
void DialogCOMPort::fillDataOnCreation() {
    //baud rate. here are most common baud rates in enum. only enum values allowed
    this->ui->comboBox_2->addItem("1200");
    this->ui->comboBox_2->addItem("2400");
    this->ui->comboBox_2->addItem("4800");
    this->ui->comboBox_2->addItem("9600");
    this->ui->comboBox_2->addItem("19200");
    this->ui->comboBox_2->addItem("38400");
    this->ui->comboBox_2->addItem("57600");
    this->ui->comboBox_2->addItem("115200");
    //data bits
    this->ui->comboBox_3->addItem("5");
    this->ui->comboBox_3->addItem("6");
    this->ui->comboBox_3->addItem("7");
    this->ui->comboBox_3->addItem("8");
    //parity
    this->ui->comboBox_4->addItem("NoParity(0)");
    this->ui->comboBox_4->addItem("EvenParity(1)");
    this->ui->comboBox_4->addItem("OddParity(2)");
    this->ui->comboBox_4->addItem("SpaceParity(3)");
    this->ui->comboBox_4->addItem("MarkParity(4)");
    //stop bits
    this->ui->comboBox_5->addItem("OneStop(1)");
    this->ui->comboBox_5->addItem("OneAndHalfStop(3)");
    this->ui->comboBox_5->addItem("TwoStop(2)");
    //adding existing com ports to the list; enumerate all comports.
    /*
    const auto infos = QSerialPortInfo::availablePorts();
    int itmNo=0;
    for (const QSerialPortInfo &info : infos) {
        QString portTip = info.systemLocation() + ";"
                + QObject::tr("Descr:") + info.description() + ";"
                + QObject::tr("PrId:") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + ";"
                + QObject::tr("Busy:") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No"));
        this->ui->comboBox->addItem(info.portName());
        this->ui->comboBox->setItemData(itmNo,portTip, Qt::ToolTipRole);
        itmNo++;
    }
    */
}

DialogCOMPort::~DialogCOMPort()
{
    delete ui;
}

//
/**
 * @brief DialogCOMPort::on_buttonBox_accepted
 * the user clicked 'accept'. Should save all the parameters and get ready to pass them!
 */
void DialogCOMPort::on_buttonBox_accepted() {
    //moved to on_buttonBox_clicked
}

void DialogCOMPort::on_buttonBox_rejected()
{
    //moved to on_buttonBox_clicked
}


void DialogCOMPort::on_buttonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::ButtonRole theRole = this->ui->buttonBox->QDialogButtonBox::buttonRole(button);
    switch (theRole) {
    case QDialogButtonBox::ApplyRole:
    case QDialogButtonBox::AcceptRole:
        this->acceptButtonClicked = true;
        this->cancelButtonClicked = false;
        //fill structure
        this->defineSettingsFromFields();
        //close may sometimes destroy the widget
        this->hide();
        break;
    case QDialogButtonBox::RejectRole:
        this->acceptButtonClicked = false;
        this->cancelButtonClicked = true;
        //close may sometimes destroy the widget. reject role closes dialog by default
        //this->hide();
        break;
    default:
        break;
    }
}

DialogCOMPort::Settings DialogCOMPort::getSettings() {
    return m_settings;
}

void DialogCOMPort::defineSettingsFromFields() {
    int baudRateSelected = this->ui->comboBox_2->currentIndex();
    //data bits
    int dataBitSelected = this->ui->comboBox_3->currentIndex();
    //parity bits
    int parityBitSelected = this->ui->comboBox_4->currentIndex();
    //stop bits
    int stopBitSelected = this->ui->comboBox_5->currentIndex();
    switch (baudRateSelected) {
        case 0: m_settings.baud = QSerialPort::Baud1200; break;
        case 1: m_settings.baud = QSerialPort::Baud2400; break;
        case 2: m_settings.baud = QSerialPort::Baud4800; break;
        case 3: m_settings.baud = QSerialPort::Baud9600; break;
        case 4: m_settings.baud = QSerialPort::Baud19200; break;
        case 5: m_settings.baud = QSerialPort::Baud38400; break;
        case 6: m_settings.baud = QSerialPort::Baud57600; break;
        case 7: m_settings.baud = QSerialPort::Baud115200; break;
    default: break;
    }
    switch (dataBitSelected) {
        case 0: m_settings.dataBits = QSerialPort::Data5; break;
        case 1: m_settings.dataBits = QSerialPort::Data6; break;
        case 2: m_settings.dataBits = QSerialPort::Data7; break;
        case 3: m_settings.dataBits = QSerialPort::Data8; break;
    default:
        break;
    }
    switch (parityBitSelected) {
        case 0: m_settings.parity = QSerialPort::NoParity; break;
        case 1: m_settings.parity = QSerialPort::EvenParity; break;
        case 2: m_settings.parity = QSerialPort::OddParity; break;
        case 3: m_settings.parity = QSerialPort::SpaceParity; break;
        case 4: m_settings.parity = QSerialPort::MarkParity; break;
    default:
        break;
    }
    switch (stopBitSelected) {
        case 0: m_settings.stopBits = QSerialPort::OneStop; break;
        case 1: m_settings.stopBits = QSerialPort::OneAndHalfStop; break;
        case 2: m_settings.stopBits = QSerialPort::TwoStop; break;
    default:
        break;
    }
    //port
    m_settings.portAddr = this->ui->comboBox->currentText();
}

void DialogCOMPort::setSettings(DialogCOMPort::Settings inp_settings) {
    if (inp_settings.portAddr != "") {
        this->ui->comboBox->setCurrentText(inp_settings.portAddr);
    }
    switch (inp_settings.baud) {
        case QSerialPort::Baud1200: this->ui->comboBox_2->setCurrentIndex(0); break;
        case QSerialPort::Baud2400: this->ui->comboBox_2->setCurrentIndex(1); break;
        case QSerialPort::Baud4800: this->ui->comboBox_2->setCurrentIndex(2); break;
        case QSerialPort::Baud9600: this->ui->comboBox_2->setCurrentIndex(3); break;
        case QSerialPort::Baud19200: this->ui->comboBox_2->setCurrentIndex(4); break;
        case QSerialPort::Baud38400: this->ui->comboBox_2->setCurrentIndex(5); break;
        case QSerialPort::Baud57600: this->ui->comboBox_2->setCurrentIndex(6); break;
        case QSerialPort::Baud115200: this->ui->comboBox_2->setCurrentIndex(7); break;
    default: break;
    }
    switch (inp_settings.dataBits) {
        case QSerialPort::Data5: this->ui->comboBox_3->setCurrentIndex(0); break;
        case QSerialPort::Data6: this->ui->comboBox_3->setCurrentIndex(1); break;
        case QSerialPort::Data7: this->ui->comboBox_3->setCurrentIndex(2); break;
        case QSerialPort::Data8: this->ui->comboBox_3->setCurrentIndex(3); break;
    default:
        break;
    }
    switch (inp_settings.parity) {
        case QSerialPort::NoParity: this->ui->comboBox_4->setCurrentIndex(0); break;
        case QSerialPort::EvenParity: this->ui->comboBox_4->setCurrentIndex(1); break;
        case QSerialPort::OddParity: this->ui->comboBox_4->setCurrentIndex(2);; break;
        case QSerialPort::SpaceParity: this->ui->comboBox_4->setCurrentIndex(3);; break;
        case QSerialPort::MarkParity: this->ui->comboBox_4->setCurrentIndex(4); break;
    default:
        break;
    }
    switch (inp_settings.stopBits) {
        case QSerialPort::OneStop: this->ui->comboBox_5->setCurrentIndex(0); break;
        case QSerialPort::OneAndHalfStop: this->ui->comboBox_5->setCurrentIndex(1); break;
        case QSerialPort::TwoStop: this->ui->comboBox_5->setCurrentIndex(2); break;
    default:
        break;
    }
}

void DialogCOMPort::setComPorts(bool inp_recalculate, QVector<knownCOMports> *inp_allCOMports) {
    if (inp_recalculate == true) {
        //inp_allCOMports = new QVector <DialogCOMPort::knownCOMports>();
        const auto infos = QSerialPortInfo::availablePorts();
        int itmNo=0;
        for (const QSerialPortInfo &info : infos) {
            QString portTip = info.systemLocation() + ";"
                    + QObject::tr("Descr:") + info.description() + ";"
                    + QObject::tr("PrId:") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + ";"
                    + QObject::tr("Busy:") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No"));
            DialogCOMPort::knownCOMports singlePortData;
            singlePortData.portName = info.portName();
            singlePortData.portInfo = portTip;
            inp_allCOMports->append(singlePortData);
            itmNo++;
        }
    }

    for (int itmNo=0; itmNo<inp_allCOMports->length(); itmNo++) {
        this->ui->comboBox->addItem(inp_allCOMports->at(itmNo).portName);
        this->ui->comboBox->setItemData(itmNo,inp_allCOMports->at(itmNo).portInfo, Qt::ToolTipRole);
    }

}
