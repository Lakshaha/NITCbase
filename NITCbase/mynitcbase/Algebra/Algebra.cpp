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


// extern int linearComparison;
// extern int bPlusComparison;


int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]){
    int srcRelId = OpenRelTable::getRelId(srcRel);
    if(srcRelId == E_RELNOTOPEN){
        return E_RELNOTOPEN;
    }

    AttrCatEntry attrCatEntry;
    // get the attribute catalog entry for attr, using AttrCacheTable::getAttrcatEntry()
    //    return E_ATTRNOTEXIST if it returns the error
    int ret = AttrCacheTable::getAttrCatEntry(srcRelId, attr, &attrCatEntry);
	if (ret == E_ATTRNOTEXIST) {
		return E_ATTRNOTEXIST;
	}
    
    /*** Convert input strVal (string) to an attribute of data type NUMBER or STRING ***/
    int type = attrCatEntry.attrType;
    Attribute attrValue;
    if(type == NUMBER){
        if(isNumber(strVal)){
            attrValue.nVal = atof(strVal);
        }
        else{
            return E_ATTRTYPEMISMATCH;
        }
    }
    else if( type == STRING){
        strcpy(attrValue.sVal, strVal);
    }

    /*** Creating and opening the target relation ***/
    // prepare arguments for createRel() in the following way:
    // get RelcatEntry of srcRel using
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);
    int src_nAttrs = relCatEntry.numAttrs;

    // store all the attributes names of secRel in 2D array, and attribute type in 1d array.
    char attr_names[src_nAttrs][ATTR_SIZE];
    int attr_type[src_nAttrs];

    for(int i = 0; i < src_nAttrs; i++){
        AttrCatEntry srcAttrCatEntry;
        AttrCacheTable::getAttrCatEntry(srcRelId, i, &srcAttrCatEntry);
        strcpy(attr_names[i],srcAttrCatEntry.attrName);
        attr_type[i] = srcAttrCatEntry.attrType;
    }

    // Create the relation for target relation by calling Schema::createRel()
    int retVal = Schema::createRel(targetRel, src_nAttrs, attr_names, attr_type);

    // little bit of error handling
    if(retVal != SUCCESS){
        Schema::deleteRel(targetRel);
        return retVal;
    }

    int targetRelId = OpenRelTable::openRel(targetRel);
    if(targetRelId < 0) {
        // Schema::deleteRel(targetRel);
        return targetRelId;
    }

    /*** Selecting and inserting records into the target relation ***/
    // Reset searchIndex for attrCacheTable [not required for this stage]

    Attribute record[src_nAttrs];
    RelCacheTable::resetSearchIndex(srcRelId);
    AttrCacheTable::resetSearchIndex(srcRelId,attr);

    // linearComparison = 0;
    // bPlusComparison = 0;

    while(BlockAccess::search(srcRelId, record, attr, attrValue, op) == SUCCESS){
        int ret = BlockAccess::insert(targetRelId, record);
        if(ret != SUCCESS){
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return ret;
        }
    }
    // printf("Linear Comparison: %d\n",linearComparison);
    // printf("B Plus Comparison: %d\n",bPlusComparison);

    Schema::closeRel(targetRel);
    // Schema::deleteRel(targetRel);
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

// int Algebra::join(char srcRelation1[ATTR_SIZE], char srcRelation2[ATTR_SIZE], char targetRelation[ATTR_SIZE], char attribute1[ATTR_SIZE], char attribute2[ATTR_SIZE]) 
// {

//     // get the srcRelation1's rel-id using OpenRelTable::getRelId() method
//     int relId1 = OpenRelTable::getRelId(srcRelation1);

//     // get the srcRelation2's rel-id using OpenRelTable::getRelId() method
//     int relId2 = OpenRelTable::getRelId(srcRelation2);

//     // if either of the two source relations is not open
//     //     return E_RELNOTOPEN
//     if (relId1 < 0 || relId1 >= MAX_OPEN || relId2 < 0 || relId2 >= MAX_OPEN)
//     {
//         return E_RELNOTOPEN;
//     }

//     AttrCatEntry attrCatEntry1, attrCatEntry2;
//     // get the attribute catalog entries for the following from the attribute cache
//     // (using AttrCacheTable::getAttrCatEntry())
//     // - attrCatEntry1 = attribute1 of srcRelation1
//     // - attrCatEntry2 = attribute2 of srcRelation2
//      // if attribute1 is not present in srcRelation1 or attribute2 is not
//     // present in srcRelation2 (getAttrCatEntry() returned E_ATTRNOTEXIST)
//     //     return E_ATTRNOTEXIST.
//     if (AttrCacheTable::getAttrCatEntry(relId1, attribute1, &attrCatEntry1) == E_ATTRNOTEXIST)
//     {
//         return E_ATTRNOTEXIST;
//     }
//     if (AttrCacheTable::getAttrCatEntry(relId2, attribute2, &attrCatEntry2) == E_ATTRNOTEXIST)
//     {
//         return E_ATTRNOTEXIST;
//     }
    

