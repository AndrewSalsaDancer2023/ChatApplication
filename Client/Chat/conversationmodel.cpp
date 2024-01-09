#include "conversationmodel.h"
#include <QDebug>
#include <QJsonObject>
#include "commonutils.h"
//#include "../../Server/src/database/DatabaseTypes.h"

ConversationModel::ConversationModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void ConversationModel::addSampleData()
{
/*     PersonInfo one{"Bill Gates", "555 7777"};
     addData(one);*/
}

bool ConversationModel::changeSampleData(const QString& name)
{
 //    removeData(one.name);
/*
    QModelIndex index = createIndexForData(name);
   if(!index.isValid())
       return false;

    QString newName = "Gillian Anderson";
    auto value = QVariant(newName);
    return  setData(index, value);
*/
    return true;
}

Qt::ItemFlags ConversationModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
 }

//http://doc.crossplatform.ru/qt/4.3.2/model-view-creating-models.html
//https://www.youtube.com/watch?v=-KJ2HmvW8CY&t=268s
bool ConversationModel::setData(const QModelIndex &index, const QVariant &value, int role)
 {
 /*    if (index.isValid() && role == Qt::EditRole) {

         Database::singleUserMessage &info = dataItems[index.row()];
         info = QString(value.toString()).toStdString();

         emit dataChanged(index, index);
         return true;
     }*/
     return false;
 }

void ConversationModel::addData(const Database::singleUserMessage& info)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    dataItems << info;
    endInsertRows();
}

int ConversationModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return dataItems.count();
}

QVariant ConversationModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= dataItems.count())
        return QVariant();

    const Database::singleUserMessage &info = dataItems[index.row()];

    if (role == UserNameRole)
        return QString::fromStdString(info.userNickName);

    if (role == DateRole)
        return convertDateTimeToStringFast(info.timestamp);

    if(role == MessageRole)
        return QString::fromStdString(info.userMessage);

    return QVariant();
}

QHash<int, QByteArray> ConversationModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[UserNameRole] = "user";
    roles[DateRole] = "date";
    roles[MessageRole] = "message";

    return roles;
}

void  ConversationModel::slotSelect(int index)
{
    qDebug() << "Selected item:" << index;
    emit itemSelected(dataItems[index]);
}

Database::singleUserMessage  ConversationModel::getItem(int index)
{
    if((index < 0) || (index >= dataItems.count()))
        return {};

    return dataItems[index];
}

void ConversationModel::removeData(const Database::singleUserMessage& message)
{
    for(int i = 0; i < dataItems.count(); ++i)
    {
        const auto& item = dataItems.value(i);
        if((item.timestamp != message.timestamp) ||
           (item.userNickName != message.userNickName))
            continue;
        else
        {
            beginRemoveRows(QModelIndex(), i, i);
            dataItems.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

QModelIndex ConversationModel::createIndexForData(const Database::singleUserMessage& item)
{
    for(int i = 0; i < dataItems.count(); ++i)
    {
        const auto& data = dataItems.value(i);

        if((item.timestamp != data.timestamp) ||
           (item.userNickName != data.userNickName))
            continue;

        return createIndex(i, 0);
    }
    return {};
}
/*
bool ConversationModel::containsData(const Database::singleUserMessage& info)
{
    return dataItems.contains(info);
}
*/
