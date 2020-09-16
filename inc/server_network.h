#ifndef __SERVER_NETWORK_H__
#define __SERVER_NETWORK_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>

#include "chat_protocol_limits.h"

#define BACK_LOG 100
#define MAX_SOCKET_FD 1024

typedef struct ServerNet ServerNet;

typedef enum ServerNetResult
{
	SERVER_OK = 1,
	SERVER_NULL_ERROR,
	SERVER_FAIL_SET_NO_BLOCK_ERROR,
	SERVER_SEND_FAILED
} ServerNetResult;

typedef void (*ReceiveReqFunc)(const void* _activator,const char* _requestMsg, int _srcFD);

ServerNet* ServerNetCreate(int _serverPort, ReceiveReqFunc _recv, const void* _activator);
void ServerNetDestroy (ServerNet* _serverNet);
void ServerNetRun (ServerNet* _serverNet);
int ServerNetReply(ServerNet* _serverNet, const char* _replyMsg, int _FD, size_t _msgLength); /* returns sentBytes, or status */
int ServerNetSetMulticast (const char* _mcastIP, int _mcastPort); /* returns mcast socket, or status */

#endif /*__SERVER_NETWORK_H__*/