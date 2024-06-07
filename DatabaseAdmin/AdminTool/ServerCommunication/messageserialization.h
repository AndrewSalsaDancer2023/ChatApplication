#pragma once

#include <string>
#include "../../Server/build/messages.pb.h"
enum class PayloadType : unsigned int;
#include <QString>
#include <QSet>
namespace Database {
struct userInfo;
struct userChatInfo;
struct chatInfo;
struct userChatMessage;
struct chatMessagesTape;
}

namespace Backend {
    struct chatInfo;
    struct userInfo;
    struct userChatMessage;
    struct chatMessagesTape;
    struct userChatInfo;
}

using ADDUserToChatInfo = std::tuple<QString, QString, QString, QSet<QString> >;
using LeaveUserFromChatInfo = std::tuple<QString, QString, QString, QString, QString>;

std::string createAuthorizationMessage(const QString& login, const QString& password, const QString& dbName);
Serialize::ChatMessage decodeMessageFromString(const QString& message);
std::string createNoPayloadMessage(PayloadType type, const QString& description = "");
std::string createGetAllUsersMessage(PayloadType type, const QString& dbName, const QString& collName);
std::string createAddUserMessage(const QString& dbName, const QString& collName, const Backend::userInfo& info);
std::string createMarkUserAsDeletedMessage(const QString& dbName, const QString& collName, const QString& nickName);
std::string createDeleteUserMessage(const QString& dbName, const QString& collName, const QString& nickName);
std::string createUpdateUserMessage(const QString& dbName, const QString& collName, const Backend::userInfo& info);
std::string createGetChatsContainUserMessage(const QString& dbName, const QString& collName, const QString& nickName);
std::string createAddUserToChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName);
std::string createDeleteUserFromChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName);
std::string createChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QSet<QString>& participants);
std::string createInfoChatMessage(const QString& dbName, const QString& chatCollectionName, const QString& nickName, const QString& message);
std::string createGetChatTapeMessage(const QString& dbName, const QString& chatCollectionName, const QString& nickName);
std::string createModifyChatParticipantsMessage(const QString& dbName, const QString&  collName,
                                                const QString&  chatTitle, const QSet<QString>& delUsrs,
                                                const QSet<QString>& addUsrs, const QString& modifierNickName,
                                                const QString& delMessage, const QString& addMessage);

std::string createLeaveFromChatMessage(const QString& dbName, const QString& chatCollectionName,
                                       const QString&  chatTitle, const QString&  nickName, const QString& leftMessagePrefix);

Backend::chatMessagesTape decodeMessageTapeFromChat(Serialize::ChatMessage& msg);
Backend::userChatMessage decodeChatMessage(Serialize::ChatMessage& msg);
Backend::chatInfo decodeParticipantsListMessage(Serialize::ChatMessage& msg);
std::vector<Backend::chatInfo> decodeChatInfoMessages(Serialize::ChatMessage& msg);
std::vector<Backend::userInfo> decodeAllUsersMessage(Serialize::ChatMessage& msg);
std::optional<ADDUserToChatInfo> decodeAddChatInfo(Serialize::ChatMessage& msg);
std::optional<Backend::userChatInfo> decodeModifyParticipantsChatMessage(Serialize::ChatMessage& msg);
LeaveUserFromChatInfo decodeLeaveUserFromChatInfo(Serialize::ChatMessage& msg);
