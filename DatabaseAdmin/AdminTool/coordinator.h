#pragma once

#include <QObject>
#include <string>
#include "./ServerCommunication/coordinatorinterface.h"
#include "../../Server/src/common_types.h"
#include "../../Server/src/database/DatabaseTypes.h"
#include "../../Server/build/messages.pb.h"
#include <map>
#include <functional>
#include "./ServerCommunication/servercommunicator.h"
#include <memory>

enum class commState
{
    Disconnected,
    Connected,
    Authenticated
};

class MainWindowInterface;

class coordinator : public QObject, public NetworkCoordinatorInterface
{
    Q_OBJECT
public:
    explicit coordinator(MainWindowInterface& window, QObject *parent = nullptr);

    void onDisconnected() override ;
    void onConnected() override;
//    void messageReceived(const std::string& message) override ;
    void messageReceived(Serialize::ChatMessage& message) override ;
    std::optional<Database::userInfo> findSelectedUser(const std::string& nick) override;
    std::vector<std::string> getUsersNicknames();
    void sendGetDBCollectionNamesRequest(const std::string& dbName) override;
    void sendGetAllUsersMessage(const std::string& dbName, const std::string& collName) override;
    void sendDeleteSelectedUserMessage(const std::string& dbName, const std::string& collName) override;
    void sendAddUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info) override;
    void sendGetDBNamesMessage() override;
    void sendAuthorizeMessage(const std::string& dbName) override;
    void sendAuthorizeMessage(const std::string& login, const std::string& password, const std::string& dbName) override;
signals:

private:
    void handleAddUserInfoError(Serialize::ChatMessage& msg);
    void handleAddUserInfo(Serialize::ChatMessage& msg);
    void handleModifyUserInfo(Serialize::ChatMessage& msg);
    void handleModifyUserInfoError(Serialize::ChatMessage& msg);
    void handleDeleteUser(Serialize::ChatMessage& msg);
    void handleDeleteUserError(Serialize::ChatMessage& msg);
    void handleGetUsersInfo(Serialize::ChatMessage& msg);
    void handleGetUsersInfoError(Serialize::ChatMessage& msg);
    void handleGetCollectionsNames(Serialize::ChatMessage& msg);
    void handleGetDatabaseNames(Serialize::ChatMessage& msg);
    void handleAuthenticationMessage(Serialize::ChatMessage& msg);
    void getUsers(Serialize::UserInfoVector& elements);
    void selectCurrentUserInList();
    std::vector<Database::userInfo> users;
    std::optional<Database::userInfo> userSelected;

    std::vector<std::string> getElements(const Serialize::StringVector& elements);
    commState state{commState::Disconnected};
    std::map<::google::protobuf::uint32, std::function<void(Serialize::ChatMessage&)> > handlers;
    MainWindowInterface& interface;
    std::unique_ptr<ServerCommunicator> serverCommunicator;

    std::map<::google::protobuf::uint32, std::function<void(Serialize::ChatMessage&)> > fhandlers;
};
