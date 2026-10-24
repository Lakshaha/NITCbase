#include "Algebra.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>


bool isNumber(char *str);

/* used to select all the records that satisfy a condition.
the arguments of the function are
- srcRel - the source relation we want to select from
- targetRel - the relation we want to select into. (ignore for now)
- attr - the attribute that the condition is checking
- op - the operator of the condition
- strVal - the value that we want to compare against (represented as a string)
*/
// int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) 
// {
// 	int srcRelId = OpenRelTable::getRelId(srcRel);
// 	if (srcRelId == E_RELNOTOPEN)
// 	{
// 		return E_RELNOTOPEN;
// 	}

// 	//get attribute entry which we need to check condiiton on
// 	AttrCatEntry attrCatEntry;
// 	if(AttrCacheTable ::getAttrCatEntry(srcRelId, attr, &attrCatEntry) == E_ATTRNOTEXIST)
// 	{
// 		return E_ATTRNOTEXIST;
// 	}

// 	//check type, convert sVal to Number or String
// 	int type = attrCatEntry.attrType;
	
// 	//Attribute type can hold both Number String wout any issues
// 	Attribute attrVal;
// 	if (type == NUMBER)
// 	{
// 		if (isNumber(strVal)) //boolean user defined function
// 		{
// 			attrVal.nVal = atof(strVal);
// 		}
// 		else
// 		{
// 			return E_ATTRTYPEMISMATCH;
// 		}
// 	}
// 	else if(type == STRING)
// 	{
// 		strcpy(attrVal.sVal, strVal);
// 	}


// 	//creating and opening the target relation
// 	//prepare args for create rel
// 	//1) get relCatentry of srcrel
// 	RelCatEntry relCatEntry;
// 	RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

// 	int src_nAttrs = relCatEntry.numAttrs;

// 	char attr_names[src_nAttrs][ATTR_SIZE]; //will store attrname of the rel

// 	int attr_types[src_nAttrs];

// 	//iterate throught the nattrs now
// 	for (int i=0; i<src_nAttrs; i++)
// 	{
// 		AttrCatEntry srcAttrCatEntry;
// 		AttrCacheTable::getAttrCatEntry(srcRelId, i, &srcAttrCatEntry);
		
// 		//fill attrName and attrType
// 		strcpy(attr_names[i], srcAttrCatEntry.attrName);
// 		attr_types[i] = attrCatEntry.attrType;
// 	}

// 	//create target relation by createRel()

// 	int ret = Schema::createRel(targetRel, src_nAttrs, attr_names, attr_types);
// 	if (ret != SUCCESS)
// 	{
// 		Schema::deleteRel(targetRel);
// 		return ret;
// 	}
	
// 	//open the newly created target rel by calling openRel
// 	int targetRelId = Schema::openRel(targetRel);
// 	if (targetRelId < 0)
// 	{
// 		Schema::deleteRel(targetRel);
// 		return targetRelId;
// 	}

// 	//selecting and inserting record into target relation
// 	//1) reset search index
// 	RelCacheTable::resetSearchIndex(srcRelId);
// 	Attribute record[src_nAttrs];
// 	//can use linear search and also B+ search

// 	while (BlockAccess::search(srcRelId, record, attr, attrVal, op) == SUCCESS)
// 	{
// 		int ret = BlockAccess::insert(targetRelId, record);
// 		if (ret != SUCCESS)
// 		{
// 			Schema::closeRel(targetRel);
// 			Schema::deleteRel(targetRel);
// 			return ret;
// 		}
// 	}
// 	Schema::closeRel(targetRel);
// 	return SUCCESS;
// }


