#include <string.h>
#include <assert.h>
#include "chat_protocol.h"

static int countDigits(size_t _num);

ProtocolStatus RegRequestPack(const RegRequest* _request, char* _msg)
{
	if (!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	
	_msg[TAG_INDEX] = (char)REGISTRATION;
	_msg[LEN_INDEX] = (char)(sizeof(_request->m_usernameLen) + _request->m_usernameLen + sizeof(_request->m_passwordLen) + _request->m_passwordLen);
	_msg[VAL_INDEX] = (char)(_request->m_usernameLen);
	strncpy(_msg + VAL_INDEX + sizeof(_request->m_usernameLen), _request->m_username, (size_t)(_request->m_usernameLen));
	_msg[VAL_INDEX + 1 + _request->m_usernameLen] = (char)_request->m_passwordLen;
	strncpy(_msg + VAL_INDEX + sizeof(_request->m_usernameLen) + _request->m_usernameLen + sizeof(_request->m_passwordLen), _request->m_password, (size_t)(_request->m_passwordLen));
	
	return PROTOCOL_SUCCESS;
}

ProtocolStatus RegRequestUnpack (RegRequest* _request, const char* _msg)
{
	unsigned char lengthUsername;
	unsigned char lengthPassword;
	char* startUsername;
	char* startPassword;
	if(!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	lengthUsername = _msg[VAL_INDEX];
	_request->m_usernameLen = lengthUsername;
	startUsername = (char*)_msg + VAL_INDEX + sizeof(_request->m_usernameLen);
	strncpy(_request->m_username, startUsername, lengthUsername);
	_request->m_username[lengthUsername] = '\0';
	lengthPassword = startUsername[lengthUsername];
	_request->m_passwordLen = lengthPassword;
	startPassword = startUsername + lengthUsername + sizeof(_request->m_passwordLen);
	strncpy(_request->m_password, startPassword, lengthPassword);
	_request->m_password[lengthPassword] = '\0';
	return PROTOCOL_SUCCESS;
}

ProtocolStatus RegReplyPack(const RegReply* _reply, char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	_msg[TAG_INDEX] = (char)REGISTRATION;
	_msg[LEN_INDEX] = (char)sizeof(_reply->m_requestStatus);
	_msg[VAL_INDEX] = (char)(_reply->m_requestStatus);
	return PROTOCOL_SUCCESS;
}

ProtocolStatus RegReplyUnpack (RegReply* _reply, const char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	_reply->m_requestStatus = (unsigned char)_msg[VAL_INDEX];
	return PROTOCOL_SUCCESS;
}

ProtocolStatus LogInRequestPack (const LogInRequest* _request, char* _msg)
{
	if (!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	
	_msg[TAG_INDEX] = (char)LOG_IN;
	_msg[LEN_INDEX] = (char)(sizeof(_request->m_usernameLen) + _request->m_usernameLen + sizeof(_request->m_passwordLen) + _request->m_passwordLen);
	_msg[VAL_INDEX] = (char)(_request->m_usernameLen);
	strncpy(_msg + VAL_INDEX + sizeof(_request->m_usernameLen), _request->m_username, (size_t)(_request->m_usernameLen));
	_msg[VAL_INDEX + sizeof(_request->m_usernameLen) + _request->m_usernameLen] = (char)_request->m_passwordLen;
	strncpy(_msg + VAL_INDEX + sizeof(_request->m_usernameLen) + _request->m_usernameLen + sizeof(_request->m_passwordLen), _request->m_password, (size_t)(_request->m_passwordLen));
	
	return PROTOCOL_SUCCESS;
}

ProtocolStatus LogInRequestUnpack (LogInRequest* _request, const char* _msg)
{
	unsigned char lengthUsername;
	unsigned char lengthPassword;
	char* startUsername;
	char* startPassword;
	if(!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	lengthUsername = _msg[VAL_INDEX];
	_request->m_usernameLen = lengthUsername;
	startUsername = (char*)_msg + VAL_INDEX + sizeof(_request->m_usernameLen);
	strncpy(_request->m_username, startUsername, lengthUsername);
	_request->m_username[lengthUsername] = '\0';
	lengthPassword = startUsername[lengthUsername];
	_request->m_passwordLen = lengthPassword;
	startPassword = startUsername + lengthUsername + sizeof(_request->m_passwordLen);
	strncpy(_request->m_password, startPassword, lengthPassword);
	_request->m_password[lengthPassword] = '\0';
	return PROTOCOL_SUCCESS;
}

ProtocolStatus LogInReplyPack (const LogInReply* _reply, char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	_msg[TAG_INDEX] = (char)LOG_IN;
	_msg[LEN_INDEX] = (char)sizeof(_reply->m_requestStatus);
	_msg[VAL_INDEX] = (char)(_reply->m_requestStatus);
	return PROTOCOL_SUCCESS;
}

ProtocolStatus LogInReplyUnpack (LogInReply* _reply, const char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	_reply->m_requestStatus = (unsigned char)_msg[VAL_INDEX];
	return PROTOCOL_SUCCESS;
}

/*--CreateGroup--*/
ProtocolStatus CreateGrpRequestPack (const CreateGrpRequest* _request, char* _msg)
{
	if (!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	
	_msg[TAG_INDEX] = (char)CREATE_GROUP;
	_msg[LEN_INDEX] = (char)(sizeof(_request->m_usernameLen) + _request->m_usernameLen + sizeof(_request->m_groupNameLen) + _request->m_groupNameLen);
	_msg[VAL_INDEX] = (char)(_request->m_usernameLen);
	strncpy(_msg + VAL_INDEX + sizeof(_request->m_usernameLen), _request->m_username, (size_t)(_request->m_usernameLen));
	_msg[VAL_INDEX + sizeof(_request->m_usernameLen) + _request->m_usernameLen] = (char)_request->m_groupNameLen;
	strncpy(_msg + VAL_INDEX + sizeof(_request->m_usernameLen)+ _request->m_usernameLen + sizeof(_request->m_groupNameLen), _request->m_groupName, (size_t)(_request->m_groupNameLen));

	return PROTOCOL_SUCCESS;
}

ProtocolStatus CreateGrpRequestUnpack (CreateGrpRequest* _request, const char* _msg)
{
	unsigned char lengthUsername;
	unsigned char lengthGroupName;
	char* startUsername;
	char* startGroupName;
	if(!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	lengthUsername = _msg[VAL_INDEX];
	_request->m_usernameLen = lengthUsername;
	startUsername = (char*)_msg + VAL_INDEX + sizeof(_request->m_usernameLen);
	strncpy(_request->m_username, startUsername, lengthUsername);
	_request->m_username[lengthUsername] = '\0';
	lengthGroupName = startUsername[lengthUsername];
	_request->m_groupNameLen = lengthGroupName;
	startGroupName = startUsername + lengthUsername + sizeof(_request->m_groupNameLen);
	strncpy(_request->m_groupName, startGroupName, lengthGroupName);
	_request->m_groupName[lengthGroupName] = '\0';
	return PROTOCOL_SUCCESS;
}

ProtocolStatus CreateGrpReplyPack (const CreateGrpReply* _reply, char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	_msg[TAG_INDEX] = (char)CREATE_GROUP;
	_msg[LEN_INDEX] = (char)(sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen) + _reply->m_mcastIPLen + sizeof(_reply->m_mcastPortLen) + _reply->m_mcastPortLen);
	_msg[VAL_INDEX] = (char)(_reply->m_requestStatus);
	_msg[VAL_INDEX + sizeof(_reply->m_requestStatus)] = (char)(_reply->m_mcastIPLen);
	strncpy(_msg + VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen), _reply->m_mcastIP,(size_t)(_reply->m_mcastIPLen));
	_msg[VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen) + _reply->m_mcastIPLen] = (char)(_reply->m_mcastPortLen);
	strncpy(_msg + VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen) + _reply->m_mcastIPLen + sizeof(_reply->m_mcastPortLen), _reply->m_mcastPort, (size_t)(_reply->m_mcastPortLen));
	return PROTOCOL_SUCCESS;
}

ProtocolStatus CreateGrpReplyUnpack (CreateGrpReply* _reply, const char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}

	_reply->m_requestStatus = (unsigned char)_msg[VAL_INDEX];
	_reply->m_mcastIPLen = (unsigned char)_msg[VAL_INDEX + sizeof(_reply->m_requestStatus)];
	strncpy(_reply->m_mcastIP, _msg + VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen), (size_t)(_reply->m_mcastIPLen));
	_reply->m_mcastIP[_reply->m_mcastIPLen] = '\0';
	_reply->m_mcastPortLen = (unsigned char)_msg[VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen)];
	strncpy(_reply->m_mcastPort, _msg + VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen) + _reply->m_mcastIPLen + sizeof(_reply->m_mcastPortLen), (size_t)(_reply->m_mcastPortLen));
	_reply->m_mcastPort[_reply->m_mcastPortLen] = '\0';

	return PROTOCOL_SUCCESS;
}

