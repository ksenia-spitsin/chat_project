#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "group_manager.h"
#include "chat_protocol.h"
#include "hashmap.h"
#include "Queue.h"
#include "common_utils.h"

#define STOP 1
#define CONTINUE 0

typedef struct Group
{
    char m_groupName[MAX_LEN_GROUP_NAME];
    char* m_ip;
    int m_port;
    int m_numOfUsers;
} Group;

struct GroupManager
{
    HashMap* m_groups;
    Queue* m_ips;
	int m_port;
};

static size_t HashingFunc(const void* _key);
static int IsKeyEquals(const void *_firstKey, const void *_secondKey);
static Group* GroupCreate(const char* _groupName, int _port, char* _ip);
static void InsertIPs(Queue* _ips);
static char* GroupDestroy(Group* _group);
static void GroupDestroyInHash(void* _group);
static void IPDestroy(void* _ip);
static int SaveGroupToFile(const void* _groupName, void* _group, void* _file);
/*static void InsertGroups(GroupManager* _manager, int _port);
static int InsertBackIP(const void* _key, void* _group, void* _queueIP);*/

GroupManager* GroupManagerCreate(int _port)
{
    GroupManager* manager = (GroupManager*)malloc(sizeof(GroupManager));
    if(manager)
    {
        manager->m_groups = HashMapCreate(MAX_GROUPS, HashingFunc, IsKeyEquals);
        if(!manager->m_groups)
        {
            free(manager);
            return NULL;
        }
        manager->m_ips = QueueCreate(MAX_GROUPS);
        if(!manager->m_ips)
        {
            free(manager->m_groups);
            free(manager);
            return NULL;
        }
        InsertIPs(manager->m_ips);
        /* InsertGroups(manager, _port); */
		manager->m_port = _port;
    }
    return manager;
}

GroupManagerResult GroupManagerCreateGroup(GroupManager* _manager, const char* _groupName, char* _groupIP, int* _groupPort)
{
    Group* group;
    char* ip;
    MapResult result;
    void* findRes;

    if(!_manager || !_groupName)
    {
        return GROUP_GENERAL_ERROR;
    }

    if(MAP_SUCCESS == HashMapFind(_manager->m_groups, _groupName, &findRes)) {
		return GROUP_EXIST;
	}

    if(QueueDequeue(_manager->m_ips, (void**)&ip) == QUEUE_UNDERFLOW)
    {
        return GROUP_MAX_ERROR;
    }
    group = GroupCreate(_groupName, _manager->m_port, ip);
    result = HashMapInsert(_manager->m_groups, _groupName, group);
    if(result != MAP_SUCCESS)
    {
        QueueEnqueue(_manager->m_ips, GroupDestroy(group));
        return (result == MAP_KEY_DUPLICATE_ERROR) ? GROUP_EXIST : GROUP_GENERAL_ERROR;
    }
    group->m_numOfUsers++;
    strcpy(_groupIP, group->m_ip);
    *_groupPort = group->m_port;
    return GROUP_OK;
}

GroupManagerResult GroupManagerAddToGroup(GroupManager* _manager, const char* _groupName, char* _groupIP, int* _groupPort)
{
    Group* group;
    if(!_manager || !_groupName)
    {
        return GROUP_GENERAL_ERROR;
    }
    if(HashMapFind(_manager->m_groups, _groupName, (void**)&group) == MAP_KEY_NOT_FOUND_ERROR)
    {
        return GROUP_NOT_EXIST;
    }
    group->m_numOfUsers++;
    strcpy(_groupIP, group->m_ip);
    *_groupPort = group->m_port;
    return GROUP_OK;
}

GroupManagerResult GroupManagerRemoveFromGroup(GroupManager* _manager, const char* _groupName)
{
    Group* group;
    if(!_manager || !_groupName)
    {
        return GROUP_GENERAL_ERROR;
    }
    if(HashMapFind(_manager->m_groups, _groupName, (void**)&group) == MAP_KEY_NOT_FOUND_ERROR)
    {
        return GROUP_NOT_EXIST;
    }
    group->m_numOfUsers--;

    return GROUP_OK;
}

