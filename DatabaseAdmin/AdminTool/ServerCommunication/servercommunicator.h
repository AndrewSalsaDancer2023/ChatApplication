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
#include <QSet>
#include <QString>

class NetworkCoordinatorInterface;

namespace Backend
{
    struct userInfo;
}

class ServerCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit ServerCommunicator(NetworkCoordinatorInterface& coord, QObject *parent = nullptr);
    void sendGetDBCollectionNamesRequest(const QString& dbName);
    void sendGetAllUsersMessage(const QString& dbName, const QString& collName);
    void sendDeleteSelectedUserMessage(const QString& dbName, const QString& collName, const QString& nickName);
    void sendAddUserMessage(const QString& dbName, const QString& collName, const Backend::userInfo& info);
    void sendUpdateUserMessage(const QString& dbName, const QString& collName, const Backend::userInfo& info);
    void sendGetDBNamesMessage();
    void sendAuthorizeMessage(const QString& login, const QString& password, const QString& dbName);

    void sendGetChatsContainUserMessage(const QString& dbName, const QString& collName, const QString& nickName);
    void sendAddUserToChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName);
    void sendDeleteUserFromChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName);
    void sendCreateChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QSet<QString>& participants);
    void sendMessageToChat(const QString& dbName, const QString& chatCollectionName, const QString& nickName, const QString& message);
    void sendGetMessageTapeFromChat(const QString& dbName, const QString& chatCollectionName, const QString& nickName);
    void sendModifyChatParticipantsMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QSet<QString>& delUsrs, const QSet<QString>& addUsrs,
                                           const QString& modifierNickName, const QString& delMessage, const QString& addMessage);
    void connectToHost();
    void sendLeaveFromChatMessage(const QString& dbName, const QString& chatCollectionName,
                                  const QString& chatTitle, const QString& nickName, const QString& leftChatPrefix);
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
