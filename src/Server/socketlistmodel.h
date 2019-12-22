#ifndef IPLISTMODEL_H
#define IPLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QPair>

class SocketThread;

class SocketListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    typedef QPair<QPair<QString,quint16>,SocketThread*> DataInfo;
    typedef QList<DataInfo> DataList;

    SocketListModel(QObject *parent);
    ~SocketListModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    //int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addSocket(QPair<QString,quint16>, SocketThread *socket);
    void removeSocket(SocketThread *socketThread);
    void clear();

    const DataList dataList() const;

signals:
    void socketAdded(const QString& host, quint16 port);
    void socketRemoved(const QString& host, quint16 port);

private:
    DataList _socketList;
};

#endif // IPLISTMODEL_H