//     // if attribute1 and attribute2 are of different types return E_ATTRTYPEMISMATCH

//     if (attrCatEntry1.attrType != attrCatEntry2.attrType)
//     {
//         return E_ATTRTYPEMISMATCH;
//     }

//     // iterate through all the attributes in both the source relations and check if
//     // there are any other pair of attributes other than join attributes
//     // (i.e. attribute1 and attribute2) with duplicate names in srcRelation1 and
//     // srcRelation2 (use AttrCacheTable::getAttrCatEntry())
//     // If yes, return E_DUPLICATEATTR

//     // get the relation catalog entries for the relations from the relation cache
//     // (use RelCacheTable::getRelCatEntry() function)
//     RelCatEntry relCatEntry1;
//     RelCatEntry relCatEntry2;

//     RelCacheTable::getRelCatEntry(relId1, &relCatEntry1);
//     RelCacheTable::getRelCatEntry(relId2, &relCatEntry2);

//     int numOfAttributes1 = relCatEntry1.numAttrs;
//     int numOfAttributes2 = relCatEntry2.numAttrs;

//     AttrCatEntry temp1, temp2;

//     for (int j=0; j<numOfAttributes2; j++)
//     {
//         if (j == attrCatEntry2.offset)
//         {
//             continue;
//         }
//         AttrCacheTable::getAttrCatEntry(relId2, j, &temp2);
//         for (int i=0; i<numOfAttributes1; i++)
//         {
//             AttrCacheTable::getAttrCatEntry(relId2, i, &temp1);
//             if (strcmp(temp1.attrName, temp2.attrName) == 0)
//             {
//                 return E_DUPLICATEATTR;
//             }
//         }

//     }



//     // if rel2 does not have an index on attr2
//     //     create it using BPlusTree:bPlusCreate()
//     //     if call fails, return the appropriate error code
//     //     (if your implementation is correct, the only error code that will
//     //      be returned here is E_DISKFULL)

//     if (attrCatEntry2.rootBlock == -1)
//     {
//         int ret = BPlusTree::bPlusCreate(relId2, attribute2);
//         if (ret == E_DISKFULL)
//         {
//             return E_DISKFULL;
//         }
//     }

//     int numOfAttributesInTarget = numOfAttributes1 + numOfAttributes2 - 1;
//     // Note: The target relation has number of attributes one less than
//     // nAttrs1+nAttrs2 (Why?)

//     // declare the following arrays to store the details of the target relation
//     char targetRelAttrNames[numOfAttributesInTarget][ATTR_SIZE];
//     int targetRelAttrTypes[numOfAttributesInTarget];

//     // iterate through all the attributes in both the source relations and
//     // update targetRelAttrNames[],targetRelAttrTypes[] arrays excluding attribute2
//     // in srcRelation2 (use AttrCacheTable::getAttrCatEntry())

//     for (int i=0; i<numOfAttributes1; i++)
//     {
//         AttrCacheTable::getAttrCatEntry(relId1, i, &temp1);
//         strcpy(targetRelAttrNames[i], temp1.attrName);
//         targetRelAttrTypes[i] = temp1.attrType;
//     }
//     for (int j=0; j<attrCatEntry2.offset; j++)
//     {
//         AttrCacheTable::getAttrCatEntry(relId2, j, &temp2);
//         strcpy(targetRelAttrNames[numOfAttributes1+j], temp2.attrName);
//         targetRelAttrTypes[ numOfAttributes1 + j] = temp2.attrType;
//     }
//     for (int j=attrCatEntry2.offset+1; j<numOfAttributes2; j++)
//     {
//         AttrCacheTable::getAttrCatEntry(relId2, j, &temp2);
//         strcpy(targetRelAttrNames[numOfAttributes1+j - 1], temp2.attrName);
//         targetRelAttrTypes[ numOfAttributes1 + j - 1] = temp2.attrType;
//     }
    

//     // create the target relation using the Schema::createRel() function

//     int ret = Schema::createRel(targetRelation, numOfAttributesInTarget, targetRelAttrNames, targetRelAttrTypes);

//     // if createRel() returns an error, return that error

//     if (ret != SUCCESS)
//     {
//         return ret;
//     }