int Algebra::select(char srcRel[ATTR_SIZE],
                    char targetRel[ATTR_SIZE],
                    char attr[ATTR_SIZE],
                    int op,
                    char strVal[ATTR_SIZE])
{
    int srcRelId = OpenRelTable::getRelId(srcRel);

    if (srcRelId == E_RELNOTOPEN)
    {
        return E_RELNOTOPEN;
    }

    AttrCatEntry attrCatEntry;

    int ret = AttrCacheTable::getAttrCatEntry(
        srcRelId, attr, &attrCatEntry
    );

    if (ret == E_ATTRNOTEXIST)
    {
        return E_ATTRNOTEXIST;
    }

    Attribute attrValue;
    int type = attrCatEntry.attrType;

    if (type == NUMBER)
    {
        if (isNumber(strVal))
        {
            attrValue.nVal = atof(strVal);
        }
        else
        {
            return E_ATTRTYPEMISMATCH;
        }
    }
    else if (type == STRING)
    {
        strcpy(attrValue.sVal, strVal);
    }

    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

    int src_nAttrs = relCatEntry.numAttrs;

    char attr_names[src_nAttrs][ATTR_SIZE];
    int attr_type[src_nAttrs];

    for (int i = 0; i < src_nAttrs; i++)
    {
        AttrCatEntry srcAttrCatEntry;

        AttrCacheTable::getAttrCatEntry(
            srcRelId, i, &srcAttrCatEntry
        );

        strcpy(attr_names[i], srcAttrCatEntry.attrName);
        attr_type[i] = srcAttrCatEntry.attrType;
    }

    int retVal = Schema::createRel(
        targetRel, src_nAttrs, attr_names, attr_type
    );

    if (retVal != SUCCESS)
    {
        return retVal;
    }

    int targetRelId = OpenRelTable::openRel(targetRel);

    if (targetRelId < 0)
    {
        Schema::deleteRel(targetRel);
        return targetRelId;
    }

    Attribute record[src_nAttrs];

    RelCacheTable::resetSearchIndex(srcRelId);
    //AttrCacheTable::resetSearchIndex(srcRelId, attr);

    while (BlockAccess::search(
               srcRelId, record, attr, attrValue, op
           ) == SUCCESS)
    {
        int insertRet = BlockAccess::insert(
            targetRelId, record
        );

        if (insertRet != SUCCESS)
        {
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return insertRet;
        }
    }

    Schema::closeRel(targetRel);

    return SUCCESS;
}




//will return ig string can be parsed as floating point num
bool isNumber(char *str)
{
	int len;
	float ignore;

	//sscanf return number of elem reached, so if %f = 0 no floating math lese 1

	int ret = sscanf(str, "%f %n", &ignore, &len);
	return ret == 1 && len == strlen(str);
} 


// ________stage-7________________-
int Algebra::insert(char relName[ATTR_SIZE], int nAttrs, char record[][ATTR_SIZE])
{
	if (strcmp(relName, "RELATIONCAT") == 0 || strcmp(relName, "ATTRIBUTECAT")==0)
	{
		return E_NOTPERMITTED;
	}

	//get rel id
	int relId = OpenRelTable::getRelId(relName);

	//if not open
	if (relId == E_RELNOTOPEN)
	{
		return E_RELNOTOPEN;
	}

	//get the relcat entry using the fn
	RelCatEntry relCatEntry;
	RelCacheTable::getRelCatEntry(relId, &relCatEntry);

	//if they have different number of attributes
	if (relCatEntry.numAttrs != nAttrs)
	{
		return E_NATTRMISMATCH;
	}

	Attribute recordValues[nAttrs];
	//converting 2d array to attribyte array record values

	for (int i=0; i<nAttrs; i++)
	{
		AttrCatEntry attrCatEntry;
		AttrCacheTable::getAttrCatEntry(relId, i, &attrCatEntry);
		int type = attrCatEntry.attrType;
		
		if (type == NUMBER)
		{
			if (isNumber(record[i]))
			{
				recordValues[i].nVal = atof(record[i]);
			}
			else
			{
				return E_ATTRTYPEMISMATCH;
			}
		}
		else if (type == STRING)
		{
			strcpy(recordValues[i].sVal, record[i]);
		}
	}
	int retVal =BlockAccess::insert(relId, recordValues);
	return retVal;

}


//_________stage-9, updated select, rest 2 are here
// int Algebra::project(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE])
// {
// 	int srcRelId = OpenRelTable::getRelId(srcRel);
// 	if (srcRelId == E_RELNOTOPEN)
// 	{
// 		return E_RELNOTOPEN;
// 	}

