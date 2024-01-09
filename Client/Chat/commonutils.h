#pragma once
#include <QString>
#include <vector>
#include "../../Server/build/authentication.pb.h"
#include "../../Server/build/messages.pb.h"
#include <string>
#include <optional>
#include <utility>

namespace Database
{
    struct userInfo;
    struct Participant;
}

QString convertDateTimeToString(int64_t milliseconds);
QString convertDateTimeToStringFast(int64_t milliseconds);
std::vector<Database::userInfo> getUsers(Serialize::UserInfoVector& elements);
std::optional<Database::Participant> getUserInfo(const std::vector<Database::userInfo>& usersArray, const std::string& nickName);
std::string convertUsersNameSurnameToString(std::optional< std::pair<std::string, std::string>> info);
std::pair<std::string, std::string> extractUserNameSurnameFromString(const std::string& nameSurname);
