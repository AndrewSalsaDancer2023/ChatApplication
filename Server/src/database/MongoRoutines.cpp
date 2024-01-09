#include "MongoRoutines.h"
#include <cstdint>
#include <vector>
#include <cassert>
#include <chrono>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

using namespace bsoncxx;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

const std::string dbName{"test"};
const std::string usersCollectionName{"users"};
const std::string chatsListCollectionName{"chatrooms"};

/*
#include <chrono>
#include <iostream>

int main()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "The system clock is currently at " << std::ctime(&t_c);
}

auto now = std::chrono::high_resolution_clock::now();
auto now_us = now.time_since_epoch() / 1us;
final_doc.append(kvp("ts",bsoncxx::types::b_int64(now_us));
*/

std::vector<std::string> getDatabasesNames(const mongocxx::client& client)
{
    return client.list_database_names();
}

std::vector<std::string> getCollectionsNames(mongocxx::database& db)
{
    return db.list_collection_names();
}

bool hasCollection(const mongocxx::database& db, const std::string& name) //const
{
    return db.has_collection(name);
}

inline std::string bsonElementToString(const bsoncxx::document::element& element)
{
    return std::string{element.get_string().value};
}

inline std::string bsonArrayElementToString(const bsoncxx::array::element& element)
{
    return std::string{element.get_string().value};
}


inline bool bsonElementToBoolean(const bsoncxx::document::element& element)
{
    return element.get_bool().value;
}

inline int64_t bsonElementToTimePoint(const bsoncxx::document::element& element)
{
    return element.get_date().to_int64();
}

std::optional<Database::userAuthInfo> findUserAuthInfo(const mongocxx::database& db, const std::string& usersCollectionName, const std::string& userNick)
{
	if(!hasCollection(db, usersCollectionName))
		return std::nullopt;

	 auto collection = db[usersCollectionName];

	auto searchDoc = make_document(kvp("nickname", userNick));
	auto find_result = collection.find_one(searchDoc.view());
	if(!find_result)
		return std::nullopt;

	auto doc = *find_result;

	return Database::userAuthInfo{bsonElementToString(doc["nickname"]), bsonElementToString(doc["password"])};

}

std::vector<Database::userInfo> getAllUsers(const mongocxx::database& db, const std::string& usersCollectionName)
{
   std::vector<Database::userInfo> result;

   if(!hasCollection(db, usersCollectionName))
       return result;

   auto collection = db[usersCollectionName];
   auto all_documents = collection.find({});

   for (auto& doc : all_documents)
   {
	   Database::userInfo user{bsonElementToString(doc["name"]), bsonElementToString(doc["surame"]),
                      bsonElementToString(doc["email"]),bsonElementToString(doc["nickname"]),
                      bsonElementToString(doc["password"]), bsonElementToString(doc["photo"]), bsonElementToBoolean(doc["deleted"])};

        result.push_back(user);
    }
    return result;
}

bool addUserToChat(const mongocxx::database& db, const std::string& chatCollectionName, const std::string& chatTitle, const std::string& userNickName)
{
    if(!hasCollection(db, chatCollectionName))
        return false;

    auto collection = db[chatCollectionName];
    auto searchDocument = make_document(kvp("title", chatTitle));
    auto updateDocument =   make_document(kvp("$addToSet", make_document(kvp("participants", userNickName))));
    auto result = collection.update_one(searchDocument.view(), updateDocument.view());

    if(!result)
        return false;

    return (*result).result().modified_count() == 1;
}

bool removeUserFromChat(const mongocxx::database& db, const std::string& chatCollectionName, const std::string& chatTitle, const std::string& userNickName)
{
    if(!hasCollection(db, chatCollectionName))
        return false;

    auto collection = db[chatCollectionName];
    auto searchDocument = make_document(kvp("title", chatTitle));
    auto updateDocument =   make_document(kvp("$pull", make_document(kvp("participants", userNickName))));
    auto result = collection.update_one(searchDocument.view(), updateDocument.view());

    if(!result)
        return false;

    return (*result).result().modified_count() == 1;
}


