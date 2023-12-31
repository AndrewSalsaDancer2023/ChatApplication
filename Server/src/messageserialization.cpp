#include "messageserialization.h"
#include <string>
#include <iostream>
#include "../../Server/src/common_types.h"

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
