#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "common_types.h"
#include "messages.pb.h"
#include "database/MongoRoutines.h"
#include "database/DatabaseTypes.h"
#include <google/protobuf/util/time_util.h>
#include "utilities.h"
/*
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>
#include <algorithm>
#include <iterator>
*/
#include <iostream>
//using boost::uuids::detail::md5;
//#define BOOST_UUID_COMPAT_PRE_1_71_MD5 ;

std::string readFileIntoString(const std::string& path) {
  struct stat sb{};
  std::string res;

  FILE* input_file = fopen(path.c_str(), "r");
  if (input_file == nullptr) {
    return {};
  }

  stat(path.c_str(), &sb);
  res.resize(sb.st_size);
  fread(const_cast<char*>(res.data()), sb.st_size, 1, input_file);
  fclose(input_file);

  return res;
}

std::string serializeNoPayloadMessage(PayloadType type, const std::string& sender)
{
 	Serialize::ChatMessage outmsg;
 	if(!sender.empty())
 		outmsg.set_sender(sender);
    outmsg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));
        
    std::string output;
	if(!outmsg.SerializeToString(&output))
	output.clear();
	
	return output;
}

std::string serializeStringVectorMessage(PayloadType type, const std::vector<std::string>& values)
{
        Serialize::StringVector elements;
        for(const auto& val: values)
        	elements.add_value(val);
        
      // Use the Any::PackFrom to encode an arbitrary PB payload.
       Serialize::ChatMessage msg;
       msg.mutable_payload()->PackFrom(elements);
       msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));
        
       std::string output;
       if(!msg.SerializeToString(&output))
       output.clear();
	
       return output;
}

std::string serializeGetUsersMessage(PayloadType type, const std::vector<Database::userInfo>& users)
{
	Serialize::UserInfoVector usersInfo;
	for(const auto& usr: users)
	{
		Serialize::UserInfo* user = usersInfo.add_users();

		user->set_name(usr.name);
		user->set_surname(usr.surname);
		user->set_email(usr.email);
		user->set_nickname(usr.nickname);
		user->set_password(usr.password);
		user->set_profilepicture(usr.profilepicture);
		user->set_deleted(usr.deleted);
	}

//	*usersInfo.mutable_users() = auth;
	      // Use the Any::PackFrom to encode an arbitrary PB payload.
	 Serialize::ChatMessage msg;
	 msg.mutable_payload()->PackFrom(usersInfo);
	 msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));

	 std::string output;
	 if(!msg.SerializeToString(&output))
		 output.clear();

	 return output;
}

std::string serializeAllChatsUserBelongsToMessage(PayloadType type, const Database::chatInfoArray& chatsList)
{
	Serialize::chatInfoArray chatsInfo;

	for(const auto& chat: chatsList.chats)
	{
			Serialize::chatInfo* newChatInfo = chatsInfo.add_chats();

			newChatInfo->set_title(chat.title);

			for(const auto& participant: chat.participants)
				newChatInfo->add_participants(participant);
	}

	Serialize::ChatMessage msg;
	msg.mutable_payload()->PackFrom(chatsInfo);
	msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));

	std::string output;
	if(!msg.SerializeToString(&output))
		return {};

	return output;
}

std::string serializeGetChatParticipantsMessage(PayloadType type, const Database::chatInfo& info)
{
	Serialize::chatInfo srlInfo;
	srlInfo.set_title(info.title);

	for(const auto& participant: info.participants)
		srlInfo.add_participants(participant);

	Serialize::ChatMessage msg;
	msg.mutable_payload()->PackFrom(srlInfo);
	msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));

	std::string output;
	if(!msg.SerializeToString(&output))
		return {};

	return output;
}

std::string serializeChatMessagesTape(PayloadType type, const Database::chatMessagesTape& tape)
{
	Serialize::chatTape chatTape;

	chatTape.set_dbname(tape.dbName);
	chatTape.set_chattitle(tape.chatTitle);

	for(const auto& message: tape.messages)
	{
		Serialize::userMessage* pMessage = chatTape.add_messages();
		pMessage->set_usernickname(message.userNickName);
		pMessage->set_usermessage(message.userMessage);
		*pMessage->mutable_timestamp() = google::protobuf::util::TimeUtil::MillisecondsToTimestamp(message.timestamp);
	}

	Serialize::ChatMessage msg;
	msg.mutable_payload()->PackFrom(chatTape);
	msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));

	std::string output;
	if(!msg.SerializeToString(&output))
		return {};

	return output;
}

std::string serializeChatMessage(PayloadType type, Serialize::ChatMessage& msg)
{
	msg.set_payload_type_id(static_cast<::google::protobuf::uint32>(type));
	std::string output;
	if(!msg.SerializeToString(&output))
		return {};

	return output;
}

ADDUserToChatInfo extractAddChatInfo(Serialize::ChatMessage& msg)
{
    Serialize::CreateChatInfo createChatRequest;
	msg.mutable_payload()->UnpackTo(&createChatRequest);
	std::cout << std::endl << "extractAddChatInfo" << std::endl;
	auto dbName = createChatRequest.dbname();
	auto chatCollectionName = createChatRequest.collectionname();
	std::cout << dbName << " " << chatCollectionName << std::endl;
	auto chatTitle = createChatRequest.chattitle();
	auto participants = createChatRequest.participants();
	std::cout << chatTitle << " part:" << std::endl;
	std::set<std::string> partcpants;

	for(const auto& participant: participants)
	{
		std::cout << participant << " " << std::endl;
		partcpants.insert(participant);
	}
	return ADDUserToChatInfo{ dbName, chatCollectionName, chatTitle, partcpants };
}

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

/*
std::string toString(const md5::digest_type &digest)
{
    const auto charDigest = reinterpret_cast<const char *>(&digest);
    std::string result;
    boost::algorithm::hex(charDigest, charDigest + sizeof(md5::digest_type), std::back_inserter(result));
    return result;
}

std::string getHash(const std::string& source)
{
	using boost::uuids::detail::md5;

	md5 hash;
	md5::digest_type digest;

	hash.process_bytes(source.data(), source.size());
	hash.get_digest(digest);

	return toString(digest);
}
*/
