#pragma once
#include <QString>
#include <vector>
#include <string>
#include <optional>

namespace Backend
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
    virtual std::optional<Backend::userInfo> findSelectedUser(const QString& nick) = 0;
    virtual void sendGetDBCollectionNamesRequest(const QString& dbName) = 0;
    virtual void sendGetAllUsersMessage(const QString& dbName, const QString& collName) = 0;
    virtual void sendDeleteSelectedUserMessage(const QString& dbName, const QString& collName) = 0;
    virtual void sendAddUserMessage(const QString& dbName, const QString& collName, const Backend::userInfo& info) = 0;
    virtual void sendGetDBNamesMessage() = 0;
    virtual void sendAuthorizeMessage(const QString& dbName) = 0;
    virtual void sendAuthorizeMessage(const QString& login, const QString& password, const QString& dbName) = 0;
};

