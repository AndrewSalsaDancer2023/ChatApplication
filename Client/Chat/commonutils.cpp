#include "commonutils.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include "chatstorage.h"
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

std::vector<Backend::userInfo> getUsers(Serialize::UserInfoVector& elements)
{
    std::vector<Backend::userInfo> result;

    for(int i = 0; i < elements.users_size(); ++i)
        {
            const Serialize::UserInfo& usrInfo = elements.users(i);
/*
            user.name = std::move(usrInfo.name());
            user.surname = std::move(usrInfo.surname());
            user.email = std::move(usrInfo.email());
            user.nickname = std::move(usrInfo.nickname());
            user.password = std::move(usrInfo.password());
            user.profilepicture = std::move(usrInfo.profilepicture());
            user.deleted = usrInfo.deleted();
*/
            Backend::userInfo user(usrInfo.name(), usrInfo.surname(), usrInfo.email(), usrInfo.nickname(),
                                   usrInfo.password(), usrInfo.profilepicture(), usrInfo.deleted());
            result.push_back(user);
        }

    return result;
}

std::optional<Backend::Participant> getUserInfo(const std::vector<Backend::userInfo>& usersArray, const QString& nickName)
{
//    QString nick = QString::fromStdString(nickName);
    auto findIt = std::find_if(usersArray.begin(), usersArray.end(), [&nickName](const auto& info){
        return nickName == info.nickname;
    });

    if(findIt == usersArray.end())
        return {};

    return Backend::Participant{(*findIt).name, (*findIt).surname, (*findIt).nickname};
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

QString addUsersToMessageTemplate(const QString& tmplate, const QSet<QString>& usrNicknames)
{
    QString res;
    if(!usrNicknames.size())
        return res;

    res = tmplate;
    for(const auto& user : usrNicknames)
    {
        res += user;
        res += ' ';
    }
    res += "\n";

    return res;
}

std::string createLeaveFromChatMessage(const std::string& tmplate, const std::string& usrNickname)
{
    return tmplate + usrNickname;
}
