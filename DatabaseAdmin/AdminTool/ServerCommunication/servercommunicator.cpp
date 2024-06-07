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
/*
    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
         [this](QAbstractSocket::SocketError error){
        QString err = m_webSocket.errorString();
        int k = 10;
    });
*/

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
//      auto curMsg = decodeMessageFromString(message.toStdString());
      auto curMsg = decodeMessageFromString(message);
//      auto id = curMsg.payload_type_id();

      coordinator.messageReceived(curMsg);
 }

 void ServerCommunicator::sendGetDBCollectionNamesRequest(const QString& dbName)
 {
     std::string res = createNoPayloadMessage(PayloadType::SERVER_GET_DB_COLLECTIONSNAMES, dbName);

     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendGetAllUsersMessage(const QString& dbName, const QString& collName)
 {
     std::string res = createGetAllUsersMessage(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE, dbName, collName);
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendDeleteSelectedUserMessage(const QString& dbName, const QString& collName, const QString& nickName)
 {
     std::string res = createDeleteUserMessage(dbName, collName, nickName);
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendAddUserMessage(const QString& dbName, const QString& collName, const Backend::userInfo& info)
 {
    std::string res = createAddUserMessage(dbName, collName, info);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendUpdateUserMessage(const QString& dbName, const QString& collName, const Backend::userInfo& info)
 {
     std::string res = createUpdateUserMessage(dbName, collName, info); //TODO refactor
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendGetDBNamesMessage()
 {
     std::string res = createNoPayloadMessage(PayloadType::SERVER_GET_DB_NAMES);
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendAuthorizeMessage(const QString& login, const QString& password, const QString& dbName)
 {
     std::string res = createAuthorizationMessage(login, password, dbName);
     m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

 void ServerCommunicator::sendGetChatsContainUserMessage(const QString& dbName, const QString& collName, const QString& nickName)
 {
    std::string res = createGetChatsContainUserMessage(dbName, collName, nickName);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
 }

void ServerCommunicator::sendAddUserToChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName)
{
    std::string res = createAddUserToChatMessage(dbName, collName, chatTitle, nickName);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void ServerCommunicator::sendDeleteUserFromChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName)
{
    std::string res = createDeleteUserFromChatMessage(dbName, collName, chatTitle, nickName);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void ServerCommunicator::sendCreateChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QSet<QString>& participants)
{
    std::string res = createChatMessage(dbName, collName, chatTitle, participants);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void ServerCommunicator::sendMessageToChat(const QString& dbName, const QString& chatCollectionName, const QString& nickName, const QString& message)
{
    std::string res = createInfoChatMessage(dbName, chatCollectionName, nickName, message);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void ServerCommunicator::sendGetMessageTapeFromChat(const QString& dbName, const QString& chatCollectionName, const QString& nickName)
{
    std::string res = createGetChatTapeMessage(dbName, chatCollectionName, nickName);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void ServerCommunicator::sendModifyChatParticipantsMessage(const QString& dbName, const QString&  collName, const QString&  chatTitle, const QSet<QString>& delUsrs, const QSet<QString>& addUsrs,
                                                           const QString& modifierNickName, const QString& delMessage, const QString& addMessage)
{
    QSet<QString> deletedUsrs;
    QSet<QString> addedUsrs;

    for(const auto& usr: delUsrs)
        deletedUsrs.insert(usr);

    for(const auto& usr: addUsrs)
        addedUsrs.insert(usr);

    std::string res = createModifyChatParticipantsMessage(dbName, collName, chatTitle, deletedUsrs, addedUsrs, modifierNickName, delMessage, addMessage);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}

void ServerCommunicator::sendLeaveFromChatMessage(const QString& dbName, const QString& chatCollectionName,
                                                  const QString& chatTitle, const QString& nickName,
                                                  const QString& leftChatPrefix)
{
    std::string res = createLeaveFromChatMessage(dbName, chatCollectionName, chatTitle, nickName, leftChatPrefix);
    m_webSocket.sendBinaryMessage(QByteArray::fromStdString(res));
}
