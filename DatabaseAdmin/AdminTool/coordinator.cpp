#include "coordinator.h"
#include "./ServerCommunication/messageserialization.h"
#include "MainWindowInterface.h"
#include "./ServerCommunication/authutils.h"

const std::string path{"../../Server/data/config.json"};

coordinator::coordinator(MainWindowInterface& window, QObject *parent) :
    QObject(parent),
    interface{window}
{
    serverCommunicator = std::make_unique<ServerCommunicator>(*this);

    fhandlers[static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION_APPROVED)] =
            std::bind(&coordinator::handleAuthenticationMessage, this, std::placeholders::_1);

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION_APPROVED)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleAuthenticationMessage(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_DB_NAMES)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleGetDatabaseNames(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_DB_COLLECTIONSNAMES)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleGetCollectionsNames(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE_SUCCESS)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleGetUsersInfo(msg);
         };
    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE_ERROR)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleGetUsersInfoError(msg);
         };
    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_USER_DELETION_SUCCESS)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleDeleteUser(msg);
         };


    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_USER_DELETION_ERROR)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleDeleteUserError(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MODIFY_USER_INFO_SUCCESS)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleModifyUserInfo(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MODIFY_USER_INFO_ERROR)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleModifyUserInfoError(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_ADD_USER_TO_DATABASE_SUCCESS)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleAddUserInfo(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_USER_ADD_TO_DATABASE_ERROR)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleAddUserInfoError(msg);
         };
}


void coordinator::onDisconnected()
{
    state = commState::Disconnected;
    QString message = "Socket disconnected!";
    interface.showMessage(message);
}

void coordinator::onConnected()
{
    state = commState::Connected;
}
/*
void coordinator::messageReceived(const std::string& message)
{
   auto curMsg = decodeMessageFromString(message);
   auto tipe = curMsg.payload_type_id();
   auto itHandler = handlers.find(curMsg.payload_type_id());
   if(itHandler != handlers.end())
       itHandler->second(curMsg);
}
*/
void coordinator::messageReceived(Serialize::ChatMessage& message)
{
    auto itHandler = handlers.find(message.payload_type_id());
    if(itHandler != handlers.end())
        itHandler->second(message);
}

void coordinator::handleAuthenticationMessage(Serialize::ChatMessage& msg)
{
    if((state == commState::Connected) &&
       (static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION_APPROVED) == msg.payload_type_id()))
       state = commState::Authenticated;
    else
    {
        QString message = "Authentication error!";
        interface.showMessage(message);
    }
}

void coordinator::handleGetDatabaseNames(Serialize::ChatMessage& msg)
{
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_DB_NAMES) == msg.payload_type_id())
    {
        if(!msg.has_payload())
              return;

      Serialize::StringVector elements;
      msg.mutable_payload()->UnpackTo(&elements);

      interface.fillDBNamesList(getElements(elements));
    }
}

void coordinator::handleGetCollectionsNames(Serialize::ChatMessage& msg)
{
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_DB_COLLECTIONSNAMES) == msg.payload_type_id())
    {
        if(!msg.has_payload())
            return;

        Serialize::StringVector elements;
        msg.mutable_payload()->UnpackTo(&elements);

        interface.fillCollectionsList(getElements(elements));
    }
}

void coordinator::getUsers(Serialize::UserInfoVector& elements)
{
    users.clear();
    for(int i = 0; i < elements.users_size(); ++i)
    {
        const Serialize::UserInfo& usrInfo = elements.users(i);

        Database::userInfo user;
        user.name = std::move(usrInfo.name());
        user.surname = std::move(usrInfo.surname());
        user.email = std::move(usrInfo.email());
        user.nickname = std::move(usrInfo.nickname());
        user.password = std::move(usrInfo.password());
        user.profilepicture = std::move(usrInfo.profilepicture());
        user.deleted = usrInfo.deleted();

        users.push_back(user);
    }
}

std::vector<std::string> coordinator::getUsersNicknames()
{
    std::vector<std::string> res;
    for(const auto& elem : users)
        res.push_back(elem.nickname);

    return res;
}

void coordinator::handleGetUsersInfo(Serialize::ChatMessage& msg)
{
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE_SUCCESS) == msg.payload_type_id())
    {
        if(!msg.has_payload())
            return;

        Serialize::UserInfoVector elements;
        msg.mutable_payload()->UnpackTo(&elements);

        getUsers(elements);

        interface.fillUsersList(getUsersNicknames());
        if(!users.size())
            return;

        userSelected = users[0];
        interface.selectFirstUser();
        interface.fillUserInfo(*userSelected);
        interface.setNickNameEditable(false);
    }
}

