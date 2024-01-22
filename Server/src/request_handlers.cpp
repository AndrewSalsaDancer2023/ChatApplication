#include "websocket_session.hpp"
#include "messages.pb.h"
#include "messageserialization.h"
#include "utilities.h"
#include "common_types.h"
#include "security.h"
#include "database/MongoRoutines.h"
#include <google/protobuf/util/time_util.h>
#include <set>
#include <chrono>

void handleUserAuthorization(websocket_session& session, Serialize::ChatMessage& msg)
{
  if(!msg.has_payload())
	  return;

  std::string result;
try {
  Serialize::Login logmsg;
  msg.mutable_payload()->UnpackTo(&logmsg);
      		
  std::string usrlogin = logmsg.login();
  std::string password = logmsg.password();
		
 auto& admin = AuthentictionData::instance();
 auto &dbInstance =  DatabaseDriver::instance();
// auto result = dbInstance.findUserAuthInfo(const std::string& dbName, const std::string& collectionName, const std::string& userNick)

  if((usrlogin == admin.getLogin()) && (password == getHash(admin.getPassword())))
  {
	result = serializeNoPayloadMessage(PayloadType::CLIENT_AUTHENTICATION_APPROVED);
  }
  else
  {
	const std::string usersCollectionName{"users"};
	//get clients from db
	  std::optional<Database::userAuthInfo> res = dbInstance.findUserAuthInfo(logmsg.database(), usersCollectionName, usrlogin);
	  if(res && ((*res).nickname == usrlogin) && (getHash((*res).password)  == password))
		  result = serializeNoPayloadMessage(PayloadType::CLIENT_AUTHENTICATION_APPROVED);
	  else
		  result = serializeNoPayloadMessage(PayloadType::CLIENT_AUTHENTICATION_REJECTED);
  } 
  //ws_.binary(true);
  auto const ss = boost::make_shared<std::string const>(std::move(result));
  session.send(ss);
}
catch(std::exception& ex)
{
	result = serializeNoPayloadMessage(PayloadType::CLIENT_AUTHENTICATION_REJECTED, ex.what());
	auto const ss = boost::make_shared<std::string const>(std::move(result));
	session.send(ss);
}
}

void handleGetDatabaseNames(websocket_session& session)
{
  auto &dbInstance =  DatabaseDriver::instance();
  auto names = dbInstance.getDatabasesNames();

  std::string result;  
  result = serializeStringVectorMessage(PayloadType::SERVER_GET_DB_NAMES, names);
  	  
  auto const ss = boost::make_shared<std::string const>(std::move(result));
  session.send(ss);
}

void handleGetCollectionNames(websocket_session& session, Serialize::ChatMessage& msg)
{
  auto &dbInstance =  DatabaseDriver::instance();
  auto names = dbInstance.getCollectionsNames(msg.sender());
   
  auto result = serializeStringVectorMessage(PayloadType::SERVER_GET_DB_COLLECTIONSNAMES, names);
    
  auto const ss = boost::make_shared<std::string const>(std::move(result));
  session.send(ss);
}

void handleGetUsersNames(websocket_session& session, Serialize::ChatMessage& msg)
{
	if(!msg.has_payload())
		  return;

	Serialize::GetUsers getUsrsMsg;
	msg.mutable_payload()->UnpackTo(&getUsrsMsg);

	auto dbName = getUsrsMsg.dbname();
	auto collName = getUsrsMsg.collectionname();
	try {
		auto &dbInstance =  DatabaseDriver::instance();
		std::vector<Database::userInfo> users = dbInstance.getAllUsers(dbName, collName);

		auto result = serializeGetUsersMessage(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE_SUCCESS, users);

		auto const ss = boost::make_shared<std::string const>(std::move(result));
		session.send(ss);
	}
	catch(std::exception& ex)
	{
		auto result = serializeNoPayloadMessage(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE_ERROR, ex.what());
		session.send(boost::make_shared<std::string const>(std::move(result)));
	}
}

