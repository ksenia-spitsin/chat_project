#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "client_manager.h"
#include "chat_protocol.h"
#include "chat_protocol_limits.h"
#include "client_group_manager.h"
#include "client_network.h"
#include "client_ui.h"
#include "List.h"
#include "common_utils.h"
#include "signal_sender.h"

extern int g_terminate;
extern sem_t semaphore;

typedef struct ReadWritePidPair
{
	pid_t writePid;
	pid_t readPid;
} ReadWritePidPair;

struct CManager
{
    size_t m_magicNum;
    CGroupMngr* m_groupMngr;
	List* m_readWritePids;
};

static void RegisterUser (int _serverFD);
static int LoginUser (int _serverFD, char* _user);
static void CreateGroup (CGroupMngr* _groupMngr, int _serverFD, const char* _user, List* _pidList);
static void ListGroups (int _serverFD, const char* _user);
static void JoinGroup (CGroupMngr* _groupMngr, int _serverFD, const char* _user, List* _pidList);
static void LeaveGroup (CGroupMngr* _groupMngr, const char* _groupName, const char* _user, int _serverFD);
static void LogoutUser (CGroupMngr* _groupMngr, int _serverFD, const char* _user);

/*static void PrintGroupsFromFile (const char* _fileName);*/
static int OpenChatWindows(const char* _mcastIP, int _mcastPort, const char* _groupName, const char* _username, List* _pidList);
static void LeaveAllGroups (CGroupMngr* _groupMngr, const char* _user, int _serverFD);
/* static void SetSigInt ();
static void SigIntHandler (int _sig); */
/* ------------------- */

static void SetSigChld();
static void SigChldHandler(int _sig);

static ReadWritePidPair* CreateReadWritePidPair(int _pidRead, int _pidWrite);
static void DestroyReadWritePidPair(void* _pair);

CManager* CManagerCreate ()
{
    CManager* manager = (CManager*)malloc(sizeof(CManager));
    if (!manager)
    {
        return NULL;
    }

    manager->m_groupMngr = CGroupMngrCreate(MAX_GROUPS);
    if (!manager->m_groupMngr)
    {
        free(manager);
        return NULL;
    }

/* 	SetSigInt(); */
	manager->m_readWritePids = ListCreate();
	if(!manager->m_readWritePids)
	{
		CGroupMngrDestroy(manager->m_groupMngr);
		free(manager);
		return NULL;
	}

	SetSigChld();
    manager->m_magicNum = MAGIC_NUM;

    return manager;
}

void CManagerDestroy (CManager* _manager)
{
    if (!_manager || _manager->m_magicNum != MAGIC_NUM)
    {
        return;
    }

    _manager->m_magicNum = NO_MAGIC_NUM;
    CGroupMngrDestroy(_manager->m_groupMngr);
	ListDestroy(&(_manager->m_readWritePids), DestroyReadWritePidPair);
    free(_manager);
}

void CManagerRun (CManager* _manager, const char* _configFile)
{
    FILE* configFP;
    char serverIP[MAX_LEN_IP + 1];
    char serverPort[MAX_LEN_PORT + 1];
    int serverFD;
    int loggedIn = FALSE;
	char user[MAX_LEN_USERNAME + 1] = "";

	configFP = fopen(_configFile, "r");
    if (!configFP)
    {
        StatusPrint("Error: failed to open address config file\n");
        return;
    }
    fscanf(configFP, "%s", serverIP);
    fscanf(configFP, "%s", serverPort);

	serverFD = CNetConnectToServer(serverIP, atoi(serverPort));
	if (serverFD == NET_ERROR)
	{
		StatusPrint("Error: connection with server failed - restart program\n");
		return;
	}

    while (!g_terminate)
    {
        if (!loggedIn)
        {
            switch (WelcomeMenuGetSelection())
            {
                case WELCOME_REGISTER:
                    RegisterUser(serverFD);
                    break;

                case WELCOME_LOGIN:
                	loggedIn = LoginUser(serverFD, user);
                    break;

                case WELCOME_QUIT:
                    g_terminate = TRUE;
                    break;

                default:
                	break;
            }
        }
        else
        {
            switch (HomeMenuGetSelection(user))
            {
                case HOME_GET_GROUPS:
                    ListGroups(serverFD, user);
/* 					PrintGroupsFromFile("groups.txt"); */
                    break;

                case HOME_CREATE_GROUP:
					CreateGroup(_manager->m_groupMngr, serverFD, user, _manager->m_readWritePids);
                    break;

                case HOME_JOIN_GROUP:
                    JoinGroup(_manager->m_groupMngr, serverFD, user, _manager->m_readWritePids);
                    break;

                case HOME_LEAVE_GROUP:
					StatusPrint("Currently not available :( Wait for next patch!\n");
                    /* LeaveGroup(); */
                    break;

                case HOME_LOGOUT:
					/* StatusPrint("Currently not available :( Wait for next patch!\n"); */
                    LogoutUser(_manager->m_groupMngr, serverFD, user);
                    loggedIn = FALSE;
                    break;

                default:
                	break;
            }
        }
    }
}

