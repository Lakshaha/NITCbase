#include "BlockAccess.h"

#include <cstring>

RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op)
{
	//get search index
	RecId prevRecId;
	RelCacheTable :: getSearchIndex(relId, &prevRecId);

	int block, slot;

	if (prevRecId.block == -1 && prevRecId.slot == -1)
	{
		//no search done prev, therefor start from beginning, use RelCatEntry
		RelCatEntry relCatEntry;
		RelCacheTable :: getRelCatEntry(relId, &relCatEntry);
		block = relCatEntry.firstBlk;
		slot = 0;
	}
	else
	{
		block = prevRecId.block;
		slot = prevRecId.slot+1;
	}

	//now begin linear search
	while (block != -1)
	{
		//creating a rec buffer object to get to access that block
		RecBuffer recBuffer(block);

		
		//get record with id, for that use recBuffer.getRecord
		//need header and slot map for this
		struct HeadInfo head;
		recBuffer.getHeader(&head);

		//slot map
		unsigned char slotMap[head.numSlots];
		recBuffer.getSlotMap(slotMap);
		
		//getting thr record
		Attribute record[head.numAttrs];
		recBuffer.getRecord(record, slot);

		if (slot >= head.numSlots)
		{
			block = head.rblock;
			slot = 0;
			continue;
		}

		//if slot is unoccupied dont bother with it
		if (slotMap[slot] == SLOT_UNOCCUPIED)
		{
			slot++;
			continue;
		}

		//compare record attribute value
		AttrCatEntry attrCatEntry;
		AttrCacheTable :: getAttrCatEntry(relId, attrName, &attrCatEntry);

		//the current attrbute val
		Attribute currentAttrVal = record[attrCatEntry.offset];

		int cmpVal = compareAttrs(currentAttrVal, attrVal, attrCatEntry.attrType);

		//if the cmpval and the op together works fine

		if (
            (op == NE && cmpVal != 0) ||    // if op is "not equal to"
            (op == LT && cmpVal < 0) ||     // if op is "less than"
            (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
            (op == EQ && cmpVal == 0) ||    // if op is "equal to"
            (op == GT && cmpVal > 0) ||     // if op is "greater than"
            (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
        )
        {
        	RecId searchIndex = {block, slot};

        	RelCacheTable :: setSearchIndex(relId, &searchIndex);

        	return searchIndex;
        }

        slot++;
	}

	return RecId{-1,-1};
}


int BlockAccess::renameRelation(char oldName[ATTR_SIZE], char newName[ATTR_SIZE])
{
	//reset search index so linear search can start -1,-1
	RelCacheTable::resetSearchIndex(RELCAT_RELID);

	Attribute newRelationName;
	strcpy(newRelationName.sVal, newName); // set newRelationName with newName

	char *relCatAttrRelName;
	strcpy(relCatAttrRelName, RELCAT_ATTR_RELNAME);
	//if the new name already exists
	RecId recId = BlockAccess::linearSearch(RELCAT_RELID, relCatAttrRelName, newRelationName,EQ);

	if (recId.block != -1 && recId.slot != -1)
	{
		return E_RELEXIST;
	}

	RelCacheTable::resetSearchIndex(RELCAT_RELID);

	Attribute oldRelationName;
	strcpy(oldRelationName.sVal, oldName);

	//search for the old relation id
	recId = BlockAccess::linearSearch(RELCAT_RELID, relCatAttrRelName, oldRelationName, EQ);

	if (recId.block == -1 && recId.slot == -1)
	{
		return E_RELNOTEXIST;
	}


	/*
        get the relation catalog record of the relation to rename using RecBuffer on
        the relation catalog [RELCAT_BLOCK] and RecBuffer.gerRecord function.
    */

    Attribute record[RELCAT_NO_ATTRS];
    RecBuffer recBuffer(RELCAT_BLOCK);
    recBuffer.getRecord(record, recId.slot);

	//update the relation name attribute w new relname
	strcpy(record[RELCAT_REL_NAME_INDEX].sVal, newName);
	recBuffer.setRecord(record, recId.slot);

	//update all attribute catalog entries
	RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
	int numAttrs = record[RELCAT_REL_NAME_INDEX].nVal;
	char *attrCatAttrRelName;
	strcpy(attrCatAttrRelName, ATTRCAT_ATTR_RELNAME);
	for (int i=0; i<numAttrs; i++)
	{
		RecId rec_id = BlockAccess::linearSearch(ATTRCAT_RELID, attrCatAttrRelName, oldRelationName, EQ);
		Attribute attrRecord[ATTRCAT_NO_ATTRS];
		RecBuffer attrBuffer(recId.block);
		attrBuffer.getRecord(attrRecord, rec_id.slot);

        attrRecord[ATTRCAT_REL_NAME_INDEX] = newRelationName;
        attrBuffer.setRecord(attrRecord, rec_id.slot);
	}
	return SUCCESS;
	
}


int BlockAccess::renameAttribute(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE])
{
	//reset search index
	RelCacheTable::resetSearchIndex(RELCAT_RELID);

	Attribute relNameAttr;
	strcpy(relNameAttr.sVal, relName);

	char *relCatAttrRelName;
	strcpy(relCatAttrRelName, RELCAT_ATTR_RELNAME);

	RecId recId = BlockAccess::linearSearch(RELCAT_RELID, relCatAttrRelName, relNameAttr, EQ);
	if (recId.block == -1 && recId.slot == -1)
	{
		return E_RELNOTEXIST;
	}

	//reset search index
	RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

	RecId attrToRenameRecId{-1,-1}; //attribute catalog recid of attr to rename
	Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];

	/* iterate over all Attribute Catalog Entry record corresponding to the
    relation to find the required attribute */
    // if found check if attrName == oldAttrName, replace then

    char *attrCatAttrRelName;
    strcpy(attrCatAttrRelName, ATTRCAT_ATTR_RELNAME);

    while (true)
    {
    	//linear search using relName
    	RecId rec_id = BlockAccess::linearSearch(ATTRCAT_RELID, attrCatAttrRelName, relNameAttr, EQ);

		if (rec_id.block == -1 && rec_id.slot == -1)
		{
			break;
		}

		RecBuffer attrBuffer(rec_id.block);
		//using getRecord to get the entry of the attribute into attrCatEntruRecord, from the foudn slot
		attrBuffer.getRecord(attrCatEntryRecord, rec_id.slot);

		if (strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, oldName) == 0)
		{
			attrToRenameRecId.block = rec_id.block;
			attrToRenameRecId.slot = rec_id.slot;
		}

		if (strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, newName) == 0)
		{
			return E_ATTREXIST;
		}
    }

    if (attrToRenameRecId.block == -1 && attrToRenameRecId.slot == -1)
    {
    	return E_ATTRNOTEXIST;
    }

    //updating entry in attrbute catalog
    RecBuffer rec_buffer(attrToRenameRecId.block);
    rec_buffer.getRecord(attrCatEntryRecord, attrToRenameRecId.slot);
    strcpy(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, newName);
    rec_buffer.setRecord(attrCatEntryRecord, attrToRenameRecId.slot);	
	return SUCCESS;
}