void handleAddUserToDatabase(websocket_session& session, Serialize::ChatMessage& msg)
{
	if(!msg.has_payload())
		  return;

	Serialize::AddUser addUsrRequest;
	msg.mutable_payload()->UnpackTo(&addUsrRequest);
	const Serialize::GetUsers& passto = addUsrRequest.passto();

	auto dbName = passto.dbname();
    auto collName = passto.collectionname();

    const Serialize::UserInfo& usr = addUsrRequest.info();
    Database::userInfo info;

    info.name = std::move(usr.name());
    info.surname = std::move(usr.surname());
    info.email = std::move(usr.email());
    info.nickname = std::move(usr.nickname());
    info.password = std::move(usr.password());
    info.profilepicture = std::move(usr.profilepicture());
    info.deleted = usr.deleted();

    std::string result;
    auto &dbInstance =  DatabaseDriver::instance();
    if(dbInstance.addUserToDatabase(dbName, collName, info))
    	result = serializeNoPayloadMessage(PayloadType::SERVER_ADD_USER_TO_DATABASE_SUCCESS, info.nickname);
    else
    	result = serializeNoPayloadMessage(PayloadType::SERVER_USER_ADD_TO_DATABASE_ERROR, info.nickname);

    auto const ss = boost::make_shared<std::string const>(std::move(result));
    session.send(ss);
}

void handleMarkUserAsDeleted(websocket_session& session, Serialize::ChatMessage& msg)
{
	if(!msg.has_payload())
			  return;

	Serialize::AddUser addUsrRequest;
	msg.mutable_payload()->UnpackTo(&addUsrRequest);
	const Serialize::GetUsers& passto = addUsrRequest.passto();

	auto dbName = passto.dbname();
	auto collName = passto.collectionname();

	const Serialize::UserInfo& usr = addUsrRequest.info();
	Database::userInfo info;

	info.nickname = std::move(usr.nickname());

	std::string result;
	auto &dbInstance =  DatabaseDriver::instance();

	if(dbInstance.markUserAsDeletedFromDatabase(dbName, collName, info.nickname))
	   	result = serializeNoPayloadMessage(PayloadType::SERVER_MARK_USER_AS_DELETED_SUCCESS, info.nickname);
	 else
	   	result = serializeNoPayloadMessage(PayloadType::SERVER_MARK_USER_AS_DELETED_ERROR, info.nickname);

	 auto const ss = boost::make_shared<std::string const>(std::move(result));
	 session.send(ss);
}

void handleDeleteUser(websocket_session& session, Serialize::ChatMessage& msg)
{
	if(!msg.has_payload())
			  return;

	Serialize::AddUser addUsrRequest;
	msg.mutable_payload()->UnpackTo(&addUsrRequest);
	const Serialize::GetUsers& passto = addUsrRequest.passto();

	auto dbName = passto.dbname();
	auto collName = passto.collectionname();

	const Serialize::UserInfo& usr = addUsrRequest.info();
	Database::userInfo info;

	info.nickname = std::move(usr.nickname());

	std::string result;
	auto &dbInstance =  DatabaseDriver::instance();

	if(dbInstance.deleteUserFromDatabase(dbName, collName, info.nickname))
	   	result = serializeNoPayloadMessage(PayloadType::SERVER_USER_DELETION_SUCCESS, info.nickname);
	 else
	   	result = serializeNoPayloadMessage(PayloadType::SERVER_USER_DELETION_ERROR, info.nickname);

	 auto const ss = boost::make_shared<std::string const>(std::move(result));
	 session.send(ss);
}

