#include "authutils.h"
#include "parametersserialization.h"
#include <QFile>
#include <QTextStream>

void saveJsonToFile(const QString& path, const QString& fileContent)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << fileContent;

    file.close();
}

QString readJsonFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QTextStream in(&file);
    QString content = in.readLine();
    QString current = content;
    while (!current.isNull()) {
            current = in.readLine();
            content += current;
        }

    return content;
}

bool isFileExists(const QString& path)
{
    QFile file(path);
    return file.exists();
}

QString getHost(const std::string& path)
{
    auto content = readJsonFromFile(QString::fromStdString(path));
    Serialize::ConnectionData data = getConnectionDataFromString(content.toStdString());
    if(!data.has_connectionparameters())
        return {};
    const auto& parameters = data.connectionparameters();
    return QString::fromStdString(parameters.host()) + ':' + QString::number(parameters.port());
}

std::pair<QString, QString> getLoginPassword(const QString& path)
{
    auto content = readJsonFromFile(path);
    Serialize::ConnectionData data = getConnectionDataFromString(content.toStdString());
    if(!data.has_authentication())
        return {};
    const auto& auth = data.authentication();
    return { QString::fromStdString(auth.login()), QString::fromStdString(auth.password())};
}

void saveClientAuthentication(const std::string& path, const std::string& login, const std::string& password, const std::string& database)
{
    auto content = serializeAuthDataToString(login, password, database);
    saveJsonToFile(QString::fromStdString(path), QString::fromStdString(content));
}

std::tuple<QString, QString, QString> getClientAuthentication(const QString& path)
{
    auto content = readJsonFromFile(path);
    Serialize::Login data = getAuthDataFromString(content.toStdString());

    return {QString::fromStdString(data.login()), QString::fromStdString(data.password()), QString::fromStdString(data.database())};
}
