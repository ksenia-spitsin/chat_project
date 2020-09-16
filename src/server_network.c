#include <signal.h> /* sigaction */
#include <stdio.h>

#include "server_network.h"
#include "List.h"
#include "ListItr.h"
#include "ListFunctions.h"
#include "common_utils.h"

#define ERROR -1

int g_ifContinue = TRUE; 

static void InitSin(struct sockaddr_in* _sin, int _sinSize, int _port);
static int SetListenSocket(ServerNet* _server);
static ServerNetResult ServerSocketNoBlock(int _socket);
static void AcceptNewClients(ServerNet* _serverNet);
static void CheckCurrentClients(ServerNet* _serverNet, fd_set* _temp, int _activityNum);
static void RemoveClient(int _clientSock, ListItr* _itr, ServerNet* _serverNet);
static void ClientDestroy(void* _client);
static void SigIntHandler (int _sig);
static void SetSigInt();

struct ServerNet
{
	int m_port;
	int m_listenSock;
	fd_set m_master;
	struct sockaddr_in m_listen;
	struct sockaddr_in m_sin;
	List* m_clients;
	const void* m_activator;
	ReceiveReqFunc m_recvReqFunc;
};

ServerNet* ServerNetCreate(int _serverPort, ReceiveReqFunc _recv, const void* _activator)
{
	ServerNet* serverNet;
	if(!_recv || !_activator || _serverPort < 0)
	{
		return NULL;
	}
	 serverNet = (ServerNet*)malloc(sizeof(ServerNet));
	if(serverNet)
	{
		serverNet->m_clients = ListCreate();
		if(!serverNet->m_clients)
		{
			free(serverNet);
			return NULL;
		}
		FD_ZERO(&(serverNet->m_master));
		serverNet->m_port = _serverPort;
		InitSin(&(serverNet->m_listen), sizeof(serverNet->m_listen), _serverPort);
		InitSin(&(serverNet->m_sin), sizeof(serverNet->m_sin), _serverPort);
		if(SetListenSocket(serverNet) < 0)
		{
			ListDestroy(&(serverNet->m_clients), NULL);
			free(serverNet);
			return NULL;
		}
		serverNet->m_activator = _activator;
		serverNet->m_recvReqFunc = _recv;
	}

	return serverNet;
}

void ServerNetRun(ServerNet* _serverNet)
{
	int activity;
	fd_set temp;
	if(_serverNet)
	{
		SetSigInt();
		while(g_ifContinue)
		{
			temp = _serverNet->m_master;
			if( (activity = select(MAX_SOCKET_FD, &temp, NULL, NULL, NULL)) > 0)
			{
				if(FD_ISSET(_serverNet->m_listenSock, &temp))
				{
					AcceptNewClients(_serverNet);
					activity--;
				}
				if(activity)
				{
					CheckCurrentClients(_serverNet, &temp, activity);
				}
			}
		}
	}
}

int ServerNetReply(ServerNet* _serverNet, const char* _replyMsg, int _FD, size_t _msgLength)
{
	if(!_serverNet || !_replyMsg || _FD < 0)
	{
		return 0;
	}
	
	printf("tag = %d\n", _replyMsg[0]);
	printf("len = %d\n", _replyMsg[1]);
	printf("msg = %s\n", _replyMsg + 2);

	return send(_FD, _replyMsg, _msgLength, 0);
}

static void InitSin(struct sockaddr_in* _sin, int _sinSize, int _port)
{
	memset(_sin, 0, _sinSize);
	_sin->sin_family = AF_INET; 
	_sin->sin_addr.s_addr = INADDR_ANY;
	_sin->sin_port = htons(_port);
}

