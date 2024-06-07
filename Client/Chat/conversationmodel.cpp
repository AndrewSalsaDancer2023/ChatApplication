#include "conversationmodel.h"
#include <QDebug>
#include <QJsonObject>
#include "commonutils.h"
//#include "../../Server/src/database/DatabaseTypes.h"

ConversationModel::ConversationModel(QObject *parent)
    : QAbstractListModel(parent)
{
}
/*
void ConversationModel::addSampleData()
{
     PersonInfo one{"Bill Gates", "555 7777"};
     addData(one);
}
*/
//bool ConversationModel::changeSampleData(const QString& name)
//{
 //    removeData(one.name);
/*
    QModelIndex index = createIndexForData(name);
   if(!index.isValid())
       return false;

    QString newName = "Gillian Anderson";
    auto value = QVariant(newName);
    return  setData(index, value);
*/
//    return true;
//}

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

int ConversationModel::numRows() const
{
    if(auto msgs = messages.lock(); msgs)
        return msgs->size();

    return 0;
}

void ConversationModel::addData(const Backend::singleUserMessage& info)
{
    auto sp = messages.lock();
    if(!sp)
        return;

    auto& container = *sp;
    beginInsertRows(QModelIndex(), numRows(), numRows());
    container.push_back(info);
    endInsertRows();
}

void ConversationModel::addMessages(std::shared_ptr<std::vector<Backend::singleUserMessage>> msgs)
{
    removeAllData();
    messages = msgs;

    int nRows = numRows();
    if(nRows <= 0)
        return;

    beginInsertRows(QModelIndex(), 0, nRows-1);
    endInsertRows();
}

int ConversationModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return numRows();
}

bool ConversationModel::notEmpty()
{
    return numRows() > 0;
}

QVariant ConversationModel::data(const QModelIndex & index, int role) const
{
    auto sp = messages.lock();
    if (index.row() < 0 || index.row() >= numRows() || !sp)
        return QVariant();

    const auto &msg = (*sp)[index.row()];
/*
    if (index.row() < 0 || index.row() >= dataItems.count())
        return QVariant();

    const Database::singleUserMessage &info = dataItems[index.row()];
*/
    if (role == UserNameRole)
        return msg.userNickName;

    if (role == DateRole)
        return convertDateTimeToStringFast(msg.timestamp);

    if(role == MessageRole)
        return msg.userMessage;

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

    auto mp = messages.lock();
    if(!mp)
        return;

    emit itemSelected((*mp)[index]);
}

Backend::singleUserMessage  ConversationModel::getItem(int index)
{
    if((index < 0) || (index >= numRows()))
        return Backend::singleUserMessage();

    auto mp = messages.lock();
    return (*mp)[index];
}

void ConversationModel::removeData(const Backend::singleUserMessage& message)
{
    auto sp = messages.lock();
    if (!sp)
        return;

    auto& container = *sp;
    for(int i = 0; i < numRows()/*dataItems.count()*/; ++i)
    {
        const auto& item = (*sp)[i];//dataItems.value(i);
        if((item.timestamp != message.timestamp) ||
           (item.userNickName != message.userNickName))
            continue;
        else
        {
            beginRemoveRows(QModelIndex(), i, i);
            //TODO delete from vector dataItems.removeAt(i);
            const auto new_end (std::remove(std::begin(container), std::end(container), item));
            container.erase(new_end, std::end(container));
            endRemoveRows();
            return;
        }
    }
}

QModelIndex ConversationModel::createIndexForData(const Backend::singleUserMessage& item)
{
    auto sp = messages.lock();
    if (!sp)
        return {};

    for(int i = 0; i < numRows()/*dataItems.count()*/; ++i)
    {
        const auto& data = (*sp)[i];//dataItems.value(i);

        if((item.timestamp != data.timestamp) ||
           (item.userNickName != data.userNickName))
            continue;

        return createIndex(i, 0);
    }
    return {};
}

void ConversationModel::removeAllData()
{
    //if(!m_chats.count())
    if(!numRows())
        return;

//     beginRemoveRows(QModelIndex(), 0, m_chats.count() - 1);
     beginRemoveRows(QModelIndex(), 0, numRows() - 1);
     endRemoveRows();
     messages.reset();
}

/*
bool ConversationModel::containsData(const Database::singleUserMessage& info)
{
    return dataItems.contains(info);
}
*/
