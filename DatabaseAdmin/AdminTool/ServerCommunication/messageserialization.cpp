#include "messageserialization.h"
#include <string>
#include <map>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "../../Server/src/common_types.h"
#include "../../Server/src/security.h"
#include "../../Server/src/database/DatabaseTypes.h"
#include "../../Client/Chat/commonutils.h"
#include "../../Client/Chat/chatstorage.h"
#include <google/protobuf/util/time_util.h>

std::string createAuthorizationMessage(const QString& login, const QString& password, const QString& dbName)
{
    Serialize::Login logmsg;
    logmsg.set_login(login.toStdString());
    auto hashedPassword = getHash(password.toStdString());
    logmsg.set_password(hashedPassword);
    logmsg.set_database(dbName.toStdString());
    // Use the Any::PackFrom to encode an arbitrary PB payload.
    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(logmsg);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
/*    if(msg.SerializeToString(&out))
        return out;
    return {};*/
}

Serialize::ChatMessage decodeMessageFromString(const QString& message)
{
    Serialize::ChatMessage msg;
    msg.ParseFromString(message.toStdString());

    return msg;
}

std::string createNoPayloadMessage(PayloadType type, const QString& description)
{
    Serialize::ChatMessage msg;
    if(!description.isEmpty())
        msg.set_sender(description.toStdString());
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));

     std::string out;
     return (msg.SerializeToString(&out) == true) ? out : "";
 /*    if(msg.SerializeToString(&out))
         return out;

     return {};*/
}

std::string createGetAllUsersMessage(PayloadType type, const QString& dbName, const QString& collName)
{
    Serialize::GetUsers usrRequest;
    usrRequest.set_dbname(dbName.toStdString());
    usrRequest.set_collectionname(collName.toStdString());

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(usrRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
/*    if(msg.SerializeToString(&out))
        return out;

      return {};*/
}

Serialize::UserInfo createSerializableUserInfo(const Backend::userInfo& info)
{
    Serialize::UserInfo usrinfo;

    usrinfo.set_name(std::move(info.name.toStdString()));
    usrinfo.set_surname(std::move(info.surname.toStdString()));
    usrinfo.set_email(std::move(info.email.toStdString()));
    usrinfo.set_nickname(std::move(info.nickname.toStdString()));
    usrinfo.set_password(std::move(info.password.toStdString()));
    usrinfo.set_profilepicture(std::move(info.profilepicture.toStdString()));
    usrinfo.set_deleted(info.deleted);

    return usrinfo;
}

std::string createAddUserMessage(const QString& dbName, const QString& collName, const Backend::userInfo& info)
{
    Serialize::GetUsers usr;
    usr.set_dbname(dbName.toStdString());
    usr.set_collectionname(collName.toStdString());

    Serialize::UserInfo usrinfo = createSerializableUserInfo(info);

    Serialize::AddUser usrRequest;
    *usrRequest.mutable_passto() = usr;
    *usrRequest.mutable_info() = usrinfo;

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(usrRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_ADD_USER_TO_DATABASE));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
/*    if(msg.SerializeToString(&out))
        return out;

      return {};*/
}

std::string createMarkUserAsDeletedMessage(const QString& dbName, const QString& collName, const QString& nickName)
{
    Serialize::GetUsers usr;
    usr.set_dbname(dbName.toStdString());
    usr.set_collectionname(collName.toStdString());

    Serialize::UserInfo usrinfo;
    usrinfo.set_nickname(nickName.toStdString());

    Serialize::AddUser usrRequest;
    *usrRequest.mutable_passto() = usr;
    *usrRequest.mutable_info() = usrinfo;

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(usrRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MARK_USER_AS_DELETED));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
 /*   if(msg.SerializeToString(&out))
        return out;

    return {};*/
}

std::string createDeleteUserMessage(const QString& dbName, const QString& collName, const QString& nickName)
{
    Serialize::GetUsers usr;
    usr.set_dbname(dbName.toStdString());
    usr.set_collectionname(collName.toStdString());

    Serialize::UserInfo usrinfo;
    usrinfo.set_nickname(nickName.toStdString());

    Serialize::AddUser usrRequest;
    *usrRequest.mutable_passto() = usr;
    *usrRequest.mutable_info() = usrinfo;

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(usrRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_DELETE_USER));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
/*    if(msg.SerializeToString(&out))
        return out;

    return {};*/
}

std::string createUpdateUserMessage(const QString& dbName, const QString& collName, const Backend::userInfo& info)
{
    Serialize::GetUsers usr;
    usr.set_dbname(dbName.toStdString());
    usr.set_collectionname(collName.toStdString());

    Serialize::UserInfo usrinfo = createSerializableUserInfo(info);

    Serialize::AddUser usrRequest;
    *usrRequest.mutable_passto() = usr;
    *usrRequest.mutable_info() = usrinfo;

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(usrRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MODIFY_USER_INFO));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
/*    if(msg.SerializeToString(&out))
        return out;

      return {};*/
}