void coordinator::handleGetUsersInfoError(Serialize::ChatMessage& msg)
{
    const std::string& errorReason = msg.sender();
    std::string message = "Unable get all users:" + errorReason;
    QString outmsg = QString::fromStdString(message);
    interface.showMessage(outmsg);
}

void coordinator::handleDeleteUserError(Serialize::ChatMessage& msg)
{
   const std::string& nickName = msg.sender();
   std::string message = "Unable delete user:" + nickName;
   QString outmsg = QString::fromStdString(message);
   interface.showMessage(outmsg);
}

void coordinator::handleDeleteUser(Serialize::ChatMessage& msg)
{
    //TODO delete user
    const std::string& nickName = msg.sender();
    auto itFind = std::find_if(users.begin(), users.end(),[&nickName](const auto& user){
            return user.nickname == nickName;
        });
    if(itFind == users.end())
        return;
    itFind->deleted = true;
}

void coordinator::handleModifyUserInfoError(Serialize::ChatMessage& msg)
{
    const std::string& nickName = msg.sender();
    std::string message = "Unable modify user:" + nickName;
    QString outmsg = QString::fromStdString(message);
    interface.showMessage(outmsg);
}

void coordinator::handleModifyUserInfo(Serialize::ChatMessage& msg)
{
    const std::string& nickName = msg.sender();
    auto itFind = std::find_if(users.begin(), users.end(),[&nickName](const auto& user){
            return user.nickname == nickName;
    });

    if((itFind == users.end()) || !userSelected)
        return;

    if((*userSelected).nickname != nickName)
        return;

    *itFind = *userSelected;
}

void coordinator::selectCurrentUserInList()
{
    if(!userSelected)
        return;

    auto nickname = (*userSelected).nickname;
    auto itFind = std::find_if(users.begin(), users.end(),[&nickname](const auto& user){
        return user.nickname == nickname;
    });

    if(itFind == users.end())
        return;

    auto dist = std::distance(users.begin(), itFind);

    interface.selectUserFromList(dist);
    interface.setNickNameEditable(false);
}

void coordinator::handleAddUserInfo(Serialize::ChatMessage& msg)
{
    const std::string& nickName = msg.sender();
    auto itFind = std::find_if(users.begin(), users.end(),[&nickName](const auto& user){
            return user.nickname == nickName;
    });

    if((itFind != users.end()) || !userSelected)
        return;

    if((*userSelected).nickname != nickName)
        return;

    users.push_back(*userSelected);
    interface.fillUsersList(getUsersNicknames());
    selectCurrentUserInList();
}

void coordinator::handleAddUserInfoError(Serialize::ChatMessage& msg)
{
    const std::string& nickName = msg.sender();
    std::string message = "Unable add user:" + nickName;
    QString outmsg = QString::fromStdString(message);
    interface.showMessage(outmsg);
}

std::vector<std::string> coordinator::getElements(const Serialize::StringVector& elements)
{
    std::vector<std::string> res;

    int size = elements.value_size();
    for(int i = 0; i < size; ++i)
        res.push_back(elements.value(i));

    return res;
}

std::optional<Database::userInfo> coordinator::findSelectedUser(const std::string& nick)
{
    auto it = std::find_if(users.begin(), users.end(), [&nick](const auto& user){
       return user.nickname == nick;
    });

    if(it != users.end())
        return *it;

    return std::nullopt;
}

void coordinator::sendGetDBCollectionNamesRequest(const std::string& dbName)
{
    serverCommunicator->sendGetDBCollectionNamesRequest(dbName);
}

void coordinator::sendGetAllUsersMessage(const std::string& dbName, const std::string& collName)
{
    serverCommunicator->sendGetAllUsersMessage(dbName, collName);
}

void coordinator::sendDeleteSelectedUserMessage(const std::string& dbName, const std::string& collName)
{
    if(!userSelected)
        return;
    serverCommunicator->sendDeleteSelectedUserMessage(dbName, collName, (*userSelected).nickname);
}

void coordinator::sendAddUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info)
{
    auto itFind = std::find_if(users.begin(), users.end(),[&info](const auto& user){
        return user.nickname == info.nickname;
    });

    userSelected = info;
    if(itFind == users.end())
        serverCommunicator->sendAddUserMessage(dbName, collName, info);
    else
        serverCommunicator->sendUpdateUserMessage(dbName, collName, info);
}

void coordinator::sendGetDBNamesMessage()
{
    serverCommunicator->sendGetDBNamesMessage();
}

void coordinator::sendAuthorizeMessage(const std::string& dbName)
{
    const auto&[login, password] = getLoginPassword(path);
    sendAuthorizeMessage(login, password, dbName);
}

void coordinator::sendAuthorizeMessage(const std::string& login, const std::string& password, const std::string& dbName)
{
    serverCommunicator->sendAuthorizeMessage(login, password, dbName);
}
