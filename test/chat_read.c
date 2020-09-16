#include <stdio.h> /* DEBUG */
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>
#include "chat_protocol.h"
#include "chat_protocol_limits.h"
#include "client_network.h"
#include "client_ui.h"
#include "common_utils.h"
#include "signal_sender.h"

#define ARGV_IP 1
#define ARGV_PORT 2
#define ARGV_GROUP 3

#define ARG_ERROR 1
#define NET_ERROR -1

extern sem_t semaphore;

int g_terminate = FALSE;

static void SetSigHup();
static void SigHupHandler(int _sig);

int main (int argc, char* argv[])
{
    int mcastSock;
    char readMsg[MAX_TOTAL_LEN_CHAT];
    ChatMsg chat;

    if (argc < ARGV_GROUP + 1)
    {
        StatusPrint("Error: missing group address parameters\n");
        return ARG_ERROR;
    }

    SetSigHup();

    mcastSock = CNetConnectToMulticast(argv[ARGV_IP], atoi(argv[ARGV_PORT]), TRUE);
    if (mcastSock < 0)
    {
        StatusPrint("Error connecting to group failed\n");
        return NET_ERROR;
    }

    ChatWelcomePrint(argv[ARGV_GROUP], TRUE);
    
    while (!g_terminate)
    {
        if (CNetReceiveFromMulticast(argv[ARGV_IP], atoi(argv[ARGV_PORT]), mcastSock, readMsg, MAX_TOTAL_LEN_CHAT) < 0)
        {
            StatusPrint("Error reading a chat message\n");
            continue;
        }

        ChatMsgUnpack(&chat, readMsg);
        ChatMsgPrint(chat.m_username, chat.m_chatMsg);
    }

    if (CNetDisconnectFromMulticast(mcastSock) < 0)
    {
        StatusPrint("Error disconnecting from group\n");
    }
    
    SendSignalToParent();

    return 0;
}

static void SetSigHup ()
{
	struct sigaction SigAct;
	
	SigAct.sa_handler = SigHupHandler;
	SigAct.sa_flags = 0;
	sigemptyset(&SigAct.sa_mask);
	
	if (sigaction(SIGHUP, &SigAct, NULL) < 0)
	{
 		perror("sigaction() failed");
	}
}

static void SigHupHandler (int _sig)
{
	UNUSED(_sig);
    g_terminate = TRUE;
}