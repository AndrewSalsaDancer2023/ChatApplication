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
/*
    Q_INVOKABLE QString getErrorLoginMessage();
    Q_INVOKABLE QString getErrorPasswordMessage();
*/
signals:
    void loginChanged();
    void passwordChanged();

public slots:
    void setLogin(QString);
    void setPassword(QString);


private:
    QString m_login;
    QString m_password;
};
