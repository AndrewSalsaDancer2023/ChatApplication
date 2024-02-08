#include "stringlistmodel.h"
#include <QDebug>
#include <QJsonObject>
//#include "../../Server/src/database/DatabaseTypes.h"

StringListModel::StringListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void StringListModel::addSampleData()
{
/*     PersonInfo one{"Bill Gates", "555 7777"};
     addData(one);*/
}

bool StringListModel::changeSampleData(const QString& name)
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

Qt::ItemFlags StringListModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
 }

//http://doc.crossplatform.ru/qt/4.3.2/model-view-creating-models.html
//https://www.youtube.com/watch?v=-KJ2HmvW8CY&t=268s
bool StringListModel::setData(const QModelIndex &index, const QVariant &value, int role)
 {
     if (index.isValid() && role == Qt::EditRole) {

//         Database::chatInfo &info = m_chats[index.row()];
//         info.title = QString(value.toString()).toStdString();
         QString &info = m_chats[index.row()];
         info = QString(value.toString());

         emit dataChanged(index, index);
         return true;
     }
     return false;
 }

void StringListModel::addData(const std::set<std::string>& data)
{
    for(auto& str : data)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_chats << QString::fromStdString(std::move(str));
        endInsertRows();
    }
}

void StringListModel::addData(const QString& info)
{
    if(m_chats.contains(info))
        return;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_chats << info;
    endInsertRows();
}

int StringListModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_chats.count();
}

QVariant StringListModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= m_chats.count())
        return QVariant();

//    const Database::chatInfo &info = m_chats[index.row()];
    const auto &info = m_chats[index.row()];

    if (role == NameRole)
        return info;
//        return QString::fromStdString(info);

/*
    if (role == NumberRole)
        return info.number;
*/
    return QVariant();
}

QHash<int, QByteArray> StringListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "title";
//    roles[NumberRole] = "number";

    return roles;
}

void  StringListModel::slotSelect(int index)
{
    qDebug() << "Selected item:" << index;
    emit itemSelected(m_chats[index]);
}

void StringListModel::slotSelect(const QString& title)
{
    qDebug() << "Selected item:" << title;
    emit itemSelected(title);
}

QString  StringListModel::getItem(int index)
{
    if((index < 0) || (index >= m_chats.count()))
        return {};

//    return QString::fromStdString(m_chats[index]);
    return m_chats[index];
}

void StringListModel::removeData(const QString& name)
{
    for(int i = 0; i < m_chats.count(); ++i)
    {
//        if(m_chats.value(i).title != name.toStdString())
        if(m_chats.value(i) != name)
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        m_chats.removeAt(i);
        endRemoveRows();
        return;
    }
}

QModelIndex StringListModel::createIndexForData(const QString& name)
{
    for(int i = 0; i < m_chats.count(); ++i)
    {
//        if(m_chats.value(i).title != name.toStdString())
        if(m_chats.value(i) != name)
            continue;

        return createIndex(i, 0);
    }
    return {};
}

bool StringListModel::containsData(const QString& info)
{
    return m_chats.contains(info);
}
