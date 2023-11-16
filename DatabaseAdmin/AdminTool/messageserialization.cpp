#include "messageserialization.h"
#include <string>
#include <map>
#include "../../Server/src/common_types.h"
#include "../../Server/src/security.h"
#include "../../Server/src/database/DatabaseTypes.h"

std::string createAuthorizationMessage(const std::string& login, const std::string& password)
{
    Serialize::Login logmsg;
    logmsg.set_login(login);
    auto hashedPassword = getHash(password);
    logmsg.set_password(hashedPassword);

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
/*
    usrinfo.set_name(std::move(info.name));
    usrinfo.set_surname(std::move(info.surname));
    usrinfo.set_email(std::move(info.email));
    usrinfo.set_nickname(std::move(info.nickname));
    usrinfo.set_password(std::move(info.password));
    usrinfo.set_profilepicture(std::move(info.profilepicture));
    usrinfo.set_deleted(info.deleted);
*/
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
/*
 int value_size() const;
  void clear_value();
  static const int kValueFieldNumber = 1;
  const ::std::string& value(int index) const;
  ::std::string* mutable_value(int index);
  void set_value(int index, const ::std::string& value);
  #if LANG_CXX11
  void set_value(int index, ::std::string&& value);
  #endif
  void set_value(int index, const char* value);
  void set_value(int index, const char* value, size_t size);
  ::std::string* add_value();
  void add_value(const ::std::string& value);
*/
