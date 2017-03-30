#include "dialogrecord.h"
#include "ui_dialogrecord.h"

DialogRecord::DialogRecord(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRecord)
{
    ui->setupUi(this);
}

DialogRecord::~DialogRecord()
{
    delete ui;
}
