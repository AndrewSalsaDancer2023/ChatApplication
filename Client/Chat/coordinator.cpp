#include "coordinator.h"
#include "../../DatabaseAdmin/AdminTool/ServerCommunication/messageserialization.h"
#include "../../DatabaseAdmin/AdminTool/ServerCommunication/authutils.h"
#include "commonutils.h"
#include <set>

const std::string configPath{"../../Server/data/config.json"};
const QString authPath{"./auth.json"};
const QString roomsCollName = "chatrooms";
const QString usersCollName = "users";

Coordinator::Coordinator(QObject *parent) :
    QObject(parent)
{
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

     handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_SEND_MESSAGE_FROM_CHAT)] =
               [this](Serialize::ChatMessage& msg)
            {
               handleGetMessageFromChat(msg);
            };

     handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_SEND_MESSAGE_TAPE_FROM_CHAT)] =
               [this](Serialize::ChatMessage& msg)
            {
               handleGetMessageTapeFromChat(msg);
            };

     handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_SEND_MESSAGE_TAPE_FROM_CHAT_ERROR)] =
               [this](Serialize::ChatMessage& msg)
            {
               handleGetMessageTapeFromChatError(msg);
            };


    connect(&chats, SIGNAL(itemSelected(std::string)), this, SLOT(onChatSelected(std::string)));

    conTimer = std::make_unique<QTimer>();
    connect(conTimer.get(), &QTimer::timeout, this, &Coordinator::onConnectionTimeout);
    serverCommunicator = std::make_unique<ServerCommunicator>(*this);
    serverCommunicator->connectToHost();
}

 void Coordinator::tryToLogin()
 {
     if(isFileExists(authPath))
     {
         emit showBusyIndicator();
         const auto& [login, password, database] = getClientAuthentication(authPath.toStdString());
         if(!login.empty() && !password.empty() && ! database.empty())
         {
            nickName = QString::fromStdString(login);
            databaseName = QString::fromStdString(database);
            usrPassword = QString::fromStdString(password);

            sendAuthorizeMessage(login, password, database);
            return;
         }
     }
     emit showLoginForm();
 }

 void Coordinator::onConnectionTimeout()
 {
    serverCommunicator->connectToHost();
 }

void Coordinator::onChatSelected(std::string item)
{
    int k = 10;
}

void Coordinator::mainWindowLoaded()
{
    sendGetAllUsersMessage(databaseName.toStdString(), usersCollName.toStdString());
}

std::optional<Participant> getParticipant(const std::string& name, const std::string& surname, const std::vector<Database::userInfo>& allUsers)
{
       auto it = std::find_if(allUsers.begin(), allUsers.end(), [&name, & surname](const auto& user){
               return (user.name == name) && (user.surname == surname);
            });

        if(it != allUsers.end())
            return Participant{(*it).name, (*it).surname, (*it).nickname};

    return {};
}

void Coordinator::prepareUsersLists(int index)
{
    if(index < 0)
        return;

    curParticipants.removeAllData();
    allUsers.removeAllData();

    QString title = chats.getItem(index);
    std::set<std::string> nicknames;
     std::set<Participant> usrs;
    for(int i = 0; i < members.rowCount(); ++i)
    {
        const auto& part = members.getItem(i);
        nicknames.insert(part.nickname);
        curParticipants.addData(part);
    }

    for(const auto& user: users)
    {
        auto it = nicknames.find(user.nickname);
        if(it != nicknames.end())
            continue;

        allUsers.addData(Participant{user.name, user.surname, user.nickname});
    }
}

void Coordinator::removeParticipant(const QString& nickName)
{
    auto part = curParticipants.getParticipant(nickName);
    if(!part)
        return;
    curParticipants.removeParticipant(nickName);
    allUsers.addData(*part);
}

void Coordinator::addParticipant(const QString& nickName)
{
    auto part = allUsers.getParticipant(nickName);
    if(!part)
        return;
    allUsers.removeParticipant(nickName);
    curParticipants.addData(*part);
}

bool Coordinator::hasCorrectChatParticipants()
{
    return false;
}

