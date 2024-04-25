#pragma once

#include <string>
#include "../../Server/build/messages.pb.h"
enum class PayloadType : unsigned int;
namespace Database {
struct userInfo;
struct userChatInfo;
struct chatInfo;
struct userChatMessage;
struct chatMessagesTape;
}

using ADDUserToChatInfo = std::tuple<std::string, std::string, std::string, std::set<std::string> >;
using LeaveUserFromChatInfo = std::tuple<std::string, std::string, std::string, std::string>;

std::string createAuthorizationMessage(const std::string& login, const std::string& password, const std::string& dbName);
Serialize::ChatMessage decodeMessageFromString(const std::string& message);
std::string createNoPayloadMessage(PayloadType type, const std::string& description = "");
std::string createGetAllUsersMessage(PayloadType type, const std::string& dbName, const std::string& collName);
std::string createAddUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info);
std::string createMarkUserAsDeletedMessage(const std::string& dbName, const std::string& collName, const std::string& nickName);
std::string createDeleteUserMessage(const std::string& dbName, const std::string& collName, const std::string& nickName);
std::string createUpdateUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info);
std::string createGetChatsContainUserMessage(const std::string& dbName, const std::string& collName, const std::string& nickName);
std::string createAddUserToChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::string& nickName);
std::string createDeleteUserFromChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::string& nickName);
std::string createChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::set<std::string>& participants);
std::string createInfoChatMessage(const std::string& dbName, const std::string& chatCollectionName, const std::string& nickName, const std::string& message);
std::string createGetChatTapeMessage(const std::string& dbName, const std::string& chatCollectionName, const std::string& nickName);
std::string createModifyChatParticipantsMessage(const std::string& dbName, const std::string&  collName,
                                                const std::string&  chatTitle, const std::set<std::string>& delUsrs, const std::set<std::string>& addUsrs);

std::string createLeaveFromChatMessage(const std::string& dbName, const std::string& chatCollectionName, const std::string&  chatTitle, const std::string&  nickName);

Database::chatMessagesTape decodeMessageTapeFromChat(Serialize::ChatMessage& msg);
Database::userChatMessage decodeChatMessage(Serialize::ChatMessage& msg);
Database::chatInfo decodeParticipantsListMessage(Serialize::ChatMessage& msg);
std::vector<Database::chatInfo> decodeChatInfoMessages(Serialize::ChatMessage& msg);
std::vector<Database::userInfo> decodeAllUsersMessage(Serialize::ChatMessage& msg);
std::optional<ADDUserToChatInfo> decodeAddChatInfo(Serialize::ChatMessage& msg);
std::optional<Database::userChatInfo> decodeModifyParticipantsChatMessage(Serialize::ChatMessage& msg);
LeaveUserFromChatInfo decodeLeaveUserFromChatInfo(Serialize::ChatMessage& msg);
