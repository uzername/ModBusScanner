#include "dialogwriteitm.h"
#include "ui_dialogwriteitm.h"

DialogWriteItm::DialogWriteItm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWriteItm)
{
    ui->setupUi(this);

    QStringList theReadList;
    theReadList =  QStringList();
    theReadList<<"CoilFlag"<<"Discrette Input"<<"HoldingRegister"<<"InputRegister";
    this->ui->ComboBoxType->addItems(theReadList);
    ui->ComboBoxType->setCurrentIndex(2);
}

DialogWriteItm::~DialogWriteItm()
{
    delete ui;
}
//retrieve data from fields
void DialogWriteItm::defineSettingsFromFields()  {
    this->dataStructType = this->ui->ComboBoxType->currentText();
    this->deviceAddress = this->ui->SpinBoxAddr->value();
    this->dataStructAddress = this->ui->SpinBoxNumber->value();
    this->dataValue = this->ui->SpinBoxData->value();
}
//set data to iface fields
void DialogWriteItm::dataEntered()  {
    this->ui->ComboBoxType->setCurrentText(this->dataStructType);
    this->ui->SpinBoxAddr->setValue(this->deviceAddress);
    this->ui->SpinBoxNumber->setValue(this->dataStructAddress);
    this->ui->SpinBoxData->setValue(this->dataValue);
}

void DialogWriteItm::on_buttonBox_clicked(QAbstractButton *button)
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