void handleModifyUser(websocket_session& session, Serialize::ChatMessage& msg)
{
	if(!msg.has_payload())
			  return;

		Serialize::AddUser addUsrRequest;
		msg.mutable_payload()->UnpackTo(&addUsrRequest);
		const Serialize::GetUsers& passto = addUsrRequest.passto();

		auto dbName = passto.dbname();
	    auto collName = passto.collectionname();

	    const Serialize::UserInfo& usr = addUsrRequest.info();
	    Database::userInfo info;

	    info.name = std::move(usr.name());
	    info.surname = std::move(usr.surname());
	    info.email = std::move(usr.email());
	    info.nickname = std::move(usr.nickname());
	    info.password = std::move(usr.password());
	    info.profilepicture = std::move(usr.profilepicture());
	    info.deleted = usr.deleted();

	    std::string result;
	    auto &dbInstance =  DatabaseDriver::instance();
	    if(dbInstance.modifyUserInfo(dbName, collName, info))
	    	result = serializeNoPayloadMessage(PayloadType::SERVER_MODIFY_USER_INFO_SUCCESS, info.nickname);
	    else
	    	result = serializeNoPayloadMessage(PayloadType::SERVER_MODIFY_USER_INFO_ERROR, info.nickname);

	    auto const ss = boost::make_shared<std::string const>(std::move(result));
	    session.send(ss);
}

using ADDUserToChatInfo = std::tuple<std::string, std::string, std::string, std::set<std::string> >;

ADDUserToChatInfo extractAddChatInfo(Serialize::ChatMessage& msg)
{
    Serialize::CreateChatInfo createChatRequest;
	msg.mutable_payload()->UnpackTo(&createChatRequest);
	std::cout << std::endl << "extractAddChatInfo" << std::endl;
	auto dbName = std::move(createChatRequest.dbname());
	auto chatCollectionName = std::move(createChatRequest.collectionname());
	std::cout << dbName << " " << chatCollectionName << std::endl;
	auto chatTitle = std::move(createChatRequest.chattitle());
	auto participants = std::move(createChatRequest.participants());
	std::cout << chatTitle << " part:" << std::endl;
	std::set<std::string> partcpants;

	for(const auto& participant: participants)
	{
		std::cout << participant << " " << std::endl;
		partcpants.insert(participant);
	}
	return ADDUserToChatInfo{ dbName, chatCollectionName, chatTitle, partcpants };
}


void handleCreateChat(websocket_session& session, Serialize::ChatMessage& msg)
{
	if(!msg.has_payload())
		return;
/*
	Serialize::AddUser addUsrRequest;
	msg.mutable_payload()->UnpackTo(&addUsrRequest);
	const Serialize::GetUsers& passto = addUsrRequest.passto();

		auto dbName = passto.dbname();
	    auto chatCollectionName = passto.collectionname();
	    Database::chatInfo info;
*/
	const auto& [dbName, chatCollectionName, chatTitle, partcpants] = extractAddChatInfo(msg);

	std::string result;
	auto &dbInstance =  DatabaseDriver::instance();
	    if(!dbInstance.createChat(dbName, chatCollectionName, chatTitle, partcpants))
	   	    	/*result = serializeNoPayloadMessage(PayloadType::SERVER_CREATE_CHAT_SUCCESS, chatTitle);
	   	    else*/
	   	    	result = serializeNoPayloadMessage(PayloadType::SERVER_CREATE_CHAT_ERROR, chatTitle);
	    else
	    	result = serializeChatMessage(PayloadType::SERVER_CREATE_CHAT_SUCCESS , msg);

    auto const ss = boost::make_shared<std::string const>(std::move(result));
    session.send(ss);
}

using DBInfo = std::tuple<std::string, std::string, std::string, std::string>;

DBInfo extractDBInfo(Serialize::ChatMessage& msg)
{
    Serialize::AddRemoveUserChatInfo addUsrToChatRequest;
	msg.mutable_payload()->UnpackTo(&addUsrToChatRequest);

	auto dbName = std::move(addUsrToChatRequest.dbname());
	auto chatCollectionName = std::move(addUsrToChatRequest.collectionname());

	auto chatTitle = std::move(addUsrToChatRequest.chattitle());
	auto userNickname = std::move(addUsrToChatRequest.usernickname());

	return { dbName, chatCollectionName, chatTitle, userNickname };
}