/* -------------------- */

static void RegisterUser (int _serverFD)
{
	int registered = FALSE;
    char username[MAX_LEN_USERNAME + 1];
    char password[MAX_LEN_PASSWORD + 1];
	struct RegRequest request;
    char requestMsg[MAX_TOTAL_LEN_MSG];
    struct RegReply reply;
    char replyMsg[MAX_TOTAL_LEN_MSG];
    int commStatus;

	while (!registered)
	{
		FieldGetInput("username", username, MAX_LEN_USERNAME, '#');
		if (!strcmp(username, "#"))
		{
			break;
		}
		strcpy(request.m_username, username);
		request.m_usernameLen = strlen(username);
		FieldGetInput("password", password, MAX_LEN_PASSWORD, '#');
		if (!strcmp(password, "#"))
		{
			break;
		}
		strcpy(request.m_password, password);
		request.m_passwordLen = strlen(password);
		
		RegRequestPack(&request, requestMsg);
		if (CNetSendToServer(_serverFD, requestMsg, requestMsg[LEN_INDEX] + VAL_INDEX) < 0)	
		{
			StatusPrint("Error: sending request to server failed - try again\n");
			continue;
		}
		commStatus = CNetReceiveFromServer(_serverFD, replyMsg, MAX_TOTAL_LEN_MSG);
		if (commStatus == NET_ERROR)
		{
			StatusPrint("Error: receiving reply from server failed - try again\n");
			continue;
		}
		else if (commStatus == NET_CLOSED)
		{
			StatusPrint("Error: server closed connection - restart program\n");
			g_terminate = TRUE;
			break;
		}
		
		if ((Tag)replyMsg[TAG_INDEX] == REGISTRATION)
		{
			RegReplyUnpack(&reply, replyMsg);
		}
		else
		{
			StatusPrint("Error: corrupt reply (tag)\n");
			continue;
		}
			
		switch ((RequestStatus)reply.m_requestStatus)
		{
			case OK:
				StatusPrint("Registration successful.\n");
				registered = TRUE;
				break;
		
			case USERNAME_ALREADY_EXISTS:
				StatusPrint("Username already exists\n");
				break;
				
			case INVALID_USERNAME:
				StatusPrint("Invalid Username\n");
				break;
				
			case INVALID_PASSWORD:
				StatusPrint("Invalid Password\n");
				break;
				
			default:
				StatusPrint("Error: corrupt reply (reply status)\n");
				break;
		}
	}
}

