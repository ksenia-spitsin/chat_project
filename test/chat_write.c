#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "chat_protocol.h"
#include "chat_protocol_limits.h"
#include "client_network.h"
#include "client_ui.h"
#include "common_utils.h"
#include "signal_sender.h"

#define ARGV_IP 1
#define ARGV_PORT 2
#define ARGV_GROUP 3
#define ARGV_USER 4

#define ARG_ERROR 1
#define NET_ERROR -1

int g_terminate = FALSE;
/* static void SetSigInt (); 
static void SigIntHandler (int _sig);*/
static void SetSigHup();
static void SigHupHandler(int _sig);

int main(int argc, char* argv[])
{
    int mcastSock;
    ChatMsg chat;
    char writeMsg[MAX_TOTAL_LEN_CHAT];

    if (argc < ARGV_USER + 1)
    {
        StatusPrint("Error: missing group address parameters and/or username\n");
        return ARG_ERROR;
    }

/*     SetSigInt(); */

    SetSigHup();

    mcastSock = CNetConnectToMulticast(argv[ARGV_IP], atoi(argv[ARGV_PORT]), FALSE);
    if (mcastSock < 0)
    {
        StatusPrint("Error connecting to group failed\n");
        return NET_ERROR;
    }

    ChatWelcomePrint(argv[ARGV_GROUP], FALSE);

    while (!g_terminate)
    {
        strcpy(chat.m_username, argv[ARGV_USER]);
        chat.m_usernameLen = strlen(chat.m_username);
        ChatMsgGetInput(chat.m_chatMsg);
        chat.m_chatMsgLen = strlen(chat.m_chatMsg);
        ChatMsgPack(&chat, writeMsg);

        if (CNetSendToMulticast(argv[ARGV_IP], atoi(argv[ARGV_PORT]), mcastSock, writeMsg, writeMsg[LEN_INDEX] + writeMsg[VAL_INDEX]) < 0)
        {
            StatusPrint("\nError sending the chat message - try again\n\n");
            continue;
        }
    }

    if (CNetDisconnectFromMulticast(mcastSock) < 0)
    {
        StatusPrint("Error disconnecting from group\n");
    }

    SendSignalToParent();

    return 0;
}

static void SetSigHup()
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

static void SigHupHandler(int _sig)
{
	UNUSED(_sig);
    g_terminate = TRUE;
}

/* 
static void SetSigInt ()
{
	struct sigaction SigAct;
	
	SigAct.sa_handler = SigIntHandler;
	SigAct.sa_flags = 0;
	sigemptyset(&SigAct.sa_mask);
	
	if (sigaction(SIGINT, &SigAct, NULL) < 0)
	{
	    perror("sigaction() failed");
	}
} */

/* static void SigIntHandler (int _sig)
{
	g_terminate = TRUE;
    UNUSED(_sig);
} */