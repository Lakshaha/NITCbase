#include "StaticBuffer.h"
//buffer capacity= 32
//block size = 2048
unsigned char StaticBuffer :: blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer :: metainfo[BUFFER_CAPACITY];

StaticBuffer :: StaticBuffer() //constructor
{	
	//initialise all the blocks free
	for (int i=0; i<BUFFER_CAPACITY; i++)
	{
		metainfo[i].free = true;
		metainfo[i].dirty = false;
		metainfo[i].timeStamp = -1;
		metainfo[i].blockNum = -1;
	}
}

StaticBuffer :: ~StaticBuffer()
{
/*	
	iterate through all the buffer blocks,
    write back blocks with metainfo as free=false,dirty=true
    using Disk::writeBlock()
*/
	for (int i=0; i<BUFFER_CAPACITY; i++)
	{
		if (!metainfo[i].free && metainfo[i].dirty)
		{
			Disk::writeBlock(StaticBuffer::blocks[i], metainfo[i].blockNum);
		}
	}

}	//deconstructor

//implemented using LRU, never fails, always assigns a block number, timestamp is reset to 0
//updates metainfo asw
//
int StaticBuffer::getFreeBuffer(int blockNum)
{
	if (blockNum < 0 || blockNum > DISK_BLOCKS) //if its a valid block num
	{
		return E_OUTOFBOUND;
	}


	// increase the timeStamp in metaInfo of all occupied buffers.
    // let bufferNum be used to store the buffer number of the free/freed buffer.
	int bufferNum=-1, bufferWithMaxTime=-1, maxTimeStamp=0;
	for (int i=0; i<BUFFER_CAPACITY; i++)
	{
		if (!metainfo[i].free)
		{
			metainfo[i].timeStamp+=1;

			if (maxTimeStamp < metainfo[i].timeStamp)
			{
				bufferWithMaxTime = i;
				maxTimeStamp = metainfo[i].timeStamp;
			}
		}
		if (metainfo[i].free && bufferNum == -1)
		{
			bufferNum = i;
		}
	}

	if (bufferNum == -1)
	{
		if (metainfo[bufferWithMaxTime].dirty)
		{
			Disk::writeBlock(StaticBuffer::blocks[bufferWithMaxTime], metainfo[bufferWithMaxTime].blockNum);
		}
		bufferNum = bufferWithMaxTime;
	}
	

	
	metainfo[bufferNum].free = false;
	metainfo[bufferNum].dirty = false;
	metainfo[bufferNum].timeStamp = 0;
	metainfo[bufferNum].blockNum = blockNum;
	return bufferNum;
}

int StaticBuffer::getBufferNum(int blockNum)
{
	if (blockNum < 0 || blockNum > DISK_BLOCKS)
	{
		return E_OUTOFBOUND;
	}

	for (int i=0; i<BUFFER_CAPACITY; i++)
	{
		if (metainfo[i].blockNum == blockNum and metainfo[i].free == false)
		{
			return i;
		}
	}
	return E_BLOCKNOTINBUFFER;


}


int StaticBuffer::setDirtyBit(int blockNum)
{
	int bufferIdx = getBufferNum(blockNum);
	if (bufferIdx == E_BLOCKNOTINBUFFER)
	{
		return E_BLOCKNOTINBUFFER;
	}

	if (bufferIdx == E_OUTOFBOUND)
	{
		return E_OUTOFBOUND;
	}	
	metainfo[bufferIdx].dirty = true;
	
	return SUCCESS;
	
}