static int LoginUser (int _serverFD, char* _user)
{
    int loggedIn = FALSE;
    char username[MAX_LEN_USERNAME + 1];
    char password[MAX_LEN_PASSWORD + 1];
	struct LogInRequest request;
    char requestMsg[MAX_TOTAL_LEN_MSG];
    struct LogInReply reply;
    char replyMsg[MAX_TOTAL_LEN_MSG];
    int commStatus;

	while (!loggedIn)
	{
		FieldGetInput("username", username, MAX_LEN_USERNAME, '#');
		if (!strcmp(username, "#"))
		{
			break;
		}
		strcpy(request.m_username, username);
		request.m_usernameLen = strlen(username);
		FieldGetInput("password", password, MAX_LEN_PASSWORD, '#');
		if (!strcmp(password, "#"))
		{
			break;
		}
		strcpy(request.m_password, password);
		request.m_passwordLen = strlen(password);
		
		LogInRequestPack(&request, requestMsg);	
		if (CNetSendToServer(_serverFD, requestMsg, requestMsg[LEN_INDEX] + VAL_INDEX) < 0)	
		{
			StatusPrint("Error: sending request to server failed - try again\n");
			continue;
		}
		commStatus = CNetReceiveFromServer(_serverFD, replyMsg, MAX_TOTAL_LEN_MSG);
		if (commStatus == NET_ERROR)
		{
			StatusPrint("Error: receiving reply from server failed - try again\n");
			continue;
		}
		else if (commStatus == NET_CLOSED)
		{
			StatusPrint("Error: server closed connection - restart program\n");
			g_terminate = TRUE;
			break;
		}
		
		if ((Tag)replyMsg[TAG_INDEX] == LOG_IN)
		{
			LogInReplyUnpack(&reply, replyMsg);
		}
		else
		{
			StatusPrint("Error: corrupt reply (tag)\n");
			continue;
		}
			
		switch ((RequestStatus)reply.m_requestStatus)
		{
			case OK:
				StatusPrint("Log-in successful. :)\n");
				strcpy(_user, username);
				loggedIn = TRUE;
				break;
		
			case USERNAME_DOESNT_EXIST:
				StatusPrint("Username doesn't exist\n");
				break;
				
			case WRONG_PASSWORD:
				StatusPrint("Username and password don't match\n");
				break;
				
			case USER_ALREADY_CONNECTED:
				StatusPrint("User associated with username is already connected\n");
				break;
				
			default:
				StatusPrint("Error: corrupt reply (reply status)\n");
				break;
		}
	}
	
	return loggedIn;
}

static void ListGroups (int _serverFD, const char* _user)
{
	/*PrintGroupsFromFile(GROUPS_FILE);*/
	struct GetGrpsRequest request;
    char requestMsg[MAX_TOTAL_LEN_MSG];
    struct GetGrpsReply reply;
    char replyMsg[MAX_TOTAL_LEN_MSG] = {0};
    int rcvdBytes;
	RequestStatus reqStat;

	strcpy(request.m_username, _user);
 	GetGrpsRequestPack(&request, requestMsg);	
	if (CNetSendToServer(_serverFD, requestMsg, requestMsg[LEN_INDEX] + VAL_INDEX) < 0)	
	{
		StatusPrint("Error: sending request to server failed - try again\n");
		return;
	}

	while (!g_terminate)
	{
		rcvdBytes = CNetReceiveFromServer(_serverFD, replyMsg, MAX_TOTAL_LEN_MSG);
		if (rcvdBytes == NET_ERROR)
		{
			StatusPrint("Error: receiving reply from server failed - try again\n");
			break;
		}
		else if (rcvdBytes == NET_CLOSED)
		{
			StatusPrint("Error: server closed connection - restart program\n");
			g_terminate = TRUE;
			break;
		}

		GetGrpsReplyUnpack(&reply, replyMsg);

		/*if (replyMsg[TAG_INDEX] == GET_GROUPS)
		{
 			GetGrpsReplyUnpack(&reply, replyMsg);
		}
		else
		{
			printf("client man list group tag = %d \n",(Tag)replyMsg[TAG_INDEX]);
			StatusPrint("Error: corrupt reply (tag) - try again\n");
			break;
		}*/

		reqStat = (RequestStatus)reply.m_requestStatus;

		if (reqStat == OK)
		{
			size_t msgSize = reply.m_totalPayloadsLen;
			char* groupsListBuffer = (char*)malloc(msgSize + 1);
			char* currentPos = groupsListBuffer;

			if(groupsListBuffer == NULL) {
				StatusPrint("Error: corrupt reply (reply status)\n");
				break;
			}

			while(msgSize > 0) {
				rcvdBytes = CNetReceiveFromServer(_serverFD, currentPos, msgSize);
				msgSize -= rcvdBytes;
				currentPos += rcvdBytes;
			}

			printf("buffer after rcv = %s\n", groupsListBuffer);
			free(groupsListBuffer);
			break;
		}
		else if (reqStat == NO_GROUPS)
		{
			StatusPrint("No groups to display\n");
			break;
		}
		else
		{
			StatusPrint("Error: corrupt reply (reply status)2323634\n");
			break;
		}
	}
}

