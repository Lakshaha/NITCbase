//Assignment 1
#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <cstring>
#include <iostream>


// int main(int argc, char *argv[]) {
	// /* Initialize the Run Copy of Disk */
	// Disk disk_run; //declaration of disk class
	// //The disl function expects 2 arguments
	// // StaticBuffer buffer; of size 2048 char, 1 char 1 byte
// 
	// unsigned char buffer[BLOCK_SIZE];
	// char message[10];
// 
	// Disk :: readBlock(buffer, 0);
	// memcpy(message, buffer, 10);
// 
	// for (auto x : message) {
		// std :: cout << (int)x << " ";
	// }
// 
	// return 0;
// 
// 
// 
	// // OpenRelTable cache;
// 
// //return FrontendInterface::handleFrontend(argc, argv);
// }

int main(int argc, char *argv[]) {
	/* Initialize the Run Copy of Disk */
	Disk disk_run; //declaration of disk class
	//The disl function expects 2 arguments
	// StaticBuffer buffer; of size 2048 char, 1 char 1 byte

	unsigned char buffer[BLOCK_SIZE];
	Disk::readBlock(buffer,20);
	char message[20];
	memcpy(message,buffer,20);

	for(int i=0; i<20; i++)
	{
		std::cout << int(message[i]) << " ";
	}

	return 0;


	// OpenRelTable cache;

//return FrontendInterface::handleFrontend(argc, argv);
}
