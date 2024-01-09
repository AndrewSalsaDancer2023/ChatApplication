#include "commonutils.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include "../../Server/src/database/DatabaseTypes.h"

QString convertDateTimeToString(int64_t milliseconds)
{
    std::chrono::milliseconds ms = std::chrono::milliseconds(milliseconds);
    std::chrono::time_point<std::chrono::system_clock> timestamp(ms);
    auto in_time_t = std::chrono::system_clock::to_time_t(timestamp);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%d-%m-%Y %X");

    return QString::fromStdString(ss.str());
}

QString convertDateTimeToStringFast(int64_t milliseconds)
{
    std::chrono::milliseconds ms = std::chrono::milliseconds(milliseconds);
    std::chrono::time_point<std::chrono::system_clock> timestamp(ms);
    auto in_time_t = std::chrono::system_clock::to_time_t(timestamp);

    std::string s(50, '\0');
    std::strftime(&s[0], s.size(), "%H:%M:%S %Y-%m-%d", std::localtime(&in_time_t));
    return QString::fromStdString(s);
}

std::vector<Database::userInfo> getUsers(Serialize::UserInfoVector& elements)
{
    std::vector<Database::userInfo> result;

    for(int i = 0; i < elements.users_size(); ++i)
        {
            const Serialize::UserInfo& usrInfo = elements.users(i);

            Database::userInfo user;
            user.name = std::move(usrInfo.name());
            user.surname = std::move(usrInfo.surname());
            user.email = std::move(usrInfo.email());
            user.nickname = std::move(usrInfo.nickname());
            user.password = std::move(usrInfo.password());
            user.profilepicture = std::move(usrInfo.profilepicture());
            user.deleted = usrInfo.deleted();

            result.push_back(user);
        }

    return result;
}

std::optional<Database::Participant> getUserInfo(const std::vector<Database::userInfo>& usersArray, const std::string& nickName)
{
    auto findIt = std::find_if(usersArray.begin(), usersArray.end(), [&nickName](const auto& info){
        return nickName == info.nickname;
    });

    if(findIt == usersArray.end())
        return {};

    return Database::Participant{(*findIt).name, (*findIt).surname, (*findIt).nickname};
}

std::string convertUsersNameSurnameToString(std::optional< std::pair<std::string, std::string>> info)
{
    return info ? ((*info).first + ' '+ (*info).second) : "";
}

std::pair<std::string, std::string> extractUserNameSurnameFromString(const std::string& nameSurname)
{
    auto pos = nameSurname.find(' ');
    if(pos == std::string::npos)
        return { nameSurname, ""};
    return {nameSurname.substr(0, pos), nameSurname.substr(pos+1)};
}