static void CreateGroup(CGroupMngr* _groupMngr, int _serverFD, const char* _user, List* _pidList)
{
	int createdGroup = FALSE;
	char groupName[MAX_LEN_GROUP_NAME + 1];
	struct CreateGrpRequest request;
    char requestMsg[MAX_TOTAL_LEN_MSG];
    struct CreateGrpReply reply;
    char replyMsg[MAX_TOTAL_LEN_MSG];
    int rcvdBytes;
	int mcastSock;

	while (!createdGroup)
	{
		FieldGetInput("group name", groupName, MAX_LEN_GROUP_NAME, '#');
		if (!strcmp(groupName, "#"))
		{
			break;
		}

		strcpy(request.m_groupName, groupName);
		request.m_groupNameLen = strlen(groupName);
		strcpy(request.m_username, _user);
		request.m_usernameLen = strlen(_user);
		
		CreateGrpRequestPack(&request, requestMsg);	
		if (CNetSendToServer(_serverFD, requestMsg, requestMsg[LEN_INDEX] + VAL_INDEX) < 0)	
		{
			StatusPrint("Error: sending request to server failed - try again\n");
			continue;
		}
		rcvdBytes = CNetReceiveFromServer(_serverFD, replyMsg, MAX_TOTAL_LEN_MSG);
		if (rcvdBytes == NET_ERROR)
		{
			StatusPrint("Error: receiving reply from server failed - try again\n");
			continue;
		}
		else if (rcvdBytes == NET_CLOSED)
		{
			StatusPrint("Error: server closed connection - restart program\n");
			g_terminate = TRUE;
			break;
		}
		
		printf("client man create group tag = %d \n",(Tag)replyMsg[TAG_INDEX]);

		if((Tag)replyMsg[TAG_INDEX] == CREATE_GROUP)
		{
			CreateGrpReplyUnpack(&reply, replyMsg);
		}
		else
		{
			StatusPrint("Error: corrupt reply (tag)\n");
			continue;
		}

		/*printf("error = %d\n",(RequestStatus)reply.m_requestStatus);	*/
		switch ((RequestStatus)reply.m_requestStatus)
		{
			case OK:
/* 				strncpy(IPStr, reply.m_mcastIP, (size_t)reply.m_mcastIPLen);
				strncpy(portStr, reply.m_mcastPort, (size_t)reply.m_mcastPortLen); */
/* 				mcastSock = OpenChatWindows(IPStr, atoi(portStr), groupName, _user); */
				mcastSock = OpenChatWindows(reply.m_mcastIP, atoi(reply.m_mcastPort), groupName, _user, _pidList);
				if (mcastSock < 0)
				{
					continue;
				}
				CGroupMngrAddGroup(_groupMngr, groupName, mcastSock);
				createdGroup = TRUE;
				break;
		
			case GROUP_ALREADY_EXISTS:
				StatusPrint("Error: A group with this name already exists\n");
				break;

			case MAX_GROUP_ERROR:
				StatusPrint("Error: Reached max group capacity\n");
				break;

			default:
				StatusPrint("Error: corrupt reply (reply status)\n");
				break;
		}
	}
}

