#include "authenticationform.h"
#include <QDebug>

AuthenticationForm::AuthenticationForm(QObject *parent) : QObject(parent)
{
}


QString AuthenticationForm::login()
{
    return m_login;
}

QString AuthenticationForm::password()
{
    return m_password;
}

void AuthenticationForm::setLogin(QString newLogin)
{
    if(m_login == newLogin)
        return;
    qDebug() << newLogin << '\n';
    m_login = newLogin;
    emit loginChanged();
}

void AuthenticationForm::setPassword(QString newPassword)
{
    if(m_password == newPassword)
        return;
    qDebug() << newPassword << '\n';
    m_password = newPassword;
    emit passwordChanged();
}

QString AuthenticationForm::getErrorLoginMessage()
{
   return QString{"login error!"};

}

QString AuthenticationForm::getErrorPasswordMessage()
{
    return {"password error!"};
}
