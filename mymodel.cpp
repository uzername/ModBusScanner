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
                case 3: { //here is the place where the register data are being transformed before display
                    QString finalStringValue="...";
                    if (this->currentNumberDisplaySetting == "Hexadecimal") {
                        finalStringValue = QString("<hex> %1").arg(this->dataItems.at(index.row()).dataStructData, 0, 16);
                    } else {
                        if (this->currentNumberDisplaySetting == "UnsignedDec") {
                            finalStringValue = QString("<dec> %1").arg(this->dataItems.at(index.row()).dataStructData, 0, 10);
                        } else {
                            if (this->currentNumberDisplaySetting == "SignedDec") {
                                union {
                                    qint16 signedint;
                                    quint16 unsignedint;
                                } un;
                                un.unsignedint = this->dataItems.at(index.row()).dataStructData;
                                finalStringValue = QString("<sgndec> %1").arg(un.signedint, 0, 10);
                            } else {
                                if (this->currentNumberDisplaySetting == "Float") {
                                    //this might be dangerous and not recommended. float is 4 bytes, our vaulue is 2 bytes. C would not support that.
                                    //proof: http://stackoverflow.com/questions/11639947/is-type-punning-through-a-union-unspecified-in-c99-and-has-it-become-specified
                                    union { //these are 4 bytes
                                        int classicalint;
                                        float classicalfloat;
                                    } un2;
                                    un2.classicalint = this->dataItems.at(index.row()).dataStructData;
                                    finalStringValue = QString("<float> %1").arg(un2.classicalfloat );
                                }
                            }
                        }
                    }
                    return finalStringValue;
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
/*we want to set only quint16 value of data*/
bool MyModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    //qDebug ("SETDATA in myModel");
    if (!index.isValid() ||  index.row() >= rowCount() || index.column() >= columnCount() )
        return false;
    int rowIndex = index.row();
    int columnIndex = index.column();
    rowDataItm theValue = this->dataItems.at(rowIndex);
    theValue.dataStructData = value.toInt();

    this->dataItems.replace(rowIndex, theValue);

    //updating view
    //the best way (see question): http://stackoverflow.com/questions/12893904/automatically-refreshing-a-qtableview-when-data-changed
    //oth ways: http://www.qtcentre.org/threads/3145-QTableView-How-to-refresh-the-view-after-an-update-of-the-model
      QModelIndex topLeft = this->index(0, 0);
      QModelIndex bottomRight = this->index(rowCount() - 1, columnCount() - 1);
      emit dataChanged(topLeft, bottomRight);
    return true;
}

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

