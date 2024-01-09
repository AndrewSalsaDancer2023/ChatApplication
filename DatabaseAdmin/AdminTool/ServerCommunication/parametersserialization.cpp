#include "parametersserialization.h"
#include <string>


Serialize::Authentication createAuthentication(const std::string& login, const std::string& password)
{
    Serialize::Authentication auth;

    auth.set_login(login);
    auth.set_password(password);

    return auth;
}

Serialize::ConnectionParameters createConnectParams(const std::string& host, unsigned port)
{
    Serialize::ConnectionParameters connection;
    connection.set_host(host);
    connection.set_port(port);//::google::protobuf::uint32 value);

    return connection;
}

Serialize::ConnectionData  createConnectionData(const std::string& login, const std::string& password,
                                                const std::string& host, unsigned port)
{
    auto auth = createAuthentication(login, password);
    auto params = createConnectParams(host, port);

    Serialize::ConnectionData res;
    *res.mutable_authentication() = auth;
    *res.mutable_connectionparameters() = params;
    return res;
}

std::string serializeConnectionDataToString(const std::string& login, const std::string& password,
                        const std::string& host, unsigned port)
{
    auto conData = createConnectionData(login, password, host, port);

    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;
    std::string json_string;
    MessageToJsonString(conData, &json_string, options);

    return json_string;
}

Serialize::ConnectionData  getConnectionDataFromString(const std::string& json_string)
{
    Serialize::ConnectionData data;
    google::protobuf::util::JsonParseOptions options;
    JsonStringToMessage(json_string, &data, options);

    return data;
}

std::string serializeAuthDataToString(const std::string& login, const std::string& password,
                                            const std::string& database)
{
    Serialize::Login logmsg;
    logmsg.set_login(login);
    logmsg.set_password(password);
    logmsg.set_database(database);


    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;
    std::string json_string;
    MessageToJsonString(logmsg, &json_string, options);

    return json_string;
}

Serialize::Login getAuthDataFromString(const std::string& json_string)
{
    Serialize::Login data;
    google::protobuf::util::JsonParseOptions options;
    JsonStringToMessage(json_string, &data, options);

    return data;
}
