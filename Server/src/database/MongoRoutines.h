#pragma once
#include <set>
#include <string>
#include <optional>
#include <map>
#include "DatabaseTypes.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/exception/exception.hpp>

std::vector<std::string> getDatabasesNames(const mongocxx::client& client);
std::vector<std::string> getCollectionsNames(mongocxx::database& db);
bool hasCollection(const mongocxx::database& db, const std::string& name);
inline std::string bsonElementToString(const bsoncxx::document::element& element);
inline std::string bsonArrayElementToString(const bsoncxx::array::element& element);
inline bool bsonElementToBoolean(const bsoncxx::document::element& element);
inline int64_t bsonElementToTimePoint(const bsoncxx::document::element& element);
std::vector<Database::userInfo> getAllUsers(const mongocxx::database& db, const std::string& usersCollectionName);
bool addUserToChat(const mongocxx::database& db, const std::string& chatCollectionName, const std::string& chatTitle, const std::string& userNickName);
bool removeUserFromChat(const mongocxx::database& db, const std::string& chatCollectionName, const std::string& chatTitle, const std::string& userNickName);
std::vector<std::string> getAllChatsUserBelongsTo(const mongocxx::database& db, const std::string& chatCollectionName, const std::string& userNickName);
bsoncxx::document::value createChatDocument(const Database::chatMessage& curMsg);
std::vector<Database::chatMessage> getChatDocuments(const mongocxx::database& db, const std::string& chatTitle);
bsoncxx::document::value createChatDescriptionDocument(const std::string& chatTitle, std::set<std::string>& users);
bsoncxx::document::value createUserDocument(const Database::userInfo& user);
bool addDocumentToCollection(const mongocxx::database& db, const std::string& collectionName, bsoncxx::document::value& document);
bool isUserAddedToDatabase(const mongocxx::database& db, const std::string& userCollection, const std::string& userNick);
bool isUserAddedToChat(const mongocxx::database& db, const std::string& chatListCollection, const std::string& chatTitle, const std::string& userNick);
std::set<std::string> getAllUsersInChat(const mongocxx::database& db, const std::string& chatListCollection, const std::string& chatTitle);
bool isChatAddedToDatabase(const mongocxx::database& db, const std::string& chatListCollection, const std::string& chatTitle);
std::optional<Database::userAuthInfo> findUserAuthInfo(const mongocxx::database& db, const std::string& usersCollectionName, const std::string& userNick);
bool deleteUserFromDatabase(const mongocxx::database& db, const std::string& collectionName, const std::string& userNickname);
bool modifyUserInfo(const mongocxx::database& db, const std::string& collectionName, const Database::userInfo& info);

class DatabaseDriver {
    public:
    static DatabaseDriver & instance()
    {
        static DatabaseDriver inst;
        return inst;
    }
    
    std::vector<std::string> getDatabasesNames()
    {
	return ::getDatabasesNames(client);
    }
    
    std::vector<std::string> getCollectionsNames(const std::string& dbName)
    {    
        auto db = client[dbName];
        return ::getCollectionsNames(db);
    }

    std::vector<Database::userInfo> getAllUsers(const std::string& dbName, const std::string& collectionName)
    {
    	auto db = client[dbName];
    	return ::getAllUsers(db, collectionName);
    }

    std::optional<Database::userAuthInfo> findUserAuthInfo(const std::string& dbName, const std::string& collectionName, const std::string& userNick)
    {
    	auto db = client[dbName];
    	return ::findUserAuthInfo(db, collectionName, userNick);
    }

    bool addUserToDatabase(const std::string& dbName, const std::string& collectionName, const Database::userInfo& info)
    {
    	auto db = client[dbName];
    	if(::isUserAddedToDatabase(db, collectionName, info.nickname))
    		return true;

    	auto userdoc = createUserDocument(info);
    	if(!addDocumentToCollection(db, collectionName, userdoc))
    		return false;

    	return true;
    }

    bool deleteUserFromDatabase(const std::string& dbName, const std::string& collectionName, const std::string& userNickname)
    {
    	auto db = client[dbName];
    	if(::isUserAddedToDatabase(db, collectionName, userNickname))
    		return false;

    	return ::deleteUserFromDatabase(db, collectionName, userNickname);
    }

    bool modifyUserInfo(const std::string& dbName, const std::string& collectionName, const Database::userInfo& info)
    {
    	auto db = client[dbName];
    	return ::modifyUserInfo(db, collectionName, info);
    }
    private:
     mongocxx::instance mongoInstance{};
     mongocxx::client client{mongocxx::uri{}};
    
    DatabaseDriver()
    {
    }

      DatabaseDriver(const DatabaseDriver&) = delete;
      DatabaseDriver& operator=(const DatabaseDriver&) = delete;
      ~DatabaseDriver() = default;
};

/*
https://jvgomez.github.io/pages/how-to-configure-a-cc-project-with-eclipse-and-cmake.html
https://www.google.com/search?q=eclipse+cmake+conan&oq=eclipse+cmake+conan&aqs=chrome..69i57j69i59.4849j0j7&sourceid=chrome&ie=UTF-8
https://stackoverflow.com/questions/71350952/mongocxx-update-an-array-within-a-document

https://stackoverflow.com/questions/71350952/mongocxx-update-an-array-within-a-document
https://copyprogramming.com/howto/mongodb-find-value-match-for-a-property-in-array-within-array-of-objects-duplicate
https://copyprogramming.com/howto/mongodb-find-value-match-for-a-property-in-array-within-array-of-objects-duplicate#how-to-get-an-object-from-an-array-in-mongo
https://www.codementor.io/@prasadsaya/working-with-arrays-in-mongodb-16s303gkd3
*/
