#pragma once
#include <QAbstractListModel>
#include <QStringList>
#include <string>
#include <functional>
#include <memory>
#include "../../Server/src/database/DatabaseTypes.h"

class ConversationModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ConversationModelRoles {
        UserNameRole = Qt::UserRole + 1,
        DateRole,
        MessageRole
    };

    ConversationModel(QObject *parent = 0);

    void addData(const Database::singleUserMessage& info);
    void addMessages(std::shared_ptr<std::vector<Database::singleUserMessage>> msgs);
//    bool containsData(const Database::singleUserMessage& info);
    Q_INVOKABLE void addSampleData();
    Q_INVOKABLE bool changeSampleData(const QString& name);
    Q_INVOKABLE bool notEmpty();
    Q_INVOKABLE Database::singleUserMessage getItem(int index);
    /////////////////////////////////////////////////////////////////////////////
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    //////////////////////////////////////////////////////////////////////////////
    Q_INVOKABLE void removeData(const Database::singleUserMessage& message);
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex createIndexForData(const Database::singleUserMessage& item);
    void removeAllData();
signals:
   void itemSelected(Database::singleUserMessage item);

public slots:
    void slotSelect(int index);
protected:
    QHash<int, QByteArray> roleNames() const;
private:

private:
    int numRows() const;
//    QList<Database::singleUserMessage> dataItems;
    std::weak_ptr<std::vector<Database::singleUserMessage>> messages;
};
