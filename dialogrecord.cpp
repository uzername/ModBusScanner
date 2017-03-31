#include "dialogrecord.h"
#include "ui_dialogrecord.h"

DialogRecord::DialogRecord(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRecord)
{
    ui->setupUi(this);

    QStringList theReadList;
    theReadList =  QStringList();
    theReadList<<"CoilFlag"<<"Discrette Input"<<"HoldingRegister"<<"InputRegister";
    this->ui->comboBoxType->addItems(theReadList);
    ui->comboBoxType->setCurrentIndex(2);

}

DialogRecord::~DialogRecord()
{
    delete ui;
}

void DialogRecord::defineSettingsFromFields()
{
    this->dataStructType = this->ui->comboBoxType->currentText();
    this->dataStructAddress = this->ui->lineEditRegAddr->text().toInt();
    this->deviceAddress = this->ui->spinBoxDeviceAddr->value();
}

void DialogRecord::on_buttonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::ButtonRole theRole = this->ui->buttonBox->QDialogButtonBox::buttonRole(button);
    switch (theRole) {
        case QDialogButtonBox::ApplyRole:
        case QDialogButtonBox::AcceptRole: {
            this->acceptButtonClicked = true;
            this->cancelButtonClicked = false;
            //prepare data for transmission in calling subroutine
            this->defineSettingsFromFields();
            //close may sometimes destroy the widget
            this->hide();
            break; }
        case QDialogButtonBox::RejectRole: {
            this->acceptButtonClicked = false;
            this->cancelButtonClicked = true;
            //close may sometimes destroy the widget. reject role closes dialog by default
            //this->hide();
            break; }
        default:
            break;
    }
}
