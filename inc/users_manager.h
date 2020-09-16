#ifndef __USERS_MANAGER_H__
#define __USERS_MANAGER_H__

typedef struct UsersManager UsersManager;

typedef enum UserManagerResult
{
	USER_OK,
	USERNAME_EXIST_ERROR,
	USERNAME_NOT_EXIST_ERROR,
	USER_PASSWORD_NOT_MATCH_ERROR,
	USER_CONNECTED_ERROR,
	USER_NOT_CONNECTED_ERROR,
	USER_NOT_IN_GROUPS_ERROR,
	USER_GENERAL_ERROR
} UserManagerResult;

UsersManager* UsersManagerCreate(); /*load from file of all users*/ 

UserManagerResult UsersManagerRegistration(UsersManager* _manager, const char* _username, const char* _password);

UserManagerResult UsersManagerLogIn(UsersManager* _manager, const char* _username, const char* _password, int _FD);

UserManagerResult UsersManagerLogOut(UsersManager* _manager, const char* _username, int _FD);

UserManagerResult UserManagerAddGroup(UsersManager* _manager, const char* _username, const char* _groupName);

UserManagerResult UserManagerRemoveGroup(UsersManager* _manager, const char* _username, const char* _groupName);

UserManagerResult UserManagerPopGroup(UsersManager* _manager, const char* _username, char* _groupName);

UserManagerResult UserManagerGetUsername(const UsersManager* _manager, int _FD, char* _username);

void UserManagerDestroy(UsersManager* _manager);

#endif /*__USERS_MANAGER_H__*/
