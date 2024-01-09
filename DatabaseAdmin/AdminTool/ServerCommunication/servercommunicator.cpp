#include "servercommunicator.h"
#include "messageserialization.h"
#include <QString>
#include "authutils.h"
#include "../../Server/src/common_types.h"
#include "coordinatorinterface.h"
const std::string path{"../../Server/data/config.json"};

ServerCommunicator::ServerCommunicator(NetworkCoordinatorInterface& coord, QObject *parent)
    : QObject(parent),
      coordinator{coord}
{
    connect(&m_webSocket, &QWebSocket::connected, this, &ServerCommunicator::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &ServerCommunicator::onDisconnected);

    host = getHost(path);
}

void ServerCommunicator::connectToHost()
{
    m_webSocket.open(QUrl(host));
}

void ServerCommunicator::onConnected()
 {
     connect(&m_webSocket, &QWebSocket::binaryMessageReceived,
             this, &ServerCommunicator::onBinaryMessageReceived);
     coordinator.onConnected();
 }

 void ServerCommunicator::onDisconnected()
 {
//    QString message = "Socket disconnected!";
//    showMessage(message);
     coordinator.onDisconnected();
 }

 void ServerCommunicator::onBinaryMessageReceived(const QByteArray &message)
 {
      QString msg{message};
      auto curMsg = decodeMessageFromString(message.toStdString());
      auto id = curMsg.payload_type_id();
//      coordinator.messageReceived(QString{message}.toStdString());
      coordinator.messageReceived(curMsg);
 }

 void ServerCommunicator::sendGetDBCollectionNamesRequest(const std::string& dbName)
 {
     std::string res = createNoPayloadMessage(PayloadType::SERVER_GET_DB_COLLECTIONSNAMES, dbName);
/*     if(res.empty())
     {
         QString message = "Unable to encode message SERVER_GET_DB_NAMES";
         showMessage(message);
         return;
     }
*/
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendGetAllUsersMessage(const std::string& dbName, const std::string& collName)
 {
     std::string res = createGetAllUsersMessage(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE, dbName, collName);
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendDeleteSelectedUserMessage(const std::string& dbName, const std::string& collName, const std::string& nickName)
 {
     std::string res = createDeleteUserMessage(dbName, collName, nickName);
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendAddUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info)
 {
    std::string res = createAddUserMessage(dbName, collName, info);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendUpdateUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info)
 {
     std::string res = createUpdateUserMessage(dbName, collName, info);
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendGetDBNamesMessage()
 {
     std::string res = createNoPayloadMessage(PayloadType::SERVER_GET_DB_NAMES);
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendAuthorizeMessage(const std::string& login, const std::string& password, const std::string& dbName)
 {
     std::string res = createAuthorizationMessage(login, password, dbName);
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendGetChatsContainUserMessage(const std::string& dbName, const std::string& collName, const std::string& nickName)
 {
    std::string res = createGetChatsContainUserMessage(dbName, collName, nickName);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

void ServerCommunicator::sendAddUserToChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::string& nickName)
{
    std::string res = createAddUserToChatMessage(dbName, collName, chatTitle, nickName);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void ServerCommunicator::sendDeleteUserFromChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::string& nickName)
{
    std::string res = createDeleteUserFromChatMessage(dbName, collName, chatTitle, nickName);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void ServerCommunicator::sendCreateChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::vector<std::string>& participants)
{
    std::string res = createChatMessage(dbName, collName, chatTitle, participants);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void ServerCommunicator::sendMessageToChat(const std::string& dbName, const std::string& chatCollectionName, const std::string& nickName, const std::string& message)
{
    std::string res = createInfoChatMessage(dbName, chatCollectionName, nickName, message);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void ServerCommunicator::sendGetMessageTapeFromChat(const std::string& dbName, const std::string& chatCollectionName, const std::string& nickName)
{
    std::string res = createGetChatTapeMessage(dbName, chatCollectionName, nickName);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}
