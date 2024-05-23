#include "chatstorage.h"
#include "../../Server/src/database/DatabaseTypes.h"
#include "../../Client/Chat/commonutils.h"
#include <algorithm>

ChatStorage::ChatStorage()
{

}

std::set<std::string> ChatStorage::getChatTiltles() const
{
    std::set<std::string> res;
    for (const auto& [key, value] : dataModel)
        res.insert(key);

    return res;
}

bool ChatStorage::chatExists(const std::string& title)
{
    return dataModel.find(title) != dataModel.end();
}

void ChatStorage::addChatTilte(const std::string& title)
{
    if(dataModel.find(title) == dataModel.end())
        dataModel[title] = std::make_shared<Database::chatData>();
}

void ChatStorage::deleteChat(const std::string& title)
{
    dataModel.erase(title);
}

std::shared_ptr<messageList> ChatStorage::getMessagesList(const std::string& title)
{
    auto itFind = dataModel.find(title);
    if(itFind != dataModel.end())
        return std::shared_ptr<messageList>((*itFind).second, &(*itFind).second->messages);
//        return (*itFind).second->messages;

    return {};
}

void ChatStorage::addMessagesList(const std::string& title, messageList& msgList)
{
    addChatTilte(title);
    dataModel[title]->messages = std::move(msgList);
}

void ChatStorage::addChatMessage(const std::string& title, Database::singleUserMessage& msg)
{
    addChatTilte(title);
    dataModel[title]->messages.push_back(std::move(msg));
}

std::shared_ptr<participantList> ChatStorage::getParticipants(const std::string& title)
{
    auto itFind = dataModel.find(title);
    if (itFind != dataModel.end())
        return std::shared_ptr<participantList>((*itFind).second, &(*itFind).second->participants);
//        return  (*itFind).second->participants;

    return {} ;
}

std::shared_ptr<participantList> ChatStorage::copyParticipants(const std::string& title)
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
        (*res).push_back(std::move(Database::Participant{user.name, user.surname, user.nickname}));

    return res;
}

std::shared_ptr<participantList> ChatStorage::copyUsersExceptChatMembers(const std::set<std::string>& members)
{
    std::shared_ptr<participantList> res = std::make_shared<participantList>();

    std::for_each(users.cbegin(), users.cend(),
                  [&res, &members](const auto& user){
                    if(members.find(user.nickname) != members.end())
                        return;
                    (*res).push_back(std::move(Database::Participant{user.name, user.surname, user.nickname}));
                  });

    return res;
}

void ChatStorage::addParticipantsList(const std::string& title, participantList& partList)
{
    addChatTilte(title);
    dataModel[title]->participants = std::move(partList);
}

void ChatStorage::fillChatsInfo(const std::vector<Database::chatInfo>& chats)
{
    for(const auto& chat : chats)
    {
//        for(int i = 0; i < chat.participants.size(); ++i)
        for(const auto& participant : chat.participants)
        {
            if(auto part = getUserInfo(users, participant))//chat.participants[i]))
                addChatParticipant(chat.title, *part);
        }
    }
}

void ChatStorage::changeChatParticipants(const Database::chatInfo& chat)
{
    std::shared_ptr<participantList> partList = getParticipants(chat.title);
    if(!partList)
        return;

    partList->clear();
    for(const auto& participant : chat.participants)
    {
        if(auto part = getUserInfo(users, participant))
            addChatParticipant(chat.title, *part);
    }
}

void ChatStorage::addChatParticipant(const std::string& title, Database::Participant& participant)
{
    addChatTilte(title);
    dataModel[title]->participants.push_back(std::move(participant));
}

void ChatStorage::removeChatParticipant(const std::string& title, const std::string& nickName)
{
    auto it = dataModel.find(title);
    if(it == dataModel.end())
        return;

    auto rit = std::remove_if(it->second->participants.begin(), it->second->participants.end(),
                              [&nickName](Database::Participant& prt){
                                return prt.nickname == nickName;
                              });
    it->second->participants.erase(rit, it->second->participants.end());
}

std::optional<Database::userInfo> ChatStorage::getAuthUserInfo(const std::string& nickname)
{
    auto it = std::find_if(users.begin(), users.end(),[&nickname](const auto& usrInfo){
        return usrInfo.nickname == nickname;
    });

    if(it == users.end())
        return {};

    return *it;
}