Serialize::AddRemoveUserChatInfo fillUserChatInfo(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName)
{
    Serialize::AddRemoveUserChatInfo addUsrToChatRequest;

     addUsrToChatRequest.set_dbname(dbName.toStdString());
     addUsrToChatRequest.set_collectionname(collName.toStdString());

     addUsrToChatRequest.set_chattitle(chatTitle.toStdString());
     addUsrToChatRequest.set_usernickname(nickName.toStdString());

     return addUsrToChatRequest;
}

std::string createGetChatsContainUserMessage(const QString& dbName, const QString& collName, const QString& nickName)
{
    auto addUsrToChatRequest = fillUserChatInfo(dbName, collName, "", nickName);
//      auto chatTitle = std::move(addUsrToChatRequest.chattitle());

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(addUsrToChatRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_CHATS_USER_BELONGS_TO));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
/*    if(msg.SerializeToString(&out))
        return out;

    return {};*/
}

std::string createAddUserToChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName)
{
    auto addUsrToChatRequest = fillUserChatInfo(dbName, collName, chatTitle, nickName);

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(addUsrToChatRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_ADD_USER_TO_CHAT));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
/*    if(msg.SerializeToString(&out))
        return out;

    return {};*/
}

std::string createDeleteUserFromChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QString& nickName)
{
    auto addUsrToChatRequest = fillUserChatInfo(dbName, collName, chatTitle, nickName);

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(addUsrToChatRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_DELETE_USER_FROM_CHAT));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
 /*   if(msg.SerializeToString(&out))
        return out;

    return {};*/
}

std::string createChatMessage(const QString& dbName, const QString& collName, const QString& chatTitle, const QSet<QString>& participants)
{
    Serialize::CreateChatInfo createChatRequest;
    createChatRequest.set_dbname(dbName.toStdString());
    createChatRequest.set_collectionname(collName.toStdString());
    createChatRequest.set_chattitle(chatTitle.toStdString());

    for(const auto& participant: participants)
        createChatRequest.add_participants(participant.toStdString());

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(createChatRequest);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_CREATE_CHAT));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
/*    if(msg.SerializeToString(&out))
        return out;

    return {};*/
}

Serialize::userMessage fillUserMessage(const QString& nickName, const QString& message)
{
    Serialize::userMessage userMessage;
    userMessage.set_usernickname(nickName.toStdString());
    userMessage.set_usermessage(message.toStdString());

    ::google::protobuf::Timestamp timestamp;
    timestamp.set_seconds(time(NULL));
    timestamp.set_nanos(0);
    *userMessage.mutable_timestamp() = timestamp;

    return userMessage;
}

void handleReceiveInfoChatMessage(Serialize::ChatMessage& msg)
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

//    auto nickname = std::move(message.usernickname());
//    auto mesg = std::move(message.usermessage());

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

std::string createInfoChatMessage(const QString& dbName, const QString& chatCollectionName, const QString& nickName, const QString& message)
{
    Serialize::userChatMessage userChatMessage;
    userChatMessage.set_dbname(dbName.toStdString());
    userChatMessage.set_chattitle(chatCollectionName.toStdString());

    *userChatMessage.mutable_message() = fillUserMessage(nickName, message);

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(userChatMessage);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_SEND_MESSAGE_TO_CHAT));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
/*    if(msg.SerializeToString(&out))
        return out;

    return {};*/
}

std::string createGetChatTapeMessage(const QString& dbName, const QString& chatCollectionName, const QString& nickName)
{
    Serialize::getChatTapeMessage chatTapeMsg;
    chatTapeMsg.set_dbname(dbName.toStdString());
    chatTapeMsg.set_chattitle(chatCollectionName.toStdString());
    chatTapeMsg.set_usernickname(nickName.toStdString());

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(chatTapeMsg);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_REQUEST_MESSAGE_TAPE_FOR_CHAT));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
 /*   if(msg.SerializeToString(&out))
        return out;

    return {};*/
}