/*--GetGroups--*/
ProtocolStatus GetGrpsRequestPack (const GetGrpsRequest* _request, char* _msg)
{
	if (!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	
	_msg[TAG_INDEX] = (char)GET_GROUPS;
	_msg[LEN_INDEX] = (char)(strlen(_request->m_username));
	strncpy(_msg + VAL_INDEX, _request->m_username, _msg[LEN_INDEX]);
	
	return PROTOCOL_SUCCESS;
}

ProtocolStatus GetGrpsRequestUnpack (GetGrpsRequest* _request, const char* _msg)
{
	if (!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	strncpy(_request->m_username, _msg + VAL_INDEX, _msg[LEN_INDEX]);
	return PROTOCOL_SUCCESS;
}

ProtocolStatus GetGrpsReplyPack(const GetGrpsReply* _reply, char* _msg)
{
	int i;
	size_t size;
	int digits;

	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}

	size = _reply->m_totalPayloadsLen;
	digits = countDigits(size);
	_msg[TAG_INDEX] = (char)GET_GROUPS;
	_msg[LEN_INDEX] = (char)(sizeof(_reply->m_requestStatus) + digits);
	_msg[VAL_INDEX] = (char)_reply->m_requestStatus;
	for(i = VAL_INDEX + LEN_INDEX; i < VAL_INDEX + LEN_INDEX + digits; ++i){
		_msg[i] = size % 10;
		size /= 10;
	}

	return PROTOCOL_SUCCESS;
}

ProtocolStatus GetGrpsReplyUnpack (GetGrpsReply* _reply, const char* _msg)
{
	int factorial = 1;
	char numOfDigits;
	int i;

	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}

	/* assert(_msg[TAG_INDEX] == GET_GROUPS); */

	_reply->m_requestStatus = _msg[VAL_INDEX];
	_reply->m_totalPayloadsLen = 0;
	numOfDigits = _msg[LEN_INDEX] - sizeof(_reply->m_requestStatus);

	for(i = VAL_INDEX + LEN_INDEX; numOfDigits > 0; --numOfDigits, ++i, factorial *= 10){
		_reply->m_totalPayloadsLen += factorial * _msg[i];
	}

	return PROTOCOL_SUCCESS;
}

