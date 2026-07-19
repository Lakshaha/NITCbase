	include "Buffer/StaticBuffer.h"

	#include "Cache/OpenRelTable.h"

	#include "Disk_Class/Disk.h"

	#include "FrontendInterface/FrontendInterface.h"

	#include <iostream>

	using namespace std;



	int main(int argc, char *argv[]) {

	  Disk disk_run;



	  //Create objects for the relation catalog and attribute catalog

	  RecBuffer relCatBuffer(RELCAT_BLOCK);

	  RecBuffer attrCatBuffer(ATTRCAT_BLOCK);



	  HeadInfo relCatHeader;

	  HeadInfo attrCatHeader;



	  // load the headers of both the blocks into relCatHeader and attrCatHeader

	  // will be implementing these function in future stages

	  relCatBuffer.getHeader(&relCatHeader);

	  attrCatBuffer.getHeader(&attrCatHeader);





	  // Modification question 1: two read across multiple records


	  // We will have to create new recordBuffer and headInfo


	  HeadInfo attrCatHeader2;


	  RecBuffer attrCatBuffer2(attrCatHeader.rblock);


	  attrCatBuffer2.getHeader(&attrCatHeader2);




	  for(int i = 0; i < relCatHeader.numEntries; i++){



	    Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog

	    relCatBuffer.getRecord(relCatRecord, i);

	    printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);



	    for(int j = 0; j < attrCatHeader.numEntries; j++){



	      //Declaring attrCatRecord and loading the attribute catalog entry into it

	      Attribute attrCatRecord[ATTRCAT_NO_ATTRS];

	      attrCatBuffer.getRecord(attrCatRecord,j);







	      // Modification Question 2, Change attribute name 'Class' of Student to 'Batch' 


	      if(strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,"Students") == 0){


	        if(strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,"Class") == 0){


	          unsigned char temp_buffer[BLOCK_SIZE];


	          Disk :: readBlock(temp_buffer,ATTRCAT_BLOCK);


	          int recordSize = ATTR_SIZE*ATTRCAT_NO_ATTRS;


	          unsigned char* newAttrName = temp_buffer + HEADER_SIZE + attrCatHeader.numSlots + recordSize*j + ATTRCAT_ATTR_NAME_INDEX*ATTR_SIZE;


	          // basically : 0 + 32 + 20 + 96*j + 1*16


	          memcpy(newAttrName,"Batch",ATTR_SIZE);


	          Disk :: writeBlock(temp_buffer, ATTRCAT_BLOCK);


	        }


	      }





	      // if attribute catalog entry corresponds to the current relation


	      if(strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal , relCatRecord[RELCAT_REL_NAME_INDEX].sVal) == 0){


	        const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER? "NUM" : "STR";


	        printf(" %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);


	      }


	    }





	    // 2nd for loop to print 2nd block of the attribute catalog


	    for(int j = 0; j < attrCatHeader2.numEntries; j++){





	      //Declaring attrCatRecord and loading the attribute catalog entry into it


	      Attribute attrCatRecord[ATTRCAT_NO_ATTRS];


	      attrCatBuffer2.getRecord(attrCatRecord,j);




	      // if attribute catalog entry corresponds to the current relation

	      if(strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal , relCatRecord[RELCAT_REL_NAME_INDEX].sVal) == 0){

	        const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER? "NUM" : "STR";

	        printf(" %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);

	      }

	    }

	    printf("\n");


	    

	  }



	  return 0;
