#include "coordinator.h"
#include "../../DatabaseAdmin/AdminTool/ServerCommunication/messageserialization.h"
#include "../../DatabaseAdmin/AdminTool/ServerCommunication/authutils.h"
#include "../../Server/src/database/DatabaseTypes.h"

Coordinator::Coordinator(QObject *parent) :
    QObject(parent)
{
    serverCommunicator = std::make_unique<ServerCommunicator>(*this);

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION_APPROVED)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleAuthenticationSuccess(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION_REJECTED)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleAuthenticationError(msg);
         };


    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_ADD_USER_TO_CHAT_SUCCESS)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleAddUserToChatSuccess(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_ADD_USER_TO_CHAT_ERROR)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleAddUserToChatError(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_DELETE_USER_FROM_CHAT_SUCCESS)] =
            [this](Serialize::ChatMessage& msg)
         {
            handledDeleteUserFromChatSuccess(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_DELETE_USER_FROM_CHAT_ERROR)] =
            [this](Serialize::ChatMessage& msg)
         {
            handledDeleteUserFromChatError(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_CREATE_CHAT_SUCCESS)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleCreateChatMessageSuccess(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_CREATE_CHAT_ERROR)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleCreateChatMessageError(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_CHATS_USER_BELONGS_TO_SUCCESS)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleGetChatsUserBelongsSuccess(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_CHATS_USER_BELONGS_TO_ERROR)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleGetChatsUserBelongsError(msg);
         };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE_SUCCESS)] =
               [this](Serialize::ChatMessage& msg)
            {
               handleGetUsersInfoSuccess(msg);
            };

     handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE_ERROR)] =
               [this](Serialize::ChatMessage& msg)
            {
               handleGetUsersInfoError(msg);
            };

    connect(&chats, SIGNAL(itemSelected(std::string)), this, SLOT(onChatSelected(std::string)));
//    connect(this, SIGNAL(weatherImageObtained(QString, unsigned)), this, SLOT(setForecastImage(QString, unsigned)));
     /*
     simplModel.setStringList(QStringList()
                                            << "Bill Smith"
                                            << "John Brown"
                                            << "Sam Wise"
                                            << "Ben Afleck"
                              << "Bill Smith"
                              << "John Brown"
                              << "Sam Wise"
                              << "Ben Afleck");

    PersonInfo one{"Bill Smith", "555 3264"};
    PersonInfo two{"John Brown", "555 8426"};
    PersonInfo three{"Sam Wise", "555 0473"};
    PersonInfo four{"Ben Afleck", "444 0478"};

    chats.addData(one);
    chats.addData(two);
    chats.addData(three);
    chats.addData(four); */
}

void Coordinator::onChatSelected(std::string item)
{
    int k = 10;
}

void Coordinator::mainWindowLoaded()
{
    QString dbName = "mydb", collName = "chatrooms", nickName = "batman";
    sendGetChatsContainUserMessage(dbName, collName, nickName);

    QString usersCollName = "users";
    sendGetAllUsersMessage(dbName.toStdString(), usersCollName.toStdString());
}


void Coordinator::onDisconnected()
{
    state = commState::Disconnected;
    QString message = "Socket disconnected!";
//    interface.showMessage(message);
}

void Coordinator::onConnected()
{
    state = commState::Connected;
}

void Coordinator::messageReceived(Serialize::ChatMessage& message)
{
    auto itHandler = handlers.find(message.payload_type_id());
    if(itHandler != handlers.end())
        itHandler->second(message);
}

void Coordinator::handleAuthenticationSuccess(Serialize::ChatMessage& msg)
{
    if((state == commState::Connected) &&
       (static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION_APPROVED) == msg.payload_type_id()))
    {
       state = commState::Authenticated;
       emit authSuccess();
/*       setAuthorized(true);
       emit clientAuthorized(true);*/
    }
}

void Coordinator::handleAuthenticationError(Serialize::ChatMessage& msg)
{
     QString message = "Authentication failed!";
    // interface.showMessage(message);
     emit authError(message);
/*     setAuthorized(false);
     emit clientAuthorized(false);*/
}

void Coordinator::handleAddUserToChatSuccess(Serialize::ChatMessage& msg)
{

}

void Coordinator::handleAddUserToChatError(Serialize::ChatMessage& msg)
{

}

void Coordinator::handledDeleteUserFromChatSuccess(Serialize::ChatMessage& msg)
{

}

void Coordinator::handledDeleteUserFromChatError(Serialize::ChatMessage& msg)
{

}

void Coordinator::handleCreateChatMessageSuccess(Serialize::ChatMessage& msg)
{

}

void Coordinator::handleCreateChatMessageError(Serialize::ChatMessage& msg)
{

}

