#include "mymodel.h"
#include <QException>

MyModel::MyModel(QObject *parent)
    :QAbstractTableModel(parent), columnHeaders(), dataItems()
{
}

int MyModel::rowCount(const QModelIndex & /*parent*/) const
{
   //return 2;
    return dataItems.size();
}

int MyModel::columnCount(const QModelIndex & /*parent*/) const
{
    //return 3;
    return columnHeaders.size();
}

QVariant MyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (index.column()) {
            case 0: {
                return QString("%1").arg(this->dataItems.at(index.row()).deviceAddress,10);
                break;}
            case 1: {
                return QString("%1").arg(this->dataItems.at(index.row()).dataStructType );
                break;
            }
            case 2: {
                return QString("%1").arg(this->dataItems.at(index.row()).dataStructAddress,10);
                break;
            }
                case 3: {
                    return QString("<hex> %1").arg(this->dataItems.at(index.row()).dataStructData, 16);
                    break;
                }
            default: {
                return "...";
            }
        }
       //return QString("Row%1, Column%2").arg(index.row() + 1).arg(index.column() +1);
    }
    return QVariant();
}

QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    /* section<this->columnCount() */
    //qDebug(section);
    if (role == Qt::DisplayRole)  {
        if ( orientation == Qt::Horizontal ) {
            //qDebug(columnHeaders.at(section).toStdString().c_str());
            return columnHeaders.at(section);

        }
    }
    return QVariant();
}
/*
 //this routine is not good
bool MyModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    // http://www.cyberforum.ru/qt/thread1088426.html
    // http://www.cyberforum.ru/post5737822.html
    //there's nothing good to have row index larger than model's size. Well, if it's equal to model size, then append data to model
    if (index.row()>dataItems.size()) {
        QException badAlloc;
        throw badAlloc;
    }
    if (index.row()==dataItems.size()) {
        //need to create item
        rowDataItm theItem;
        theItem.dataStructAddress=0;
        theItem.dataStructData = 0;
        theItem.dataStructType = "";
        theItem.deviceAddress = 0;
        dataItems.append(theItem);
    }

}
*/
void MyModel::brandNewAppendData(unsigned int deviceAddrInp, QString structTypeInp, unsigned int structAddrInp) {
    int newRow = dataItems.count();
    beginInsertRows(QModelIndex(), newRow, newRow);
    rowDataItm theItem;
    theItem.dataStructAddress= structAddrInp;
    theItem.dataStructData = 0;
    theItem.dataStructType = structTypeInp;
    theItem.deviceAddress = deviceAddrInp;
    dataItems.append(theItem);
    endInsertRows();
}

