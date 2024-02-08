// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#ifndef BOOST_BEAST_EXAMPLE_WEBSOCKET_CHAT_MULTI_SHARED_STATE_HPP
#define BOOST_BEAST_EXAMPLE_WEBSOCKET_CHAT_MULTI_SHARED_STATE_HPP

#include <boost/smart_ptr.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <boost/bimap.hpp>

namespace Database {
	struct chatInfoArray;
	struct chatInfo;
}
// Forward declaration
class websocket_session;

// Represents the shared server state
class shared_state
{
    // This mutex synchronizes all access to sessions_
    std::mutex mutex_;

    using bm_type = boost::bimap< std::string, websocket_session* >;
    // Keep a list of all the connected clients
    std::unordered_set<websocket_session*> sessions_;
    std::unordered_map<std::string, std::set<std::string>> channelUsersMap;// channel to user relation
    bm_type userSessionMap; //user to session relation
public:
    shared_state();

    void join(websocket_session* session);
    void leave(websocket_session* session);
    void send(std::string message);
    void send(const std::string& chatTitle, std::string message);

    void addRegisteredUser(const std::string& userNickname, websocket_session* session);
    void removeRegisteredUser(websocket_session* session);
    void addChatsUserInfo(const Database::chatInfoArray& info);
    void updateChatUserInfo(const Database::chatInfo& info);
    void addUsersToChat(const std::string& dbName, const std::string& chatTitle, const std::set<std::string>& usersToAdd);
    void deleteUsersFromChat(const std::string& dbName, const std::string& chatTitle, const std::set<std::string>& usersToDelete);
};

#endif
