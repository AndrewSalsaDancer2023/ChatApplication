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

QString getHost()
{
    QString path{"../../Server/data/config.json"};
    auto content = readJsonFromFile(path);
    Serialize::ConnectionData data = getConnectionDataFromString(content.toStdString());
    if(!data.has_connectionparameters())
        return {};
    const auto& parameters = data.connectionparameters();
    return QString::fromStdString(parameters.host()) + ':' + QString::number(parameters.port());
}

std::pair<std::string, std::string> getLoginPassword()
{
    QString path{"../../Server/data/config.json"};
    auto content = readJsonFromFile(path);
    Serialize::ConnectionData data = getConnectionDataFromString(content.toStdString());
    if(!data.has_authentication())
        return {};
    const auto& auth = data.authentication();
    return { auth.login(),  auth.password()};
}

int64_t getCurrentTimeInNanoseconds()
{
/*    std::chrono::time_point<std::chrono::system_clock> timepoint = std::chrono::system_clock::now();
    std::chrono::seconds ns =
    std::chrono::duration_cast<std::chrono::seconds>(timepoint.time_since_epoch()).count();
    return ns;
*/
    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return timestamp;
}