void Coordinator::onDisconnected()
{
    if(conTimer)
    {
        conTimer->stop();
        conTimer->start(10'000);
    }
    state = commState::Disconnected;

    emit showBusyIndicator();
}

void Coordinator::onConnected()
{
    state = commState::Connected;
    conTimer->stop();
    emit hideBusyIndicator();
    tryToLogin();
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
       if(loginScreenShown)
        saveClientAuthentication(authPath.toStdString(), nickName.toStdString(), usrPassword.toStdString(), databaseName.toStdString());
       emit authSuccess();
    }
}
/*
void Coordinator::saveAuthData()
{
    saveClientAuthentication(authPath.toStdString(), nickName.toStdString(), usrPassword.toStdString(), databaseName.toStdString());
    serializeAuthDataToString(nickName.toStdString(), usrPassword.toStdString(),
                                                databaseName.toStdString());
}
*/
void Coordinator::handleAuthenticationError(Serialize::ChatMessage& msg)
{
    if(loginScreenShown)
    {
        emit hideBusyIndicator();
        QString message = "Authentication failed!";
        emit authError(message);
    }
    else
        emit showLoginForm();
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
//refactor
void Coordinator::handleGetChatsUserBelongsSuccess(Serialize::ChatMessage& msg)
{
  if(!msg.has_payload())
    return;

  Serialize::chatInfoArray chatsInfo;
  msg.mutable_payload()->UnpackTo(&chatsInfo);

  for(int i = 0; i < chatsInfo.chats_size(); ++i)
  {
     const Serialize::chatInfo& curInfo = chatsInfo.chats(i);
     Database::chatInfo curChat;
     curChat.title = curInfo.title();

     for(int j = 0 ; j < curInfo.participants_size(); ++j)
     {
         curChat.participants.push_back(curInfo.participants(j));
         if(auto res = getUserInfo(users, curInfo.participants(j)))
            members.addData(*res);
     }
     chats.addData(curChat.title);
  }
  getChatTapes();
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
   usrPassword = password;
   sendAuthorizeMessage(nickName.toStdString(), password.toStdString(), databaseName.toStdString());
}

void Coordinator::handleGetUsersInfoSuccess(Serialize::ChatMessage& msg)
{
    if(!msg.has_payload())
        return;

     Serialize::UserInfoVector elements;
     msg.mutable_payload()->UnpackTo(&elements);

     users = std::move(getUsers(elements));
     sendGetChatsContainUserMessage(databaseName, roomsCollName, nickName);
}

void Coordinator::handleGetUsersInfoError(Serialize::ChatMessage& msg)
{
    const std::string& reason = msg.sender();
    std::string message = "Unable get users info:" + reason;
    QString outmsg = QString::fromStdString(message);
//    interface.showMessage(outmsg);
    emit showError(outmsg);
}

void Coordinator::handleGetMessageFromChat(Serialize::ChatMessage& msg)
{
    const auto& decodedMsg = decodeChatMessage(msg);
    if((databaseName.toStdString() != decodedMsg.dbName) || (!chats.containsData(decodedMsg.chatTitle)))
        return;
     convModel.addData(decodedMsg.message);
}

void Coordinator::handleGetMessageTapeFromChat(Serialize::ChatMessage& msg)
{
    Database::chatMessagesTape tape = decodeMessageTapeFromChat(msg);

    if(databaseName.toStdString() != tape.dbName)
        return;

    if(!chats.containsData(tape.chatTitle))
        return;

    for(const auto& msg : tape.messages)
        convModel.addData(msg);
}

void Coordinator::handleGetMessageTapeFromChatError(Serialize::ChatMessage& msg)
{
    const std::string& reason = msg.sender();
    std::string message = "Unable get users info:" + reason;
    QString outmsg = QString::fromStdString(message);
    emit showError(outmsg);
}

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
    const auto&[login, password] = getLoginPassword(configPath);
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
     emit clearMessageField();

    serverCommunicator->sendMessageToChat(databaseName.toStdString(), channel.toStdString(), nickName.toStdString(), text.toStdString());
}

void Coordinator::getChatTapes()
{
    for(int i = 0 ; i < chats.rowCount(); ++i)
        getChatTape(chats.getItem(i));
}

void Coordinator::getChatTape(const QString& channel)
{
    if(channel.isEmpty())
        return;

    serverCommunicator->sendGetMessageTapeFromChat(databaseName.toStdString(), channel.toStdString(), nickName.toStdString());
}