Database::chatInfoArray getAllChatsUserBelongsTo(const mongocxx::database& db, const std::string& chatCollectionName, const std::string& userNickName)
{
   Database::chatInfoArray result;

   if(!hasCollection(db, chatCollectionName))
       return result;

   auto collection = db[chatCollectionName];

   auto searchCriteria = make_document(kvp("participants", userNickName));
   auto all_documents = collection.find(searchCriteria.view());

   for (auto& doc : all_documents)
   {
	   auto participants = doc["participants"];

	   if (participants.type() != type::k_array)
	          continue;

	   Database::chatInfo chat;
	   bsoncxx::array::view partview{participants.get_array().value};
	   for (const bsoncxx::array::element& msg : partview)
	   {
		   if (msg.type() != type::k_string)
			   continue;
		   chat.participants.push_back(bsonArrayElementToString(msg));
	   }

	   chat.title = bsonElementToString(doc["title"]);
	   result.chats.push_back(chat);
//	   result.push_back(bsonElementToString(doc["title"]));
   }
    return result;
}
/*
bsoncxx::document::value createChatDocument(const Database::chatMessage& curMsg)
{
    auto time = bsoncxx::types::b_date(curMsg.message.time);
    auto document = make_document(kvp("from", curMsg.message.userNickName), kvp("message", curMsg.message.userMessage), kvp("time", time));

    return document;
}
*/
bsoncxx::document::value createChatDocument(const std::string& nickName, const std::chrono::milliseconds& tstamp, const std::string& message)
{
    auto time = bsoncxx::types::b_date(tstamp);
    auto document = make_document(kvp("from", nickName), kvp("message", message), kvp("time", time));

    return document;
}

Database::chatMessagesTape getChatDocuments(const mongocxx::database& db, const std::string& dbName, const std::string& chatTitle)
{
	Database::chatMessagesTape result;
    if(!hasCollection(db, chatTitle))
        return result;

    result.chatTitle = chatTitle;
    result.dbName = dbName;

    auto collection = db[chatTitle];
    auto all_documents = collection.find({});
    std::cout << "getChatDocuments" << std::endl;
    for (auto& doc : all_documents)
    {
    	Database::singleUserMessage curMsg;

        curMsg.userNickName = bsonElementToString(doc["from"]);
        curMsg.userMessage = bsonElementToString(doc["message"]);
        curMsg.timestamp = bsonElementToTimePoint(doc["time"]);

        result.messages.push_back(curMsg);
    }

    return result;
}

bsoncxx::document::value createChatDescriptionDocument(const std::string& chatTitle, const std::set<std::string>& users)
{
/*
    auto doc_value = make_document();

    auto arrayDoc = doc_value << "$each" << bsoncxx::builder::core::open_array
    for (auto& user : users)
            arrayDoc << user;
        arrayDoc << bsoncxx::builder::core::close_array;
  */
    bsoncxx::builder::basic::document update_builder;
    bsoncxx::builder::basic::array participants_array;
    for (const auto& user : users)
        participants_array.append(user);

    auto document = make_document(kvp("title", chatTitle), kvp("participants", participants_array), kvp("deleted", false));
    std::cout << "New chat" << to_json(document) << std::endl << std::endl << std::endl;
    return document;
}

bsoncxx::document::value createUserDocument(const Database::userInfo& user)
{
    auto document = make_document(  kvp("name", user.name), kvp("surame", user.surname),
                                    kvp("email", user.email), kvp("nickname", user.nickname),
                                    kvp("password", user.password), kvp("photo", user.profilepicture), kvp("deleted", false));

    return document;
}

bool addDocumentToCollection(const mongocxx::database& db, const std::string& collectionName, bsoncxx::document::value& document)
{
    std::cout << "New document" << to_json(document) << std::endl << std::endl << std::endl;
    auto collection = db[collectionName];
    auto insert_one_result = collection.insert_one(std::move(document));

    return (insert_one_result && ((*insert_one_result).result().inserted_count() == 1));
}

bool isUserAddedToDatabase(const mongocxx::database& db, const std::string& userCollection, const std::string& userNick)
{
    auto collection = db[userCollection];
    auto searchDoc = make_document(kvp("nickname", userNick));
    auto find_result = collection.find_one(searchDoc.view());

    return (find_result ? true : false);
}

