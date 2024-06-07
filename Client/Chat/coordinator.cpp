#include "coordinator.h"
#include "../../DatabaseAdmin/AdminTool/ServerCommunication/messageserialization.h"
#include "../../DatabaseAdmin/AdminTool/ServerCommunication/authutils.h"
#include "commonutils.h"
#include <set>
#include <algorithm>
#include <iterator>

const QString configPath{"../../Server/data/config.json"};
const QString authPath{"./auth.json"};
const QString roomsCollName = "chatrooms";
const QString usersCollName = "users";

Coordinator::Coordinator(QObject *parent) :
    QObject(parent)
{
/*
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

     handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_LEAVE_USER_FROM_CHAT_SUCCESS)] =
               [this](Serialize::ChatMessage& msg)
            {
                //handleDeleteUserFromChat(msg);
                handleLeaveUserFromChat(msg);
            };

    handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_LEAVE_USER_FROM_CHAT_ERROR)] =
               [this](Serialize::ChatMessage& msg)
            {
                handleLeaveUserFromChatError(msg);
            };

     handlers[static_cast<::google::protobuf::uint32>(PayloadType::SERVER_CHAT_PARTICIPANTS_UPDATED)] =
               [this](Serialize::ChatMessage& msg)
            {
               handleUpdateChatParticipants(msg);
            };
*/
    setupHandlers();
    connect(&chats, SIGNAL(itemSelected(QString)), this, SLOT(onChatSelected(QString)));

    conTimer = std::make_unique<QTimer>();
    connect(conTimer.get(), &QTimer::timeout, this, &Coordinator::onConnectionTimeout);
    serverCommunicator = std::make_unique<ServerCommunicator>(*this);
    serverCommunicator->connectToHost();
}

