#include <stdio.h>
#include "server_manager.h"

#define CONFIG_FILE "addr_config"
#define MAX_IP_LENGTH 16

int main()
{
	FILE* _configFile;
	ServerManager* _manager;
	char _ip[MAX_IP_LENGTH];
	int _portNum;
	
	_configFile = fopen(CONFIG_FILE, "r");
	if(_configFile == NULL) {
		perror("An error opening configuration file\n");
		return 1;
	}
	fscanf(_configFile, "%s\n %d", _ip, &_portNum);
	_manager = ServerManagerCreate(_portNum);
	fclose(_configFile);
	ServerManagerRun(_manager);
	ServerManagerDestroy(_manager);

	return 0;
}
