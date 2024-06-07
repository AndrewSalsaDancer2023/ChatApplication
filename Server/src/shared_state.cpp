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
#include "common_types.h"

shared_state::shared_state(){}
/*
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
*/
// Broadcast a message to all websocket client sessions
/*
void
shared_state::
send(std::string message)
{
	std::cout << "shared_state::send message" << message << std::endl;
    auto const ss = boost::make_shared<std::string const>(std::move(message));

    std::vector<boost::weak_ptr<websocket_session>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());
        for(auto p : sessions_)
            v.emplace_back(p->weak_from_this());
    }

    for(auto const& wp : v)
        if(auto sp = wp.lock())
            sp->send(ss);
}
*/

activeUserSessions shared_state::getActiveSessions(const std::set<std::string>& usersToAdd)
{
	activeUserSessions v;
	v.reserve(userSessionMap.size());

	std::lock_guard<std::mutex> lock(session_mutex);
	for(const auto& user : usersToAdd)
	{
		auto left_iter = userSessionMap.left.find(user);
		if(left_iter == userSessionMap.left.end())
			continue;

		v.emplace_back(left_iter->second->weak_from_this());
	}
	return v;
}

void shared_state::sendMessageToActiveSessions(const activeUserSessions& sessions, boost::shared_ptr<std::string const> const& message)
{
    for(auto const& session : sessions)
    {
    	if(auto sp = session.lock())
    		sp->send(message);
    }
}

std::set<std::string> shared_state::getChatParticipants(const std::string& chatTitle)
{
	std::cout << "shared_state::getChatParticipants chat " << chatTitle << std::endl;
	std::lock_guard<std::mutex> lock(chat_mutex);

	auto itFind = chatUsersMap.find(chatTitle);
	if(itFind == chatUsersMap.end())
		return {};

	return itFind->second;
}

void shared_state::send(const std::string& chatTitle, std::string message)
{
	std::cout << "shared_state::send message" << message << std::endl;


/*
	auto itFind = channelUsersMap.find(chatTitle);
	if(itFind == channelUsersMap.end())
		return;
*/
	 auto participants = getChatParticipants(chatTitle);
	 auto sessions = getActiveSessions(participants);

	 // Put the message in a shared pointer so we can re-use it for each client
	 auto const sharedmessage = boost::make_shared<std::string const>(std::move(message));

	 sendMessageToActiveSessions(sessions, sharedmessage);
	// Make a local list of all the weak pointers representing
	// the sessions, so we can do the actual sending without
	// holding the mutex:
/*
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
*/
	// For each session in our local list, try to acquire a strong
	// pointer. If successful, then send the message on that session.
/*	for(auto const& wp : v)
		if(auto sp = wp.lock())
			sp->send(ss);
*/
}

void shared_state::addRegisteredUser(const std::string& userNickname, websocket_session* session)
{
	if(!session)
		return;
	std::cout << "addRegisteredUser:" << userNickname << std::endl;
//	bm.insert( bm_type::value_type( 1, "one" ) );
	std::lock_guard<std::mutex> lock(session_mutex);
	userSessionMap.insert( bm_type::value_type( userNickname, session ) );
}

void shared_state::deleteUsersFromChat(const std::string& dbName, const std::string& chatTitle, const std::set<std::string>& usersToDelete)
{
	std::cout << "shared_state::deleteUsersFromChat" << std::endl;
	if(!usersToDelete.empty())
		std::cout << "deleteUsersFromChat::user:" << *usersToDelete.cbegin() << std::endl;

//	activeUserSessions sessions = getActiveSessions(usersToDelete);
	deleteUserNickFromChatInfo(chatTitle, usersToDelete);
	activeUserSessions sessions = getActiveSessions(usersToDelete);
	auto const sharedmessage = boost::make_shared<std::string const>(std::move(createModifyChatParticipantsMessage(dbName, chatTitle, PayloadType::SERVER_DELETE_USER_FROM_CHAT)));
	sendMessageToActiveSessions(sessions, sharedmessage);
}

void shared_state::addUsersToChat(const std::string& dbName, const std::string& chatTitle, const std::set<std::string>& usersToAdd)
{
	std::cout << "shared_state::addUsersToChat" << std::endl;
		if(!usersToAdd.empty())
			std::cout << "addUsersToChat::user:" << *usersToAdd.cbegin() << std::endl;

//	activeUserSessions sessions = getActiveSessions(usersToAdd);
	addUserNickToChatInfo(chatTitle, usersToAdd);
//	auto const message = boost::make_shared<std::string const>(std::move(createModifyChatParticipantsMessage(dbName, chatTitle, PayloadType::SERVER_ADD_USER_TO_CHAT)));
//	sendMessageToActiveSessions(sessions, message);
}

void shared_state::removeRegisteredUser(websocket_session* session)
{
	std::lock_guard<std::mutex> lock(session_mutex);
	auto right_iter = userSessionMap.right.find(session);
	if(right_iter == userSessionMap.right.end())
		return;

	std::cout << "removeRegisteredUser:" << right_iter->first << std::endl;
//	userSessionMap.left.erase(right_iter->second);
	userSessionMap.right.erase(right_iter->first);
}

void shared_state::addChatsUserInfo(const Database::chatInfoArray& info)
{
	std::lock_guard<std::mutex> lock(chat_mutex);
	for(const auto& chatInfo: info.chats)
	{
		if(chatUsersMap.find(chatInfo.title) == chatUsersMap.end())
			chatUsersMap[chatInfo.title] = chatInfo.participants;
	}
}

void shared_state::addUserNickToChatInfo(const std::string& chatTitle, const std::set<std::string>& usersToAdd)
{
	auto it = chatUsersMap.find(chatTitle);
	if(it == chatUsersMap.end())
		return;

	for(const auto& userNickName: usersToAdd)
		it->second.insert(userNickName);
}

void shared_state::deleteUserNickFromChatInfo(const std::string& chatTitle, const std::set<std::string>& userNicknames)
{
	std::lock_guard<std::mutex> lock(chat_mutex);

	auto it = chatUsersMap.find(chatTitle);
	if(it == chatUsersMap.end())
		return;

	for(const auto& userNickName: userNicknames)
		it->second.erase(userNickName);
}

void shared_state::updateChatUserInfo(const Database::chatInfo& info)
{
	std::lock_guard<std::mutex> lock(chat_mutex);
	auto itFind = chatUsersMap.find(info.title);
	if(itFind == chatUsersMap.end())
		return;
	itFind->second  = info.participants;
}

