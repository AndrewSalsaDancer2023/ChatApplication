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
void handleModifyChatUsersList(websocket_session& session, Serialize::ChatMessage& msg);
void handleLeaveUserFromChat(websocket_session& session, Serialize::ChatMessage& msg);
void handleGetChatsUserBelongsTo(websocket_session& session, Serialize::ChatMessage& msg);
bool handleAddMessageToDatabase(websocket_session& session, Serialize::ChatMessage& msg);
void translateMessageToParticipants(websocket_session& session, const std::string& chatTitle, Serialize::ChatMessage& msg);
void handleSendChatTapeToClient(websocket_session& session, Serialize::ChatMessage& msg);
void handleAddMessageToChat(websocket_session& session, Serialize::ChatMessage& msg);
void handleDeleteAllMessagesFromChat(const std::string& dbName, const std::string& chatCollName);

bool userBelongsToChat(const std::string& dbName, const std::string& chatCollectionName, const std::string& userNickname);
