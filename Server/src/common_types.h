enum class PayloadType : unsigned int
{
	CLIENT_AUTHENTICATION,
    CLIENT_AUTHENTICATION_APPROVED,
	CLIENT_AUTHENTICATION_REJECTED,
    SERVER_GET_DB_NAMES,
    SERVER_GET_DB_COLLECTIONSNAMES,
	SERVER_GET_DB_DATA_ERROR,

	SERVER_GET_ALL_USERS_IN_DATABASE,
	SERVER_GET_ALL_USERS_IN_DATABASE_SUCCESS,
	SERVER_GET_ALL_USERS_IN_DATABASE_ERROR,

	SERVER_CREATE_CHAT,
	SERVER_CREATE_CHAT_SUCCESS,
	SERVER_CREATE_CHAT_ERROR,

	SERVER_MODIFY_CHAT_USERS_LIST,

	SERVER_ADD_USER_TO_CHAT,
	SERVER_ADD_USER_TO_CHAT_SUCCESS,
	SERVER_ADD_USER_TO_CHAT_ERROR,
	SERVER_ADD_USERS_TO_CHAT_ERROR,

	SERVER_DELETE_USER_FROM_CHAT,
	SERVER_DELETE_USER_FROM_CHAT_SUCCESS,
	SERVER_DELETE_USER_FROM_CHAT_ERROR,
	SERVER_DELETE_USERS_FROM_CHAT_ERROR,

	SERVER_MODIFY_CHAT_USERS_SUCCESS,
	SERVER_MODIFY_CHAT_USERS_ERROR,

	SERVER_ADD_USER_TO_DATABASE,
	SERVER_ADD_USER_TO_DATABASE_SUCCESS,
//	SERVER_USER_ALREADY_ADDED_TO_DATABASE,
	SERVER_USER_ADD_TO_DATABASE_ERROR,

	SERVER_MARK_USER_AS_DELETED,
	SERVER_MARK_USER_AS_DELETED_SUCCESS,
	SERVER_MARK_USER_AS_DELETED_ERROR,

    SERVER_DELETE_USER,
    SERVER_USER_DELETION_SUCCESS,
    SERVER_USER_DELETION_ERROR,

    SERVER_MODIFY_USER_INFO,
    SERVER_MODIFY_USER_INFO_SUCCESS,
    SERVER_MODIFY_USER_INFO_ERROR,

	SERVER_GET_CHATS_USER_BELONGS_TO,
	SERVER_GET_CHATS_USER_BELONGS_TO_SUCCESS,
	SERVER_GET_CHATS_USER_BELONGS_TO_ERROR,

	CLIENT_SEND_MESSAGE_TO_CHAT,
	CLIENT_SEND_MESSAGE_TO_CHAT_SUCCESS,
	CLIENT_SEND_MESSAGE_TO_CHAT_ERROR,

    SERVER_CHAT_PARTICIPANTS_UPDATED,
	SERVER_SEND_MESSAGE_FROM_CHAT,

	CLIENT_RECIEVE_MESSAGE_FROM_CHAT,
	CLIENT_REQUEST_MESSAGE_TAPE_FOR_CHAT,
	SERVER_SEND_MESSAGE_TAPE_FROM_CHAT,
	SERVER_SEND_MESSAGE_TAPE_FROM_CHAT_ERROR
};
