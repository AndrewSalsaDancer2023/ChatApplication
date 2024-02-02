#ifndef SERVERCOMMUNICATOR_H
#define SERVERCOMMUNICATOR_H

#include <QObject>
#include <QUrl>
#include <QtWebSockets/QWebSocket>
#include "../../Server/build/messages.pb.h"
#include <memory.h>
#include <string>
#include <vector>
#include <set>

class NetworkCoordinatorInterface;

namespace Database
{
    struct userInfo;
}

class ServerCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit ServerCommunicator(NetworkCoordinatorInterface& coord, QObject *parent = nullptr);
    void sendGetDBCollectionNamesRequest(const std::string& dbName);
    void sendGetAllUsersMessage(const std::string& dbName, const std::string& collName);
    void sendDeleteSelectedUserMessage(const std::string& dbName, const std::string& collName, const std::string& nickName);
    void sendAddUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info);
    void sendUpdateUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info);
    void sendGetDBNamesMessage();
    void sendAuthorizeMessage(const std::string& login, const std::string& password, const std::string& dbName);

    void sendGetChatsContainUserMessage(const std::string& dbName, const std::string& collName, const std::string& nickName);
    void sendAddUserToChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::string& nickName);
    void sendDeleteUserFromChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::string& nickName);
    void sendCreateChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::set<std::string>& participants);
    void sendMessageToChat(const std::string& dbName, const std::string& chatCollectionName, const std::string& nickName, const std::string& message);
    void sendGetMessageTapeFromChat(const std::string& dbName, const std::string& chatCollectionName, const std::string& nickName);
    void sendModifyChatParticipantsMessage(const std::string& dbName, const std::string&  collName, const std::string&  chatTitle, std::set<std::string>& delUsrs, std::set<std::string>& addUsrs);
    void connectToHost();
signals:

public slots:
    void onDisconnected();
private Q_SLOTS:
    void onConnected();
    void onBinaryMessageReceived(const QByteArray &message);

//     commState state{commState::Disconnected};
private:
     QString host;
     QWebSocket m_webSocket;
     NetworkCoordinatorInterface& coordinator;
};

#endif // SERVERCOMMUNICATOR_H
