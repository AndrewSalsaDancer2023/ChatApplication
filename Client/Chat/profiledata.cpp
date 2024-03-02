#include "profiledata.h"

#include <QDebug>

ProfileData::ProfileData(const QString& name, const QString& surname, const QString& nickname, QObject *parent)
    : QObject(parent), profName{name}, profSurname{surname}, profNickName{nickname}
{
}


QString ProfileData::login()
{
    return m_login;
}

QString ProfileData::password()
{
    return m_password;
}

void ProfileData::setLogin(QString newLogin)
{
    if(m_login == newLogin)
        return;
    qDebug() << newLogin << '\n';
    m_login = newLogin;
    emit loginChanged();
}

void ProfileData::setPassword(QString newPassword)
{
    if(m_password == newPassword)
        return;
    qDebug() << newPassword << '\n';
    m_password = newPassword;
    emit passwordChanged();
}

void ProfileData::setName(QString newName)
{
    profName = newName;
}

void ProfileData::setSurname(QString newSurname)
{
    profSurname = newSurname;
}

void ProfileData::setNickname(QString newNickname)
{
    profNickName = newNickname;
}