std::string createLeaveFromChatMessage(const QString& dbName, const QString& chatCollectionName,
                                       const QString& chatTitle, const QString& nickName,
                                       const QString& leftMessagePrefix)
{
    Serialize::LeaveUserFromChatInfo leaveUsrMsg;

    leaveUsrMsg.set_dbname(dbName.toStdString());
    leaveUsrMsg.set_collectionname(chatCollectionName.toStdString());
    leaveUsrMsg.set_chattitle(chatTitle.toStdString());

    leaveUsrMsg.set_usertodelete(nickName.toStdString());
    QString leftMessage = leftMessagePrefix + nickName;
    leaveUsrMsg.set_leavemessage(leftMessage.toStdString());

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(leaveUsrMsg);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_LEAVE_USER_FROM_CHAT));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
  /*  if(msg.SerializeToString(&out))
        return out;

    return {};*/
}


Backend::userChatMessage decodeChatMessage(Serialize::ChatMessage& msg)
{
    Serialize::userChatMessage userChatMessage;
    msg.mutable_payload()->UnpackTo(&userChatMessage);

    if(!userChatMessage.has_message())
        return {};

    const Serialize::userMessage& message = userChatMessage.message();
    if(!message.has_timestamp())
        return {};

    Backend::userChatMessage packMessage;
/*  packMessage.dbName = std::move(userChatMessage.dbname());
    packMessage.chatTitle = std::move(userChatMessage.chattitle());

    packMessage.message.userNickName = std::move(message.usernickname());
    packMessage.message.userMessage = std::move(message.usermessage()); */
    packMessage.dbName = QString::fromStdString(userChatMessage.dbname());
    packMessage.chatTitle = QString::fromStdString(userChatMessage.chattitle());

    packMessage.message.userNickName = QString::fromStdString(message.usernickname());
    packMessage.message.userMessage = QString::fromStdString(message.usermessage());
    packMessage.message.timestamp =
        google::protobuf::util::TimeUtil::TimestampToMilliseconds(message.timestamp());

    return packMessage;
}

Backend::chatMessagesTape decodeMessageTapeFromChat(Serialize::ChatMessage& msg)
{
    Backend::chatMessagesTape chatContent;
    if(!msg.has_payload())
        return chatContent;

    Serialize::chatTape chatTape;
    msg.mutable_payload()->UnpackTo(&chatTape);

    chatContent.dbName = QString::fromStdString(chatTape.dbname());
    chatContent.chatTitle = QString::fromStdString(chatTape.chattitle());

    for(int i = 0; i < chatTape.messages_size(); ++i)
    {
        Serialize::userMessage* pCurMessage = chatTape.mutable_messages(i);
        if(!pCurMessage)
            continue;
/*
        Database::singleUserMessage usrmsg;
        usrmsg.userNickName = std::move(pCurMessage->usernickname());
        usrmsg.userMessage = std::move(pCurMessage->usermessage());
        usrmsg.timestamp = google::protobuf::util::TimeUtil::TimestampToMilliseconds(pCurMessage->timestamp());*/

        Backend::singleUserMessage usrmsg;
        usrmsg.userNickName = QString::fromStdString(pCurMessage->usernickname());
        usrmsg.userMessage = QString::fromStdString(pCurMessage->usermessage());
        usrmsg.timestamp = google::protobuf::util::TimeUtil::TimestampToMilliseconds(pCurMessage->timestamp());

        chatContent.messages.push_back(std::move(usrmsg));
    }

    return chatContent;
}

Backend::chatInfo decodeParticipantsListMessage(Serialize::ChatMessage& msg)
{
    Backend::chatInfo partInfo;
    if(!msg.has_payload())
      return partInfo;

    Serialize::chatInfo srlInfo;
    msg.mutable_payload()->UnpackTo(&srlInfo);

    partInfo.title = QString::fromStdString(srlInfo.title());

    for(int i = 0; i < srlInfo.participants_size(); ++i)
        partInfo.participants.insert(QString::fromStdString(std::move(*srlInfo.mutable_participants(i))));

    return partInfo;
}

std::string createModifyChatParticipantsMessage(const QString& dbName, const QString& collName,
                                                const QString& chatTitle, const QSet<QString>& delUsrs,
                                                const QSet<QString>& addUsrs, const QString& modifierNickName,
                                                const QString& delMessage, const QString& addMessage)
{
    Serialize::ModifyUsersChatInfo modifyInfoMsg;

    modifyInfoMsg.set_dbname(dbName.toStdString());
    modifyInfoMsg.set_collectionname(collName.toStdString());
    modifyInfoMsg.set_chattitle(chatTitle.toStdString());

    for(auto user: delUsrs)
        modifyInfoMsg.add_userstodelete(std::move(user.toStdString()));

    for(auto user: addUsrs)
        modifyInfoMsg.add_userstoadd(std::move(user.toStdString()));

    modifyInfoMsg.set_modifiernick(modifierNickName.toStdString());

    modifyInfoMsg.set_delmessage(delMessage.toStdString());
    modifyInfoMsg.set_addmessage(addMessage.toStdString());

    Serialize::ChatMessage msg;
    msg.mutable_payload()->PackFrom(modifyInfoMsg);
    msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MODIFY_CHAT_USERS_LIST));

    std::string out;
    return (msg.SerializeToString(&out) == true) ? out : "";