using ModifyUserInfo = std::tuple<std::string, std::string, std::string, std::set<std::string>, std::set<std::string>>;

ModifyUserInfo extractModifyUserInfo(Serialize::ChatMessage& msg)
{
    Serialize::ModifyUsersChatInfo modifyUsrToChatRequest;
	msg.mutable_payload()->UnpackTo(&modifyUsrToChatRequest);

	auto dbName = std::move(modifyUsrToChatRequest.dbname());
	std::cout << "db:" << dbName << std::endl;
	auto chatCollectionName = std::move(modifyUsrToChatRequest.collectionname());
	std::cout << "chatCollectionName:" << chatCollectionName << std::endl;
	auto chatTitle = std::move(modifyUsrToChatRequest.chattitle());

	std::set<std::string> usersToDelete;
	for(int i = 0; i < modifyUsrToChatRequest.userstodelete_size(); ++i)
	{
		std::cout << "userstodelete:" << *modifyUsrToChatRequest.mutable_userstodelete(i) << std::endl;
		usersToDelete.insert(*modifyUsrToChatRequest.mutable_userstodelete(i));
	}
	std::set<std::string> usersToAdd;
	for(int i = 0; i < modifyUsrToChatRequest.userstoadd_size(); ++i)
		usersToAdd.insert(*modifyUsrToChatRequest.mutable_userstoadd(i));

	return { dbName, chatCollectionName, chatTitle, usersToDelete, usersToAdd };
}

void handleAddUserToChat(websocket_session& session, Serialize::ChatMessage& msg)
{
	if(!msg.has_payload())
		return;

	const auto& [dbName, chatCollectionName, chatTitle, userNickname] = extractDBInfo(msg);
	std::string result;
    auto &dbInstance =  DatabaseDriver::instance();
    if(dbInstance.addUserToChat(dbName, chatCollectionName, chatTitle, userNickname))
   	    	result = serializeNoPayloadMessage(PayloadType::SERVER_ADD_USER_TO_CHAT_SUCCESS, chatTitle);
   	    else
   	    	result = serializeNoPayloadMessage(PayloadType::SERVER_ADD_USER_TO_CHAT_ERROR, chatTitle);

    auto const ss = boost::make_shared<std::string const>(std::move(result));
    session.send(ss);
}

void handleDeleteUserFromChat(websocket_session& session, Serialize::ChatMessage& msg)
{
	if(!msg.has_payload())
		return;

	const auto& [dbName, chatCollectionName, chatTitle, userNickname] = extractDBInfo(msg);

    std::string result;
    auto &dbInstance =  DatabaseDriver::instance();
    if(dbInstance.removeUserFromChat(dbName, chatCollectionName, chatTitle, userNickname))
   	    	result = serializeNoPayloadMessage(PayloadType::SERVER_DELETE_USER_FROM_CHAT_SUCCESS, userNickname);
   	    else
   	    	result = serializeNoPayloadMessage(PayloadType::SERVER_DELETE_USER_FROM_CHAT_ERROR, userNickname);

    auto const ss = boost::make_shared<std::string const>(std::move(result));
    session.send(ss);
}

void handleGetChatParticipants(const std::string& dbName, const std::string& chatCollectionName, const std::string& chatTitle, websocket_session& session)
{
	auto &dbInstance =  DatabaseDriver::instance();

	Database::chatInfo info = dbInstance.getChatParticipants(dbName, chatCollectionName, chatTitle);

	std::string result = serializeGetChatParticipantsMessage(PayloadType::SERVER_CHAT_PARTICIPANTS_UPDATED, info);
	auto const ss = boost::make_shared<std::string const>(std::move(result));
	session.send(ss);
}