static void JoinGroup(CGroupMngr* _groupMngr, int _serverFD, const char* _user, List* _pidList)
{
	int joinedGroup = FALSE;
	char groupName[MAX_LEN_GROUP_NAME + 1];
	struct JoinGrpRequest request;
    char requestMsg[MAX_TOTAL_LEN_MSG];
    struct JoinGrpReply reply;
    char replyMsg[MAX_TOTAL_LEN_MSG];
    int rcvdBytes;
	int mcastSock;
	char IPStr[MAX_LEN_IP];
	char portStr[MAX_LEN_PORT];

	while (!joinedGroup)
	{
		FieldGetInput("group name", groupName, MAX_LEN_GROUP_NAME, '#');
		if (!strcmp(groupName, "#"))
		{
			break;
		}
		if (HasCGroupMngrGroup(_groupMngr, groupName))
		{
			StatusPrint("Error: You are already in this group\n");
			continue;
		}

		strcpy(request.m_groupName, groupName);
		request.m_groupNameLen = strlen(groupName);
		strcpy(request.m_username, _user);
		request.m_usernameLen = strlen(_user);
		
		JoinGrpRequestPack(&request, requestMsg);	
		if (CNetSendToServer(_serverFD, requestMsg, requestMsg[LEN_INDEX] + VAL_INDEX) < 0)	
		{
			StatusPrint("Error: sending request to server failed - try again\n");
			continue;
		}
		rcvdBytes = CNetReceiveFromServer(_serverFD, replyMsg, MAX_TOTAL_LEN_MSG);
		if (rcvdBytes == NET_ERROR)
		{
			StatusPrint("Error: receiving reply from server failed - try again\n");
			continue;
		}
		else if (rcvdBytes == NET_CLOSED)
		{
			StatusPrint("Error: server closed connection - restart program\n");
			g_terminate = TRUE;
			break;
		}
		
		if ((Tag)replyMsg[TAG_INDEX] == JOIN_GROUP)
		{
			JoinGrpReplyUnpack(&reply, replyMsg);
		}
		else
		{
			StatusPrint("Error: corrupt reply (tag)\n");
			continue;
		}
			
		switch ((RequestStatus)reply.m_requestStatus)
		{
			case OK:
				strncpy(IPStr, reply.m_mcastIP, (size_t)reply.m_mcastIPLen);
				strncpy(portStr, reply.m_mcastPort, (size_t)reply.m_mcastPortLen);
				mcastSock = OpenChatWindows(IPStr, atoi(portStr), groupName, _user, _pidList);
				if (mcastSock < 0)
				{
					continue;
				}
				CGroupMngrAddGroup(_groupMngr, groupName, mcastSock);
				joinedGroup = TRUE;
				break;
		
			case GROUP_DOESNT_EXIST:
				StatusPrint("Error: Group entered doesn't exist\n");
				break;

			default:
				StatusPrint("Error: corrupt reply (reply status)\n");
				break;
		}
	}
}

static int OpenChatWindows(const char* _mcastIP, int _mcastPort, const char* _groupName, const char* _username, List* _pidList)
{
	pid_t readPid, writePid;
	char commandRead[100];
	char commandWrite[100];

	sprintf(commandRead, "gnome-terminal --geometry=100x40+100-200 sh --command \"test/chat_read %s %d %s\"", _mcastIP, _mcastPort, _groupName);
	sprintf(commandWrite, "gnome-terminal --geometry=100x40+100-200 sh --command \"test/chat_write %s %d %s %s\"", _mcastIP, _mcastPort, _groupName, _username);

	readPid = fork();
	if(readPid == 0)
	{
		execl("/bin/sh", "sh", "-c", commandRead, (char *) 0);
	}
	else
	{
		writePid = fork();
		if(writePid == 0)
		{
			execl("/bin/sh", "sh", "-c", commandWrite, (char *) 0);
		}
		else
		{
			ReadWritePidPair* pair = CreateReadWritePidPair(readPid, writePid);
			if(!pair)
			{
				SendSignalInit(writePid);
				SendSignalInit(readPid);
				waitpid(writePid, NULL, 0);
				waitpid(readPid, NULL, 0);
				return FALSE;
			}
			ListPushTail(_pidList, pair);
		}
	}

	return TRUE;
}

static void LeaveGroup (CGroupMngr* _groupMngr, const char* _groupName, const char* _user, int _serverFD)
{
	UNUSED(_groupMngr);
	UNUSED(_groupName);
	UNUSED(_user);
	UNUSED(_serverFD);
	
	/*remove from group 
		if your the last one in the group need to remove the group from groups*/
	
}

