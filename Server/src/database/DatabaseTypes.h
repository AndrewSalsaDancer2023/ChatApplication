#pragma once
#include <string>
#include <vector>
#include <set>
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
	std::vector<std::string> participants;
};

struct chatInfoArray
{
	std::vector<chatInfo> chats;
};


struct userChatInfo
{
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
};

/*
struct userMessage
{
    std::string chatTitle;
    std::string userNickName;
    std::string userMessage;
    int64_t timestamp;
};

struct chatMessage
{
	std::string chatTitle;
    userMessage message;
    std::vector<userMessage> comments;
};
*/
struct singleUserMessage
{
	int64_t timestamp;
    std::string userNickName;
    std::string userMessage;
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

}