bool isUserAddedToChat(const mongocxx::database& db, const std::string& chatListCollection, const std::string& chatTitle, const std::string& userNick)
{
    auto collection = db[chatListCollection];
    auto searchDoc = make_document(kvp("title", chatTitle));
    auto find_result = collection.find_one(searchDoc.view());

    if(!find_result)
        return false;

    auto doc = *find_result;
    auto participants = doc["participants"];

    if (participants.type() != type::k_array)
        return false;

    bsoncxx::array::view partview{participants.get_array().value};
    for (const bsoncxx::array::element& msg : partview)
    {
        if (msg.type() != type::k_string)
            continue;
        if(userNick == bsonArrayElementToString(msg))
            return true;
    }
    return false;
}

std::set<std::string> getAllUsersInChat(const mongocxx::database& db, const std::string& chatListCollection, const std::string& chatTitle)
{
    std::set<std::string> result;

    auto collection = db[chatListCollection];
    auto searchDoc = make_document(kvp("title", chatTitle));
    auto find_result = collection.find_one(searchDoc.view());

    if(!find_result)
        return result;

    auto doc = *find_result;
    auto participants = doc["participants"];

    if (participants.type() != type::k_array)
        return result;

    bsoncxx::array::view partview{participants.get_array().value};
    for (const bsoncxx::array::element& msg : partview)
    {
        if (msg.type() != type::k_string)
            continue;
        result.insert(bsonArrayElementToString(msg));
    }
    return result;
}


bool isChatAddedToDatabase(const mongocxx::database& db, const std::string& chatListCollection, const std::string& chatTitle)
{
    if(!hasCollection(db, chatListCollection))
        return false;

    auto collection = db[chatListCollection];
    auto searchDoc = make_document(kvp("title", chatTitle));
    auto find_result = collection.find_one(searchDoc.view());

    return (find_result ? true : false);
}

bool markUserAsDeletedFromDatabase(const mongocxx::database& db, const std::string& collectionName, const std::string& userNickname)
{
	  if(!hasCollection(db, collectionName))
	        return false;

	  auto collection = db[collectionName];
	  auto searchDocument = make_document(kvp("nickname", userNickname));
	  auto updateDocument =  make_document(kvp("$set", make_document(kvp("deleted", true))));
	  auto result = collection.update_one(searchDocument.view(), updateDocument.view());

	  if(!result)
		  return false;

	  return ((*result).result().modified_count() == 1);
}

bool deleteUserFromDatabase(const mongocxx::database& db, const std::string& collectionName, const std::string& userNickname)
{
	  if(!hasCollection(db, collectionName))
	        return false;

	  auto collection = db[collectionName];
//	  auto searchDocument = make_document(kvp("nickname", userNickname));
//	  auto updateDocument =  make_document(kvp("$set", make_document(kvp("deleted", true))));

	  auto result = collection.delete_one(make_document(kvp("nickname", userNickname)));
//	  assert(delete_one_result);
//	  assert(delete_one_result->deleted_count() == 1);

	  //auto result = collection.update_one(searchDocument.view(), updateDocument.view());

	  if(!result)
		  return false;

	  return ((*result).result().deleted_count() == 1);
}


bool modifyUserInfo(const mongocxx::database& db, const std::string& collectionName, const Database::userInfo& info)
{
    if(!hasCollection(db, collectionName))
        return false;

    auto collection = db[collectionName];
    auto searchDocument = make_document(kvp("nickname", info.nickname));

    auto doc_builder = bsoncxx::builder::stream::document{};

    bsoncxx::document::value updateInfo = doc_builder
                      << "name" << info.name
                      << "surame" << info.surname
                      << "email" << info.email
                      << "nickname" << info.nickname
                      << "password" << info.password
                      << "photo" << info.profilepicture
                      << "deleted" << info.deleted
              << bsoncxx::builder::stream::finalize;

//    std::cout << bsoncxx::to_json(updateInfo, bsoncxx::ExtendedJsonMode::k_relaxed) << std::endl;


//    auto updateDocument =   make_document(kvp("$set", make_document(kvp("participants", collectionName))));
    auto updateDocument =   make_document(kvp("$set", updateInfo));
    auto result = collection.update_one(searchDocument.view(), updateDocument.view());

    if(!result)
        return false;

    return (*result).result().modified_count() == 1;
}

