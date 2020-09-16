#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include <stddef.h>

#define NET_ERROR -1
#define NET_CLOSED 0

/* TCP */
int CNetConnectToServer (const char* _serverIP, int _serverPort); /* returns serverFD or error */
int CNetDisconnectFromServer (int _serverFD);
int CNetSendToServer (int _serverFD, const char* _bufferSend, size_t _lenSend);
int CNetReceiveFromServer (int _serverFD, char* _bufferRecv, size_t _lenRecv);

/* UDP */
int CNetConnectToMulticast (const char* _mcastIP, int _mcastPort, int _flagRead); /* returns mcastFD or error */
int CNetDisconnectFromMulticast (int _mcastFD);
int CNetReceiveFromMulticast (const char* _mcastIP, int _mcastPort, int _mcastFD, char* _bufferRecv, size_t _lenRecv);
int CNetSendToMulticast (const char* _mcastIP, int _mcastPort, int _mcastFD, const char* _bufferSend, size_t _lenSend);

/* or enum ClientStatus */
/* int CNetCommunicateWithServer (int _serverFD, const char* _bufferSend, size_t _lenSend, char* _bufferRecv, size_t _lenRecv); */

#endif /* CLIENT_NETWORK_H */
