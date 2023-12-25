#pragma once
#include <QObject>
#include <QString>
#include <memory>
//#include "coordinator.h"

class AuthenticationForm : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString login READ login WRITE setLogin NOTIFY loginChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

public:
    explicit AuthenticationForm(QObject *parent = nullptr);
    QString login();
    QString password();
//    Q_INVOKABLE bool isLoginCorrect(QString login);{return false;}
//    Q_INVOKABLE bool isPasswordCorrect(QString password); {return false;}
//    Q_INVOKABLE void setAuthenticationData(QString login, QString password, QString dbName);

    Q_INVOKABLE QString getErrorLoginMessage();
    Q_INVOKABLE QString getErrorPasswordMessage();

signals:
    void loginChanged();
    void passwordChanged();

public slots:
//    void callMe();
    void setLogin(QString);
    void setPassword(QString);


private:
    QString m_login;
    QString m_password;
};