/*--JoinGroup--*/
ProtocolStatus JoinGrpRequestPack (const JoinGrpRequest* _request, char* _msg)
{
	if (!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	
	_msg[TAG_INDEX] = (char)JOIN_GROUP;
	_msg[LEN_INDEX] = (char)(sizeof(_request->m_usernameLen) + _request->m_usernameLen + sizeof(_request->m_groupNameLen) + _request->m_groupNameLen);
	_msg[VAL_INDEX] = (char)(_request->m_usernameLen);
	strncpy(_msg + VAL_INDEX + sizeof(_request->m_usernameLen), _request->m_username, (size_t)(_request->m_usernameLen));
	_msg[VAL_INDEX + sizeof(_request->m_usernameLen) + _request->m_usernameLen] = (char)_request->m_groupNameLen;
	strncpy(_msg + VAL_INDEX + sizeof(_request->m_usernameLen) + _request->m_usernameLen + sizeof(_request->m_groupNameLen), _request->m_groupName, (size_t)(_request->m_groupNameLen));

	return PROTOCOL_SUCCESS;
}

ProtocolStatus JoinGrpRequestUnpack (JoinGrpRequest* _request, const char* _msg)
{
	unsigned char lengthUsername;
	unsigned char lengthGroupName;
	char* startUsername;
	char* startGroupName;
	if(!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	lengthUsername = _msg[VAL_INDEX];
	_request->m_usernameLen = lengthUsername;
	startUsername = (char*)_msg + VAL_INDEX + sizeof(_request->m_usernameLen);
	strncpy(_request->m_username, startUsername, lengthUsername);
	_request->m_username[lengthUsername] = '\0';
	lengthGroupName = startUsername[lengthUsername];
	_request->m_groupNameLen = lengthGroupName;
	startGroupName = startUsername + lengthUsername + sizeof(_request->m_groupNameLen);
	strncpy(_request->m_groupName, startGroupName, lengthGroupName);
	_request->m_groupName[lengthGroupName] = '\0';
	return PROTOCOL_SUCCESS;
}

ProtocolStatus JoinGrpReplyPack(const JoinGrpReply* _reply, char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	_msg[TAG_INDEX] = (char)JOIN_GROUP;
	_msg[LEN_INDEX] = (char)(sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen) + _reply->m_mcastIPLen + sizeof(_reply->m_mcastPortLen) + _reply->m_mcastPortLen);
	_msg[VAL_INDEX] = (char)(_reply->m_requestStatus);
	_msg[VAL_INDEX + sizeof(_reply->m_requestStatus)] = (char)(_reply->m_mcastIPLen);
	strncpy(_msg + VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen), _reply->m_mcastIP,(size_t)(_reply->m_mcastIPLen));
	_msg[VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen) + _reply->m_mcastIPLen] = (char)(_reply->m_mcastPortLen);
	strncpy(_msg + VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen) + _reply->m_mcastIPLen + sizeof(_reply->m_mcastPortLen), _reply->m_mcastPort, (size_t)(_reply->m_mcastPortLen));
	return PROTOCOL_SUCCESS;
}