//     // Open the targetRelation using OpenRelTable::openRel()

//     int targetRelId = OpenRelTable::openRel(targetRelation);

//     // if openRel() fails (No free entries left in the Open Relation Table)
//     if (targetRelId < 0 || targetRelId > MAX_OPEN)
//     {
//         // delete target relation by calling Schema::deleteRel()
//         // return the error code
//         Schema::deleteRel(targetRelation);
//         return targetRelId;
//     }

//     Attribute record1[numOfAttributes1];
//     Attribute record2[numOfAttributes2];
//     Attribute targetRecord[numOfAttributesInTarget];

//     // this loop is to get every record of the srcRelation1 one by one
//     while (BlockAccess::project(relId1, record1) == SUCCESS) {

//         // reset the search index of `srcRelation2` in the relation cache
//         // using RelCacheTable::resetSearchIndex()
//         RelCacheTable::resetSearchIndex(relId2);

//         // reset the search index of `attribute2` in the attribute cache
//         // using AttrCacheTable::resetSearchIndex()
//         AttrCacheTable::resetSearchIndex(relId2, attribute2);

//         // this loop is to get every record of the srcRelation2 which satisfies
//         //the following condition:
//         // record1.attribute1 = record2.attribute2 (i.e. Equi-Join condition)
//         while (BlockAccess::search(
//             relId2, record2, attribute2, record1[attrCatEntry1.offset], EQ
//         ) == SUCCESS ) {

//             // copy srcRelation1's and srcRelation2's attribute values(except
//             // for attribute2 in rel2) from record1 and record2 to targetRecord

//             for (int i=0; i<numOfAttributes1; i++)
//             {
//                 targetRecord[i] = record1[i];
//             }
//             for (int j=0; j<attrCatEntry2.offset; j++)
//             {
//                 targetRecord[numOfAttributes1 + j] = record2[j];
//             }

//             for (int j=attrCatEntry2.offset+1; j<numOfAttributes2; j++)
//             {
//                 targetRecord[numOfAttributes1 + j - 1] = record2[j];
//             }

//             // insert the current record into the target relation by calling
//             // BlockAccess::insert()
//             ret = BlockAccess::insert(targetRelId, targetRecord);

//             if(ret != SUCCESS) {

//                 // close the target relation by calling OpenRelTable::closeRel()
//                 OpenRelTable::closeRel(targetRelId);
//                 // delete targetRelation (by calling Schema::deleteRel())
//                 Schema::deleteRel(targetRelation);
//                 return E_DISKFULL;
//             }
//         }
//     }

//     // close the target relation by calling OpenRelTable::closeRel()
//     return SUCCESS;
// }

