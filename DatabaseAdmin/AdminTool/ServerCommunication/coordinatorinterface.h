#pragma once
#include <QString>
#include <vector>
#include <string>
#include <optional>

namespace Database
{
    struct userInfo;
}

namespace Serialize {
    class ChatMessage;
}

class NetworkCoordinatorInterface
{
public:
//    virtual void messageReceived(const std::string& message) = 0;
    virtual void messageReceived(Serialize::ChatMessage& message) = 0 ;
    virtual void onDisconnected() = 0;
    virtual void onConnected() = 0;
    virtual std::optional<Database::userInfo> findSelectedUser(const std::string& nick) = 0;
    virtual void sendGetDBCollectionNamesRequest(const std::string& dbName) = 0;
    virtual void sendGetAllUsersMessage(const std::string& dbName, const std::string& collName) = 0;
    virtual void sendDeleteSelectedUserMessage(const std::string& dbName, const std::string& collName) = 0;
    virtual void sendAddUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info) = 0;
    virtual void sendGetDBNamesMessage() = 0;
    virtual void sendAuthorizeMessage(const std::string& dbName) = 0;
    virtual void sendAuthorizeMessage(const std::string& login, const std::string& password, const std::string& dbName) = 0;
};

