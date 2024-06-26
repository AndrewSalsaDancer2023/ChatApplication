#pragma once
#include "parametersserialization.h"
#include <vector>

enum class PayloadType : unsigned int;
using ADDUserToChatInfo = std::tuple<std::string, std::string, std::string, std::set<std::string> >;
using ModifyUserInfo = std::tuple<std::string, std::string, std::string,
								  std::set<std::string>, std::set<std::string>, std::string, std::string, std::string>;
using LeaveUserFromChatInfo = std::tuple<std::string, std::string, std::string, std::string, std::string>;

namespace Database {
 struct userInfo;
 struct chatInfo;
 struct chatInfoArray;
 struct chatMessagesTape;
}

std::string readFileIntoString(const std::string& path);

class AuthentictionData {
    public:
    static AuthentictionData & instance()
    {
        static AuthentictionData inst;
        return inst;
    }

    const std::string& getLogin()
    {
	    return login;
    }
    
    const std::string& getPassword()
    {
	    return password;
    }

    private:
    std::string login;
    std::string password;    
    
    AuthentictionData()
    {
    	const std::string path = "../data/config.json";
    	std::string content = readFileIntoString(path);
//	std::cerr << "file cont:" << content;   
	
	Serialize::ConnectionData data = getConnectionDataFromString(content);
	if(data.has_authentication())
	{
	    const auto& auth = data.authentication();
	    login = auth.login();
//	    std::cerr << "login:" << login << std::endl;   
	    password = auth.password();
//    	    std::cerr << "passw:" << password << std::endl;   
	}
    }

      AuthentictionData(const AuthentictionData&) = delete;
      AuthentictionData& operator=(const AuthentictionData&) = delete;
      ~AuthentictionData() = default;
};

std::string serializeNoPayloadMessage(PayloadType type, const std::string& sender = "");
std::string serializeStringVectorMessage(PayloadType type, const std::vector<std::string>& values);
std::string serializeGetUsersMessage(PayloadType type, const std::vector<Database::userInfo>& users);
std::string serializeAllChatsUserBelongsToMessage(PayloadType type, const Database::chatInfoArray& chatsList);
std::string serializeChatMessagesTape(PayloadType type, const Database::chatMessagesTape& tape);
std::string serializeChatMessage(PayloadType type, Serialize::ChatMessage& msg);
std::string serializeGetChatParticipantsMessage(PayloadType type, const Database::chatInfo& info);
std::string serializeMessageToChat(const std::string& dbName, const std::string& chatCollectionName, const std::string& nickName, const std::string& message);

ADDUserToChatInfo extractAddChatInfo(Serialize::ChatMessage& msg);
ModifyUserInfo extractModifyUserInfo(Serialize::ChatMessage& msg);
LeaveUserFromChatInfo extractLeaveUserFromChatInfo(Serialize::ChatMessage& msg);