static int SetListenSocket(ServerNet* _server)
{
	int optval = 1;
	_server->m_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if(_server->m_listenSock > 0)
	{
		if(setsockopt(_server->m_listenSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		{
			perror("Failed\n");
		}
		ServerSocketNoBlock(_server->m_listenSock);
		if(bind(_server->m_listenSock, (struct sockaddr*)&(_server->m_listen), sizeof(_server->m_listen)) < 0)
		{
			perror("Failed:\n");
			close(_server->m_listenSock);
			return ERROR;
		}
		if(listen(_server->m_listenSock, BACK_LOG) < 0)
		{
			close(_server->m_listenSock);
			return ERROR;
		}
		FD_SET(_server->m_listenSock, &(_server->m_master));
	}
	return _server->m_listenSock;
}

static ServerNetResult ServerSocketNoBlock(int _socket)
{
	int flags;
	if((flags = fcntl(_socket, F_GETFL)) < 0)
	{
		return SERVER_FAIL_SET_NO_BLOCK_ERROR;
	}
	if(fcntl(_socket, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		return SERVER_FAIL_SET_NO_BLOCK_ERROR;
	}
	return SERVER_OK;
}

static void AcceptNewClients(ServerNet* _serverNet)
{
	int newClient;
	unsigned int sinLength = sizeof(_serverNet->m_sin);
	int* sockFDPtr;
	newClient = accept(_serverNet->m_listenSock, (struct sockaddr*)(&(_serverNet->m_sin)), &sinLength);
	if(newClient < 0)
	{
		return;
	}
	if(ListSize(_serverNet->m_clients) == MAX_CLIENTS)
	{
		close(newClient);
		return;		
	}
	sockFDPtr = (int*)malloc(sizeof(int));
	if(!sockFDPtr)
	{
		close(newClient);
		return;
	}
	*sockFDPtr = newClient;
	if(ListPushTail(_serverNet->m_clients, sockFDPtr) != LIST_SUCCESS)
	{
		free(sockFDPtr);
		close(newClient);
		return;
	}
	FD_SET(newClient, &(_serverNet->m_master));
}

static void CheckCurrentClients(ServerNet* _serverNet, fd_set* _temp, int _activityNum)
{
	char msg[MAX_TOTAL_LEN_MSG];
	ListItr currItr = ListItrBegin(_serverNet->m_clients);
	int clientSock;
	while(_activityNum)
	{
		clientSock = *((int*)ListItrGet(currItr));
		if(FD_ISSET(clientSock, _temp))
		{
			if(recv(clientSock, msg, sizeof(msg), 0) <= 0)
			{
				_serverNet->m_recvReqFunc(_serverNet->m_activator, NULL, clientSock);
				RemoveClient(clientSock, &currItr, _serverNet);
			}
			else
			{
				_serverNet->m_recvReqFunc(_serverNet->m_activator, msg, clientSock);
			}
			_activityNum--;
		}
		currItr = ListItrNext(currItr);
	}
}

void ServerNetDestroy(ServerNet* _serverNet)
{
	if(_serverNet)
	{
		ListDestroy(&(_serverNet->m_clients), ClientDestroy);
	}
}

static void ClientDestroy(void* _client)
{
	int sock = *(int*)_client;
	close(sock);
	free(_client);
}

static void RemoveClient(int _clientSock, ListItr* _itr, ServerNet* _serverNet)
{
	FD_CLR(_clientSock, &(_serverNet->m_master));
	close(_clientSock);
	*_itr = ListItrNext(*_itr);
	ListItrRemove(ListItrPrev(*_itr));
	*_itr = ListItrPrev(*_itr);
}

static void SigIntHandler (int _sig)
{
	UNUSED(_sig);

	g_ifContinue = FALSE;
}

static void SetSigInt()
{
	struct sigaction SigAct;
	
	SigAct.sa_handler = SigIntHandler;
	SigAct.sa_flags = 0;
	sigemptyset(&SigAct.sa_mask);
	
	if (sigaction(SIGINT, &SigAct, NULL) == -1)
	{
		perror("sigaction() failed");
		exit(1);
	}
}
