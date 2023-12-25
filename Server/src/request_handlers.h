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
void handleMarkUserAsDeleted(websocket_session& session, Serialize::ChatMessage& msg);
void handleDeleteUser(websocket_session& session, Serialize::ChatMessage& msg);
void handleModifyUser(websocket_session& session, Serialize::ChatMessage& msg);

void handleCreateChat(websocket_session& session, Serialize::ChatMessage& msg);
void handleAddUserToChat(websocket_session& session, Serialize::ChatMessage& msg);
void handleDeleteUserFromChat(websocket_session& session, Serialize::ChatMessage& msg);
void handleGetChatsUserBelongsTo(websocket_session& session, Serialize::ChatMessage& msg);
void handleAddMessageToDatabase(websocket_session& session, Serialize::ChatMessage& msg);
void translateMessageToParticipants(websocket_session& session, Serialize::ChatMessage& msg);
void handleSendChatTapeToClient(websocket_session& session, Serialize::ChatMessage& msg);
