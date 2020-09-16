#include <stddef.h> /* NULL */
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "client_manager.h"
#include "common_utils.h"

#define CONFIG_FILE "addr_config"

int g_terminate = FALSE;

/* static void SetSigInt ();*/
/* static void SigIntHandler (int _sig);  */

int main ()
{
	CManager* client;

/* 	SetSigInt(); */

	client = CManagerCreate();
	if (!client)
	{
		return 1;
	}
	CManagerRun(client, CONFIG_FILE);
	CManagerDestroy(client);

	return 0;
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
} */


/*  static void SigIntHandler (int _sig)
{
	g_terminate = TRUE;
	UNUSED(_sig);
}  */
