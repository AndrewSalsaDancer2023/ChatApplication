#ifndef MESSAGESERIALIZATION_H
#define MESSAGESERIALIZATION_H

#include <string>
#include "../../Server/build/messages.pb.h"
std::string createAuthorizationMessage(const std::string& login, const std::string& password);
Serialize::ChatMessage decodeMessageFromString(const std::string& message);
#endif // MESSAGESERIALIZATION_H