/*
    join()
*/
int Algebra::join(char srcRelation1[ATTR_SIZE], char srcRelation2[ATTR_SIZE], char targetRelation[ATTR_SIZE], char attribute1[ATTR_SIZE], char attribute2[ATTR_SIZE]){

    // get relation1's and relation2's relId
    int relId1 = OpenRelTable::getRelId(srcRelation1);
    int relId2 = OpenRelTable::getRelId(srcRelation2);

    if(relId1 < 0 || relId1 >= MAX_OPEN || relId2 < 0 || relId2 >= MAX_OPEN){
        return E_RELNOTOPEN;
    }

    // get attribute catalog entries for the source relations corresponding to their give attributes.
    AttrCatEntry attrCatEntry1, attrCatEntry2;
    if(AttrCacheTable::getAttrCatEntry(relId1, attribute1, &attrCatEntry1) == E_ATTRNOTEXIST){
        return E_ATTRNOTEXIST;
    }
    if(AttrCacheTable::getAttrCatEntry(relId2, attribute2, &attrCatEntry2) == E_ATTRNOTEXIST){
        return E_ATTRNOTEXIST;
    }
    
    // if attribute1 and attribute2 are of different types, return error
    if(attrCatEntry1.attrType != attrCatEntry2.attrType){
        return E_ATTRTYPEMISMATCH;
    }

    /*
        iterate through all the attributes in both the source relation and check if there are
        any other pair of attributes other than join attributes

        if yes, then return error
    */
    RelCatEntry relCatEntry1, relCatEntry2;
    RelCacheTable::getRelCatEntry(relId1, &relCatEntry1);
    RelCacheTable::getRelCatEntry(relId2, &relCatEntry2);

    AttrCatEntry temp1, temp2;
    for(int j = 0; j < relCatEntry2.numAttrs; j++){
        if(j == attrCatEntry2.offset){
            continue;
        }

        AttrCacheTable::getAttrCatEntry(relId2, j, &temp2);
        
        for(int i = 0; i < relCatEntry1.numAttrs; i++){
            AttrCacheTable::getAttrCatEntry(relId1, i, &temp1);

            if(strcmp(temp2.attrName, temp1.attrName) == 0){

                return E_DUPLICATEATTR;
            }
        }
    }

    int numOfAttributes1 = relCatEntry1.numAttrs;
    int numOfAttributes2 = relCatEntry2.numAttrs;

    /*
        If srcRelation2 doesn't have an index on attribute2, create index

        This will reduce time complexity from O(m.n) -> O(mlogn + n)
        where m = no. of records in relation_1, n = no. of records in relation_2
    */
    if(attrCatEntry2.rootBlock == -1){
        int ret = BPlusTree::bPlusCreate(relId2, attribute2);
        if(ret != SUCCESS){
            return E_DISKFULL;
        }
    }

    int numOfAttributesInTarget = numOfAttributes1 + numOfAttributes2 - 1;
    
    // declaring the following arrays to store the details of the target relation
    char targetRelAttrNames[numOfAttributesInTarget][ATTR_SIZE];
    int targetRelAttrTypes[numOfAttributesInTarget];

    /*
        Iterate through all the attributes in both the source relations and update
        targetRelAttrNames[], targetRelAttrTypes[] arrays excluding attribute2
    */
    for(int i = 0; i < numOfAttributes1; i++){
        AttrCacheTable::getAttrCatEntry(relId1, i, &temp1);
        strcpy(targetRelAttrNames[i], temp1.attrName);
        targetRelAttrTypes[i] = temp1.attrType;
    }
    for(int j = 0; j < attrCatEntry2.offset; j++){
        AttrCacheTable::getAttrCatEntry(relId2, j, &temp2);
        strcpy(targetRelAttrNames[numOfAttributes1 + j], temp2.attrName);
        targetRelAttrTypes[numOfAttributes1 + j] = temp2.attrType;
    }
    for(int j = attrCatEntry2.offset + 1; j < numOfAttributes2; j++){
        AttrCacheTable::getAttrCatEntry(relId2, j, &temp2);
        strcpy(targetRelAttrNames[numOfAttributes1 + j - 1], temp2.attrName);
        targetRelAttrTypes[numOfAttributes1 + j - 1] = temp2.attrType;
    }

    // create the target relation
    int ret = Schema::createRel(targetRelation, numOfAttributesInTarget, targetRelAttrNames, targetRelAttrTypes);
    if(ret != SUCCESS){
        return ret;
    }

    // open and get targetRelation id
    int targetRelId = OpenRelTable::openRel(targetRelation);
    if(targetRelId < 0 || targetRelId >= MAX_OPEN){
        // delete the relation
        Schema::deleteRel(targetRelation);
        return targetRelId;
    }

    Attribute record1[numOfAttributes1];
    Attribute record2[numOfAttributes2];
    Attribute targetRecord[numOfAttributesInTarget];

    RelCacheTable::resetSearchIndex(relId1);

    // outer while loop to get all the records from srcRelation1 one by one
    while(BlockAccess::project(relId1, record1) == SUCCESS){

        // reset search index for srcRelation2, cause for every record in srcRelation1, we start from beginning
        RelCacheTable::resetSearchIndex(relId2);

        // reset search index for attribute2, cause we want record with whose attribute2 matches with attribute1
        AttrCacheTable::resetSearchIndex(relId2, attribute2);

        /*
            inner loop will get every record of srcRelation2 which satisfy the following condition:
            record1.attribute1 = record2.attribute2
        */
        while(BlockAccess::search(relId2, record2, attribute2, record1[attrCatEntry1.offset], EQ) == SUCCESS){
            /*
                copy srcRelation1's anf srcRelation2's attribute values (except for attribute2 in rel2) from record1
                and record2 to targetRecord
            */
            for(int i = 0; i < numOfAttributes1; i++){
                targetRecord[i] = record1[i];
            }
            for(int i = 0; i < attrCatEntry2.offset; i++){
                targetRecord[numOfAttributes1 + i] = record2[i];
            }
            for(int i = attrCatEntry2.offset + 1; i < numOfAttributes2; i++){
                targetRecord[numOfAttributes1 + i - 1] = record2[i];
            }

            // insert the currnt record into the target relation
            ret = BlockAccess::insert(targetRelId, targetRecord);
            if(ret != SUCCESS){
                OpenRelTable::closeRel(targetRelId);
                Schema::deleteRel(targetRelation);
                return E_DISKFULL;
            }
        }
    }

    OpenRelTable::closeRel(targetRelId);
    return SUCCESS;
}
