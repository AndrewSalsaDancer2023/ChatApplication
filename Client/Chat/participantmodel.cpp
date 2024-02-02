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
//void ParticipantModel::addData(const Participant& info)
void ParticipantModel::addParticipant(const Participant& info)
{
    auto sp = participants.lock();
    if(!sp)
        return;
    auto& container = *sp;
    beginInsertRows(QModelIndex(), numRows(), numRows());

    container.push_back(info);
    endInsertRows();
}


void ParticipantModel::setParticipants(std::shared_ptr<std::vector<Database::Participant>> part)
{
    removeAllData();
    participants = part;
    int nRows = numRows();
    if(nRows > 0)
    {
        beginInsertRows(QModelIndex(), 0, nRows-1);
        endInsertRows();
    }
/*  QModelIndex topLeft = createIndex(0,0);
    QModelIndex bottomRight = createIndex(nRows, 0);
    emit dataChanged(topLeft, bottomRight);*/
}

int ParticipantModel::numRows() const
{
    if(auto parts = participants.lock(); parts)
        return parts->size();

    return 0;
}

int ParticipantModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);

    return numRows();
}

QVariant ParticipantModel::data(const QModelIndex & index, int role) const
{
    auto sp = participants.lock();
    if (index.row() < 0 || index.row() >= numRows() || !sp)
        return QVariant();

    const auto &part = (*sp)[index.row()];
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

    auto sp = participants.lock();
    if(!sp)
        return;
    const auto &part = (*sp)[index];

    emit itemSelected(part);
}

const Participant&  ParticipantModel::getItem(int index)
{
//    return m_chats[index];
    auto sp = participants.lock();
    return (*sp)[index];
}

void ParticipantModel::removeParticipant(const QString& nickName)
{
    auto sp = participants.lock();
    if(!sp)
        return;

    auto& container = *sp;
    for(int i = 0; i < numRows(); ++i)
    {
        const auto& part = container[i];//m_chats.value(i);
        if(part.nickname != nickName.toStdString())
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        const auto new_end (std::remove(std::begin(container), std::end(container), part));
        container.erase(new_end, std::end(container));
//        m_chats.removeAt(i);
        endRemoveRows();
        return;
    }
}

std::optional<Participant>  ParticipantModel::getParticipant(const QString& nickName)
{
    auto sp = participants.lock();
    if(!sp)
        return {};

    for(int i = 0; i < numRows(); ++i)
    {
        const auto& part = (*sp)[i];
        if(part.nickname != nickName.toStdString())
            continue;

        return (*sp)[i];
    }

    return {};
}

void ParticipantModel::removeAllData()
{
    //if(!m_chats.count())
    if(!numRows())
        return;

//     beginRemoveRows(QModelIndex(), 0, m_chats.count() - 1);
     beginRemoveRows(QModelIndex(), 0, numRows() - 1);
     endRemoveRows();
     participants.reset();
//     m_chats.clear();
}

QModelIndex ParticipantModel::createIndexForData(const Participant& part)
{
    auto sp = participants.lock();
    if(!sp)
        return {};

    for(int i = 0; i < numRows(); ++i)
    {
        if((*sp)[i].nickname != part.nickname)
            continue;

        return createIndex(i, 0);
    }
    return {};
}

bool ParticipantModel::containsData(const Participant& info)
{
    auto sp = participants.lock();
    if(!sp)
        return false;

    const auto& container = *sp;
    auto it = std::find_if(container.begin(), container.end(), [&info](const auto& part){
       return part == info;
    });

    return it != container.end();
}
