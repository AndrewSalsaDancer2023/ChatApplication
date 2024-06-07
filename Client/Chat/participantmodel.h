#pragma once
#include <QAbstractListModel>
#include <memory>
#include <vector>
#include <functional>

namespace Backend {
    struct Participant;
}
using Backend::Participant;

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

    void setParticipants(std::shared_ptr<std::vector<Backend::Participant>> part);
    void addParticipant(const Participant& info);
    bool containsData(const Participant& info);
//    Q_INVOKABLE bool notEmpty() { return m_chats.count() > 0; }
//    Q_INVOKABLE QString  getItem(int index);
    const Participant&  getItem(int index);
    /////////////////////////////////////////////////////////////////////////////
    int numRows() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    //////////////////////////////////////////////////////////////////////////////
    Q_INVOKABLE void removeParticipant(const QString& nickName);
    std::optional<Participant>  getParticipant(const QString& nickName);

    void removeAllData();
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex createIndexForData(const Participant& part);

signals:
   void itemSelected(Participant item);

public slots:
    void slotSelect(int index);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    std::weak_ptr<std::vector<Backend::Participant>> participants;
    int rowsNumber{};
};

