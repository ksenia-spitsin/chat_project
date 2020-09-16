#ifndef CLIENT_GROUPMANAGER_H
#define CLIENT_GROUPMANAGER_H

typedef enum CGroupResult
{
    GROUP_OK,
    ALREADY_IN_GROUP,
    NOT_IN_GROUP,
    GROUP_GENERAL_ERROR
} CGroupResult;

typedef struct CGroupMngr CGroupMngr;

CGroupMngr* CGroupMngrCreate ();

void CGroupMngrDestroy (CGroupMngr* _groupMngr);

CGroupResult CGroupMngrAddGroup (CGroupMngr* _groupMngr, const char* _groupName, int _groupFD);

CGroupResult CGroupMngrRemoveGroup (CGroupMngr* _groupMngr, const char* _groupName, int* _groupFD);

int HasCGroupMngrGroup (CGroupMngr* _groupMngr, const char* _groupName);

CGroupResult WriteGroupsToFile(const CGroupMngr* _groupManager, const char* _fileName);

#endif /* CLIENT_GROUPMANAGER_H */
