// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#include "websocket_session.hpp"
#include <iostream>
#include "messages.pb.h"
#include "messageserialization.h"
#include "utilities.h"
#include "common_types.h"
#include "request_handlers.h"
#include "./database/DatabaseTypes.h"

websocket_session::
websocket_session(
    tcp::socket&& socket,
    boost::shared_ptr<shared_state> const& state)
    : ws_(std::move(socket))
    , state_(state)
{
	 ws_.binary(true);
}

websocket_session::
~websocket_session()
{
    // Remove this session from the list of active sessions
//    state_->leave(this);
    removeRegisteredUser();
}

void
websocket_session::
fail(beast::error_code ec, char const* what)
{
    std::cout << "websocket_session::fail called" << what << std::endl << std::endl;
    // Don't report these
    if( ec == net::error::operation_aborted ||
        ec == websocket::error::closed)
        {
            if(ec == websocket::error::closed)
                std::cout << "websocket_session::fail connection closed, op:" << what << std::endl;
            return;
        }
     if(ec == websocket::error::no_connection)    
        std::cout << "websocket_session::fail no connection error:" << what << std::endl;        
    std::cerr << what << ": " << ec.message() << "\n";
}

void
websocket_session::
on_accept(beast::error_code ec)
{
    std::cout << "websocket_session::on_accept called" << std::endl;
    // Handle the error, if any
    if(ec)
        return fail(ec, "accept");

    // Add this session to the list of active sessions
  //  state_->join(this);

    // Read a message
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &websocket_session::on_read,
            shared_from_this()));
}

void websocket_session::on_read(beast::error_code ec, std::size_t)
{
try{
    std::cout << "websocket_session::on_read called" << std::endl;
    // Handle the error, if any
    if(ec)
        return fail(ec, "read");

    std::string resvdData = beast::buffers_to_string(buffer_.data());

    std::cout << "resvdData: " << resvdData << std::endl;

    auto msg = decodeMessageFromString(resvdData);
    auto handlerCode = static_cast<unsigned int>(msg.payload_type_id());
    auto itHandler = handlers.find(handlerCode);
    if(itHandler != handlers.end())
    	itHandler->second(*this, msg);
/*
    if(static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_AUTHENTICATION) == msg.payload_type_id())
    {
//    	std::cout << "handleUserAuthorization called" << std::endl;
    	handleUserAuthorization(*this, msg);
    }
    else
//    std::cout << "payload_type_id: " << std::to_string(msg.payload_type_id()) << std::endl;
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_DB_NAMES) == msg.payload_type_id())
    {
//        std::cout << "handleGetDatabaseNames called" << std::endl;
        handleGetDatabaseNames(*this, msg);
    }
    else
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_DB_COLLECTIONSNAMES) == msg.payload_type_id())
    {
	    handleGetCollectionNames(*this, msg);
    }
    else
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE) == msg.payload_type_id())
    {
    	handleGetUsersNames(*this, msg);
    }
    else
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_ADD_USER_TO_DATABASE) == msg.payload_type_id())
    {
    	handleAddUserToDatabase(*this, msg);
    }
    else
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MARK_USER_AS_DELETED) == msg.payload_type_id())
    {
    	handleMarkUserAsDeleted(*this, msg);
    }
    else
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_DELETE_USER) == msg.payload_type_id())
    {
    	handleDeleteUser(*this, msg);
    }
    else
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MODIFY_USER_INFO) == msg.payload_type_id())
    {
    	handleModifyUser(*this, msg);
    }

    else
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_CREATE_CHAT) == msg.payload_type_id())
    {
    	handleCreateChat(*this, msg);
    }
    else
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_LEAVE_USER_FROM_CHAT) == msg.payload_type_id())
    {
    	handleLeaveUserFromChat(*this, msg);
    }
    if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_MODIFY_CHAT_USERS_LIST) == msg.payload_type_id())
    {
    	handleModifyChatUsersList(*this, msg);
    }
    else
        if(static_cast<::google::protobuf::uint32>(PayloadType::SERVER_GET_CHATS_USER_BELONGS_TO) == msg.payload_type_id())
        {
        	handleGetChatsUserBelongsTo(*this, msg);
        }
    else
    	if(static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_SEND_MESSAGE_TO_CHAT) == msg.payload_type_id())
    	{
    		handleAddMessageToChat(*this, msg);
    	}
    	else
    		if(static_cast<::google::protobuf::uint32>(PayloadType::CLIENT_REQUEST_MESSAGE_TAPE_FOR_CHAT) == msg.payload_type_id())
    		{
    			handleSendChatTapeToClient(*this, msg);
    		}
    */
    // Clear the buffer
    buffer_.consume(buffer_.size());
}
catch(std::exception& ex)
{
	std::cout << "Exception occured:" << ex.what() << std::endl;
}
    // Read another message
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &websocket_session::on_read,
            shared_from_this()));
}

