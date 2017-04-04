#include "dialogdisplayparams.h"
#include "ui_dialogdisplayparams.h"

DialogDisplayParams::DialogDisplayParams(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDisplayParams)
{
    ui->setupUi(this);

    QStringList theReadList2;
    theReadList2 =  QStringList();
    theReadList2<<"UnsignedDec"<<"SignedDec"<<"Hexadecimal"<<"Float";
    this->ui->comboBox->addItems(theReadList2);
}

DialogDisplayParams::~DialogDisplayParams()
{
    delete ui;
}

void DialogDisplayParams::setDisplayParam(QString inpParamValue)  {
    returnedParam = inpParamValue;
    ui->comboBox->setCurrentText(inpParamValue);
}

void DialogDisplayParams::on_buttonBox_clicked(QAbstractButton *button)
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

void DialogDisplayParams::defineSettingsFromFields() {
    this->returnedParam = this->ui->comboBox->currentText();
}
