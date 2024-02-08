#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <chrono>

namespace Database {


extern const std::string nameField;
extern const std::string surnameField;
extern const std::string emailField;
extern const std::string nicknameField;
extern const std::string passwordField;
extern const std::string photoField;
extern const std::string deletedField;

std::set<std::string> getuserInfoFieldTitles();

struct userInfo
{
    std::string name;
    std::string surname;
    std::string email;
    std::string nickname;
    std::string password;
    std::string profilepicture;
    bool deleted{false};
};

struct chatInfo
{
	std::string title;
    std::set<std::string> participants;
};

struct chatInfoArray
{
	std::vector<chatInfo> chats;
};


struct userChatInfo
{
    std::string dbName;
	std::string chatTitle;
	std::string userNickname;
};


struct userAuthInfo
{
    std::string nickname;
    std::string password;
};

struct Participant
{
    std::string name;
    std::string surname;
    std::string nickname;

    bool operator == (const Participant& other)
    {
        return (name == other.name) && (surname == other.surname) && (nickname == other.nickname);
    }

    friend bool operator==(const Participant& lhs, const Participant& rhs)
    {
        return (lhs.name == rhs.name) && (lhs.surname == rhs.surname) && (lhs.nickname == rhs.nickname);
    }
};


struct singleUserMessage
{
	int64_t timestamp;
    std::string userNickName;
    std::string userMessage;

    friend bool operator==(const singleUserMessage& lhs, const singleUserMessage& rhs)
    {
        return (lhs.timestamp == rhs.timestamp) && (lhs.userNickName == rhs.userNickName) && (lhs.userMessage == rhs.userMessage);
    }
};

struct userChatMessage
{
   std::string dbName;
   std::string chatTitle;
   singleUserMessage message;
};

struct chatMessagesTape
{
	std::string dbName;
	std::string chatTitle;
	std::vector<singleUserMessage> messages;
};

struct chatData
{
	std::vector<Participant> participants;
	std::vector<singleUserMessage> messages;
};

}
