#pragma once

#include <QObject>
#include <QString>
#include <utility>
#include <string>
#include <utility>
#include <chrono>
#include <ctime>

void saveJsonToFile(const QString& path, const QString& fileContent);
QString readJsonFromFile(const QString& path);
QString getHost(const std::string& path);
std::pair<QString, QString> getLoginPassword(const QString& path);
void saveClientAuthentication(const std::string& path, const std::string& login, const std::string& password, const std::string& database);
std::tuple<QString, QString, QString> getClientAuthentication(const QString& path);
bool isFileExists(const QString& path);