void GroupManagerDestroy(GroupManager* _manager)
{
    if(_manager)
    {
        QueueDestroy(&_manager->m_ips, IPDestroy);
        HashMapDestroy(&_manager->m_groups, NULL, GroupDestroyInHash);
        free(_manager);
    }
}

GroupManagerResult GroupManagerGetGroups(const GroupManager* _manager, const char* _fileName)
{
    FILE* fileToWriteTo;
    if(!_manager || !_fileName)
    {
        return GROUP_GENERAL_ERROR;
    }
    
    fileToWriteTo = fopen(_fileName, "w");
    if(!fileToWriteTo)
    {
        return GROUP_GENERAL_ERROR;
    }

    HashMapForEach(_manager->m_groups, SaveGroupToFile, fileToWriteTo);
    fclose(fileToWriteTo);

    return GROUP_OK;
}

static size_t HashingFunc(const void* _key)
{
    char* groupName = (char*)_key;
    size_t i;
    size_t groupNameLength = strlen(groupName);
    size_t sum = 0;
    for(i = 0; i < groupNameLength; i++)
    {
        sum += groupName[i];
    }
    return sum;
}

static int IsKeyEquals(const void *_firstKey, const void *_secondKey)
{
    char* firstGroupName = (char*) _firstKey;
    char* secondGroupName = (char*) _secondKey;
    if(!strcmp(firstGroupName, secondGroupName))
    {
        return TRUE;
    }
    return FALSE;
}

static int SaveGroupToFile(const void* _groupName, void* _group, void* _file)
{
    const char* groupName = (const char*) _groupName;
    FILE* filePtr = (FILE*)_file;
    
    UNUSED(_group);

    printf("group name = %s\n", groupName);
    
    return fprintf(filePtr, "%s\n", groupName);
}

static void InsertIPs(Queue* _ips)
{
    char ip[] = "230.0.0.%d";
    char* ipPtr;
    int i = 0;
    int j = 0;

    while(i < MAX_GROUPS)
    {
        ipPtr = (char*)malloc(sizeof(char) * MAX_LEN_IP + 1);
        if(!ipPtr)
        {
            break;
        }
        sprintf(ipPtr, ip, j++);
        if(QueueEnqueue(_ips, ipPtr) == QUEUE_OVERFLOW)
        {
			free(ipPtr);
            break;
        }
        if(j == 256)
        {
            strcpy(ip, "230.0.1.%d");
        }
    }
}

/* static void InsertGroups(GroupManager* _manager, int _port)
{
    Group* group;
    char* ip;
    char* names[] = {"h61", "FUNFUNFUN", "FOOOOD", "ChatChatChat"};
    size_t size = sizeof(names)/sizeof(names[0]);
	size_t i;

    for(i = 0; i < size; i++)
    {
        QueueDequeue(_manager->m_ips, (void**)&ip);
        group = GroupCreate(names[i], _port, ip);
        if(HashMapInsert(_manager->m_groups, group->m_groupName, group) != MAP_SUCCESS)
        {
            QueueEnqueue(_manager->m_ips, GroupDestroy(group));
            break;
        }
    }
} */

static Group* GroupCreate(const char* _groupName, int _port, char* _ip)
{
    Group* group = (Group*)malloc(sizeof(Group));
    if(group)
    {
        strcpy(group->m_groupName, _groupName);
        group->m_ip = _ip;
        group->m_port = _port;
        group->m_numOfUsers = 0;
    }
    return group;
}

static char* GroupDestroy(Group* _group)
{
    char* groupIP = 0;
    if(_group)
    {
        groupIP = _group->m_ip;
        free(_group);
    }
    return groupIP;
}

static void GroupDestroyInHash(void* _group)
{
    Group* group = (Group*)_group;
    free(group->m_ip);
    free(group);
}

static void IPDestroy(void* _ip)
{
    free(_ip);
}
