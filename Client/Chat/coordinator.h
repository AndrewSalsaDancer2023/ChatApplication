#pragma once

#include <QObject>
#include <QTimer>
#include "../../DatabaseAdmin/AdminTool/ServerCommunication/coordinatorinterface.h"
#include "../../Server/src/common_types.h"
#include "../../Server/src/database/DatabaseTypes.h"
#include "../../Server/build/messages.pb.h"
#include "../../DatabaseAdmin/AdminTool/ServerCommunication/servercommunicator.h"
#include "stringlistmodel.h"
#include "conversationmodel.h"
#include "participantmodel.h"
#include "chatstorage.h"
#include <string>
#include <map>
#include <functional>
#include <set>
#include <memory>

enum class commState
{
    Disconnected,
    Connected,
    Authenticated
};

namespace Database
{
    struct userInfo;
};

//class ChatStorage;

//https://blog.felgo.com/cross-platform-app-development/how-to-expose-a-qt-cpp-class-with-signals-and-slots-to-qml#how-to-register-your-cpp-class-as-a-qml-type
class Coordinator : public QObject, public NetworkCoordinatorInterface
{
    Q_OBJECT
    Q_PROPERTY(bool authorized READ isAuthorized WRITE setAuthorized NOTIFY clientAuthorized)

public:
    explicit Coordinator(QObject *parent = nullptr);
    void onDisconnected() override ;
    void onConnected() override;

    void messageReceived(Serialize::ChatMessage& message) override ;
    std::optional<Database::userInfo> findSelectedUser(const std::string& nick) override;
//    std::vector<std::string> getUsersNicknames();
    void sendGetDBCollectionNamesRequest(const std::string& dbName) override;
    void sendGetAllUsersMessage(const std::string& dbName, const std::string& collName) override;
    void sendDeleteSelectedUserMessage(const std::string& dbName, const std::string& collName) override;
    void sendAddUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info) override;
    void sendGetDBNamesMessage() override;
    void sendAuthorizeMessage(const std::string& dbName) override;
    void sendAuthorizeMessage(const std::string& login, const std::string& password, const std::string& dbName) override;

    void sendGetChatsContainUserMessage(const QString& dbName, const QString& collName, const QString& nickName);
    void sendAddUserToChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName);
    void sendDeleteUserFromChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName);
    void sendCreateChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const std::set<std::string>& participants);

    Q_INVOKABLE void setAuthenticationData(QString login, QString password, QString dbName);
    Q_INVOKABLE void mainWindowLoaded();
    Q_INVOKABLE void sendChatMessage(const QString& text);
    Q_INVOKABLE void setLoginScreenShown(bool shown) { loginScreenShown = shown; }
    Q_INVOKABLE void prepareUsersLists(int index);
    Q_INVOKABLE void prepareMembersList();
    Q_INVOKABLE void leaveFromChat();

    Q_INVOKABLE bool hasCorrectChatParticipants();
    Q_INVOKABLE void modifyChatParticipants(const QString& chat);
    Q_INVOKABLE void removeParticipant(const QString& nickName);
    Q_INVOKABLE void addParticipant(const QString& nickName);
    Q_INVOKABLE void createChat(const QString& chatTitle);

    Q_INVOKABLE QString getUserName();
    Q_INVOKABLE QString getUserSurname();
    Q_INVOKABLE QString getUserNickname();

//    void getChatTape(const QString& channel);
    void getChatTapes();
    bool isAuthorized() { return authorized; }
    void setAuthorized(bool res) { authorized = res; };

    StringListModel& getChatsList() { return chats; }
    ParticipantModel& getMembersList() { return members; }
    ConversationModel& getConversationModel() { return convModel;}

    ParticipantModel& getAllUsers() { return allUsers; }
    ParticipantModel& getParticipants() { return curParticipants; }

    Q_INVOKABLE QString getCurrentChatTitle() { return curChat; }
    Q_INVOKABLE void copyChatParticipants();
    void fillConversationModel(const std::string& item);
private slots:
    void onChatSelected(QString chatTitle);
    void onConnectionTimeout();
signals:
    void authSuccess();
    void authError(QString message);
    void clientAuthorized(bool result);
    void changeSendButtonState();
    void showLoginForm();

//    void chatsListChanged();
    void getChatsUserBelongSuccess();
    void getChatsUserBelongError(QString message);
    void clearMessageField();
    void showError(QString message);

    void showBusyIndicator();
    void hideBusyIndicator();

    void scrollListToTheEnd();
    void usersListObtained();
private:
    void handleAuthenticationSuccess(Serialize::ChatMessage& msg);
    void handleAuthenticationError(Serialize::ChatMessage& msg);

    void handleCreateChatMessageSuccess(Serialize::ChatMessage& msg);
    void handleCreateChatMessageError(Serialize::ChatMessage& msg);

    void handleGetChatsUserBelongsSuccess(Serialize::ChatMessage& msg);
    void handleGetChatsUserBelongsError(Serialize::ChatMessage& msg);

    void handleGetUsersInfoSuccess(Serialize::ChatMessage& msg);
    void handleGetUsersInfoError(Serialize::ChatMessage& msg);

    void handleGetMessageFromChat(Serialize::ChatMessage& msg);
    void handleGetMessageTapeFromChat(Serialize::ChatMessage& msg);
    void handleGetMessageTapeFromChatError(Serialize::ChatMessage& msg);

    void handleDeleteUsersFromChatError(Serialize::ChatMessage& msg);
    void handleAddUsersFromChatError(Serialize::ChatMessage& msg);

    void handleUpdateChatParticipants(Serialize::ChatMessage& msg);
    void handleDeleteUserFromChat(Serialize::ChatMessage& msg);
    void handleLeaveUserFromChat(Serialize::ChatMessage& msg);
    void handleLeaveUserFromChatError(Serialize::ChatMessage& msg);
//    void handleAddUserToChat(Serialize::ChatMessage& msg);

    commState state{commState::Disconnected};
    std::map<::google::protobuf::uint32, std::function<void(Serialize::ChatMessage&)> > handlers;
    std::unique_ptr<ServerCommunicator> serverCommunicator;

    void tryToLogin();
    std::set<std::string> getNickNames(ParticipantModel& model);
    std::set<std::string> participants;

    bool authorized{false};
    StringListModel chats;
    ParticipantModel members;
    ConversationModel convModel;
    ParticipantModel  allUsers;
    ParticipantModel  curParticipants;

    std::optional<Database::userInfo> userSelected;
    std::map<std::string, std::set<std::string>> chatsInfo;

    ChatStorage chatStorage;

    QString nickName;
    QString databaseName;
    QString usrPassword;

    QString curChat;
    bool loginScreenShown = false;
    std::shared_ptr<participantList> allUsersModel;
    std::shared_ptr<participantList> curPartcpantsModel;
    std::unique_ptr<QTimer> conTimer;
};
