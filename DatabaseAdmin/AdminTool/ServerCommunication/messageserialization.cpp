#include "messageserialization.h"
#include <string>
#include <map>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "../../Server/src/common_types.h"
#include "../../Server/src/security.h"
#include "../../Server/src/database/DatabaseTypes.h"
#include <google/protobuf/util/time_util.h>

std::string createAuthorizationMessage(const std::string& login, const std::string& password, const std::string& dbName)
{
    Serialize::Login logmsg;
    logmsg.set_login(login);
    auto hashedPassword = getHash(password);
    logmsg.set_password(hashedPassword);
    logmsg.set_database(dbName);
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
    Serialize::ChatMessage msg;
    msg.ParseFromString(message);

    return msg;
}

std::string createNoPayloadMessage(PayloadType type, const std::string& description)
{
    Serialize::ChatMessage msg;
    if(!description.empty())
        msg.set_sender(description);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));

     std::string out;
     if(msg.SerializeToString(&out))
         return out;

     return {};
}

std::string createGetAllUsersMessage(PayloadType type, const std::string& dbName, const std::string& collName)
{
    Serialize::GetUsers usrRequest;
    usrRequest.set_dbname(dbName);
    usrRequest.set_collectionname(collName);

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(usrRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

      return {};
}

Serialize::UserInfo createSerializableUserInfo(const Database::userInfo& info)
{
    Serialize::UserInfo usrinfo;

    usrinfo.set_name(std::move(info.name));
    usrinfo.set_surname(std::move(info.surname));
    usrinfo.set_email(std::move(info.email));
    usrinfo.set_nickname(std::move(info.nickname));
    usrinfo.set_password(std::move(info.password));
    usrinfo.set_profilepicture(std::move(info.profilepicture));
    usrinfo.set_deleted(info.deleted);

    return usrinfo;
}

std::string createAddUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info)
{
    Serialize::GetUsers usr;
    usr.set_dbname(dbName);
    usr.set_collectionname(collName);

    Serialize::UserInfo usrinfo = createSerializableUserInfo(info);

    Serialize::AddUser usrRequest;
    *usrRequest.mutable_passto() = usr;
    *usrRequest.mutable_info() = usrinfo;

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(usrRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_ADD_USER_TO_DATABASE));

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

      return {};
}

std::string createMarkUserAsDeletedMessage(const std::string& dbName, const std::string& collName, const std::string& nickName)
{
    Serialize::GetUsers usr;
    usr.set_dbname(dbName);
    usr.set_collectionname(collName);

    Serialize::UserInfo usrinfo;
    usrinfo.set_nickname(nickName);

    Serialize::AddUser usrRequest;
    *usrRequest.mutable_passto() = usr;
    *usrRequest.mutable_info() = usrinfo;

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(usrRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MARK_USER_AS_DELETED));

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

    return {};
}

std::string createDeleteUserMessage(const std::string& dbName, const std::string& collName, const std::string& nickName)
{
    Serialize::GetUsers usr;
    usr.set_dbname(dbName);
    usr.set_collectionname(collName);

    Serialize::UserInfo usrinfo;
    usrinfo.set_nickname(nickName);

    Serialize::AddUser usrRequest;
    *usrRequest.mutable_passto() = usr;
    *usrRequest.mutable_info() = usrinfo;

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(usrRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_DELETE_USER));

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

    return {};
}

std::string createUpdateUserMessage(const std::string& dbName, const std::string& collName, const Database::userInfo& info)
{
    Serialize::GetUsers usr;
    usr.set_dbname(dbName);
    usr.set_collectionname(collName);

    Serialize::UserInfo usrinfo = createSerializableUserInfo(info);

    Serialize::AddUser usrRequest;
    *usrRequest.mutable_passto() = usr;
    *usrRequest.mutable_info() = usrinfo;

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(usrRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MODIFY_USER_INFO));

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

      return {};
}

Serialize::AddRemoveUserChatInfo fillUserChatInfo(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::string& nickName)
{
    Serialize::AddRemoveUserChatInfo addUsrToChatRequest;

     addUsrToChatRequest.set_dbname(dbName);
     addUsrToChatRequest.set_collectionname(collName);

     addUsrToChatRequest.set_chattitle(chatTitle);
     addUsrToChatRequest.set_usernickname(nickName);

     return addUsrToChatRequest;
}

std::string createGetChatsContainUserMessage(const std::string& dbName, const std::string& collName, const std::string& nickName)
{
    auto addUsrToChatRequest = fillUserChatInfo(dbName, collName, "", nickName);
//      auto chatTitle = std::move(addUsrToChatRequest.chattitle());

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(addUsrToChatRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_CHATS_USER_BELONGS_TO));

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

    return {};
}

std::string createAddUserToChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::string& nickName)
{
    auto addUsrToChatRequest = fillUserChatInfo(dbName, collName, chatTitle, nickName);

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(addUsrToChatRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_ADD_USER_TO_CHAT));

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

    return {};
}

