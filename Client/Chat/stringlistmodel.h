#pragma once
#include <QAbstractListModel>
//#include <QStringList>
#include <string>
#include <set>
#include <functional>

class StringListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ChatListModelRoles {
        NameRole = Qt::UserRole + 1
//        NumberRole
     //   SizeRole
    };

    StringListModel(QObject *parent = 0);
    void addData(const QSet<QString>& data);
    void addData(const QString& info);
    bool containsData(const QString& info);
//    Q_INVOKABLE void addSampleData();
//    Q_INVOKABLE bool changeSampleData(const QString& name);
    Q_INVOKABLE bool notEmpty() { return m_chats.count() > 0; }
    Q_INVOKABLE QString  getItem(int index);
    /////////////////////////////////////////////////////////////////////////////
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    //////////////////////////////////////////////////////////////////////////////
    Q_INVOKABLE void removeData(const QString& name);
    //[virtual] bool QAbstractItemModel::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
    //The dataChanged()
    //[override virtual] Qt::ItemFlags QAbstractListModel::flags(const QModelIndex &index) const
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex createIndexForData(const QString& name);

signals:
   void itemSelected(QString item);

public slots:
    void slotSelect(int index);
    void slotSelect(const QString& title);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<QString> m_chats;
};
//https://stackoverflow.com/questions/11246022/how-to-get-qstring-from-qlistview-selected-item-in-qt
