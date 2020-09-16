#ifndef CHAT_PROTOCOL_H
#define CHAT_PROTOCOL_H

#include "chat_protocol_limits.h"

#define TAG_INDEX 0
#define LEN_INDEX 1
#define VAL_INDEX 2

#define PROTOCOL_SUCCESS 1
#define PROTOCOL_FAIL 0

typedef unsigned char ProtocolStatus;

typedef enum Tag
{
	REGISTRATION,
	LOG_IN,
	CREATE_GROUP,
	GET_GROUPS,
	JOIN_GROUP,
	LEAVE_GROUP,
	LOG_OUT,
	CHAT
} Tag;

typedef enum RequestStatus
{
	OK = 0,
	
	/* Registration */
	USERNAME_ALREADY_EXISTS,
	INVALID_USERNAME, /* לא חובה לממש */
	INVALID_PASSWORD, /* לא חובה לממש */
	
	/* Log-in */
	USERNAME_DOESNT_EXIST,
	WRONG_PASSWORD,
	USER_ALREADY_CONNECTED,
	
	/* Create Group */
	GROUP_ALREADY_EXISTS,
	MAX_GROUP_ERROR,

	/* Get Groups */
	NO_GROUPS,
	
	/* Join Group */
	GROUP_DOESNT_EXIST,
	
	/* Leave Group */
	LEAVE_FAILED,
	
	/* Log-out */
	LOGOUT_FAILED,
	
	/* General */
	GENERAL_ERROR
	
} RequestStatus;

typedef struct RegRequest
{
	unsigned char m_usernameLen;
	char m_username[MAX_LEN_USERNAME + 1];
	unsigned char m_passwordLen;
	char m_password[MAX_LEN_PASSWORD + 1];
} RegRequest;

typedef struct RegReply
{
	unsigned char m_requestStatus;
} RegReply;

typedef struct LogInRequest
{
	unsigned char m_usernameLen;
	char m_username[MAX_LEN_USERNAME + 1];
	unsigned char m_passwordLen;
	char m_password[MAX_LEN_PASSWORD + 1];
} LogInRequest;

typedef struct LogInReply
{
	unsigned char m_requestStatus;
} LogInReply;


typedef struct CreateGrpRequest
{
	unsigned char m_usernameLen;
	char m_username[MAX_LEN_USERNAME + 1];
	unsigned char m_groupNameLen;
	char m_groupName[MAX_LEN_GROUP_NAME + 1];
} CreateGrpRequest;

typedef struct CreateGrpReply
{
	unsigned char m_requestStatus;
	unsigned char m_mcastIPLen;
	char m_mcastIP[MAX_LEN_IP + 1];
	unsigned char m_mcastPortLen;
	char m_mcastPort[MAX_LEN_PORT + 1];
} CreateGrpReply;

typedef struct GetGrpsRequest
{
	char m_username[MAX_LEN_USERNAME + 1];
} GetGrpsRequest;

typedef struct GetGrpsReply
{
	unsigned char m_requestStatus;
	size_t m_totalPayloadsLen;
} GetGrpsReply;

typedef struct JoinGrpRequest
{
	unsigned char m_usernameLen;
	char m_username[MAX_LEN_USERNAME + 1];
	unsigned char m_groupNameLen;
	char m_groupName[MAX_LEN_GROUP_NAME + 1];
} JoinGrpRequest;

typedef struct JoinGrpReply
{
	unsigned char m_requestStatus;
	unsigned char m_mcastIPLen;
	char m_mcastIP[MAX_LEN_IP + 1];
	unsigned char m_mcastPortLen;
	char m_mcastPort[MAX_LEN_PORT + 1];
} JoinGrpReply;

typedef struct LeaveGrpRequest
{
	unsigned char m_usernameLen;
	char m_username[MAX_LEN_USERNAME + 1];
	unsigned char m_groupNameLen;
	char m_groupName[MAX_LEN_GROUP_NAME + 1];
} LeaveGrpRequest;

typedef struct LeaveGrpReply
{
	unsigned char m_requestStatus;
} LeaveGrpReply;

