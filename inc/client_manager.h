#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

typedef struct CManager CManager;

CManager* CManagerCreate ();
void CManagerDestroy (CManager* _manager);
void CManagerRun (CManager* _manager, const char* _configFile);

#endif /* CLIENT_MANAGER_H */
