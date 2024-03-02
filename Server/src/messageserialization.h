#ifndef MESSAGESERIALIZATION_H
#define MESSAGESERIALIZATION_H

#include <string>
#include "../../Server/build/messages.pb.h"

enum class PayloadType: unsigned int;

namespace Database {
	struct userChatMessage;
}

std::string createAuthorizationMessage(const std::string& login, const std::string& password);
Serialize::ChatMessage decodeMessageFromString(const std::string& message);
Database::userChatMessage decodeChatMessage(Serialize::ChatMessage& msg);
std::string createChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::set<std::string>& participants);
std::string createModifyChatParticipantsMessage(const std::string& dbName, const std::string& chatTitle, PayloadType msgType);
#endif // MESSAGESERIALIZATION_H
