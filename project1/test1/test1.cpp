// test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileDirectory.h"
#include <iostream>

int main()
{
	char file1Data[1024];
	for (int i = 0; i < 256; i++)
	{
		file1Data[i] = i;
	}
	
	//	create and write a file; file1;  of 40 bytes;
	FileDirectory f1;	//create fileDirectory object called f1
	
	//	create and write a file; file1;  of 40 bytes;
	if (f1.create("file1", 40))
	{
		f1.write("file1", 40, file1Data, 2014, 1, 9, 15, 26, 32);
	}
	f1.printDirectory();
	f1.printClusters("file1");

	//	create and write a file; file2;  of 200 bytes;
	if (f1.create("file2", 200))
	{
		f1.write("file2", 200, file1Data, 1984, 2, 8, 1, 2, 2);
	}
	f1.printDirectory();
	f1.printClusters("file2");

	//	create and write a file; file3;  of 300 bytes;	
	if (f1.create("file3", 300))
	{
		f1.write("file3", 300, file1Data, 1996, 3, 19, 16, 17, 18);
	}
	f1.printDirectory();
	f1.printClusters("file3");

	//	create and write a file, file4, of 500 bytes.
	if (f1.create("file 4", 500))
	{
		f1.write("file 4", 500, file1Data, 2004, 4, 17, 17, 18, 18);
	}

	//	delete file2
	f1.deleteFile("file2");

	//	create and write a file; file4;  of 500 bytes.
	if (f1.create("file 4", 500))
	{
		f1.write("file 4", 500, file1Data, 2004, 4, 17, 17, 18, 18);
	}
	f1.printDirectory();
	f1.printClusters("file 4");

	return 0;
}

