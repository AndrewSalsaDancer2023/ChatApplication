#pragma once

#include <string>
#include "../../Server/build/messages.pb.h"
enum class PayloadType : unsigned int;
namespace Database {
struct userInfo;
}
std::string createAuthorizationMessage(const std::string& login, const std::string& password);
Serialize::ChatMessage decodeMessageFromString(const std::string& message);
std::string createNoPayloadMessage(PayloadType type, const std::string& description = "");
std::string createGetAllUsersMessage(PayloadType type, const std::string& dbName, const std::string& collName);
std::string createAddUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info);
std::string createDeleteUserMessage(const std::string& dbName, const std::string& collName, const std::string& nickName);
std::string createUpdateUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info);