std::string createDeleteUserFromChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::string& nickName)
{
    auto addUsrToChatRequest = fillUserChatInfo(dbName, collName, chatTitle, nickName);

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(addUsrToChatRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_DELETE_USER_FROM_CHAT));

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

    return {};
}

std::string createChatMessage(const std::string& dbName, const std::string& collName, const std::string& chatTitle, const std::vector<std::string>& participants)
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

Serialize::userMessage fillUserMessage(const std::string& nickName, const std::string& message)
{
    Serialize::userMessage userMessage;
    userMessage.set_usernickname(nickName);
    userMessage.set_usermessage(message);

    ::google::protobuf::Timestamp timestamp;
    timestamp.set_seconds(time(NULL));
    timestamp.set_nanos(0);
    *userMessage.mutable_timestamp() = timestamp;

    return userMessage;
}

void handleReceiveInfoChatMessage(/*websocket_session& session,*/ Serialize::ChatMessage& msg)
{
    if(!msg.has_payload())
        return;

    Serialize::userChatMessage userChatMessage;
    msg.mutable_payload()->UnpackTo(&userChatMessage);

    if(!userChatMessage.has_message())
        return;

    auto dbName = std::move(userChatMessage.dbname());
    auto chatTitle = std::move(userChatMessage.chattitle());

    std::cout << dbName << std::endl;
    std::cout << chatTitle << std::endl;

    const Serialize::userMessage& message = userChatMessage.message();
    if(!message.has_timestamp())
        return;

    auto nickname = std::move(message.usernickname());
    auto mesg = std::move(message.usermessage());

//    std::chrono::nanoseconds ns = std::chrono::nanoseconds(google::protobuf::util::TimeUtil::TimestampToNanoseconds(message.timestamp()));
//    std::chrono::time_point<std::chrono::system_clock> timestamp(ns);

    std::chrono::milliseconds ms = std::chrono::milliseconds(google::protobuf::util::TimeUtil::TimestampToMilliseconds(message.timestamp()));
    std::chrono::time_point<std::chrono::system_clock> timestamp(ms);

//    std::string s(50, '\0');
//    std::strftime(&s[0], s.size(), "%H:%M:%S %Y-%m-%d", std::localtime(&timestamp));
//    std::cout << s << std::endl;
    auto in_time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::string s(50, '\0');
    std::strftime(&s[0], s.size(), "%H:%M:%S %Y-%m-%d", std::localtime(&in_time_t));
    std::cout << s << std::endl;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%d-%m-%Y %X");
    std::cout << ss.str() << std::endl;
}

std::string createInfoChatMessage(const std::string& dbName, const std::string& chatCollectionName, const std::string& nickName, const std::string& message)
{
    Serialize::userChatMessage userChatMessage;
    userChatMessage.set_dbname(dbName);
    userChatMessage.set_chattitle(chatCollectionName);

    *userChatMessage.mutable_message() = fillUserMessage(nickName, message);

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(userChatMessage);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_SEND_MESSAGE_TO_CHAT));

//    handleReceiveInfoChatMessage(msg);

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

    return {};
}

std::string createGetChatTapeMessage(const std::string& dbName, const std::string& chatCollectionName, const std::string& nickName)
{
    Serialize::getChatTapeMessage chatTapeMsg;
    chatTapeMsg.set_dbname(dbName);
    chatTapeMsg.set_chattitle(chatCollectionName);
    chatTapeMsg.set_usernickname(nickName);

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(chatTapeMsg);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_REQUEST_MESSAGE_TAPE_FOR_CHAT));

    std::string out;
    if(msg.SerializeToString(&out))
        return out;

    return {};
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

Database::chatMessagesTape decodeMessageTapeFromChat(Serialize::ChatMessage& msg)
{
    Serialize::chatTape chatTape;
    msg.mutable_payload()->UnpackTo(&chatTape);

    Database::chatMessagesTape chatContent;

    chatContent.dbName = std::move(chatTape.dbname());
    chatContent.chatTitle = std::move(chatTape.chattitle());

//    int numMessages = chatTape.messages_size();
    for(int i = 0; i < chatTape.messages_size(); ++i)
    {
        Serialize::userMessage* pCurMessage = chatTape.mutable_messages(i);
        if(!pCurMessage)
            continue;

        Database::singleUserMessage usrmsg;
        usrmsg.userNickName = std::move(pCurMessage->usernickname());
        usrmsg.userMessage = std::move(pCurMessage->usermessage());
        usrmsg.timestamp = google::protobuf::util::TimeUtil::TimestampToMilliseconds(pCurMessage->timestamp());

        chatContent.messages.push_back(std::move(usrmsg));
    }

    return chatContent;
}
/*
message userMessage
+{
+	google.protobuf.Timestamp timestamp = 1;
+    string userNickName = 2;
+    string userMessage = 3;
+}
+
+message userChatMessage
+{
+	string chatTitle = 1;
+	userMessage message = 2;
+}
+
+message chatMessages
+{
+	string chatTitle = 1;
+	repeated userMessage  messages = 2;
+}
*/
