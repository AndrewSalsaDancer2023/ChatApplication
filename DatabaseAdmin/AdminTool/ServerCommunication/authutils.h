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
QString getHost();
std::pair<std::string, std::string> getLoginPassword();
int64_t getCurrentTimeInNanoseconds();
