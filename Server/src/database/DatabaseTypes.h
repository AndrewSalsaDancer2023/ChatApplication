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


struct userAuthInfo
{
    std::string nickname;
    std::string password;
};

struct userMessage
{
    std::string userNickName;
    std::string userMessage;
    std::chrono::time_point<std::chrono::system_clock> time;
//    std::chrono::time_point<std::chrono::high_resolution_clock> time;
};

struct chatMessage
{
    userMessage message;
    std::vector<userMessage> comments;
};
}
