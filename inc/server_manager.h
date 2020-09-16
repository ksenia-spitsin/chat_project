#ifndef __SERVER_MANAGER_H__
#define __SERVER_MANAGER_H__

typedef struct ServerManager ServerManager;

ServerManager* ServerManagerCreate ();
void ServerManagerRun (ServerManager* _manager);
void ServerManagerDestroy (ServerManager* _manager);

#endif /*__SERVER_MANAGER_H__ */
