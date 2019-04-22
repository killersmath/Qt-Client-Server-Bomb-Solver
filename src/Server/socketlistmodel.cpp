#include "socketlistmodel.h"
#include "socketthread.h"

#include <QHostAddress>

SocketListModel::SocketListModel(QObject *parent) : QAbstractListModel(parent)
{

}

SocketListModel::~SocketListModel()
{
    clear();
}

int SocketListModel::rowCount(const QModelIndex &/*parent*/) const
{
    return _dataList.count();
}

QVariant SocketListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() < 0 || index.row() > rowCount())
        return QVariant();

    if(role == Qt::DisplayRole){
        const DataInfo item = _dataList.at(index.row());
        const QPair<QString,quint16> socketInfo = item.first;
        return QString("%1:%2").arg(socketInfo.first).arg(socketInfo.second);
    }
    else if(role == Qt::TextAlignmentRole){
        return Qt::AlignHCenter + Qt::AlignVCenter;
    }

    return QVariant();
}

void SocketListModel::addSocket(QPair<QString, quint16> socketInfo, SocketThread *socketThread)
{
    for(DataInfo data : _dataList){
        if(data.first == socketInfo)
            return;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _dataList << qMakePair(socketInfo, socketThread);
    endInsertRows();

    emit socketAdded(socketInfo.first, socketInfo.second);
}

void SocketListModel::removeSocket(SocketThread *socketThread)
{
    if(!rowCount())
        return;

    for(int i=0; i < rowCount(); i++){
        const DataInfo data = _dataList.at(i);
        if(socketThread == data.second){
            beginRemoveRows(QModelIndex(), i, i);
            _dataList.removeAt(i);
            beginRemoveRows(QModelIndex(), i, i);

            emit socketRemoved(data.first.first, data.first.second);

            break;
        }
    }
}

void SocketListModel::clear()
{
    for(DataInfo dataInfo : qAsConst(_dataList))
        emit dataInfo.second->disconnectFromHost();
}

const SocketListModel::DataList SocketListModel::dataList() const
{
    return _dataList;
}
