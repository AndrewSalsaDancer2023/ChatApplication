#include "coordinator.h"
#include "../../DatabaseAdmin/AdminTool/ServerCommunication/messageserialization.h"
#include "../../DatabaseAdmin/AdminTool/ServerCommunication/authutils.h"
#include "commonutils.h"
#include <set>
#include <algorithm>
#include <iterator>

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

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_DELETE_USER_FROM_CHAT)] =
            [this](Serialize::ChatMessage& msg)
         {
            handleDeleteUserFromChat(msg);
         };


/*
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
*/
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

     handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_DELETE_USERS_FROM_CHAT_ERROR)] =
               [this](Serialize::ChatMessage& msg)
            {
               handleDeleteUsersFromChatError(msg);
            };

     handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_ADD_USERS_TO_CHAT_ERROR)] =
               [this](Serialize::ChatMessage& msg)
            {
               handleAddUsersFromChatError(msg);
            };
/*
     handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MODIFY_CHAT_USERS_SUCCESS)] =
               [this](Serialize::ChatMessage& msg)
            {
               handleModifyChatUsersSuccess(msg);
            };
*/
     handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_CHAT_PARTICIPANTS_UPDATED)] =
               [this](Serialize::ChatMessage& msg)
            {
               handleUpdateChatParticipants(msg);
            };

    connect(&chats, SIGNAL(itemSelected(QString)), this, SLOT(onChatSelected(QString)));

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

 void Coordinator::fillConversationModel(const std::string& item)
 {
     convModel.addMessages(chatStorage.getMessagesList(item));
     emit scrollListToTheEnd();
 }

