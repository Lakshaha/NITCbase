#include "Schema.h"

#include <cmath>
#include <cstring>

int Schema::openRel(char relName[ATTR_SIZE])
{
	int ret = OpenRelTable::openRel(relName);

	//if rel id is a positive number, therefore rel-id returned else Error
	if (ret >= 0)
	{
		return SUCCESS;
	}
	//any error msg whatsoever
	return ret;
}

int Schema::closeRel(char relName[ATTR_SIZE])
{
	//relCatRelName = "RELATIONCAT" 
	//attrCatRelName = "ATTRIBUTECAT" 
	if (strcmp(relName, RELCAT_RELNAME) == 0 || strcmp(relName,ATTRCAT_RELNAME)==0)
	{
		return E_NOTPERMITTED;
	}

	int relId = OpenRelTable::getRelId(relName);

	if (relId = E_RELNOTOPEN)
	{
		return E_RELNOTOPEN;
	}

	return OpenRelTable::closeRel(relId);
}
