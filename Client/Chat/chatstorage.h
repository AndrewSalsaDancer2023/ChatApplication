#pragma once
#include "../../Server/src/database/DatabaseTypes.h"
//#include <string>
#include <QString>
#include <memory>
#include <vector>
#include <QSet>

namespace Backend {

struct Participant
{
    QString name;
    QString surname;
    QString nickname;

    Participant(const Database::Participant& prtcipant)
    {
        name = QString::fromStdString(prtcipant.name);
        surname = QString::fromStdString(prtcipant.surname);
        nickname = QString::fromStdString(prtcipant.nickname);
    }

    Participant(const QString& othName, const QString& othSurname, const QString& othNickname)
    {
        name = othName;
        surname = othSurname;
        nickname = othNickname;
    }

    bool operator == (const Backend::Participant& other)
    {
        return (name == other.name) && (surname == other.surname) && (nickname == other.nickname);
    }

    friend bool operator==(const Backend::Participant& lhs, const Backend::Participant& rhs)
    {
        return (lhs.name == rhs.name) && (lhs.surname == rhs.surname) && (lhs.nickname == rhs.nickname);
    }
};

struct singleUserMessage
{
    int64_t timestamp{};
    QString userNickName;
    QString userMessage;

    singleUserMessage() = default;
    singleUserMessage(Database::singleUserMessage& usrMessage)
    {
        timestamp = std::move(usrMessage.timestamp);
        userNickName = QString::fromStdString(usrMessage.userNickName);
        userMessage = QString::fromStdString(usrMessage.userMessage);
    }

    friend bool operator==(const singleUserMessage& lhs, const singleUserMessage& rhs)
    {
        return (lhs.timestamp == rhs.timestamp) && (lhs.userNickName == rhs.userNickName) && (lhs.userMessage == rhs.userMessage);
    }
};

struct userInfo
{
    QString name;
    QString surname;
    QString email;
    QString nickname;

    QString password;
    QString profilepicture;

    bool deleted{false};

    userInfo(Database::userInfo& usrInfo)
    {
        name = QString::fromStdString(usrInfo.name);
        surname = QString::fromStdString(usrInfo.surname);
        email = QString::fromStdString(usrInfo.email);
        nickname =QString::fromStdString(usrInfo.nickname);

        password =QString::fromStdString(usrInfo.password);
        profilepicture =QString::fromStdString(usrInfo.profilepicture);

        deleted = usrInfo.deleted;
    }

    userInfo(const std::string& name, const std::string& surname, const std::string& email,
             const std::string& nickname, const std::string& password, const std::string& profpicuture, bool deleted)
    {
        this->name = QString::fromStdString(name);
        this->surname = QString::fromStdString(surname);
        this->email = QString::fromStdString(email);
        this->nickname = QString::fromStdString(nickname);

        this->password = QString::fromStdString(password);
        this->profilepicture = QString::fromStdString(profpicuture);

        this->deleted = deleted;
    }
};

struct chatData
{
    std::vector<Backend::Participant> participants;
    std::vector<Backend::singleUserMessage> messages;
};

struct userChatMessage
{
   QString dbName;
   QString chatTitle;
   singleUserMessage message;
};

struct chatMessagesTape
{
    QString dbName;
    QString chatTitle;
    std::vector<singleUserMessage> messages;
};

struct userChatInfo
{
    QString dbName;
    QString chatTitle;
    QString userNickname;
};

using chatDataModel = std::map<QString, std::shared_ptr<Backend::chatData>>;
using participantList = std::vector<Backend::Participant>;
using messageList = std::vector<Backend::singleUserMessage>;
using usersList = std::vector<Backend::userInfo>;

class ChatStorage
{
public:
    QSet<QString> getChatTiltles() const;
    std::shared_ptr<messageList> getMessagesList(const QString& title);
    void addMessagesList(const QString& title, messageList& msgList);
    void addChatMessage(const QString& title, Database::singleUserMessage& msg);
    void deleteChat(const QString& title);
    std::shared_ptr<participantList> getParticipants(const QString& title);
    std::shared_ptr<participantList> copyParticipants(const QString& title);
    std::shared_ptr<participantList> copyUsers();
    std::shared_ptr<participantList> copyUsersExceptChatMembers(const std::set<QString>& members);

    void addParticipantsList(const QString& title, participantList& partList);
    void fillChatsInfo(const std::vector<Backend::chatInfo>& chats);
    void changeChatParticipants(const Backend::chatInfo& chat);
    void addChatParticipant(const QString& title, Backend::Participant& participant);
    void removeChatParticipant(const QString& title, const QString& nickName);
    void addUsers(usersList usrs) { users = std::move(usrs); }
    const usersList& getUsers() { return users; }
    bool chatExists(const QString& title);
    std::optional<Backend::userInfo> getAuthUserInfo(const QString& nickname);
private:
    void addChatTilte(const QString& title);

private:
    usersList users;
    chatDataModel dataModel;
};
}