void Coordinator::onChatSelected(QString chatTitle)
{
    curChat = chatTitle;
    members.setParticipants(chatStorage.getParticipants(curChat.toStdString()));
    fillConversationModel(curChat.toStdString());
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

std::set<std::string> Coordinator::getNickNames(ParticipantModel& model)
{
    std::set<std::string> result;
    for(int i = 0; i < model.rowCount(); ++i)
    {
        Database::Participant item = model.getItem(i);
        result.insert(item.nickname);
    }
    return result;
}

void Coordinator::prepareMembersList()
{
    curParticipants.removeAllData();
    allUsers.removeAllData();

    curPartcpantsModel = std::make_shared<participantList>();
    curParticipants.setParticipants(curPartcpantsModel);

    allUsersModel = chatStorage.copyUsers();
    allUsers.setParticipants(allUsersModel);
}

void Coordinator::prepareUsersLists(int index)
{
    if(index < 0)
        return;

    curParticipants.removeAllData();
    allUsers.removeAllData();

    QString title = chats.getItem(index);
    std::set<std::string> membersNicknames;

    for(int i = 0; i < members.rowCount(); ++i)
    {
        const auto& part = members.getItem(i);
        membersNicknames.insert(part.nickname);
//        curParticipants.addParticipant(part);
    }
    curPartcpantsModel = chatStorage.copyParticipants(title.toStdString());
    curParticipants.setParticipants(curPartcpantsModel);

    allUsersModel = chatStorage.copyUsersExceptChatMembers(membersNicknames);
    allUsers.setParticipants(allUsersModel);
    /*
    for(const auto& user: chatStorage.getUsers())
    {
        auto it = membersNicknames.find(user.nickname);
        if(it != membersNicknames.end())
            continue;

        allUsers.addParticipant(Participant{user.name, user.surname, user.nickname});
    }*/
/*
    curUsers = std::make_shared<std::vector<Database::Participant>>();
    for(const auto& user: chatStorage.getUsers())
    {
        auto it = membersNicknames.find(user.nickname);
        if(it != membersNicknames.end())
            continue;
        curUsers->push_back(std::move(Participant{user.name, user.surname, user.nickname}));
        //allUsers.addParticipant(Participant{user.name, user.surname, user.nickname});
    }
    allUsers.setParticipants(curUsers);
    */
}

void Coordinator::removeParticipant(const QString& nickName)
{
    auto part = curParticipants.getParticipant(nickName);
    if(!part)
        return;
    curParticipants.removeParticipant(nickName);
    allUsers.addParticipant(*part);
}

void Coordinator::addParticipant(const QString& nickName)
{
    auto part = allUsers.getParticipant(nickName);
    if(!part)
        return;
    allUsers.removeParticipant(nickName);
    curParticipants.addParticipant(*part);
}

void Coordinator::copyChatParticipants()
{
    participants = getNickNames(curParticipants);
    int k = 10;
}

void Coordinator::modifyChatParticipants(const QString& chat)
{
    std::set<std::string> dest = getNickNames(curParticipants);

    std::set<std::string> delUsrs, addUsrs;

    std::set_difference(participants.begin(), participants.end(), dest.begin(), dest.end(),
                             std::inserter(delUsrs, delUsrs.begin()));

    std::set_difference(dest.begin(), dest.end(), participants.begin(), participants.end(),
                             std::inserter(addUsrs, addUsrs.begin()));

    if(delUsrs.empty() && addUsrs.empty())
        return;

    serverCommunicator->sendModifyChatParticipantsMessage(databaseName.toStdString(), roomsCollName.toStdString(), chat.toStdString(), delUsrs, addUsrs);
}

bool Coordinator::hasCorrectChatParticipants()
{
    return (curParticipants.rowCount() <= 1) ? false : true;
}


void Coordinator::createChat(const QString& chatTitle/*, const QString& description*/)
{
    const QString collName{"chatrooms"};
    std::set<std::string> participants = getNickNames(curParticipants);
    sendCreateChatMessage(databaseName, collName, chatTitle, participants);
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

void Coordinator::handleDeleteUsersFromChatError(Serialize::ChatMessage& msg)
{
    const auto& chatTitle = msg.sender();
    QString message = "Unable to delete users from chat:"+QString::fromStdString(chatTitle);
    emit showError(message);
}

void Coordinator::handleAddUsersFromChatError(Serialize::ChatMessage& msg)
{
    const auto& chatTitle = msg.sender();
    QString message = "Unable to add users to chat:"+QString::fromStdString(chatTitle);
    emit showError(message);
}

void Coordinator::handleUpdateChatParticipants(Serialize::ChatMessage& msg)
{
    auto res = decodeParticipantsListMessage(msg);
    chatStorage.changeChatParticipants(res);

    if(curChat != QString::fromStdString(res.title))
        return;

    members.setParticipants(chatStorage.getParticipants(res.title));
}

void Coordinator::handleCreateChatMessageSuccess(Serialize::ChatMessage& msg)
{
    auto optInfo = decodeAddChatInfo(msg);
    if(!optInfo)
        return;

    const auto& [dbName, chatCollectionName, chatTitle, partcpants] = *optInfo;
    if(databaseName.toStdString() != dbName)
        return;

    Database::chatInfo info{chatTitle, partcpants};
    chatStorage.fillChatsInfo({info});
    chats.addData(QString::fromStdString(chatTitle));
}

void Coordinator::handleCreateChatMessageError(Serialize::ChatMessage& msg)
{
    const auto& chatTitle = msg.sender();
    int k = 10;
}

void Coordinator::handleGetChatsUserBelongsSuccess(Serialize::ChatMessage& msg)
{
  chatStorage.fillChatsInfo(decodeChatInfoMessages(msg));
  chats.addData(chatStorage.getChatTiltles());
  if(!chats.rowCount())
      return;
  curChat = chats.getItem(0);

  members.setParticipants(chatStorage.getParticipants(curChat.toStdString()));
  onChatSelected(curChat);
  getChatTapes();

//  emit changeSendButtonState();
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
/*    if(!msg.has_payload())
        return;

     Serialize::UserInfoVector elements;
     msg.mutable_payload()->UnpackTo(&elements);

    users = std::move(getUsers(elements));*/
    chatStorage.addUsers(decodeAllUsersMessage(msg));
    sendGetChatsContainUserMessage(databaseName, roomsCollName, nickName);
}

void Coordinator::handleGetUsersInfoError(Serialize::ChatMessage& msg)
{
    const std::string& reason = msg.sender();
    std::string message = "Unable get users info:" + reason;
    QString outmsg = QString::fromStdString(message);
    emit showError(outmsg);
}

void Coordinator::handleGetMessageFromChat(Serialize::ChatMessage& msg)
{
    auto decodedMsg = decodeChatMessage(msg);
    if((databaseName.toStdString() != decodedMsg.dbName) || (!chatStorage.chatExists(decodedMsg.chatTitle)))
        return;
//     convModel.addData(decodedMsg.message);
//    chatStorage.addChatMessage(decodedMsg.chatTitle, decodedMsg.message);
    convModel.addData(decodedMsg.message);
    emit scrollListToTheEnd();
}

void Coordinator::handleGetMessageTapeFromChat(Serialize::ChatMessage& msg)
{
    Database::chatMessagesTape tape = decodeMessageTapeFromChat(msg);

    if(databaseName.toStdString() != tape.dbName)
        return;

    if(!chatStorage.chatExists(tape.chatTitle))
        return;

    chatStorage.addMessagesList(tape.chatTitle, tape.messages);
    if(curChat.toStdString() == tape.chatTitle)
        fillConversationModel(tape.chatTitle);
}

void Coordinator::handleDeleteUserFromChat(Serialize::ChatMessage& msg)
{
    auto usrInfo = decodeDeleteUserFromChatMessage(msg);
    if(!usrInfo)
        return;
    if(databaseName.toStdString() != (*usrInfo).dbName)
        return;
    chats.removeData(QString::fromStdString((*usrInfo).chatTitle));
    convModel.removeAllData();
    chatStorage.deleteChat((*usrInfo).chatTitle);

    if(chats.rowCount() > 0)
        onChatSelected(chats.getItem(0));
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
    const auto& users = chatStorage.getUsers();
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

void Coordinator::sendCreateChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const std::set<std::string>& participants)
{
   /* std::vector<std::string> prtcpants;
    for (const auto& participant: participants)
        prtcpants.push_back(participant.toStdString());
*/
    serverCommunicator->sendCreateChatMessage(dbName.toStdString(), collName.toStdString(), chatTitle.toStdString(), participants);
}

void Coordinator::sendChatMessage(const QString& text)
{
    if(text.isEmpty())
        return;
     emit clearMessageField();

    serverCommunicator->sendMessageToChat(databaseName.toStdString(), curChat.toStdString(), nickName.toStdString(), text.toStdString());
}

void Coordinator::getChatTapes()
{
    for(const auto& title : chatStorage.getChatTiltles())
//    {
        serverCommunicator->sendGetMessageTapeFromChat(databaseName.toStdString(), title, nickName.toStdString());
//    return; //DEL
//    }
}
