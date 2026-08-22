#include "Schema.h"

#include <cmath>
#include <cstring>

int Schema::openRel(char relName[ATTR_SIZE])
{
	int ret = OpenRelTable::openRel(relName);
	//openRel returns relid if successfull else error, valid rel id will be bw 0 to 12

	if (ret >= 0)
	{
		return SUCCESS;
	}
	return ret;	
}

int Schema::closeRel(char relName[ATTR_SIZE])
{
	if (strcmp(relName, RELCAT_RELNAME) == 0 || strcmp(relName, ATTRCAT_RELNAME) == 0)
	{
		return E_NOTPERMITTED;
	}
	//returns relId if relation is open
	int relId = OpenRelTable::getRelId(relName);

	if (relId == E_RELNOTOPEN)
	{
		return E_RELNOTOPEN;
	}
	return OpenRelTable::closeRel(relId);
}