bool createChat(const mongocxx::database& db, const std::string& chatCollectionName, const std::string& chatTitle, const std::set<std::string>& participants)
{
	  if(!hasCollection(db, chatCollectionName))
	        return false;

	 if(isChatAddedToDatabase(db, chatCollectionName, chatTitle))
		 return false;
/*
	 std::set<std::string> users;

	 for(const auto& user: info.participants)
		 users.insert(user);
*/
	 auto newChat = createChatDescriptionDocument(chatTitle, participants);
	 if(!addDocumentToCollection(db, chatCollectionName, newChat))
		 return false;

	return true;
}

bool addMessageToChat(const mongocxx::database& db, const std::string& chatCollectionName, const std::string& nickName, const std::chrono::milliseconds& tstamp, const std::string& message)
{
//   Database::chatInfoArray result;

   if(!hasCollection(db, chatCollectionName))
       return false;

//   auto chatRoom = db[chatCollectionName];

   auto documet = createChatDocument(nickName, tstamp, message);
   if(!addDocumentToCollection(db, chatCollectionName, documet))
	   return false;

   return true;
}

bool deleteAllMessagesFromChat(const mongocxx::database& db, const std::string& chatCollectionName)
{
	  if(!hasCollection(db, chatCollectionName))
	        return false;

	  auto collection = db[chatCollectionName];
	  auto result = collection.delete_many({});

	  if(!result)
		  return false;

	 std::cout << "deleteAllMessagesFromChat :"	<< (*result).result().deleted_count() << std::endl;

	  return ((*result).result().deleted_count() > 0);
}
/*
 * auto update_many_result =
    collection.update_many(make_document(kvp("i", make_document(kvp("$gt", 0)))),
                            make_document(kvp("$set", make_document(kvp("foo", "buzz")))));
assert(update_many_result);  // Acknowledged writes return results.
assert(update_many_result->modified_count() == 2);
 * */


