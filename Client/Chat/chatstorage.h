#pragma once
#include "../../Server/src/database/DatabaseTypes.h"
#include <string>
#include <memory>
#include <vector>

namespace Database {
    struct chatInfo;
}

using chatDataModel = std::map<std::string, std::shared_ptr<Database::chatData>>;
using participantList = std::vector<Database::Participant>;
using messageList = std::vector<Database::singleUserMessage>;
using usersList = std::vector<Database::userInfo>;

class ChatStorage
{
public:
    ChatStorage();

    std::set<std::string> getChatTiltles() const;
    std::shared_ptr<messageList> getMessagesList(const std::string& title);
    void addMessagesList(const std::string& title, messageList& msgList);
    void addChatMessage(const std::string& title, Database::singleUserMessage& msg);
    void deleteChat(const std::string& title);
    std::shared_ptr<participantList> getParticipants(const std::string& title);
    std::shared_ptr<participantList> copyParticipants(const std::string& title);
    std::shared_ptr<participantList> copyUsers();
    std::shared_ptr<participantList> copyUsersExceptChatMembers(const std::set<std::string>& members);

    void addParticipantsList(const std::string& title, participantList& partList);
    void fillChatsInfo(const std::vector<Database::chatInfo>& chats);
    void changeChatParticipants(const Database::chatInfo& chat);
    void addChatParticipant(const std::string& title, Database::Participant& participant);
    void addUsers(std::vector<Database::userInfo> usrs) { users = std::move(usrs); }
    const usersList& getUsers() { return users; }
    bool chatExists(const std::string& title);
private:
    void addChatTilte(const std::string& title);

private:
    std::vector<Database::userInfo> users;
    chatDataModel dataModel;
};
