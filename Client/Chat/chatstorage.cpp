#include "chatstorage.h"
#include "../../Server/src/database/DatabaseTypes.h"
#include "../../Client/Chat/commonutils.h"

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

void ChatStorage::addParticipantsList(const std::string& title, participantList& partList)
{
    addChatTilte(title);
    dataModel[title]->participants = std::move(partList);
}

void ChatStorage::fillChatsInfo(const std::vector<Database::chatInfo>& chats)
{
    for(const auto& chat : chats)
    {
        for(int i = 0; i < chat.participants.size(); ++i)
        {
            if(auto part = getUserInfo(users, chat.participants[i]))
                addChatParticipant(chat.title, *part);
        }
    }
}

void ChatStorage::addChatParticipant(const std::string& title, Database::Participant& participant)
{
    addChatTilte(title);
    dataModel[title]->participants.push_back(std::move(participant));
}