ProtocolStatus JoinGrpReplyUnpack (JoinGrpReply* _reply, const char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}

	_reply->m_requestStatus = (unsigned char)_msg[VAL_INDEX];
	_reply->m_mcastIPLen = (unsigned char)_msg[VAL_INDEX + sizeof(_reply->m_requestStatus)];
	strncpy(_reply->m_mcastIP, _msg + VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen), (size_t)(_reply->m_mcastIPLen));
	_reply->m_mcastIP[_reply->m_mcastIPLen] = '\0';
	_reply->m_mcastPortLen = (unsigned char)_msg[VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen) + _reply->m_mcastIPLen];
	strncpy(_reply->m_mcastPort, _msg + VAL_INDEX + sizeof(_reply->m_requestStatus) + sizeof(_reply->m_mcastIPLen) + _reply->m_mcastIPLen + sizeof(_reply->m_mcastPortLen), (size_t)(_reply->m_mcastPortLen));
	_reply->m_mcastPort[_reply->m_mcastPortLen] = '\0';

	return PROTOCOL_SUCCESS;
}

/*--LeaveGroup--*/
ProtocolStatus LeaveGrpRequestPack (const LeaveGrpRequest* _request, char* _msg)
{
	if (!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	_msg[TAG_INDEX] = (char)LEAVE_GROUP;
	_msg[LEN_INDEX] = (char)(sizeof(_request->m_usernameLen) + _request->m_usernameLen + sizeof(_request-> m_groupNameLen) + _request-> m_groupNameLen);
	_msg[VAL_INDEX] = (char)(_request->m_usernameLen);
	strncpy(_msg + VAL_INDEX + sizeof(_request->m_usernameLen), _request->m_username, (size_t)(_request->m_usernameLen));
	_msg[VAL_INDEX + sizeof(_request->m_usernameLen) + _request->m_usernameLen] = (char)_request->m_groupNameLen;
	strncpy(_msg + VAL_INDEX + sizeof(_request->m_usernameLen) + _request->m_usernameLen + sizeof(_request-> m_groupNameLen), _request->m_groupName, (size_t)(_request->m_groupNameLen));
	return PROTOCOL_SUCCESS;
}

ProtocolStatus LeaveGrpRequestUnpack (LeaveGrpRequest* _request, const char* _msg)
{
	unsigned char lengthUsername;
	unsigned char lengthGroupName;
	char* startUsername;
	char* startGroupName;
	if(!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	lengthUsername = _msg[VAL_INDEX];
	_request->m_usernameLen = lengthUsername;
	startUsername = (char*)_msg + VAL_INDEX + sizeof(_request->m_usernameLen);
	strncpy(_request->m_username, startUsername, lengthUsername);
	_request->m_username[lengthUsername] = '\0';
	lengthGroupName = startUsername[lengthUsername];
	_request->m_groupNameLen = lengthGroupName;
	startGroupName = startUsername + lengthUsername + sizeof(_request->m_groupNameLen);
	strncpy(_request->m_groupName, startGroupName, lengthGroupName);
	_request->m_groupName[lengthGroupName] = '\0';
	return PROTOCOL_SUCCESS;
}

ProtocolStatus LeaveGrpReplyPack (const LeaveGrpReply* _reply, char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	_msg[TAG_INDEX] = (char)LEAVE_GROUP;
	_msg[LEN_INDEX] = (char)sizeof(_reply->m_requestStatus);
	_msg[VAL_INDEX] = (char)(_reply->m_requestStatus);
	return PROTOCOL_SUCCESS;
}

ProtocolStatus LeaveGrpReplyUnpack (LeaveGrpReply* _reply, const char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}
	_reply->m_requestStatus = (unsigned char)_msg[VAL_INDEX];
	return PROTOCOL_SUCCESS;
}

/*--LogOut--*/
ProtocolStatus LogOutRequestPack (const LogOutRequest* _request, char* _msg)
{
	if(!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}

	_msg[TAG_INDEX] = (char)LOG_OUT;
	_msg[LEN_INDEX] = (char)(strlen(_request->m_username));
	strncpy(_msg + VAL_INDEX, _request->m_username, _msg[LEN_INDEX]);
	return PROTOCOL_SUCCESS;
}

ProtocolStatus LogOutRequestUnpack (LogOutRequest* _request, const char* _msg)
{
	if(!_request || !_msg)
	{
		return PROTOCOL_FAIL;
	}

	strncpy(_request->m_username, _msg + VAL_INDEX, _msg[LEN_INDEX]);
	_request->m_username[(int)_msg[LEN_INDEX]] = '\0';
	return PROTOCOL_SUCCESS;
}

ProtocolStatus LogOutReplyPack (const LogOutReply* _reply, char* _msg)
{
	if(!_reply || !_msg)
	{
		return PROTOCOL_FAIL;
	}

	_msg[TAG_INDEX] = (char)LOG_OUT;
	_msg[LEN_INDEX] = (char)sizeof(unsigned char);
	_msg[VAL_INDEX] = (char)_reply->m_requestStatus;
	return PROTOCOL_SUCCESS;
}

ProtocolStatus LogOutReplyUnpack (LogOutReply* _reply, const char* _msg)
{
	if(!_reply || !_msg || _msg[TAG_INDEX] != LOG_OUT )
	{
		return PROTOCOL_FAIL;
	}

	_reply->m_requestStatus = (unsigned char)_msg[VAL_INDEX];
	return PROTOCOL_SUCCESS;
}

/*--ChatMsg--*/
ProtocolStatus ChatMsgPack (const ChatMsg* _chat, char* _msgSend)
{
	if(!_chat || !_msgSend)
	{
		return PROTOCOL_FAIL;
	}

	_msgSend[TAG_INDEX] = (char)CHAT;
	_msgSend[LEN_INDEX] = (char)(_chat->m_usernameLen + _chat->m_chatMsgLen);
	_msgSend[VAL_INDEX] = (char)(_chat->m_usernameLen);
	strncpy(_msgSend + VAL_INDEX + sizeof(_chat->m_usernameLen), _chat->m_username, (size_t)(_chat->m_usernameLen));
	_msgSend[VAL_INDEX + sizeof(_chat->m_usernameLen) + _chat->m_usernameLen] = (char)(_chat->m_chatMsgLen);
	strncpy(_msgSend + VAL_INDEX + sizeof(_chat->m_usernameLen) + _chat->m_usernameLen + sizeof(_chat->m_chatMsgLen), _chat->m_chatMsg, (size_t)(_chat->m_chatMsgLen));

	return PROTOCOL_SUCCESS;
}

ProtocolStatus ChatMsgUnpack (ChatMsg* _chat, const char* _msgRecv)
{
	if(!_chat || !_msgRecv)
	{
		return PROTOCOL_FAIL;
	}

	_chat->m_usernameLen = (unsigned char)_msgRecv[VAL_INDEX];
	strncpy(_chat->m_username, _msgRecv + VAL_INDEX + sizeof(_chat->m_usernameLen), (size_t)(_chat->m_usernameLen));
	_chat->m_username[_chat->m_usernameLen] = '\0';
	_chat->m_chatMsgLen = (unsigned char)_msgRecv[VAL_INDEX + sizeof(_chat->m_usernameLen) + _chat->m_usernameLen];
	strncpy(_chat->m_chatMsg, _msgRecv + VAL_INDEX + sizeof(_chat->m_usernameLen) + _chat->m_usernameLen + sizeof(_chat->m_chatMsgLen), (size_t)(_chat->m_chatMsgLen));
	_chat->m_chatMsg[_chat->m_chatMsgLen] = '\0';

	return PROTOCOL_SUCCESS;
}

/* static functions */
static int countDigits(size_t _num){
	int counter = 1;

	while(_num >= 10){
		_num /= 10;
		++counter;
	}

	return counter;
}