// 	//get rel cat entry
// 	RelCatEntry relCatEntry;
// 	RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

// 	//get number of attrs
// 	int numAttrs = relCatEntry.numAttrs;

// 	//attr name and attr type
// 	char attrNames[numAttrs][ATTR_SIZE];
// 	int attrTypes[numAttrs];

// 	for (int i=0; i<numAttrs; i++)
// 	{
// 		//get each attribute ka entry
// 		AttrCatEntry attrCatEntry;
// 		AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);
// 		strcpy(attrNames[i], attrCatEntry.attrName);
// 		attrTypes[i] = attrCatEntry.attrType;
// 	}

// 	//creating and opening target relation
// 	int ret = Schema::createRel(targetRel, numAttrs, attrNames, attrTypes);

// 	if (ret != SUCCESS)
// 	{
// 		return ret;
// 	}

// 	//open the newly creating targed rel id
// 	int targetRelId = OpenRelTable::openRel(targetRel);
// 	//if doesnt open successfully delete the rel and return the val
// 	if (targetRelId < 0)
// 	{
// 		Schema::deleteRel(targetRel);
// 		return targetRelId;
// 	}

// 	//inserting proijected records into target relations
// 	//reset search index
// 	RelCacheTable::resetSearchIndex(srcRelId);

// 	Attribute record[numAttrs];

// 	while (BlockAccess::project(srcRelId, record) == SUCCESS)
// 	{
// 		ret = BlockAccess::insert(targetRelId, record);
// 		if (ret != SUCCESS)
// 		{
// 			Schema::closeRel(targetRel);
// 			Schema::deleteRel(targetRel);
// 			return ret;
// 		}
// 	}
// 	Schema::closeRel(targetRel);
// 	return SUCCESS;
// }

// int Algebra::project(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], int tar_nAttrs, char tar_Attrs[][ATTR_SIZE]) 
// {
// 	int srcRelId = OpenRelTable::getRelId(srcRel);

// 	if (srcRelId == E_RELNOTOPEN)
// 	{
// 		return E_RELNOTOPEN;
// 	}

// 	//get relcat entry
// 	RelCatEntry relCatEntry;
// 	RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

// 	//get number of attrs in relation
// 	int nAttrs = relCatEntry.numAttrs;

// 	//ith entry will story the offest in a record of srcRel for ith attribute
// 	int attr_offset[tar_nAttrs];
// 	//ith entry will store the type of ith attribute in target relation
// 	int attr_types[tar_nAttrs];
	
// 	//checking if attribute of target are present in source relation and storing its offset and types
// 	for (int i=0; i<tar_nAttrs; i++)
// 	{
// 		//get attr cat of entry with the given attrs name
// 		AttrCatEntry attrCatEntry;
// 		int ret = AttrCacheTable::getAttrCatEntry(srcRelId, tar_Attrs[i], &attrCatEntry);
// 		if (ret != SUCCESS)
// 		{
// 			return ret;
// 		}
// 		attr_offset[i] = attrCatEntry.offset;
// 		attr_types[i] = attrCatEntry.attrType;	
// 	}

// 	//creating and opening the target relation
// 	int ret = Schema::createRel(targetRel, tar_nAttrs, tar_Attrs, attr_types);
// 	if (ret != SUCCESS)
// 	{
// 		return ret;
// 	}
// 	int targetRelId = Schema::openRel(targetRel);
// 	if (targetRelId < 0)
// 	{
// 		Schema::deleteRel(targetRel);
// 		return targetRelId;
// 	}

// 	//inserting projected records into target relation
// 	//reset the search index 
// 	RelCacheTable::resetSearchIndex(srcRelId);

// 	Attribute record[nAttrs];

// 	while (BlockAccess::project(srcRelId, record))
// 	{
// 		Attribute proj_record[tar_nAttrs];