void Coordinator::handleGetChatsUserBelongsSuccess(Serialize::ChatMessage& msg)
{
  if(!msg.has_payload())
    return;

  Serialize::chatInfoArray chatsInfo;
  msg.mutable_payload()->UnpackTo(&chatsInfo);

//  std::vector<Database::chatInfo> result;

  for(int i = 0; i < chatsInfo.chats_size(); ++i)
  {
     const Serialize::chatInfo& curInfo = chatsInfo.chats(i);
     Database::chatInfo curChat;
     curChat.title = curInfo.title();

     for(int j = 0 ; j < curInfo.participants_size(); ++j)
     {
         curChat.participants.push_back(curInfo.participants(j));
         participants.addData(curInfo.participants(j));
     }
     chats.addData(curChat.title);//curChat);
//     result.push_back(curChat);
  }

  emit changeSendButtonState();
}

void Coordinator::handleGetChatsUserBelongsError(Serialize::ChatMessage& msg)
{
    QString message = "Unable get chats for user:" + QString::fromStdString(msg.sender());
    emit getChatsUserBelongError(message);
}

void Coordinator::setAuthenticationData(QString login, QString password, QString dbName)
{
   nickName = login;
   databaseName = dbName;
   sendAuthorizeMessage(nickName.toStdString(), password.toStdString(), databaseName.toStdString());
}

void Coordinator::getUsers(Serialize::UserInfoVector& elements)
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

std::vector<std::string> Coordinator::getUsersNicknames()
{
    std::vector<std::string> res;
    for(const auto& elem : users)
        res.push_back(elem.nickname);

    return res;
}

void Coordinator::handleGetUsersInfoSuccess(Serialize::ChatMessage& msg)
{
    if(!msg.has_payload())
        return;

     Serialize::UserInfoVector elements;
     msg.mutable_payload()->UnpackTo(&elements);

     getUsers(elements);
}


void Coordinator::handleGetUsersInfoError(Serialize::ChatMessage& msg)
{
    const std::string& reason = msg.sender();
    std::string message = "Unable get users info:" + reason;
    QString outmsg = QString::fromStdString(message);
//    interface.showMessage(outmsg);
    emit showError(outmsg);
}

/*
void coordinator::handleDeleteUser(Serialize::ChatMessage& msg)
{
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
*/
std::optional<Database::userInfo> Coordinator::findSelectedUser(const std::string& nick)
{
/*    auto it = std::find_if(users.begin(), users.end(), [&nick](const auto& user){
       return user.nickname == nick;
    });

    if(it != users.end())
        return *it;
*/
    return std::nullopt;
}

void Coordinator::sendGetDBCollectionNamesRequest(const std::string& dbName)
{
    serverCommunicator->sendGetDBCollectionNamesRequest(dbName);
}

void Coordinator::sendGetAllUsersMessage(const std::string& dbName, const std::string& collName)
{
    serverCommunicator->sendGetAllUsersMessage(dbName, collName);
}

void Coordinator::sendDeleteSelectedUserMessage(const std::string& dbName, const std::string& collName)
{
    if(!userSelected)
        return;
    serverCommunicator->sendDeleteSelectedUserMessage(dbName, collName, (*userSelected).nickname);
}

void Coordinator::sendAddUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info)
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

void Coordinator::sendGetDBNamesMessage()
{
    serverCommunicator->sendGetDBNamesMessage();
}

void Coordinator::sendAuthorizeMessage(const std::string& dbName)
{
    const auto&[login, password] = getLoginPassword();
    sendAuthorizeMessage(login, password, dbName);
}

void Coordinator::sendAuthorizeMessage(const std::string& login, const std::string& password, const std::string& dbName)
{
    serverCommunicator->sendAuthorizeMessage(login, password, dbName);
}

void Coordinator::sendGetChatsContainUserMessage(const QString& dbName, const QString& collName, const QString& nickName)
{
    serverCommunicator->sendGetChatsContainUserMessage(dbName.toStdString(), collName.toStdString(), nickName.toStdString());
}

void Coordinator::sendAddUserToChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName)
{
    serverCommunicator->sendAddUserToChatMessage(dbName.toStdString(), collName.toStdString(), chatTitle.toStdString(), nickName.toStdString());
}

void Coordinator::sendDeleteUserFromChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName)
{
    serverCommunicator->sendDeleteUserFromChatMessage(dbName.toStdString(), collName.toStdString(), chatTitle.toStdString(), nickName.toStdString());
}

void Coordinator::sendCreateChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const std::vector<QString>& participants)
{
    std::vector<std::string> prtcpants;
    for (const auto& participant: participants)
        prtcpants.push_back(participant.toStdString());

    serverCommunicator->sendCreateChatMessage(dbName.toStdString(), collName.toStdString(), chatTitle.toStdString(), prtcpants);//participants);
}

void Coordinator::sendChatMessage(const QString& channel, const QString& text)
{
    if(text.isEmpty())
        return;
    //QString dbName = "mydb", collName = "chatrooms", nickName = "batman";
    serverCommunicator->sendMessageToChat(databaseName.toStdString(), channel.toStdString(), nickName.toStdString(), text.toStdString());
}

