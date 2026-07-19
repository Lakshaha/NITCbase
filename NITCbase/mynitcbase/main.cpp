#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <cstring>
#include <iostream>

int main(int argc, char *argv[]) {

	//an instance of the Disk class to run readblock and write block
	Disk disk_run;

	//create object for RC, AC
	RecBuffer relCatBuffer(RELCAT_BLOCK);
	RecBuffer attrCatBuffer(ATTRCAT_BLOCK);

	//struct type of HeadInfo, storing meta info about blocks
	HeadInfo relCatHeader;
	HeadInfo attrCatHeader;

	
  	// load the headers of both the blocks into relCatHeader and attrCatHeader
	relCatBuffer.getHeader(&relCatHeader);
	attrCatBuffer.getHeader(&attrCatHeader);

	for (int i=0; i<relCatHeader.numEntries; i++)
	{
		//RELCAT_NO_ATTRS global const for 6 here
		Attribute relCatRecord[RELCAT_NO_ATTRS];
		relCatBuffer.getRecord(relCatRecord, i);

		printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);

		for (int j=0; j<attrCatHeader.numEntries; j++)
		{
			Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
			attrCatBuffer.getRecord(attrCatRecord, j);

			if(strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal , relCatRecord[RELCAT_REL_NAME_INDEX].sVal) == 0)
			{
				const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
		        printf("  %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
			}
		}
		printf("\n");

		
	}

	return 0;
	


}


