#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "client_network.h"
#include <fcntl.h>
#include <netinet/ip.h> /* struct ip_mreq */

/* static int SetClientNonBlocking(int _socket); */

int CNetConnectToServer (const char* _serverIP, int _serverPort)
{
	struct sockaddr_in sin;
	int sock;
	
	memset(&sin, 0, sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(_serverIP);
	if ((sin.sin_addr.s_addr) == ( in_addr_t)(-1))
	{
		return NET_ERROR;
	}
	sin.sin_port = htons(_serverPort);
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		return NET_ERROR;
	}
	
	if (connect(sock, (struct sockaddr*)&sin, sizeof(struct sockaddr_in)) < 0)
	{
		close(sock);
		return NET_ERROR;
	}

/* 	if(1 != SetClientNonBlocking(sock)){
		printf("SetClientNonBlocking fail\n");
		return NET_ERROR;
	} */
	
	return sock;
}

int CNetDisconnectFromServer (int _serverFD)
{
	return close(_serverFD);
}

int CNetSendToServer (int _serverFD, const char* _bufferSend, size_t _lenSend)
{
	return send(_serverFD, _bufferSend, _lenSend, 0);
}

int CNetReceiveFromServer (int _serverFD, char* _bufferRecv, size_t _lenRecv)
{
	/*int ReadBytesFromServer = 0;
	int currentsBytes = 0;
	char* bufferPtr = _bufferRecv;

	while(1) {
		currentsBytes = recv(_serverFD, bufferPtr, _lenRecv, 0);
		ReadBytesFromServer += currentsBytes;
		bufferPtr += currentsBytes;

		if(currentsBytes <= 0){
			break;
		}
	}

	return ReadBytesFromServer;*/

	return recv(_serverFD, _bufferRecv, _lenRecv, 0);
}

int CNetConnectToMulticast (const char* _mcastIP, int _mcastPort, int _flagRead)
{
	int mcastSock;
	int optVal = 1;
	struct sockaddr_in sin;
	struct ip_mreq mreq;
	
	mcastSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (mcastSock < 0)
	{
		return NET_ERROR;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(_mcastIP);
	if ((sin.sin_addr.s_addr) == ( in_addr_t)(-1))
	{
		return NET_ERROR;
	}
	sin.sin_port = htons(_mcastPort);

	if (_flagRead)
	{
		if (setsockopt(mcastSock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0)
		{
			return NET_ERROR;
		}

		if (bind(mcastSock, (struct sockaddr*)&sin, sizeof(sin)) < 0)
		{
			return NET_ERROR;
		}
	}
	
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr = sin.sin_addr;
	mreq.imr_interface.s_addr = INADDR_ANY;

	if (setsockopt(mcastSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		perror("setsockopt mcast");
		return NET_ERROR;
	}

	return mcastSock;
}

int CNetDisconnectFromMulticast (int _mcastFD)
{
	return close(_mcastFD);
}

int CNetReceiveFromMulticast (const char* _mcastIP, int _mcastPort, int _mcastFD, char* _bufferRecv, size_t _lenRecv)
{
	struct sockaddr_in sinSrc;
	socklen_t sinLen = sizeof(sinSrc);

	memset(&sinSrc, 0, sizeof(sinSrc));
	sinSrc.sin_family = AF_INET;
	sinSrc.sin_addr.s_addr = inet_addr(_mcastIP);
	if ((sinSrc.sin_addr.s_addr) == ( in_addr_t)(-1))
	{
		return NET_ERROR;
	}
	sinSrc.sin_port = htons(_mcastPort);

	return recvfrom(_mcastFD, _bufferRecv, _lenRecv, 0, (struct sockaddr*)&sinSrc, &sinLen);
}
/* don't forget to null terminate the buffer before printing */

int CNetSendToMulticast (const char* _mcastIP, int _mcastPort, int _mcastFD, const char* _bufferSend, size_t _lenSend)
{
	struct sockaddr_in sinDest;
	socklen_t sinLen = sizeof(sinDest);

	memset(&sinDest, 0, sizeof(sinDest));
	sinDest.sin_family = AF_INET;
	sinDest.sin_addr.s_addr = inet_addr(_mcastIP);
	if ((sinDest.sin_addr.s_addr) == ( in_addr_t)(-1))
	{
		return NET_ERROR;
	}
	sinDest.sin_port = htons(_mcastPort);
	
	return sendto(_mcastFD, _bufferSend, _lenSend, 0, (struct sockaddr*)&sinDest, sinLen);
}

/* static int SetClientNonBlocking(int _socket)
{
	int flags;
	if((flags = fcntl(_socket, F_GETFL)) < 0)
	{
		return NET_ERROR;
	}
	if(fcntl(_socket, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		return NET_ERROR;
	}

	return 1;
} */


/* int CNetCommunicateWithServer (int _serverFD, const char* _bufferSend, size_t _lenSend, char* _bufferRecv, size_t _lenRecv)
{
	int rcvdBytes;

	if(send(_serverFD, _bufferSend, _lenSend, 0) < 0)
	{
		return NET_ERROR;
	}
	
	rcvdBytes = recv(_serverFD, _bufferRecv, _lenRecv, 0);
	if (rcvdBytes < 0)
	{
		return NET_ERROR;
	}
	else if (rcvdBytes == 0)
	{
		return NET_CLOSED;
	}
	
	return rcvdBytes;
} */