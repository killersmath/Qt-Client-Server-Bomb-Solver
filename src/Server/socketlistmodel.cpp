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
    return _socketList.count();
}

QVariant SocketListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() < 0 || index.row() > rowCount() || index.column() > 0)
        return QVariant();

    if(role == Qt::DisplayRole){
        const DataInfo& item = _socketList.at(index.row());
        const QPair<QString,quint16>& socketInfo = item.first;
        return QString("%1:%2").arg(socketInfo.first).arg(socketInfo.second);
    }
    else if(role == Qt::TextAlignmentRole){
        return Qt::AlignHCenter + Qt::AlignVCenter;
    }

    return QVariant();
}

void SocketListModel::addSocket(QPair<QString, quint16> socketInfo, SocketThread *socketThread)
{
    for(const DataInfo& data : qAsConst(_socketList)){
        if(data.first == socketInfo)
            return;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _socketList << qMakePair(socketInfo, socketThread);
    endInsertRows();

    emit socketAdded(socketInfo.first, socketInfo.second);
}

void SocketListModel::removeSocket(SocketThread *socketThread)
{
    for(int i=0; i< _socketList.count(); i++){
        const DataInfo& data = _socketList.at(i);
        if(socketThread == data.second){
            beginRemoveRows(QModelIndex(), i, i);
            _socketList.removeAt(i);
            endRemoveRows();

            emit socketRemoved(data.first.first, data.first.second);
            break;
        }
    }
}

void SocketListModel::clear()
{
    if(!rowCount())
        return;

    for(const DataInfo& socketData : qAsConst(_socketList)){
        emit socketData.second->disconnectFromHost();
    }
}

const SocketListModel::DataList SocketListModel::dataList() const
{
    return _socketList;
}
