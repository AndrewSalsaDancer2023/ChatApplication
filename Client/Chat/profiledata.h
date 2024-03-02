#pragma once
#include <QObject>
#include <QString>
#include <memory>

class ProfileData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString login READ login WRITE setLogin NOTIFY loginChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString surname READ surname WRITE setSurname NOTIFY surnameChanged)
    Q_PROPERTY(QString nickname READ nickname WRITE setNickname NOTIFY nicknameChanged)

public:
    explicit ProfileData(const QString& name, const QString& surname, const QString& nickname, QObject *parent = nullptr);
    QString login();
    QString password();

    QString name() { return profName; }
    QString surname() { return profSurname; }
    QString nickname() { return profNickName; }

signals:
    void loginChanged();
    void passwordChanged();

    void nameChanged();
    void surnameChanged();
    void nicknameChanged();


public slots:

    void setName(QString);
    void setSurname(QString);
    void setNickname(QString);

    void setLogin(QString);
    void setPassword(QString);


private:
    QString m_login;
    QString m_password;

    QString profName;
    QString profSurname;
    QString profNickName;
};