void handleModifyChatUsersList(websocket_session& session, Serialize::ChatMessage& msg)
{
	if(!msg.has_payload())
			return;

	const auto& [dbName, chatCollectionName, chatTitle,
				 usersToDelete, usersToAdd] = extractModifyUserInfo(msg);

	auto &dbInstance =  DatabaseDriver::instance();
	bool deleteOk = true, addOk = true;

	if(dbInstance.deleteUsersFromChat(dbName, chatCollectionName, chatTitle, usersToDelete))
		deleteOk = false;

	if(dbInstance.addUsersToChat(dbName, chatCollectionName, chatTitle, usersToAdd))
		addOk = false;

	std::string result;
	if(!deleteOk)
		result = serializeNoPayloadMessage(PayloadType::SERVER_DELETE_USERS_FROM_CHAT_ERROR, chatTitle);
	else
		if(!addOk)
			result = serializeNoPayloadMessage(PayloadType::SERVER_ADD_USERS_TO_CHAT_ERROR, chatTitle);
//TO DELETE
	 result = serializeNoPayloadMessage(PayloadType::SERVER_MODIFY_CHAT_USERS_SUCCESS, chatTitle);
	 auto const ss = boost::make_shared<std::string const>(std::move(result));
	 session.send(ss);
//
	 handleGetChatParticipants(dbName, chatCollectionName, chatTitle, session);
}

void handleGetChatsUserBelongsTo(websocket_session& session, Serialize::ChatMessage& msg)
{
	if(!msg.has_payload())
		return;
	try
	{
		const auto& [dbName, chatCollectionName, chatTitle, userNickname] = extractDBInfo(msg);
		auto &dbInstance =  DatabaseDriver::instance();

		const auto& res = dbInstance.getAllChatsUserBelongsTo(dbName, chatCollectionName, userNickname);
		std::string result = serializeAllChatsUserBelongsToMessage(PayloadType::SERVER_GET_CHATS_USER_BELONGS_TO_SUCCESS, res);
		auto const ss = boost::make_shared<std::string const>(std::move(result));
		session.send(ss);
	}
	catch(std::exception& ex)
	{
		std::string result = serializeNoPayloadMessage(PayloadType::SERVER_GET_CHATS_USER_BELONGS_TO_ERROR, ex.what());
		auto const ss = boost::make_shared<std::string const>(std::move(result));
		session.send(ss);
	}
}

bool userBelongsToChat(const std::string& dbName, const std::string& chatCollectionName, const std::string& userNickname)
{
//	std::cout << "userBelongsToChat: " << dbName << chatCollectionName << userNickname << std::endl;
	const std::string chatsListCollectionName{"chatrooms"};
	auto &dbInstance =  DatabaseDriver::instance();
	const auto& chatsArray = dbInstance.getAllChatsUserBelongsTo(dbName, chatsListCollectionName, userNickname);
//	std::cout << "userBelongsTo Num Chats: " << chatsArray.chats.size() << std::endl;

	for(const auto& chatInfo: chatsArray.chats)
	{
//		std::cout << "chatInfo.title:" << chatInfo.title << std::endl;
		if(chatInfo.title != chatCollectionName)
			continue;
		else
			return true;
/*		{
			auto itFind = std::find_if(chatInfo.participants.begin(), chatInfo.participants.end(),[&userNickname](const auto& member){
				std::cout << "userBelongsToChat userNickname:" << userNickname << std::endl;
				std::cout << "userBelongsToChat member:" << member << std::endl;
				return userNickname == member;
			});
			return itFind != chatInfo.participants.end();
		}*/
	}
	return false;
}

