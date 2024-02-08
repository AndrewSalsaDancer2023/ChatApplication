#include "messageserialization.h"
#include <string>
#include <iostream>
#include "../../Server/src/common_types.h"
#include "../../Server/src/database/DatabaseTypes.h"
#include <google/protobuf/util/time_util.h>

std::string createAuthorizationMessage(const std::string& login, const std::string& password)
{

    Serialize::Login logmsg;
    logmsg.set_login(login);
    logmsg.set_password(password);

    // Use the Any::PackFrom to encode an arbitrary PB payload.
    Serialize::ChatMessage msg;
     msg.mutable_payload()->PackFrom(logmsg);
     msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION));

     std::string out;
     if(msg.SerializeToString(&out))
         return out;

     return {};
}

Serialize::ChatMessage decodeMessageFromString(const std::string& message)
{
	std::cout << "decoding message:" << message << std::endl;
    Serialize::ChatMessage msg;
    if(!msg.ParseFromString(message))
    {
    	std::cout << "Error ParseFromString:" << message << std::endl;
    }

    return msg;
}

Database::userChatMessage decodeChatMessage(Serialize::ChatMessage& msg)
{
    Serialize::userChatMessage userChatMessage;
    msg.mutable_payload()->UnpackTo(&userChatMessage);

    if(!userChatMessage.has_message())
        return {};

    const Serialize::userMessage& message = userChatMessage.message();
    if(!message.has_timestamp())
        return {};

    Database::userChatMessage packMessage;
    packMessage.dbName = std::move(userChatMessage.dbname());
    packMessage.chatTitle = std::move(userChatMessage.chattitle());

    packMessage.message.userNickName = std::move(message.usernickname());
    packMessage.message.userMessage = std::move(message.usermessage());
    packMessage.message.timestamp =
        google::protobuf::util::TimeUtil::TimestampToMilliseconds(message.timestamp());

    return packMessage;
}

std::string createChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::set<std::string>& participants)
{
    Serialize::CreateChatInfo createChatRequest;
    createChatRequest.set_dbname(dbName);
    createChatRequest.set_collectionname(collName);
    createChatRequest.set_chattitle(chatTitle);

    for(const auto& participant: participants)
        createChatRequest.add_participants(participant);

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(createChatRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_CREATE_CHAT));

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

    return {};
}

std::string createRemoveUserFromChatMessage(const std::string& dbName, const std::string& chatTitle, const std::string& userNick)
{
	Serialize::userChatInfo info;
	info.set_dbname(dbName);
	info.set_chattitle(chatTitle);
	info.set_usernickname(userNick);

	Serialize::ChatMessage msg;
	msg.mutable_payload()->PackFrom(info);
	msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_DELETE_USER_FROM_CHAT));

   std::string out;
   if(msg.SerializeToString(&out))
       return out;

   return {};
}
