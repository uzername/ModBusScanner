#ifndef DIALOGWRITEITM_H
#define DIALOGWRITEITM_H

#include <QDialog>
#include <QAbstractButton>
//this one is quite similar to dialogrecord
namespace Ui {
class DialogWriteItm;
}

class DialogWriteItm : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWriteItm(QWidget *parent = 0);
    ~DialogWriteItm();

    bool acceptButtonClicked;
    bool cancelButtonClicked;

    unsigned int deviceAddress;
    QString dataStructType;
    unsigned int dataStructAddress;

    unsigned int dataValue;

    void defineSettingsFromFields();
    void  dataEntered();
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
private:
    Ui::DialogWriteItm *ui;
};

#endif // DIALOGWRITEITM_H
