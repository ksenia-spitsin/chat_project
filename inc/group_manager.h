#ifndef __GROUP_MANAGER_H__
#define __GROUP_MANAGER_H__

typedef struct GroupManager GroupManager;

typedef enum GroupManagerResult
{
    GROUP_OK,
    GROUP_GENERAL_ERROR,
    GROUP_NOT_EXIST,
    GROUP_EXIST,
    GROUP_MAX_ERROR
}GroupManagerResult;

GroupManager* GroupManagerCreate(int _port);

GroupManagerResult GroupManagerCreateGroup(GroupManager* _manager, const char* _groupName, char* _groupIP, int* _groupPort);

GroupManagerResult GroupManagerAddToGroup(GroupManager* _manager, const char* _groupName, char* _groupIP, int* _groupPort);

GroupManagerResult GroupManagerRemoveFromGroup(GroupManager* _manager, const char* _groupName);

GroupManagerResult GroupManagerGetGroups(const GroupManager* _manager, const char* _fileName);

void GroupManagerDestroy(GroupManager* _manager);

#endif /*__GROUP_MANAGER_H__*/
