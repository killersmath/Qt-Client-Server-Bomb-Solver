#include "socketlistmodel.h"
#include "socketthread.h"

#include <QHostAddress>

SocketListModel::SocketListModel(QObject *parent) : QAbstractListModel(parent)
{

}

int SocketListModel::rowCount(const QModelIndex &/*parent*/) const
{
    return _socketList.count();
}

QVariant SocketListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() < 0 || index.row() > rowCount() || index.column() > 0)
        return QVariant();

    if(role == Qt::DisplayRole){
        const DataInfo item = _socketList.at(index.row());
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
    for(DataInfo data : _socketList){
        if(data.first == socketInfo)
            return;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _socketList << qMakePair(socketInfo, socketThread);
    endInsertRows();

    emit socketAdd(socketInfo.first, socketInfo.second);
}

void SocketListModel::removeSocket(SocketThread *socketThread)
{
    for(int i=0; i< _socketList.count(); i++){
        const DataInfo data = _socketList.at(i);
        if(socketThread == data.second){
            beginRemoveRows(QModelIndex(), i, i);
            _socketList.removeAt(i);
            beginRemoveRows(QModelIndex(), i, i);

            emit socketRem(data.first.first, data.first.second);
            break;
        }
    }
}

void SocketListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());

    for(DataInfo info : qAsConst(_socketList)){
        emit info.second->disconnectFromHost();
    }

    _socketList.clear();
    beginRemoveRows(QModelIndex(), 0, rowCount());
}

const SocketListModel::DataList SocketListModel::getData() const
{
    return _socketList;
}
