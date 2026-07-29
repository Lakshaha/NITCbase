#include "BlockBuffer.h"

#include <cstdlib>
#include <cstring>

BlockBuffer::BlockBuffer(int blockNum)
{
	this->blockNum = blockNum;	 // initialise this.blockNum with the argument
}

//calls the parent class constructor
RecBuffer :: RecBuffer(int blockNum) : BlockBuffer :: BlockBuffer(blockNum){}

//load the block header into argument pointer
int BlockBuffer::getHeader(struct HeadInfo* head) //tells its block buffer class
{
	unsigned char buffer[BLOCK_SIZE];

	Disk :: readBlock(buffer, this->blockNum);

	// populate the numEntries, numAttrs and numSlots fields in *head
    memcpy(&head->numSlots, buffer + 24, 4);
    memcpy(&head->numEntries, buffer + 16, 4);
    memcpy(&head->numAttrs, buffer + 20, 4);
    memcpy(&head->rblock, buffer + 12, 4);
    memcpy(&head->lblock, buffer + 8, 4);

    return SUCCESS;
}


// load the record at slotNum into the argument pointer, basically slotNum in rec
int RecBuffer :: getRecord(union Attribute *rec, int slotNum)
{
	struct HeadInfo head;
	
	// get the header using this.getHeader() function
	this->getHeader(&head);

	int attrCount = head.numAttrs;
	int slotCount = head.numSlots;
	

	
    // read the block at this.blockNum into a buffer
	unsigned char buffer[BLOCK_SIZE];
	Disk::readBlock(buffer, this->blockNum);

	int recordSize = attrCount * ATTR_SIZE; //attrSize is 16
	int offset = HEADER_SIZE + head.numSlots + (recordSize * slotNum);
	unsigned char *slotPointer = buffer + offset;

	memcpy(rec, slotPointer, recordSize);

	return SUCCESS;

}