void Coordinator::setupHandlers()
{
    handlers[static_cast<unsigned int>(PayloadType::CLIENT_AUTHENTICATION_APPROVED)] = &Coordinator::handleAuthenticationSuccess;
    handlers[static_cast<unsigned int>(PayloadType::CLIENT_AUTHENTICATION_REJECTED)] = &Coordinator::handleAuthenticationError;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_DELETE_USER_FROM_CHAT)] = &Coordinator::handleDeleteUserFromChat;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_CREATE_CHAT_SUCCESS)] = &Coordinator::handleCreateChatMessageSuccess;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_CREATE_CHAT_ERROR)] = &Coordinator::handleCreateChatMessageError;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_GET_CHATS_USER_BELONGS_TO_SUCCESS)] = &Coordinator::handleGetChatsUserBelongsSuccess;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_GET_CHATS_USER_BELONGS_TO_ERROR)] = &Coordinator::handleGetChatsUserBelongsError;

    handlers[static_cast<unsigned int>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE_SUCCESS)] = &Coordinator::handleGetUsersInfoSuccess;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE_ERROR)] = &Coordinator::handleGetUsersInfoError;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_SEND_MESSAGE_FROM_CHAT)] = &Coordinator::handleGetMessageFromChat;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_SEND_MESSAGE_TAPE_FROM_CHAT)] = &Coordinator::handleGetMessageTapeFromChat;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_SEND_MESSAGE_TAPE_FROM_CHAT_ERROR)] = &Coordinator::handleGetMessageTapeFromChatError;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_DELETE_USERS_FROM_CHAT_ERROR)] = &Coordinator::handleDeleteUsersFromChatError;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_ADD_USERS_TO_CHAT_ERROR)] = &Coordinator::handleAddUsersFromChatError;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_LEAVE_USER_FROM_CHAT_SUCCESS)] = &Coordinator::handleLeaveUserFromChat;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_LEAVE_USER_FROM_CHAT_ERROR)] = &Coordinator::handleLeaveUserFromChatError;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_CHAT_PARTICIPANTS_UPDATED)] = &Coordinator::handleUpdateChatParticipants;
}

 void Coordinator::tryToLogin()
 {
     if(isFileExists(authPath))
     {
         emit showBusyIndicator();
         const auto& [login, password, database] = getClientAuthentication(authPath);
         if(!login.isEmpty() && !password.isEmpty() && ! database.isEmpty())
         {
            nickName = login;
            databaseName = database;
            usrPassword = password;

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

 void Coordinator::fillConversationModel(const QString& chat)
 {
     convModel.addMessages(chatStorage.getMessagesList(chat));
     emit scrollListToTheEnd();
 }

void Coordinator::onChatSelected(QString chatTitle)
{
    curChat = chatTitle;
    members.setParticipants(chatStorage.getParticipants(curChat));
    fillConversationModel(curChat);
}

void Coordinator::mainWindowLoaded()
{
    sendGetAllUsersMessage(databaseName, usersCollName);
}
/*
std::optional<Participant> getParticipant(const std::string& name, const std::string& surname, const std::vector<Database::userInfo>& allUsers)
{
       auto it = std::find_if(allUsers.begin(), allUsers.end(), [&name, & surname](const auto& user){
               return (user.name == name) && (user.surname == surname);
            });

        if(it != allUsers.end())
            return Participant{(*it).name, (*it).surname, (*it).nickname};

    return {};
}
*/
QSet<QString> Coordinator::getNickNames(ParticipantModel& model)
{
    QSet<QString> result;
    for(int i = 0; i < model.rowCount(); ++i)
    {
        Backend::Participant item = model.getItem(i);
        result.insert(item.nickname);
    }
    return result;
}

void Coordinator::prepareMembersList()
{
    curParticipants.removeAllData();
    allUsers.removeAllData();

    curPartcpantsModel = std::make_shared<Backend::participantList>();
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
    std::set<QString> membersNicknames;

    for(int i = 0; i < members.rowCount(); ++i)
    {
        const auto& part = members.getItem(i);
        membersNicknames.insert(part.nickname);
//        curParticipants.addParticipant(part);
    }
    curPartcpantsModel = chatStorage.copyParticipants(title);
    curParticipants.setParticipants(curPartcpantsModel);

    allUsersModel = chatStorage.copyUsersExceptChatMembers(membersNicknames);
    allUsers.setParticipants(allUsersModel);
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
    QSet<QString> dest = getNickNames(curParticipants);
/*
    QSet<QString> delUsrs, addUsrs;

    std::set_difference(participants.begin(), participants.end(), dest.begin(), dest.end(),
                             std::inserter(delUsrs, delUsrs.begin()));

    std::set_difference(dest.begin(), dest.end(), participants.begin(), participants.end(),
                             std::inserter(addUsrs, addUsrs.begin()));
*/
    auto partCopy = participants; //TODO check if needed
    QSet<QString> &delUsrs = partCopy.subtract(dest);
//    partCopy = participants;
    QSet<QString> &addUsrs = dest.subtract(participants);

    if(delUsrs.empty() && addUsrs.empty())
        return;

    QString deletedUsers = "Deleted from chat:";

    QString addedUsers = "Added to chat:";

    serverCommunicator->sendModifyChatParticipantsMessage(databaseName, roomsCollName, chat,
                                                          delUsrs, addUsrs, nickName, addUsersToMessageTemplate(deletedUsers, delUsrs), addUsersToMessageTemplate(addedUsers, addUsrs));
}

bool Coordinator::hasCorrectChatParticipants()
{
    return (curParticipants.rowCount() <= 1) ? false : true;
}


void Coordinator::createChat(const QString& chatTitle/*, const QString& description*/)
{
    const QString collName{"chatrooms"};
    QSet<QString> particpnts = getNickNames(curParticipants);
    sendCreateChatMessage(databaseName, collName, chatTitle, particpnts);
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
    auto itHandler = handlers.find(static_cast<unsigned int>(message.payload_type_id()));
    if(itHandler != handlers.end())
        (this->*itHandler->second)(message);
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
    if(!chatStorage.chatExists(res.title))
    {
//        chatStorage.addParticipantsList(res.title, res.participants);
        chatStorage.fillChatsInfo({res});
        chats.addData(res.title);
        serverCommunicator->sendGetMessageTapeFromChat(databaseName, res.title, nickName);
        return;
    }
    chatStorage.changeChatParticipants(res);

    if(curChat != res.title)
        return;

    members.setParticipants(chatStorage.getParticipants(res.title));
}

void Coordinator::handleCreateChatMessageSuccess(Serialize::ChatMessage& msg)
{
    auto optInfo = decodeAddChatInfo(msg);
    if(!optInfo)
        return;

    const auto& [dbName, chatCollectionName, chatTitle, partcpants] = *optInfo;
    if(databaseName != dbName)
        return;

    Backend::chatInfo info{chatTitle, partcpants};
    chatStorage.fillChatsInfo({info});
    chats.addData(chatTitle);
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

  members.setParticipants(chatStorage.getParticipants(curChat));
  onChatSelected(curChat);
  getChatTapes();
}

void Coordinator::handleGetChatsUserBelongsError(Serialize::ChatMessage& msg)
{
    QString message = "Unable get chats for user:" + QString::fromStdString(msg.sender());
    emit getChatsUserBelongError(message);
}

QString Coordinator::getUserName()
{
    auto authInfo = chatStorage.getAuthUserInfo(nickName);
    QString res = authInfo ? (*authInfo).name : "";
    return res;

}

QString Coordinator::getUserSurname()
{
    auto authInfo = chatStorage.getAuthUserInfo(nickName);
    QString res = authInfo ? (*authInfo).surname : "";

    return res;
}

QString Coordinator::getUserNickname()
{
    auto authInfo = chatStorage.getAuthUserInfo(nickName);
    QString res = authInfo ? (*authInfo).nickname : "";

    return res;
}

void Coordinator::setAuthenticationData(QString login, QString password, QString dbName)
{
   nickName = login;
   databaseName = dbName;
   usrPassword = password;
   sendAuthorizeMessage(nickName, password, databaseName);
}

void Coordinator::handleGetUsersInfoSuccess(Serialize::ChatMessage& msg)
{
    chatStorage.addUsers(decodeAllUsersMessage(msg));
    sendGetChatsContainUserMessage(databaseName, roomsCollName, nickName);
    emit usersListObtained();
}

void Coordinator::handleGetUsersInfoError(Serialize::ChatMessage& msg)
{
//    const std::string& reason = msg.sender();
    QString message = "Unable get users info:" + QString::fromStdString(msg.sender());
//    QString outmsg = QString::fromStdString(message);
    emit showError(message);
}

void Coordinator::handleGetMessageFromChat(Serialize::ChatMessage& msg)
{
    auto decodedMsg = decodeChatMessage(msg);
    if((databaseName != decodedMsg.dbName) || (!chatStorage.chatExists(decodedMsg.chatTitle)))
        return;
//     convModel.addData(decodedMsg.message);
//    chatStorage.addChatMessage(decodedMsg.chatTitle, decodedMsg.message);
    convModel.addData(decodedMsg.message);
    emit scrollListToTheEnd();
}

void Coordinator::handleGetMessageTapeFromChat(Serialize::ChatMessage& msg)
{
    Backend::chatMessagesTape tape = decodeMessageTapeFromChat(msg);

    if(databaseName != tape.dbName)
        return;

    if(!chatStorage.chatExists(tape.chatTitle))
        return;

    chatStorage.addMessagesList(tape.chatTitle, tape.messages);
    if(curChat == tape.chatTitle)
        fillConversationModel(tape.chatTitle);
}

void Coordinator::handleDeleteUserFromChat(Serialize::ChatMessage& msg)
{
    auto usrInfo = decodeModifyParticipantsChatMessage(msg);
    if(!usrInfo)
        return;
    if(databaseName != (*usrInfo).dbName)
        return;
    chats.removeData((*usrInfo).chatTitle);
    convModel.removeAllData();
    chatStorage.deleteChat((*usrInfo).chatTitle);

    if(chats.rowCount() > 0)
        onChatSelected(chats.getItem(0));
}

void Coordinator::handleLeaveUserFromChat(Serialize::ChatMessage& msg)
{
    const auto& [dbName, collName, chatTitle, userToLeave, leaveMessage] = decodeLeaveUserFromChatInfo(msg);
    if((databaseName != dbName) || (roomsCollName != collName) || (nickName != userToLeave))
        return;

    if(nickName == userToLeave)
    {
        chats.removeData(chatTitle);
        chatStorage.deleteChat(chatTitle);
        //delete chat with chatTitle

        convModel.removeAllData();
        if(chats.rowCount() > 0)
            onChatSelected(chats.getItem(0));
    }
    else
    {
        chatStorage.removeChatParticipant(chatTitle, userToLeave);
        members.setParticipants(chatStorage.getParticipants(curChat));
    }
}

void Coordinator::handleLeaveUserFromChatError(Serialize::ChatMessage& msg)
{
    const auto& [dbName, collName, chatTitle, userToLeave, leaveMessage] = decodeLeaveUserFromChatInfo(msg);

    if((databaseName != dbName) || (roomsCollName != collName) || (nickName != userToLeave))
        return;

    QString outmsg = "Error while leaving from chat:"+ chatTitle;
    emit showError(outmsg);
}

/*
void Coordinator::handleAddUserToChat(Serialize::ChatMessage& msg)
{
    auto usrInfo = decodeModifyParticipantsChatMessage(msg);
    if(!usrInfo)
        return;
    if(databaseName.toStdString() != (*usrInfo).dbName)
        return;
    auto title = QString::fromStdString((*usrInfo).chatTitle);
    serverCommunicator->sendGetMessageTapeFromChat(databaseName.toStdString(), title.toStdString(), nickName.toStdString());

    chats.addData(title);
    int k = 10;
}
*/
void Coordinator::handleGetMessageTapeFromChatError(Serialize::ChatMessage& msg)
{
    QString message = "Unable get users info:" + QString::fromStdString(msg.sender());
    emit showError(message);
}

void Coordinator::leaveFromChat()
{
    const QString collName{"chatrooms"};
    QString leftChatPrefix = "Left from chat ";
//    QString tmplate = "Left from chat ";
//    std::string msg = createLeaveFromChatMessage(tmplate, nickName);
    serverCommunicator->sendLeaveFromChatMessage(databaseName, collName, curChat, nickName, leftChatPrefix);
}

std::optional<Backend::userInfo> Coordinator::findSelectedUser(const QString& nick)
{
/*    auto it = std::find_if(users.begin(), users.end(), [&nick](const auto& user){
       return user.nickname == nick;
    });

    if(it != users.end())
        return *it;
*/
    return std::nullopt;
}

void Coordinator::sendGetDBCollectionNamesRequest(const QString& dbName)
{
    serverCommunicator->sendGetDBCollectionNamesRequest(dbName);
}

void Coordinator::sendGetAllUsersMessage(const QString& dbName, const QString& collName)
{
    serverCommunicator->sendGetAllUsersMessage(dbName, collName);
}

void Coordinator::sendDeleteSelectedUserMessage(const QString& dbName, const QString& collName)
{
    if(!userSelected)
        return;
    serverCommunicator->sendDeleteSelectedUserMessage(dbName, collName, (*userSelected).nickname);
}

void Coordinator::sendAddUserMessage(const QString& dbName, const QString& collName, const Backend::userInfo& info)
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

void Coordinator::sendAuthorizeMessage(const QString& dbName)
{
    const auto&[login, password] = getLoginPassword(configPath);
    sendAuthorizeMessage(login, password, dbName);
}

void Coordinator::sendAuthorizeMessage(const QString& login, const QString& password, const QString& dbName)
{
    serverCommunicator->sendAuthorizeMessage(login, password, dbName);
}

void Coordinator::sendGetChatsContainUserMessage(const QString& dbName, const QString& collName, const QString& nickName)
{
    serverCommunicator->sendGetChatsContainUserMessage(dbName, collName, nickName);
}

void Coordinator::sendAddUserToChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName)
{
    serverCommunicator->sendAddUserToChatMessage(dbName, collName, chatTitle, nickName);
}

void Coordinator::sendDeleteUserFromChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName)
{
    serverCommunicator->sendDeleteUserFromChatMessage(dbName, collName, chatTitle, nickName);
}

void Coordinator::sendCreateChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QSet<QString>& participants)
{
    serverCommunicator->sendCreateChatMessage(dbName, collName, chatTitle, participants);
}

void Coordinator::sendChatMessage(const QString& text)
{
    if(text.isEmpty())
        return;
     emit clearMessageField();

    serverCommunicator->sendMessageToChat(databaseName, curChat, nickName, text);
}

void Coordinator::getChatTapes()
{
    for(const auto& title : chatStorage.getChatTiltles())
//    {
        serverCommunicator->sendGetMessageTapeFromChat(databaseName, title, nickName);
//    return; //DEL
//    }
}
