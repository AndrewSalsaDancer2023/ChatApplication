#include "chatstorage.h"
#include "../../Server/src/database/DatabaseTypes.h"
#include "../../Client/Chat/commonutils.h"
#include <algorithm>

using namespace Backend;

QSet<QString> ChatStorage::getChatTiltles() const
{
    QSet<QString> res;
    for (const auto& [key, value] : dataModel)
        res.insert(key);

    return res;
}

bool ChatStorage::chatExists(const QString& title)
{
    return dataModel.find(title) != dataModel.end();
}

void ChatStorage::addChatTilte(const QString& title)
{
    if(dataModel.find(title) == dataModel.end())
        dataModel[title] = std::make_shared<Backend::chatData>();
}

void ChatStorage::deleteChat(const QString& title)
{
    dataModel.erase(title);
}

std::shared_ptr<messageList> ChatStorage::getMessagesList(const QString& title)
{
    auto itFind = dataModel.find(title);
    if(itFind != dataModel.end())
        return std::shared_ptr<messageList>((*itFind).second, &(*itFind).second->messages);
//        return (*itFind).second->messages;

    return {};
}

void ChatStorage::addMessagesList(const QString& title, messageList& msgList)
{
    addChatTilte(title);
    dataModel[title]->messages = std::move(msgList);
}

void ChatStorage::addChatMessage(const QString& title, Database::singleUserMessage& msg)
{
    addChatTilte(title);
    dataModel[title]->messages.emplace_back(std::move(Backend::singleUserMessage(msg)));
}

std::shared_ptr<participantList> ChatStorage::getParticipants(const QString& title)
{
    auto itFind = dataModel.find(title);
    if (itFind != dataModel.end())
        return std::shared_ptr<participantList>((*itFind).second, &(*itFind).second->participants);
//        return  (*itFind).second->participants;

    return {} ;
}

std::shared_ptr<participantList> ChatStorage::copyParticipants(const QString& title)
{
    auto itFind = dataModel.find(title);
    if (itFind != dataModel.end())
    {
        std::shared_ptr<participantList> res = std::make_shared<participantList>();
        *res = (*itFind).second->participants;
        return res;
    }
    return {} ;
}

std::shared_ptr<participantList> ChatStorage::copyUsers()
{
    std::shared_ptr<participantList> res = std::make_shared<participantList>();

    for(const auto& user: users)
        (*res).push_back(std::move(Backend::Participant{user.name, user.surname, user.nickname}));

    return res;
}

std::shared_ptr<participantList> ChatStorage::copyUsersExceptChatMembers(const std::set<QString>& members)
{
    std::shared_ptr<participantList> res = std::make_shared<participantList>();

    std::for_each(users.cbegin(), users.cend(),
                  [&res, &members](const auto& user){
                    if(members.find(user.nickname) != members.end())
                        return;
                    (*res).emplace_back(std::move(Backend::Participant{user.name, user.surname, user.nickname}));
                  });

    return res;
}

void ChatStorage::addParticipantsList(const QString& title, participantList& partList)
{
    addChatTilte(title);
    dataModel[title]->participants = std::move(partList);
}

void ChatStorage::fillChatsInfo(const std::vector<Backend::chatInfo>& chats)
{
    for(const auto& chat : chats)
    {
        for(const auto& participant : chat.participants)
        {
            if(auto part = getUserInfo(users, participant))
            {
                Backend::Participant prt(*part);
//                QString title = QString::fromStdString(chat.title);
//                addChatParticipant(title, prt);
                addChatParticipant(chat.title, prt);
            }
        }
    }
}

void ChatStorage::changeChatParticipants(const Backend::chatInfo& chat)
{
    std::shared_ptr<participantList> partList = getParticipants(chat.title);
    if(!partList)
        return;

    partList->clear();
    for(const auto& participant : chat.participants)
    {
        if(auto part = getUserInfo(users, participant))
        {
            Backend::Participant prt(*part);
//            QString title = QString::fromStdString(chat.title);
//            addChatParticipant(title, prt);
            addChatParticipant(chat.title, prt);
        }
    }
}

void ChatStorage::addChatParticipant(const QString& title, Backend::Participant& participant)
{
    addChatTilte(title);
    dataModel[title]->participants.push_back(std::move(participant));
}

void ChatStorage::removeChatParticipant(const QString& title, const QString& nickName)
{
    auto it = dataModel.find(title);
    if(it == dataModel.end())
        return;

    auto rit = std::remove_if(it->second->participants.begin(), it->second->participants.end(),
                              [&nickName](Backend::Participant& prt){
                                return prt.nickname == nickName;
                              });
    it->second->participants.erase(rit, it->second->participants.end());
}

std::optional<Backend::userInfo> ChatStorage::getAuthUserInfo(const QString& nickname)
{
    auto it = std::find_if(users.begin(), users.end(),[&nickname](const auto& usrInfo){
        return usrInfo.nickname == nickname;
    });

    if(it == users.end())
        return {};

    return *it;
}
