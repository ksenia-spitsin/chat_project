#include <stdio.h>
#include <stddef.h> /* size_t */
#include <string.h>
#include "client_ui.h"
#include "chat_protocol_limits.h"

#define FALSE 0
#define TRUE !FALSE

extern int g_terminate;

CMenu WelcomeMenuGetSelection ()
{
	int selection;

	while (!g_terminate)
	{
		putchar('\n');
		printf("Welcome to The Best Chat Messenger!\n");
		putchar('\n');
		printf("1: Register (new user)\n");
		printf("2: Log-in (existing user)\n");
		printf("3: Quit application\n");
		putchar('\n');
		
		scanf("%d", &selection);
		putchar('\n');
		
		if (selection > 0 && selection <= 3)
		{
			return selection;
		}

		printf("Selection error - try again\n");
		putchar('\n');
	}

	return 0;
}

CMenu HomeMenuGetSelection (const char* _userToWelcome)
{
	int selection;

	while (1)
	{
		putchar('\n');
		printf("Hey %s, what would you like to do?\n", _userToWelcome);
		putchar('\n');
		printf("1: List the groups of The Best Chat Messenger\n");
		printf("2: Create a new group\n");
		printf("3: Join a group\n");
		printf("4: Leave a group\n");
		printf("5: Log-out from session\n");
		putchar('\n');
		
		scanf("%d", &selection);
		putchar('\n');
		
		if (selection > 0 && selection <= 5)
		{
			return selection + WELCOME_QUIT;
		}

		printf("Selection error - try again\n");
		putchar('\n');
	}
}

void FieldGetInput (char* _requestedField, char* _buffer, size_t _maxFieldSize, char _returnChar)
{
	char c;

	printf("\nEnter %s or '%c' to return to menu:\n", _requestedField, _returnChar);
	printf ("(%s over %ld chars will be trimmed)\n", _requestedField, _maxFieldSize);
	scanf("%16s", _buffer);
	while ((c = fgetc(stdin)) != '\n' && c != EOF); /* flush stdin */
	putchar('\n');
}

void GroupListPrint (const char* _groupList)
{
	if (_groupList)
    {
    	printf("%s\n", _groupList);
    }
}

void ChatWelcomePrint (const char* _groupName, int _flagRead)
{
	char welcomeRead[] = "\tWelcome to group ";
	char welcomeWrite[] = "\tGROUP ";
	char borderRead[] = ".--..--..--..--..--..--..--..--..--..--.";
	char borderWrite[] = "_____________________________";
	int indent = (strlen(borderRead) - ((strlen(welcomeRead) + strlen(_groupName))))/2;

	if (_groupName)
	{
		if (_flagRead)
		{
			printf("%*s", indent, welcomeRead);
			printf("%s\n", _groupName);
			printf("%s\n", borderRead);
		}
  		else
		{
			printf("%*s", indent, welcomeWrite);
			printf("%s\n", _groupName);
			printf("%s\n", borderWrite);
		}
		  
	}
}

void ChatMsgPrint (const char* _username, const char* _chatMsg)
{
    if (_username && _chatMsg)
    {
    	printf("%s: %s\n", _username, _chatMsg);
    }
}

void ChatMsgGetInput (char* _chatMsg)
{
	char writeMsg[MAX_TOTAL_LEN_CHAT + 1];
    
    if (_chatMsg)
    {
    	printf("\nEnter a message (max %d chars):\n", MAX_TOTAL_LEN_CHAT);
		fgets(writeMsg, MAX_TOTAL_LEN_CHAT, stdin);
		sprintf(_chatMsg, "%s", writeMsg);
    }
}

void StatusPrint (const char* _status)
{
	if (_status)
	{
		printf("%s", _status);
	}
}

/*
void UsernameGetInput (char* _bufferUN, char _returnChar)
{
	char c;

	printf("Enter Username or '%c' to return to menu:\n", _returnChar);
	printf ("(username over 16 chars will be trimmed)\n");
	scanf("%16s", _bufferUN);
	while ((c = fgetc(stdin)) != '\n' && c != EOF);
	putchar('\n');
}

void PasswordGetInput (char* _bufferPW, char _returnChar)
{
	char c;

	printf("Enter Password or '%c' to return to menu:\n", _returnChar);
	printf ("(password over 16 chars will be trimmed)\n");
	scanf("%16s", _bufferPW);
	while ((c = fgetc(stdin)) != '\n' && c != EOF);
	putchar('\n');
}
*/
