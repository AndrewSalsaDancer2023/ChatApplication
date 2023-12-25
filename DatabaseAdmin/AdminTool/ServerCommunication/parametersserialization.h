#ifndef PARAMETERSSERIALIZATION_H
#define PARAMETERSSERIALIZATION_H
#include <string>
#include "../../Server/build/authentication.pb.h"
#include <google/protobuf/util/json_util.h>

Serialize::Authentication createAuthentication(const std::string& login, const std::string& password);
Serialize::ConnectionParameters createConnectParams(const std::string& host, unsigned port);
Serialize::ConnectionData  createConnectionData(const std::string& login, const std::string& password,
                                                const std::string& host, unsigned port);
std::string serializeConnectionDataToString(const std::string& login, const std::string& password,
                                            const std::string& host, unsigned port);
Serialize::ConnectionData  getConnectionDataFromString(const std::string& json_string);
#endif // PARAMETERSSERIALIZATION_H
