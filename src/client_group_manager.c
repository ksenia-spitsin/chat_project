#include <stdlib.h>
#include <stdio.h> /* FILE */
#include <string.h>
#include "client_group_manager.h"
#include "hashmap.h"
#include "common_utils.h"

struct CGroupMngr
{
    HashMap* m_cGroups;
};

size_t HashGroupName (const void* _groupName);
int EqualityGroupName (const void* _firstGN, const void* _secondGN);
void GroupNameDestroyer (void* _groupName);
void GroupFDDestroyer (void* _groupFD);
static int WriteGroupToFile(const void *_key, void *_value, void *_context);

/* -------------- */

CGroupMngr* CGroupMngrCreate (size_t _maxGroups)
{
	CGroupMngr* groupMngr = (CGroupMngr*)malloc(sizeof(CGroupMngr));
	if (!groupMngr)
	{
		return NULL;
	}
	
	groupMngr->m_cGroups = HashMapCreate(_maxGroups, HashGroupName, EqualityGroupName);
	if (!groupMngr->m_cGroups)
	{
		free(groupMngr);
		return NULL;
	}
	
	return groupMngr;
}

void CGroupMngrDestroy (CGroupMngr* _groupMngr)
{
	if (_groupMngr)
	{
		HashMapDestroy(&(_groupMngr->m_cGroups), GroupNameDestroyer, GroupFDDestroyer);
		free(_groupMngr);
	}
}

CGroupResult CGroupMngrAddGroup (CGroupMngr* _groupMngr, const char* _groupName, int _groupFD)
{
	char* groupNamePtr;
	int* groupFDptr;
	MapResult addResult;

	groupNamePtr = (char*)malloc(sizeof(char) * strlen(_groupName) + 1);
	if (!groupNamePtr)
	{
		return GROUP_GENERAL_ERROR;
	}
	strcpy(groupNamePtr, _groupName);

	groupFDptr = (int*)malloc(sizeof(int));
	if (!groupFDptr)
	{
		return GROUP_GENERAL_ERROR;
	}
	*groupFDptr = _groupFD;

	addResult = HashMapInsert(_groupMngr->m_cGroups, groupNamePtr, groupFDptr);
	if (addResult != MAP_SUCCESS)
	{
		free(groupNamePtr);
		free(groupFDptr);
		return (addResult == MAP_KEY_DUPLICATE_ERROR) ? ALREADY_IN_GROUP : GROUP_GENERAL_ERROR;
	}
	return GROUP_OK;
}

CGroupResult CGroupMngrRemoveGroup (CGroupMngr* _groupMngr, const char* _groupName, int* _groupFD)
{
	char* removedGroupName = NULL;
	int* removedGroupFD;
	MapResult removeResult;
	
	removeResult = HashMapRemove(_groupMngr->m_cGroups, _groupName, (void**)&removedGroupName, (void**)&removedGroupFD);
	if (removeResult != MAP_SUCCESS)
	{
		return (removeResult == MAP_KEY_NOT_FOUND_ERROR) ? NOT_IN_GROUP : GROUP_GENERAL_ERROR;
	}

	free(removedGroupName);
	*_groupFD = *removedGroupFD;
	free(removedGroupFD);

	return GROUP_OK;
}

int HasCGroupMngrGroup (CGroupMngr* _groupMngr, const char* _groupName)
{
	int* foundGroupFD;

	return (HashMapFind(_groupMngr->m_cGroups, _groupName, (void**)&foundGroupFD) == MAP_SUCCESS);
}

/* -------------- */

size_t HashGroupName (const void* _groupName)
{
	int numOfChars = strlen((char*)_groupName);
	int sumOfChars = 0;
	int i = 0;

	for (; i < numOfChars; i++)
	{
		sumOfChars += (int)(*((char*)_groupName + i));
	}

	return sumOfChars * numOfChars;
}

int EqualityGroupName (const void* _firstGN, const void* _secondGN)
{
	return (!strcmp((char*)_firstGN, (char*)_secondGN));
}

void GroupNameDestroyer (void* _groupName)
{
	free(_groupName);
}

void GroupFDDestroyer (void* _groupFD)
{
	free(_groupFD);
}

CGroupResult WriteGroupsToFile(const CGroupMngr* _groupManager, const char* _fileName)
{
	FILE* fp;

	if(NULL == _fileName) 
	{
		return GROUP_GENERAL_ERROR;
	}

	fp = fopen(_fileName, "w");
	if(NULL == fp)
	{
		return GROUP_GENERAL_ERROR;
	}
	
	HashMapForEach(_groupManager->m_cGroups, WriteGroupToFile, fp);
	fclose(fp);

	return GROUP_OK;
}

static int WriteGroupToFile(const void *_key, void *_value, void *_context)
{
	const char* groupName = (const char*)_key;
	FILE* fp = (FILE*)_context;

	UNUSED(_value);

	return fprintf(fp, "%s\n", groupName);
}