// 		for (int i=0; i<tar_nAttrs; i++)
// 		{
// 			proj_record[i] = record[attr_offset[i]];
// 		}
// 		int ret = BlockAccess::insert(targetRelId, proj_record);
// 		if (ret != SUCCESS)
// 		{
// 			Schema::closeRel(targetRel);
// 			Schema::deleteRel(targetRel);
// 			return ret;
// 		}
// 	}

// 	Schema::closeRel(targetRel);
// 	return SUCCESS;
// }


// PROJECT all attributes and records.
int Algebra::project(char srcRel[ATTR_SIZE],
                     char targetRel[ATTR_SIZE])
{
    int srcRelId = OpenRelTable::getRelId(srcRel);

    if (srcRelId < 0)
    {
        return srcRelId;
    }

    RelCatEntry srcRelCatEntry;

    RelCacheTable::getRelCatEntry(
        srcRelId, &srcRelCatEntry
    );

    int numAttrs = srcRelCatEntry.numAttrs;

    char attrNames[numAttrs][ATTR_SIZE];
    int attrTypes[numAttrs];

    for (int i = 0; i < numAttrs; i++)
    {
        AttrCatEntry attrCatEntry;

        AttrCacheTable::getAttrCatEntry(
            srcRelId, i, &attrCatEntry
        );

        strcpy(attrNames[i], attrCatEntry.attrName);
        attrTypes[i] = attrCatEntry.attrType;
    }

    int ret = Schema::createRel(
        targetRel, numAttrs, attrNames, attrTypes
    );

    if (ret != SUCCESS)
    {
        return ret;
    }

    int targetRelId = OpenRelTable::openRel(targetRel);

    if (targetRelId < 0)
    {
        Schema::deleteRel(targetRel);
        return targetRelId;
    }

    RelCacheTable::resetSearchIndex(srcRelId);

    Attribute record[numAttrs];

    while (BlockAccess::project(srcRelId, record) == SUCCESS)
    {
        ret = BlockAccess::insert(targetRelId, record);

        if (ret != SUCCESS)
        {
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return ret;
        }
    }

    Schema::closeRel(targetRel);

    return SUCCESS;
}


// PROJECT selected attributes and all records.
int Algebra::project(char srcRel[ATTR_SIZE],
                     char targetRel[ATTR_SIZE],
                     int tar_nAttrs,
                     char tar_Attrs[][ATTR_SIZE])
{
    int srcRelId = OpenRelTable::getRelId(srcRel);

    if (srcRelId < 0)
    {
        return srcRelId;
    }

    RelCatEntry srcRelCatEntry;

    RelCacheTable::getRelCatEntry(
        srcRelId, &srcRelCatEntry
    );

    int numAttrs = srcRelCatEntry.numAttrs;

    int attr_offset[tar_nAttrs];
    int attr_types[tar_nAttrs];

    for (int i = 0; i < tar_nAttrs; i++)
    {
        AttrCatEntry attrCatEntry;

        int ret = AttrCacheTable::getAttrCatEntry(
            srcRelId, tar_Attrs[i], &attrCatEntry
        );

        if (ret != SUCCESS)
        {
            return ret;
        }

        attr_offset[i] = attrCatEntry.offset;
        attr_types[i] = attrCatEntry.attrType;
    }

    int ret = Schema::createRel(
        targetRel, tar_nAttrs, tar_Attrs, attr_types
    );

    if (ret != SUCCESS)
    {
        return ret;
    }

    int targetRelId = OpenRelTable::openRel(targetRel);

    if (targetRelId < 0)
    {
        Schema::deleteRel(targetRel);
        return targetRelId;
    }

    RelCacheTable::resetSearchIndex(srcRelId);

    Attribute record[numAttrs];

    while (BlockAccess::project(srcRelId, record) == SUCCESS)
    {
        Attribute proj_record[tar_nAttrs];

        for (int i = 0; i < tar_nAttrs; i++)
        {
            proj_record[i] = record[attr_offset[i]];
        }

        ret = BlockAccess::insert(targetRelId, proj_record);

        if (ret != SUCCESS)
        {
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return ret;
        }
    }

    Schema::closeRel(targetRel);

    return SUCCESS;
}
