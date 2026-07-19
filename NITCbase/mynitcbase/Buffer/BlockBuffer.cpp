#include "BlockBuffer.h"

#include <cstdlib>
#include <cstring>

BlockBuffer::BlockBuffer(int blockNum)
{
	this->blockNum = blockNum;	
}

//calls the parent class constructor
RecBuffer :: RecBuffer(int blockNum) : BlockBuffer :: BlockBuffer(blockNum){}

//load the block header into argument pointer
int BlockBuffer :: getHeader(struct HeadInfo *head){
	unsigned char buffer[BLOCK_SIZE];

	//reading the block at the given block number to given buffer
	Disk :: readBlock(buffer, this->blockNum);

	//populate the num entries, num attr, numslots inm head
	memcpy(&head->numSlots, buffer+24,4);	
	memcpy(&head->numAttrs, buffer+20,4);	
	memcpy(&head->numEntries, buffer+16,4);
	memcpy(&head->rblock, buffer+12,4);
	memcpy(&head->lblock, buffer+8,4);

	return SUCCESS;
}

int RecBuffer::getRecord(union Attribute *rec, int slotNum)
{
	struct HeadInfo head;

	//get the header using getHeader(&pointer)
	this->getHeader(&head);

	int attrCount = head.numAttrs;
	int slotCount = head.numSlots;

	//read the block at this.blocknum to buffer
	unsigned char buffer[BLOCK_SIZE];
	Disk::readBlock(buffer, this->blockNum);

	/* record at slotNum will be at offset HEADER_SIZE + slotMapSize + (recordSize * slotNum)
	     - each record will have size attrCount * ATTR_SIZE
	     - slotMap will be of size slotCount
	 */

	int recordSize = attrCount * ATTR_SIZE;
	unsigned char *slotPointer = buffer + HEADER_SIZE + head.numSlots + (recordSize * slotNum);

	memcpy(rec, slotPointer, recordSize);
	return SUCCESS;
}
