#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <cstring>
#include <iostream>

int main(int argc, char* argv[])
{
	Disk disk_run;
	//declaring buffer class aftrer main
	StaticBuffer buffer;
	//declare cache for openRelTable
	OpenRelTable cache;

	return FrontendInterface::handleFrontend(argc,argv);

	
}

// RelCatEntry relEntry;
// AttrCatEntry attrEntry;
// 
// //to get the catalog info
// 
// for (int i=0; i<3; i++)
// {
// RelCacheTable::getRelCatEntry(i, &relEntry);
// printf("Relation: %s\n", relEntry.relName);
// //print each of its attribute
// for (int j=0; j<5; j++)
// {
// AttrCacheTable::getAttrCatEntry(i,j,&attrEntry);
// printf(" %s: %s\n", attrEntry.attrName, (attrEntry.attrType == NUMBER) ? "NUM" : "STR");
// }
// printf("\n");
// }
