#ifndef MESSAGESERIALIZATION_H
#define MESSAGESERIALIZATION_H

#include <string>
#include "../../Server/build/messages.pb.h"

namespace Database {
	struct userChatMessage;
}

std::string createAuthorizationMessage(const std::string& login, const std::string& password);
Serialize::ChatMessage decodeMessageFromString(const std::string& message);
Database::userChatMessage decodeChatMessage(Serialize::ChatMessage& msg);
#endif // MESSAGESERIALIZATION_H
