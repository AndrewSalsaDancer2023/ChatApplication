#pragma once

class websocket_session;
namespace Serialize {
 class ChatMessage;
}
void handleUserAuthorization(websocket_session& session, Serialize::ChatMessage& msg);
void handleGetDatabaseNames(websocket_session& session);
void handleGetCollectionNames(websocket_session& session, Serialize::ChatMessage& msg);
void handleGetUsersNames(websocket_session& session, Serialize::ChatMessage& msg);
void handleAddUserToDatabase(websocket_session& session, Serialize::ChatMessage& msg);
void handleDeleteUser(websocket_session& session, Serialize::ChatMessage& msg);
void handleModifyUser(websocket_session& session, Serialize::ChatMessage& msg);