/*
int main() {
try {
        mongocxx::instance instance{};
        mongocxx::client client{mongocxx::uri{}};

        std::vector<std::string> dbNames = getDatabasesNames(client);
        for(const auto& name : dbNames)
            std::cout << "db name:" << name << std::endl;

        auto db = client["mydb"];
        std::vector<std::string> collNames = getCollectionsNames(db);
        for(const auto& name : collNames)
            std::cout << "coll name:" << name << std::endl;
        return 0;
        std::string userNick = "batman";
        if(isUserAddedToDatabase(db, usersCollectionName, userNick))
        {
            std::cout << "Already added" << userNick << std::endl << std::endl << std::endl;
        }
        else
        {
            userInfo batman{"bruce", "wayne", "bat@gmail.com", "batman", "12345"};
            auto batdoc = createUserDocument(batman);
            if(!addDocumentToCollection(db, usersCollectionName, batdoc))
            {
                std::cout << "Error during insert document" << "batman" << std::endl << std::endl << std::endl;
            }
            else
                std::cout << "Insert document" << "batman" << std::endl << std::endl << std::endl;
        }

        userNick = "catwoman";
        if(isUserAddedToDatabase(db, usersCollectionName, userNick))
        {
            std::cout << "Already added " << userNick << std::endl << std::endl << std::endl;
        }
        else
        {
            userInfo cat{"celina", "kyle", "cat@gmail.com", "catwoman", "123456"};
            auto catdoc = createUserDocument(cat);
            if(!addDocumentToCollection(db, usersCollectionName, catdoc))
            {
                std::cout << "Error during insert document " << "catwoman" << std::endl << std::endl << std::endl;
            }
        }

        std::string room = "news";
        if(isChatAddedToDatabase(db, chatsListCollectionName, room))
        {
            std::cout << "chat already added: " << room << std::endl << std::endl << std::endl;
        }
        else
        {
            //std::set<std::string> users{"batman", "catwoman"};
            std::set<std::string> users{"batman"};
            auto newChat = createChatDescriptionDocument(room, users);
            if(!addDocumentToCollection(db, chatsListCollectionName, newChat))
            {
                std::cout << "Error during creating new chat: " << room << std::endl << std::endl << std::endl;
            }
            else
                std::cout << "created new chat: " << room << std::endl << std::endl << std::endl;
        }

        std::vector<userInfo> usrs =  getAllUsers(db, usersCollectionName);
        for(const auto& user : usrs)
        {
            std::cout << "registered user: " << user.nickname << std::endl;
        }
        userNick = "batman";
        std::vector<std::string> chats = getAllChatsUserBelongsTo(db, chatsListCollectionName, userNick);
        std::cout << "user: " << userNick << std::endl;
        for(const auto& chat : chats)
        {
            std::cout << "chat title: " << chat << std::endl;
        }

        if(removeUserFromChat(db, chatsListCollectionName, room, userNick))
            std::cout << "User deleted from chat: " << userNick << std::endl;
        else
            std::cout << "Error deleting user from chat: " << userNick << std::endl << std::endl << std::endl;

//        userNick = "catwoman";
        if(isUserAddedToChat(db, chatsListCollectionName, room, userNick))
            std::cout << "User already in chat: " << userNick << std::endl << std::endl << std::endl;
        else
        {
            if(addUserToChat(db, chatsListCollectionName, room, userNick))
                std::cout << "User added to chat: " << userNick << std::endl;
            else
                std::cout << "Error adding user to chat: " << userNick << std::endl << std::endl << std::endl;
        }

        chatMessage msg;
       // msg.message.userNickName = "batman";
        msg.message.userNickName = "catwoman";
        //msg.message.userMessage = "Hello cat!";
        msg.message.userMessage = "Hello, bat!";
        msg.message.time = std::chrono::system_clock::now();

        auto batdocumet = createChatDocument(msg);
        if(!addDocumentToCollection(db, room, batdocumet))
        {
            std::cout << "Error during posting new chat message: " << room << std::endl << std::endl << std::endl;
        }
        else
            std::cout << "success post chat: " << room << std::endl << std::endl << std::endl;

        std::vector<chatMessage> messages = getChatDocuments(db, room);
        for(const auto& msage : messages)
        {
            std::cout << msage.message.userNickName << ": " << msage.message.userMessage << std::endl;
        }

        std::set<std::string> users = getAllUsersInChat(db, chatsListCollectionName, room);
        std::cout << "chat participants: " << room << std::endl;
        for(const auto& usr : users)
        {
            std::cout << usr << std::endl;
        }
        return 0;

    catch(mongocxx::query_exception& ex)
    {
        std::cout << "Query exception occured:" << ex.what() << std::endl;
    }
    catch(mongocxx::operation_exception& ex)
    {
        std::cout << "Bad operation occured:" << ex.what() << std::endl;
    }
    catch(bsoncxx::exception& ex)
    {
        std::cout << "Bson exception occured:" << ex.what() << std::endl;
    }
	return 0;
}
*/	

/*
https://jvgomez.github.io/pages/how-to-configure-a-cc-project-with-eclipse-and-cmake.html
https://www.google.com/search?q=eclipse+cmake+conan&oq=eclipse+cmake+conan&aqs=chrome..69i57j69i59.4849j0j7&sourceid=chrome&ie=UTF-8
https://stackoverflow.com/questions/71350952/mongocxx-update-an-array-within-a-document

https://stackoverflow.com/questions/71350952/mongocxx-update-an-array-within-a-document
https://copyprogramming.com/howto/mongodb-find-value-match-for-a-property-in-array-within-array-of-objects-duplicate
https://copyprogramming.com/howto/mongodb-find-value-match-for-a-property-in-array-within-array-of-objects-duplicate#how-to-get-an-object-from-an-array-in-mongo
https://www.codementor.io/@prasadsaya/working-with-arrays-in-mongodb-16s303gkd3
*/