typedef struct LogOutRequest
{
	char m_username[MAX_LEN_USERNAME + 1];
} LogOutRequest;

typedef struct LogOutReply
{
	unsigned char m_requestStatus;
} LogOutReply;

typedef struct ChatMsg
{
	unsigned char m_usernameLen;
	char m_username[MAX_LEN_USERNAME + 1];
	unsigned char m_chatMsgLen;
	char m_chatMsg[MAX_TOTAL_LEN_CHAT + 1];
} ChatMsg;

typedef struct MsgSize
{
	unsigned char m_requestStatus;
	size_t m_msgSize;
} MsgSize;

/*--Registration--*/
ProtocolStatus RegRequestPack (const RegRequest* _request, char* _msg);
ProtocolStatus RegRequestUnpack (RegRequest* _request, const char* _msg);
ProtocolStatus RegReplyPack (const RegReply* _reply, char* _msg);
ProtocolStatus RegReplyUnpack (RegReply* _reply, const char* _msg);

/*--LogIn--*/
ProtocolStatus LogInRequestPack (const LogInRequest* _request, char* _msg);
ProtocolStatus LogInRequestUnpack (LogInRequest* _request, const char* _msg);
ProtocolStatus LogInReplyPack (const LogInReply* _reply, char* _msg);
ProtocolStatus LogInReplyUnpack (LogInReply* _reply, const char* _msg);

/*--CreateGroup--*/
ProtocolStatus CreateGrpRequestPack (const CreateGrpRequest* _request, char* _msg);
ProtocolStatus CreateGrpRequestUnpack (CreateGrpRequest* _request, const char* _msg);
ProtocolStatus CreateGrpReplyPack (const CreateGrpReply* _reply, char* _msg);
ProtocolStatus CreateGrpReplyUnpack (CreateGrpReply* _reply, const char* _msg);

/*--GetGroups--*/
ProtocolStatus GetGrpsRequestPack (const GetGrpsRequest* _request, char* _msg);
ProtocolStatus GetGrpsRequestUnpack (GetGrpsRequest* _request, const char* _msg);
ProtocolStatus GetGrpsReplyPack (const GetGrpsReply* _reply, char* _msg);
ProtocolStatus GetGrpsReplyUnpack (GetGrpsReply* _reply, const char* _msg);

/*--JoinGroup--*/
ProtocolStatus JoinGrpRequestPack (const JoinGrpRequest* _request, char* _msg);
ProtocolStatus JoinGrpRequestUnpack (JoinGrpRequest* _request, const char* _msg);
ProtocolStatus JoinGrpReplyPack (const JoinGrpReply* _reply, char* _msg);
ProtocolStatus JoinGrpReplyUnpack (JoinGrpReply* _reply, const char* _msg);

/*--LeaveGroup--*/
ProtocolStatus LeaveGrpRequestPack (const LeaveGrpRequest* _request, char* _msg);
ProtocolStatus LeaveGrpRequestUnpack (LeaveGrpRequest* _request, const char* _msg);
ProtocolStatus LeaveGrpReplyPack (const LeaveGrpReply* _reply, char* _msg);
ProtocolStatus LeaveGrpReplyUnpack (LeaveGrpReply* _reply, const char* _msg);

/*--LogOut--*/
ProtocolStatus LogOutRequestPack (const LogOutRequest* _request, char* _msg);
ProtocolStatus LogOutRequestUnpack (LogOutRequest* _request, const char* _msg);
ProtocolStatus LogOutReplyPack (const LogOutReply* _reply, char* _msg);
ProtocolStatus LogOutReplyUnpack (LogOutReply* _reply, const char* _msg);

/*--ChatMsg--*/
ProtocolStatus ChatMsgPack (const ChatMsg* _chat, char* _msgSend);
ProtocolStatus ChatMsgUnpack (ChatMsg* _chat, const char* _msgRecv);

/*--MSG_SIZE--*/
/*
ProtocolStatus MsgSizePack (const MsgSize* _sizeOfMsg, char* _msg);
ProtocolStatus MsgSizeUnpack (MsgSize* _sizeOfMsg, const char* _msg);
 */

#endif /* CHAT_PROTOCOL_H */
