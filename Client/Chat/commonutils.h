#pragma once
#include <QString>
#include <vector>
#include "../../Server/build/authentication.pb.h"
#include "../../Server/build/messages.pb.h"
#include <string>
#include <QString>
#include <QSet>
#include <optional>
#include <utility>

namespace Database
{
    struct userInfo;
    struct Participant;
}

namespace Backend
{
    struct userInfo;
    struct Participant;
}

QString convertDateTimeToString(int64_t milliseconds);
QString convertDateTimeToStringFast(int64_t milliseconds);
std::vector<Backend::userInfo> getUsers(Serialize::UserInfoVector& elements);
std::optional<Backend::Participant> getUserInfo(const std::vector<Backend::userInfo>& usersArray, const QString& nickName);
std::string convertUsersNameSurnameToString(std::optional< std::pair<std::string, std::string>> info);
std::pair<std::string, std::string> extractUserNameSurnameFromString(const std::string& nameSurname);
QString addUsersToMessageTemplate(const QString& tmplate, const QSet<QString>& usrNicknames);
std::string createLeaveFromChatMessage(const std::string& tmplate, const std::string& usrNickname);

