#include "participantmodel.h"
#include <QDebug>
#include <QJsonObject>
#include "../../Server/src/database/DatabaseTypes.h"

ParticipantModel::ParticipantModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Qt::ItemFlags ParticipantModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
 }

//http://doc.crossplatform.ru/qt/4.3.2/model-view-creating-models.html
//https://www.youtube.com/watch?v=-KJ2HmvW8CY&t=268s
bool ParticipantModel::setData(const QModelIndex &index, const QVariant &value, int role)
 {
/*     if (index.isValid() && role == Qt::EditRole) {

         Participant &info = m_chats[index.row()];
         info = QString(value.toString()).toStdString();

         emit dataChanged(index, index);
         return true;
     }*/
     return false;
 }

void ParticipantModel::addData(const Participant& info)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_chats << info;
    endInsertRows();
}

int ParticipantModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_chats.count();
}

QVariant ParticipantModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_chats.count())
        return QVariant();

    const auto &part = m_chats[index.row()];
    switch(role)
    {
        case NameRole:
            return QString::fromStdString(part.name);
        case SurnameRole:
            return QString::fromStdString(part.surname);
        case NickNameRole:
            return QString::fromStdString(part.nickname);
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> ParticipantModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[SurnameRole] = "surname";
    roles[NickNameRole] = "nickname";

    return roles;
}

void  ParticipantModel::slotSelect(int index)
{
    qDebug() << "Selected item:" << index;
    emit itemSelected(m_chats[index]);
}
/*
QString  ParticipantModel::getItem(int index)
{
    if((index < 0) || (index >= m_chats.count()))
        return {};

    const auto& part = m_chats[index];
    return QString::fromStdString(part.name + ':' + part.surname + ':' + part.nickname);
}
*/

const Participant&  ParticipantModel::getItem(int index)
{
    return m_chats[index];
}
/*
void ParticipantModel::removeData(const Participant& part)
{
    for(int i = 0; i < m_chats.count(); ++i)
    {
        if(m_chats.value(i).nickname != part.nickname)
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        m_chats.removeAt(i);
        endRemoveRows();
        return;
    }
}
*/
void ParticipantModel::removeParticipant(const QString& nickName)
{
    for(int i = 0; i < m_chats.count(); ++i)
    {
        const auto& part = m_chats.value(i);
        if(part.nickname != nickName.toStdString())
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        m_chats.removeAt(i);
        endRemoveRows();
        return;
    }
}

std::optional<Participant>  ParticipantModel::getParticipant(const QString& nickName)
{
    for(int i = 0; i < m_chats.count(); ++i)
    {
        const auto& part = m_chats.value(i);
        if(part.nickname != nickName.toStdString())
            continue;

        return m_chats.value(i);
    }

    return {};
}

void ParticipantModel::removeAllData()
{
    if(!m_chats.count())
        return;

     beginRemoveRows(QModelIndex(), 0, m_chats.count() - 1);
     endRemoveRows();
     m_chats.clear();
}

QModelIndex ParticipantModel::createIndexForData(const Participant& part)
{
    for(int i = 0; i < m_chats.count(); ++i)
    {
        if(m_chats.value(i).nickname != part.nickname)
            continue;

        return createIndex(i, 0);
    }
    return {};
}

bool ParticipantModel::containsData(const Participant& info)
{
    return m_chats.contains(info);
}