void websocket_session::setupHandlers()
{
	handlers[static_cast<unsigned int>(PayloadType::CLIENT_AUTHENTICATION)] = handleUserAuthorization;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_GET_DB_NAMES)] = handleGetDatabaseNames;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_GET_DB_COLLECTIONSNAMES)] = handleGetCollectionNames;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_GET_ALL_USERS_IN_DATABASE)] = handleGetUsersNames;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_ADD_USER_TO_DATABASE)] = handleAddUserToDatabase;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_MARK_USER_AS_DELETED)] = handleMarkUserAsDeleted;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_DELETE_USER)] = handleDeleteUser;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_MODIFY_USER_INFO)] = handleModifyUser;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_CREATE_CHAT)] = handleCreateChat;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_LEAVE_USER_FROM_CHAT)] = handleLeaveUserFromChat;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_MODIFY_CHAT_USERS_LIST)] = handleModifyChatUsersList;
    handlers[static_cast<unsigned int>(PayloadType::SERVER_GET_CHATS_USER_BELONGS_TO)] = handleGetChatsUserBelongsTo;
    handlers[static_cast<unsigned int>(PayloadType::CLIENT_SEND_MESSAGE_TO_CHAT)] = handleAddMessageToChat;
    handlers[static_cast<unsigned int>(PayloadType::CLIENT_REQUEST_MESSAGE_TAPE_FOR_CHAT)] = handleSendChatTapeToClient;

}

void websocket_session::send(boost::shared_ptr<std::string const> const& ss)
{
    std::cout << "websocket_session::send called with data: " << *ss << std::endl;

    // Post our work to the strand, this ensures
    // that the members of `this` will not be
    // accessed concurrently.

    net::post(
        ws_.get_executor(),
        beast::bind_front_handler(
            &websocket_session::on_send,
            shared_from_this(),
            ss));
}

void
websocket_session::
on_send(boost::shared_ptr<std::string const> const& ss)
{
     std::cout << "websocket_session::on_send called with data: " << *ss << std::endl;
    // Always add to queue
    queue_.push_back(ss);

    // Are we already writing?
    if(queue_.size() > 1)
        return;

    // We are not currently writing, so send this immediately
    ws_.async_write(
        net::buffer(*queue_.front()),
        beast::bind_front_handler(
            &websocket_session::on_write,
            shared_from_this()));
}

void
websocket_session::
on_write(beast::error_code ec, std::size_t)
{
    // Handle the error, if any
    if(ec)
        return fail(ec, "write");

    // Remove the string from the queue
    queue_.erase(queue_.begin());

    // Send the next message if any
    if(! queue_.empty())
        ws_.async_write(
            net::buffer(*queue_.front()),
            beast::bind_front_handler(
                &websocket_session::on_write,
                shared_from_this()));
}

void websocket_session::addRegisteredUser(const std::string& userNickname)
{
	state_->addRegisteredUser(userNickname, this);
}

void websocket_session::removeRegisteredUser()
{
	state_->removeRegisteredUser(this);
}

void websocket_session::addChatsUserInfo(const Database::chatInfoArray& info)
{
	state_->addChatsUserInfo(info);
}

void websocket_session::sendChatMessageToAllParticipants(const std::string& chatTitle, const std::string& message)
{
	state_->send(chatTitle, message);
}

void websocket_session::updateChatUserInfo(const Database::chatInfo& info)
{
	state_->updateChatUserInfo(info);
}

void websocket_session::deleteUsersFromChat(const std::string& dbName, const std::string& chatTitle, const std::set<std::string>& usersToDelete)
{
	state_->deleteUsersFromChat(dbName, chatTitle, usersToDelete);
}

void websocket_session::addUsersToChat(const std::string& dbName, const std::string& chatTitle, const std::set<std::string>& usersToAdd)
{
	state_->addUsersToChat(dbName, chatTitle, usersToAdd);
}
/*
 bm_type bm;
    bm.insert( bm_type::value_type( 1, "one" ) );
    bm.insert( bm_type::value_type( 2, "two" ) );
    bm.insert( bm_type::value_type( 3, "three" ) );

    auto right_iter = bm.right.find("three");
    if(right_iter != bm.right.end())
    {
        bm.left.erase(right_iter->second);
//        bm.right.erase(right_iter->first);
        int k = bm.size();
        k = 10;
    }
 */