bool handleAddMessageToDatabase(websocket_session& session, Serialize::ChatMessage& msg)
{
    std::cout << std::endl << "handleAddMessageToDatabase" << std::endl;
	if(!msg.has_payload())
		return false;

    Serialize::userChatMessage userChatMessage;
    msg.mutable_payload()->UnpackTo(&userChatMessage);

    if(!userChatMessage.has_message())
    	return false;

    auto dbName = std::move(userChatMessage.dbname());
    auto chatTitle = std::move(userChatMessage.chattitle());

    std::cout << dbName << std::endl;
    std::cout << chatTitle << std::endl;

    const Serialize::userMessage& message = userChatMessage.message();
    if(!message.has_timestamp())
    	return false;

    auto nickname = std::move(message.usernickname());
    auto mesg = std::move(message.usermessage());

//    bool userBelongsToChat = userBelongsToChat(dbName, chatTitle, nickname);

//	    int64_t ns = google::protobuf::util::TimeUtil::TimestampToNanoseconds(message.timestamp());
    std::chrono::milliseconds ms =
    		std::chrono::milliseconds(google::protobuf::util::TimeUtil::TimestampToMilliseconds(message.timestamp()));

    auto &dbInstance =  DatabaseDriver::instance();
    bool res = dbInstance.addMessageToChat(dbName, chatTitle, nickname, ms, mesg);
    if(res)
    	std::cout << std::endl << "handleAddMessageToDatabase: success" << std::endl;
    else
    	std::cout << std::endl << "handleAddMessageToDatabase: failed" << std::endl;

    return res;
}

void translateMessageToParticipants(websocket_session& session, Serialize::ChatMessage& msg)
{
	std::string result = serializeChatMessage(PayloadType::SERVER_SEND_MESSAGE_FROM_CHAT, msg);
	auto const ss = boost::make_shared<std::string const>(std::move(result));
	session.send(ss);
}

void handleSendChatTapeToClient(websocket_session& session, Serialize::ChatMessage& msg)
{
	Serialize::getChatTapeMessage getTapeMessage;

	if(!msg.has_payload())
			return;

    msg.mutable_payload()->UnpackTo(&getTapeMessage);

    auto dbName = getTapeMessage.dbname();
    auto chatCollectionName = getTapeMessage.chattitle();
    auto userNickname = getTapeMessage.usernickname();

    bool userBelongsChat = userBelongsToChat(dbName, chatCollectionName, userNickname);
    if(!userBelongsChat)
    {
		std::string result = serializeNoPayloadMessage(PayloadType::SERVER_SEND_MESSAGE_TAPE_FROM_CHAT_ERROR, "User is not added to chat!");
    	auto const ss = boost::make_shared<std::string const>(std::move(result));
    	session.send(ss);
    	return;
    }
    auto &dbInstance =  DatabaseDriver::instance();
    std::cout << std::endl << "before dbInstance.getChatDocuments" << std::endl;
    const Database::chatMessagesTape& res =  dbInstance.getChatDocuments(dbName, chatCollectionName);
    std::cout << std::endl << "before serializeChatMessagesTape" << std::endl;
    auto result = serializeChatMessagesTape(PayloadType::SERVER_SEND_MESSAGE_TAPE_FROM_CHAT, res);
    auto const ss = boost::make_shared<std::string const>(std::move(result));
    session.send(ss);
}

void handleAddMessageToChat(websocket_session& session, Serialize::ChatMessage& msg)
{
	const auto& packMessage = decodeChatMessage(msg);

	bool userInChat = userBelongsToChat(packMessage.dbName, packMessage.chatTitle, packMessage.message.userNickName);
	if(!userInChat)
	{
		std::string result = serializeNoPayloadMessage(PayloadType::CLIENT_SEND_MESSAGE_TO_CHAT_ERROR, "User is not added to chat!");
		auto const ss = boost::make_shared<std::string const>(std::move(result));
		session.send(ss);
		return;
	}
	handleAddMessageToDatabase(session, msg);
	translateMessageToParticipants(session, msg);
}

void handleDeleteAllMessagesFromChat(const std::string& dbName, const std::string& chatCollName)
{
	auto &dbInstance =  DatabaseDriver::instance();
	dbInstance.deleteAllMessagesFromChat(dbName, chatCollName);
}