/*    if(msg.SerializeToString(&out))
        return out;

    return {};*/

}

std::vector<Backend::chatInfo> decodeChatInfoMessages(Serialize::ChatMessage& msg)
{
    std::vector<Backend::chatInfo> res;

    if(!msg.has_payload())
      return res;

    Serialize::chatInfoArray chatsInfo;
    msg.mutable_payload()->UnpackTo(&chatsInfo);

    for(int i = 0; i < chatsInfo.chats_size(); ++i)
    {
       const Serialize::chatInfo& curInfo = chatsInfo.chats(i);
       Backend::chatInfo curChat;
       curChat.title = QString::fromStdString(curInfo.title());

       for(int j = 0 ; j < curInfo.participants_size(); ++j)
           curChat.participants.insert(QString::fromStdString(curInfo.participants(j)));

       res.push_back(curChat);
    }

    return res;
}

std::vector<Backend::userInfo> decodeAllUsersMessage(Serialize::ChatMessage& msg)
{
    if(!msg.has_payload())
        return {};

    Serialize::UserInfoVector elements;
    msg.mutable_payload()->UnpackTo(&elements);

    return getUsers(elements);
}

std::optional<ADDUserToChatInfo> decodeAddChatInfo(Serialize::ChatMessage& msg)
{
    if(!msg.has_payload())
        return {};

    Serialize::CreateChatInfo createChatRequest;
    msg.mutable_payload()->UnpackTo(&createChatRequest);

//    auto dbName = QString::fromStdString(std::move(createChatRequest.dbname()));
    auto dbName = QString::fromStdString(createChatRequest.dbname());
//    auto chatCollectionName = QString::fromStdString(std::move(createChatRequest.collectionname()));
    auto chatCollectionName = QString::fromStdString(createChatRequest.collectionname());
//    auto chatTitle = QString::fromStdString(std::move(createChatRequest.chattitle()));
    auto chatTitle = QString::fromStdString(createChatRequest.chattitle());
//    auto participants = std::move(createChatRequest.participants());

    QSet<QString> partcpants;

//    for(const auto& participant: participants)
    for(const auto& participant: createChatRequest.participants())
    {
//        std::cout << participant << " " << std::endl;
        partcpants.insert(QString::fromStdString(participant));
    }
    return ADDUserToChatInfo{ dbName, chatCollectionName, chatTitle, partcpants };
}

std::optional<Backend::userChatInfo> decodeModifyParticipantsChatMessage(Serialize::ChatMessage& msg)
{
    if(!msg.has_payload())
        return {};

    Serialize::userChatInfo usrInfo;
    msg.mutable_payload()->UnpackTo(&usrInfo);

    Backend::userChatInfo res;

    res.dbName = QString::fromStdString(usrInfo.dbname());
    res.chatTitle = QString::fromStdString(usrInfo.chattitle());
    res.userNickname = QString::fromStdString(usrInfo.usernickname());

    return res;
}

LeaveUserFromChatInfo decodeLeaveUserFromChatInfo(Serialize::ChatMessage& msg)
{
    Serialize::LeaveUserFromChatInfo leaveUsrFromChatRequest;
    msg.mutable_payload()->UnpackTo(&leaveUsrFromChatRequest);

//    auto dbName = std::move(leaveUsrFromChatRequest.dbname());
    auto dbName = QString::fromStdString(leaveUsrFromChatRequest.dbname());
//    std::cout << "db:" << dbName << std::endl;
//    auto chatCollectionName = std::move(leaveUsrFromChatRequest.collectionname());
    auto chatCollectionName = QString::fromStdString(leaveUsrFromChatRequest.collectionname());
//    std::cout << "chatCollectionName:" << chatCollectionName << std::endl;
//    auto chatTitle = std::move(leaveUsrFromChatRequest.chattitle());
    auto chatTitle = QString::fromStdString(leaveUsrFromChatRequest.chattitle());

//    auto userToLeave = std::move(leaveUsrFromChatRequest.usertodelete());
//    auto leaveMessage = std::move(leaveUsrFromChatRequest.leavemessage());

    auto userToLeave = QString::fromStdString(leaveUsrFromChatRequest.usertodelete());
    auto leaveMessage = QString::fromStdString(leaveUsrFromChatRequest.leavemessage());

    std::cout << "leaveMessage:" << leaveMessage.toStdString() << std::endl;
    return { dbName, chatCollectionName, chatTitle, userToLeave, leaveMessage };
}
