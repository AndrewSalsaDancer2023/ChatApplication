#pragma once
#include <QAbstractListModel>
#include <QStringList>
#include <string>
#include <functional>

namespace Database {
    struct Participant;
}
using Database::Participant;

class ParticipantModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ChatListModelRoles {
        NameRole = Qt::UserRole + 1,
        SurnameRole,
        NickNameRole
    };

    ParticipantModel(QObject *parent = 0);

    void addData(const Participant& info);
    bool containsData(const Participant& info);
//    Q_INVOKABLE bool notEmpty() { return m_chats.count() > 0; }
//    Q_INVOKABLE QString  getItem(int index);
    const Participant&  getItem(int index);
    /////////////////////////////////////////////////////////////////////////////
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    //////////////////////////////////////////////////////////////////////////////
    Q_INVOKABLE void removeParticipant(const QString& nickName);
    std::optional<Participant>  getParticipant(const QString& nickName);

    void removeAllData();
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex createIndexForData(const Participant& part);

signals:
   void itemSelected(Participant item);

public slots:
    void slotSelect(int index);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<Participant> m_chats;
};

