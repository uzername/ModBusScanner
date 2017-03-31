#ifndef DIALOGRECORD_H
#define DIALOGRECORD_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class DialogRecord;
}

class DialogRecord : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRecord(QWidget *parent = 0);
    ~DialogRecord();
    bool acceptButtonClicked;
    bool cancelButtonClicked;

    unsigned int deviceAddress;
    QString dataStructType;
    unsigned int dataStructAddress;

    void defineSettingsFromFields();


private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::DialogRecord *ui;
};

#endif // DIALOGRECORD_H
