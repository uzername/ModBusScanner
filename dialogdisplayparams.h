#ifndef DIALOGDISPLAYPARAMS_H
#define DIALOGDISPLAYPARAMS_H

#include <QDialog>
#include <QAbstractButton>
#include "displaytypes.h"
namespace Ui {
class DialogDisplayParams;
}

class DialogDisplayParams : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDisplayParams(QWidget *parent = 0);
    ~DialogDisplayParams();
    void setDisplayParam(QString inpParamValue);
    QString returnedParam;
    bool acceptButtonClicked;
    bool cancelButtonClicked;
    void defineSettingsFromFields();
public slots:
    void on_buttonBox_clicked(QAbstractButton *button);
private:
    Ui::DialogDisplayParams *ui;
};

#endif // DIALOGDISPLAYPARAMS_H
