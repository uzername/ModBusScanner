#ifndef MYMODEL_H
#define MYMODEL_H
#include <QAbstractTableModel>

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    struct rowDataItm{
        unsigned int deviceAddress;
        QString dataStructType;
        unsigned int dataStructAddress; /*maximum address is 65536.*/
        quint16 dataStructData;
    };

    MyModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;

    QVector<QString> columnHeaders;
    QVector<rowDataItm> dataItems;
    void brandNewAppendData(unsigned int deviceAddrInp, QString structTypeInp, unsigned int structAddrInp);
    /**
     * @brief rowDataItmToString
     * @return String representation of rowDataItm
     */
    static QString rowDataItmToString(rowDataItm devicerequest) {
        QString lineOutput = QString("device: %1 d; reg.type: %2 ; reg.address: %3 d; current value: %4 d").arg(devicerequest.deviceAddress, 10).arg(devicerequest.dataStructType).arg(devicerequest.dataStructAddress, 10).arg(devicerequest.dataStructData, 10);
        return lineOutput;
    }
};

#endif // MYMODEL_H
