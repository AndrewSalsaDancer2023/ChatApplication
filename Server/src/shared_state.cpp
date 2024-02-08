// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//
#include <iostream>
#include "shared_state.hpp"
#include "websocket_session.hpp"
#include "./database/DatabaseTypes.h"
#include "messageserialization.h"

shared_state::
/*shared_state(std::string doc_root)
    : doc_root_(std::move(doc_root))*/
shared_state()
{
}

void
shared_state::
join(websocket_session* session)
{
	std::cout << "shared_state::join session" << std::endl;
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(session);
}

void
shared_state::
leave(websocket_session* session)
{
	std::cout << "shared_state::leave session" << std::endl;
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(session);
}

// Broadcast a message to all websocket client sessions
void
shared_state::
send(std::string message)
{
	std::cout << "shared_state::send message" << message << std::endl;
    // Put the message in a shared pointer so we can re-use it for each client
    auto const ss = boost::make_shared<std::string const>(std::move(message));

    // Make a local list of all the weak pointers representing
    // the sessions, so we can do the actual sending without
    // holding the mutex:
    std::vector<boost::weak_ptr<websocket_session>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());
        for(auto p : sessions_)
            v.emplace_back(p->weak_from_this());
    }

    // For each session in our local list, try to acquire a strong
    // pointer. If successful, then send the message on that session.
    for(auto const& wp : v)
        if(auto sp = wp.lock())
            sp->send(ss);
}

void shared_state::send(const std::string& chatTitle, std::string message)
{
	std::cout << "shared_state::send message" << message << std::endl;
	// Put the message in a shared pointer so we can re-use it for each client
	auto const ss = boost::make_shared<std::string const>(std::move(message));


	auto itFind = channelUsersMap.find(chatTitle);
	if(itFind == channelUsersMap.end())
		return;

	// Make a local list of all the weak pointers representing
	// the sessions, so we can do the actual sending without
	// holding the mutex:
	std::vector<boost::weak_ptr<websocket_session>> v;
	{
		std::lock_guard<std::mutex> lock(mutex_);

		const auto& users = itFind->second;
		v.reserve(users.size());

		for(const auto& user : users)
		{
			auto findIter = userSessionMap.left.find(user);
			if(findIter == userSessionMap.left.end())
				continue;

			websocket_session* pSession = findIter->second;
			v.emplace_back(pSession->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
	// pointer. If successful, then send the message on that session.
	for(auto const& wp : v)
		if(auto sp = wp.lock())
			sp->send(ss);
}

void shared_state::addRegisteredUser(const std::string& userNickname, websocket_session* session)
{
	if(!session)
		return;
	std::cout << "addRegisteredUser:" << userNickname << std::endl;
//	bm.insert( bm_type::value_type( 1, "one" ) );
	userSessionMap.insert( bm_type::value_type( userNickname, session ) );//[userNickname] = session;
}

void shared_state::deleteUsersFromChat(const std::string& dbName, const std::string& chatTitle, const std::set<std::string>& usersToDelete)
{
	for(const auto& user : usersToDelete)
	{
	    auto left_iter = userSessionMap.left.find(user);
	    if(left_iter == userSessionMap.left.end())
	    		continue;

	    auto const ss = boost::make_shared<std::string const>(std::move(createRemoveUserFromChatMessage(dbName, chatTitle, user)));
	    websocket_session* pSession = left_iter->second;
	    pSession->send(ss);
	}
}

void shared_state::addUsersToChat(const std::string& dbName, const std::string& chatTitle, const std::set<std::string>& usersToAdd)
{
	for(const auto& user : usersToAdd)
	{
	    auto left_iter = userSessionMap.left.find(user);
	    if(left_iter == userSessionMap.left.end())
	    		continue;

	    auto const ss = boost::make_shared<std::string const>(std::move(createChatMessage(dbName, "", chatTitle, usersToAdd)));
	    websocket_session* pSession = left_iter->second;
	    pSession->send(ss);
	}
}

void shared_state::removeRegisteredUser(websocket_session* session)
{
	auto right_iter = userSessionMap.right.find(session);
	if(right_iter == userSessionMap.right.end())
		return;

	std::cout << "removeRegisteredUser:" << right_iter->first << std::endl;
//	userSessionMap.left.erase(right_iter->second);
	userSessionMap.right.erase(right_iter->first);
}

void shared_state::addChatsUserInfo(const Database::chatInfoArray& info)
{
	for(const auto& chatInfo: info.chats)
	{
		if(channelUsersMap.find(chatInfo.title) == channelUsersMap.end())
			channelUsersMap[chatInfo.title] = chatInfo.participants;
	}
}

void shared_state::updateChatUserInfo(const Database::chatInfo& info)
{
	auto itFind = channelUsersMap.find(info.title);
	if(itFind == channelUsersMap.end())
		return;
	itFind->second  = info.participants;
}