static void LogoutUser (CGroupMngr* _groupMngr, int _serverFD, const char* _user)
{
	LogOutRequest request;
	LogOutReply reply;
	char requestMsg[MAX_TOTAL_LEN_MSG];
	char replyMsg[MAX_TOTAL_LEN_MSG];
	int rcvdBytes;
 	
	strcpy(request.m_username, _user);
	assert(PROTOCOL_SUCCESS == LogOutRequestPack(&request, requestMsg));	

	if(CNetSendToServer(_serverFD, requestMsg, requestMsg[LEN_INDEX] + VAL_INDEX) < 0)	
	{
		StatusPrint("Error: sending request to server failed - try again\n");
	}

	rcvdBytes = CNetReceiveFromServer(_serverFD, replyMsg, MAX_TOTAL_LEN_MSG);
	if (rcvdBytes == NET_ERROR)
	{
		StatusPrint("Error: receiving reply from server failed - try again\n");
	}
	else if (rcvdBytes == NET_CLOSED)
	{
		StatusPrint("Error: server closed connection - restart program\n");
	}
	
	assert(PROTOCOL_SUCCESS == LogOutReplyUnpack(&reply, replyMsg));

	switch (reply.m_requestStatus)
	{
		case OK:
			LeaveAllGroups(_groupMngr, _user, _serverFD);
			break;

		case LOGOUT_FAILED:
			StatusPrint("Error: logout failed! please try again :) \n");
			break;
			
		default:
			StatusPrint("Error: corrupt reply (reply status)\n");
			break;
	}
}


/* ------------------ helpful functions ----------------- */
/* static void PrintGroupsFromFile (const char* _fileName)
{
	FILE* fp;
	char groupName[MAX_LEN_GROUP_NAME];

	fp = fopen(_fileName, "r");
	if (!fp)
	{
		return;
	}

	StatusPrint("-*- GROUPS: -*-\n\n");
	fscanf(fp, "%s", groupName);
	while (!feof(fp))
	{
		GroupListPrint(groupName);
		fscanf(fp, "%s", groupName);
	}
} */

static void LeaveAllGroups(CGroupMngr* _groupMngr, const char* _user, int _serverFD)
{
	FILE* fp;
	char groupName[MAX_LEN_GROUP_NAME + 1];
	
	if(GROUP_OK != WriteGroupsToFile(_groupMngr ,LOGOUT_FILE))
	{
		StatusPrint("Error: logout failed - try again\n");
		return;
	}
	
	fp = fopen(LOGOUT_FILE, "r");
	if(NULL == fp)
	{
		StatusPrint("Error: logout failed - try again\n");
		return;
	}
	
	fgets(groupName, MAX_LEN_GROUP_NAME + 1, fp);	
	while(!feof(fp))
	{
		LeaveGroup(_groupMngr ,groupName, _user, _serverFD);
		fgets(groupName, MAX_LEN_GROUP_NAME + 1, fp);
	}
}



/* static void SetSigInt ()
{
	struct sigaction SigAct;
	
	SigAct.sa_handler = SigIntHandler;
	SigAct.sa_flags = 0;
	sigemptyset(&SigAct.sa_mask);
	
	if (sigaction(SIGINT, &SigAct, NULL) < 0)
	{
 		perror("sigaction() failed");
		exit(1); 
	}
}

static void SigIntHandler (int _sig)
{
	g_terminate = TRUE;
} */

static void SetSigChld()
{
	struct sigaction SigAct;
	
	SigAct.sa_handler = SigChldHandler;
	SigAct.sa_flags = 0;
	sigemptyset(&SigAct.sa_mask);
	
	if (sigaction(SIGCHLD, &SigAct, NULL) < 0)
	{
 		perror("sigaction() failed");
	}
}

static void SigChldHandler(int _sig)
{
	printf("Got signal!\n");

	UNUSED(_sig);
	/*
	sem_post(&semaphore);
	fp = fopen(SHARED_FILE, "r");
	fscanf("%d, %s", &childId, groupName);
	*/
}

static ReadWritePidPair* CreateReadWritePidPair(int _pidRead, int _pidWrite)
{
	ReadWritePidPair* pair = (ReadWritePidPair*)malloc(sizeof(ReadWritePidPair));
	if(pair)
	{
		pair->readPid = _pidRead;
		pair->writePid = _pidWrite;
	}
	return pair;
}

static void DestroyReadWritePidPair(void* _pair)
{
	free(_pair);
}