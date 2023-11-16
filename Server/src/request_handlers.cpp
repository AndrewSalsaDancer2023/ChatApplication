#include "websocket_session.hpp"
#include "messages.pb.h"
#include "messageserialization.h"
#include "utilities.h"
#include "common_types.h"
#include "security.h"
#include "database/MongoRoutines.h"

void handleUserAuthorization(websocket_session& session, Serialize::ChatMessage& msg)
{
  if(!msg.has_payload())
	  return;

  Serialize::Login logmsg;
  msg.mutable_payload()->UnpackTo(&logmsg);
      		
  std::string usrlogin = logmsg.login();
  std::string password = logmsg.password();
		
 auto& admin = AuthentictionData::instance();
 auto &dbInstance =  DatabaseDriver::instance();
// auto result = dbInstance.findUserAuthInfo(const std::string& dbName, const std::string& collectionName, const std::string& userNick)

  std::string result;
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

	auto &dbInstance =  DatabaseDriver::instance();
	std::vector<Database::userInfo> users = dbInstance.getAllUsers(dbName, collName);

	auto result = serializeGetUsersMessage(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE, users);

	auto const ss = boost::make_shared<std::string const>(std::move(result));
	session.send(ss);

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
