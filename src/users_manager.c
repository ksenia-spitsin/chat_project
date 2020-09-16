#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "users_manager.h"
#include "chat_protocol_limits.h"
#include "BinTree.h"
#include "hashmap.h"
#include "List.h"
#include "ListItr.h"
#include "common_utils.h"

#define FILE_OF_USERS "Users.txt"

#define MAX_FD 1024
#define MAP_CAPACITY 50

#define ACTIVE 1
#define NOT_ACTIVE 0
#define SET_ACTIVE_USER(user, _FD) {(user)->m_isActive = ACTIVE; (user)->m_FD = (_FD);}
#define SET_NOT_ACTIVE_USER(user) {(user)->m_isActive = NOT_ACTIVE;}

typedef struct User
{
	char m_username[MAX_LEN_USERNAME];
	char m_password[MAX_LEN_PASSWORD];
	int m_FD;
	int m_isActive;
	List* m_groups;
}User;

struct UsersManager
{
	HashMap* m_users;
	FILE* m_fileUsers;
	const char* m_tableFD[MAX_FD];
};

static size_t HashFunc(const void* _user);
static int IsUsersEqual(const void* _firstUserKey, const void* _secondUserKey);
static void LoadFromFile(UsersManager* _manager);
static int SaveToFile(const void* _key, void* _user, void* _file);
static User* UserCreate(const char* _username, const char* _password);
static void UserDestroy(User* _user);
static UserManagerResult FindUser(UsersManager* _manager, const char* _username, User** _user);

void GroupNameDestroy(void* _groupName);
static void DestroyFunc(void* _element);

UsersManager* UsersManagerCreate()
{
	UsersManager* manager = (UsersManager*)calloc(1, sizeof(UsersManager));
	if(manager)
	{
		manager->m_users = HashMapCreate(MAP_CAPACITY, HashFunc, IsUsersEqual);
		if(!manager->m_users)
		{
			free(manager);
			return NULL;
		}
		manager->m_fileUsers = fopen(FILE_OF_USERS, "r");
		if(manager->m_fileUsers)
		{
			LoadFromFile(manager);
			fclose(manager->m_fileUsers);
		}
	}
	return manager;
}

UserManagerResult UsersManagerRegistration(UsersManager* _manager, const char* _username, const char* _password)
{
	User* newUser;
	MapResult status;
	void* findRes;

	if(!_manager || !_username || !_password)
	{
		return USER_GENERAL_ERROR;
	}
	/* search if user name allready exsist */
	if(MAP_SUCCESS == HashMapFind(_manager->m_users, _username, &findRes)) {
		return USERNAME_EXIST_ERROR;
	}

	newUser = UserCreate(_username, _password);
	if(!newUser)
	{
		return USER_GENERAL_ERROR;
	}
	
	status = HashMapInsert(_manager->m_users, newUser->m_username, newUser);
	if(status != MAP_SUCCESS)
	{
		UserDestroy(newUser);
		return USER_GENERAL_ERROR;
	}

	SET_NOT_ACTIVE_USER(newUser)
	return USER_OK;
}

UserManagerResult UsersManagerLogIn(UsersManager* _manager, const char* _username, const char* _password, int _FD)
{
	User* user;
	UserManagerResult status;
	if(!_manager || !_username || !_password || _FD < 0)
	{
		return USER_GENERAL_ERROR;
	}
	status = FindUser(_manager,_username, &user);
	if(status == USER_OK)
	{
		if(strcmp(user->m_password, _password))
		{
			return USER_PASSWORD_NOT_MATCH_ERROR;
		}
		if(user->m_isActive)
		{
			return USER_CONNECTED_ERROR;
		}
	}
	_manager->m_tableFD[_FD] = user->m_username;
	SET_ACTIVE_USER(user, _FD)
	return status;
}

UserManagerResult UsersManagerLogOut(UsersManager* _manager, const char* _username, int _FD)
{
	UserManagerResult status;
	User* user;
	if(!_manager || _FD < 0)
	{
		return USER_GENERAL_ERROR;
	}
	status = FindUser(_manager,_username, &user);
	if(status == USER_OK)
	{
		if(!user->m_isActive)
		{
			return USER_NOT_CONNECTED_ERROR;
		}
		SET_NOT_ACTIVE_USER(user)
	}
	return status;
}

UserManagerResult UserManagerAddGroup(UsersManager* _manager, const char* _username, const char* _groupName)
{
	UserManagerResult status;
	User* user;
	char* groupName;
	if(!_manager || !_username || !_groupName)
	{
		return USER_GENERAL_ERROR;
	}
	status = FindUser(_manager,_username, &user);
	if(status == USER_OK)
	{
		groupName = (char*)malloc(sizeof(char) * (strlen(_groupName) + 1));
		if(!groupName)
		{
			return USER_GENERAL_ERROR;;
		}
		strcpy(groupName, _groupName);
		if(ListPushTail(user->m_groups, groupName) != LIST_SUCCESS)
		{
			free(groupName);
			return USER_GENERAL_ERROR;
		}
	}
	return status;
}

