#ifndef CLIENT_UI_H
#define CLIENT_UI_H

#include <stddef.h> /* size_t */

typedef enum CMenu
{
    WELCOME_REGISTER = 1,
    WELCOME_LOGIN,
    WELCOME_QUIT,
    HOME_GET_GROUPS,
    HOME_CREATE_GROUP,
    HOME_JOIN_GROUP,
    HOME_LEAVE_GROUP,
    HOME_LOGOUT
} CMenu;

CMenu WelcomeMenuGetSelection (); /* if error in selection, func will prompt again */
CMenu HomeMenuGetSelection (const char* _userToWelcome); /* if error in selection, func will prompt again */
void FieldGetInput (char* _requestedField, char* _buffer, size_t _maxFieldSize, char _returnChar);
void GroupListPrint (const char* _groupList);

void ChatWelcomePrint (const char* _groupName, int _flagRead);
void ChatMsgPrint (const char* _username, const char* _chatMsg);
void ChatMsgGetInput (char* _chatMsg);

void StatusPrint (const char* _status);

/*
void GroupListPrint (const char* _groupList, size_t _totalLists);

void UsernameGetInput (char* _bufferUN, char _returnChar);
void PasswordGetInput (char* _bufferPW, char _returnChar);
void GroupNameGetInput (char* _bufferGN, char _returnChar);
*/

#endif /* CLIENT_UI_H */