UserManagerResult UserManagerRemoveGroup(UsersManager* _manager, const char* _username, const char* _groupName)
{
	UserManagerResult status;
	ListItr listItr;
	User* user;
	char* groupNameInList;
	if(!_manager || !_username || !_groupName)
	{
		return USER_GENERAL_ERROR;
	}
	status = FindUser(_manager,_username, &user);
	if(status == USER_OK)
	{
		listItr = ListItrBegin(user->m_groups);
		while(ListItrEquals(listItr,ListItrEnd(user->m_groups)))
		{
			groupNameInList = (char*)ListItrGet(listItr);
			if(!strcmp(groupNameInList, _groupName))
			{
				ListItrRemove(listItr);
				break;
			}
			listItr = ListItrNext(listItr);
		}
		return USER_NOT_IN_GROUPS_ERROR;
	}
	return status;
}
UserManagerResult UserManagerGetUsername(const UsersManager* _manager, int _FD, char* _username)
{
	if(!_manager || !_username || _FD < 0)
	{
		return USER_GENERAL_ERROR;
	}
	if(!(_manager->m_tableFD[_FD]))
	{
		return USER_NOT_CONNECTED_ERROR;
	}
	strcpy(_username, _manager->m_tableFD[_FD]);
	return USER_OK;
}

UserManagerResult UserManagerPopGroup(UsersManager* _manager, const char* _username, char* _groupName)
{
	UserManagerResult status;
	User* user;
	char* groupName;
	if(!_manager || !_username || !_groupName)
	{
		return USER_GENERAL_ERROR;
	}
	status = FindUser(_manager,_username, &user);
	if(status == USER_OK)
	{
		if(ListPopHead(user->m_groups, (void**)&groupName) == LIST_UNDERFLOW_ERROR)
		{
			return USER_NOT_IN_GROUPS_ERROR;
		}
		strcpy(_groupName, groupName);
		free(groupName);
	}
	return status;
}

void UserManagerDestroy(UsersManager* _manager)
{
	if(_manager)
	{
		_manager->m_fileUsers = fopen(FILE_OF_USERS, "w");
		if(_manager->m_fileUsers)
		{
			HashMapForEach(_manager->m_users, SaveToFile, _manager->m_fileUsers);
			fclose(_manager->m_fileUsers);
		}
		HashMapDestroy(&(_manager->m_users), NULL, DestroyFunc);
		free(_manager);
	}
}

static void LoadFromFile(UsersManager* _manager)
{
	char username[MAX_LEN_USERNAME];
	char password[MAX_LEN_PASSWORD];
	User* user;
	rewind(_manager->m_fileUsers);
	fscanf(_manager->m_fileUsers, "%s %s", username, password);
	while(!feof(_manager->m_fileUsers))
	{
		user = UserCreate(username, password);
		if(HashMapInsert(_manager->m_users, user->m_username, user) != MAP_SUCCESS)
		{
			UserDestroy(user);
			return;
		}
		fscanf(_manager->m_fileUsers, "%s %s", username, password);
	}
}

static User* UserCreate(const char* _username, const char* _password)
{
	User* user = (User*)malloc(sizeof(User));
	if(user)
	{
		user->m_groups = ListCreate();
		if(!user->m_groups)
		{
			free(user);
			return NULL;
		}
		strncpy(user->m_username, _username, MAX_LEN_USERNAME);
		strncpy(user->m_password, _password, MAX_LEN_PASSWORD);
		SET_NOT_ACTIVE_USER(user)
	}
	return user;
}

static void DestroyFunc(void* _element)
{
	UserDestroy((User*)_element);
}

static void UserDestroy(User* _user)
{
	if(_user)
	{
		ListDestroy(&(_user->m_groups), GroupNameDestroy);
		free(_user);
	}
}

/* static int LessUser(void* _userFirst, void* _userSecond)
{
	User* userFirst = (User*)_userFirst;
	User* userSecond = (User*)_userSecond;
	return strcmp(userFirst->m_username, userSecond->m_username);
} */

static int SaveToFile(const void* _key, void* _user, void* _file)
{
	User* user = (User*)_user;
	FILE* fileUsers = (FILE*)_file;

	UNUSED(_key);

	return fprintf(fileUsers, "%s %s\n", user->m_username, user->m_password);
}

static UserManagerResult FindUser(UsersManager* _manager, const char* _username, User** _user)
{
	if(HashMapFind(_manager->m_users, _username, (void**)_user) == MAP_KEY_NOT_FOUND_ERROR)
	{
		return USERNAME_NOT_EXIST_ERROR;
	}
	return USER_OK;
}

static size_t HashFunc(const void* _username)
{	
	char* username = (char*) _username;
	size_t sum = 0;
	size_t i;
	size_t usernameLength = strlen(_username);
	for(i = 0; i < usernameLength; i++)
	{
		sum += username[i];
	}
	return sum;
}

static int IsUsersEqual(const void* _firstUserKey, const void* _secondUserKey)
{
	char* firstUsername = (char*)_firstUserKey;
	char* secondUsername = (char*)_secondUserKey;
	return (!strcmp(firstUsername, secondUsername));
}

void GroupNameDestroy(void* _groupName)
{
	free(_groupName);
}
