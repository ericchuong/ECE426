#include "stdafx.h"
#include "FileDirectory.h"
#include <iostream>
#include <iomanip>

using namespace std;

FileDirectory::FileDirectory()
{
	/*
	Purpose:Initialize all entries in the fileDirectory and FAT16 to be 0
	Input:	none
	Output:	none
	*/

	for (int i = 0; i < 256; i++)
	{
		FAT16[i] = 0;
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			fileDirectory[i][j] = 0;
		}
	}
}

bool FileDirectory::create(char filename[], int numberBytes)
{
	/*
	Purpose:Check to see if there is enough memory in the fileDirectory and clusters available to create the file
	Input:	filename - the name of the file
			numberBytes - the number of bytes the file will allocate
	Output: true - the file was created
			false - not enough memory to create the file
	*/
	int i;
	int availableBytes = 0;

	//(1)	to check if there is an unused entry in the File Directory;  (i.e.  the first character of the file name in the File Directory is zero). Return false if not true.
	for (i = 0; i < 4; i++)
	{
		if (fileDirectory[i][0] == 0) break;
	}
	if (i == 4) return false;

	//(2)	to check if there are enough unused clusters to store the file with the numberBytes. Return false if not true.

	for (int j = 0; j < 256; j++)
	{
		if (FAT16[j] == 0 || FAT16[j] == 1)
			availableBytes = availableBytes + 1;
	}

	if (availableBytes < (numberBytes / 4) + ((numberBytes % 4 != 0)))
	{
		cout << "There is not enough memory to create the file. File was not created.\n\n";
		return false;
	}

	return true;
};
bool FileDirectory::deleteFile(char filename[])
{
	/*
	Purpose:Deletes a file, frees memory in the fileDirectory and frees clusters in the FAT16
	Input:	filename - the name of the file
	Output: true - file was deleted
			false - file could not be found in the directory
	*/
	int i, j;
	unsigned int size;
	unsigned int firstSectorAddress, lastSectorAddress;

	//(0)	to check if the file to be deleted; filename[]; is in the Directory.If not; return false.
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (fileDirectory[i][j] != filename[j]) break;
		}
		if (j == 8) break;
	}
	if (i == 4)
	{
		cout << "The file can not be found in the directory\n";
		return false;			// can't find the file name
	}

	//(1)	to change the first character of the file name in the File Directory to be zero;
	fileDirectory[i][0] = 0;

	//(2)	to change all entries of the clusters of this file in the FAT to 1; i.e.; unused.
	size = ((fileDirectory[i][31] << 24) + (fileDirectory[i][30] << 16) + (fileDirectory[i][29] << 8) + (fileDirectory[i][28]));
	firstSectorAddress = ((fileDirectory[i][27] << 8) + fileDirectory[i][26]);
	lastSectorAddress = (firstSectorAddress + (size / 4));
	for (int m = firstSectorAddress; m != lastSectorAddress; m++)
	{
		FAT16[m] = 0;			//change all clusters to 1
	}

	return true;
};
bool FileDirectory::read(char filename[])
{
	/*
	Purpose:Reads the data from data[] array from the specified file
	Input:	filename - the name of the file
	Output: true - file was read
			false - file could not be found in the directory
	*/
	int i, j, k;
	unsigned short int firstClusterAddress, ClusterAddress, storeClusterLocation[256];
	unsigned char fileData[1024];

	for (int count = 0; count < 256; count++)
	{
		storeClusterLocation[count] = 0;
	}

	//(0)	to check if the file to be read; filename[]; is in the Directory.If not; return false.
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (fileDirectory[i][j] != filename[j]) break;
		}
		if (j == 8) //found a file in the directory matching the input file name
		{
			//(1)	use the file name to get the file information from the File Directory; including date; time; number of bytes and the first cluster address;
			firstClusterAddress = fileDirectory[i][26] + (fileDirectory[i][27] << 8);		// first sector address is located in [27:26]
			ClusterAddress = firstClusterAddress;
			//(2)	use the first cluster address to get all the cluster addresses of this file from the FAT - 16;
			for (k = 0; k < 256 && ClusterAddress < 0xfff8 && ClusterAddress != 255; k++)
			{
				storeClusterLocation[k] = ClusterAddress;
				ClusterAddress = FAT16[ClusterAddress];
			}
			//(3)	use all the cluster addresses to read the data from the disk / flash memory.

			int counter = 0;
			
			for (int count = 0; count < 256; count++)
			{
				if (storeClusterLocation[count] != 0)
				{
					counter++;
				}
			}
			for (int count = 0; count < counter*4; count++)
			{
				fileData[count] = data[count];
			}
			break;
		}
	}
	if (i == 4)
	{
		cout << "File could not be found.\n";
		return false;
	}
	else
		return true;
};
bool FileDirectory::write(char filename[], int numberBytes, char data[], int year, int month, int day, int hour, int minute, int second)
{
	/*
	Purpose:Writes numberBytes bytes of data from data[] array into the specified file
	Input:	filename - the name of the file
			numberBytes - number of bytes the file will allocate
			data[] - data array
			year - date the file was created
			month - date the file was created
			day - date the file was created
			hour - time the file was created
			minute - time the file was created
			second - time the file was created
	Output: true - file was written successfully
			false - file was not written
	*/
	//(0)	to look for the first unused entry(0 or 1) in the FAT - 16; and use it as the First Cluster Address.
	unsigned char firstClusterAddress, nextClusterAddress;
	unsigned short int date, time;
	int availableBytes = 0;
	int aa;

	for (int i = 2; i < 256; i++)
	{
		if (FAT16[i] == 0 || FAT16[i] == 1)
		{
			firstClusterAddress = i;
			break;
		}
	}

	if (firstClusterAddress == 256)
	{
		cout << "There are no free clusters left.";
		return false;
	}
	//(1)	to look for the next unused entry(0 or 1) in the FAT - 16; and use it as the Next Cluster Address; and write its value into the FAT - 16.
	//(2)	Repeat Step 2 until all clusters are found and the FAT - 16 is updated.

	for (int temp = 0; temp < 256; temp++)
	{
		if (FAT16[temp] == 0 || FAT16[temp] == 1)
			availableBytes = availableBytes + 1;
	}

	if (availableBytes < (numberBytes / 4) + ((numberBytes % 4 != 0)))
	{
		cout << "There are no free clusters left. File was not written.\n\n";
		return false;
	}

	int counter = 1;
	int minusAddress = 0;
	int z;
	nextClusterAddress = firstClusterAddress + 1;
	for (z = firstClusterAddress; counter < (((numberBytes / 4) + ((numberBytes % 4 != 0) ? 1 : 0) ) + 1); z++)
	{
		if (FAT16[z + 1] == 0 || FAT16[z + 1] == 1)
		{
			nextClusterAddress = (z + 1);
			FAT16[z - minusAddress] = nextClusterAddress;
			counter++;
			minusAddress = 0;
		}
		else
			minusAddress = minusAddress + 1;
	}
	
	FAT16[z-1] = 255;

	//(3)	to write / update the file name; extension; date; time; file length and first cluster address into the first unused entry in the File Directory;
	unsigned int empty;
	int i;
	for (i = 0; i < 4; i++)
	{
		if (fileDirectory[i][0] == 0)
		{
			empty = i;
			break;
		}
	}

	//write filler extension .cpp
	fileDirectory[i][8] = 'c';
	fileDirectory[i][9] = 'p';
	fileDirectory[i][10] = 'p';

	//check if a file with that name already exists in the directory
	//if one exists, for now, do not allow it
	for (aa = 0; aa < 8; aa++)
	{
		if (filename[aa] != fileDirectory[0][aa] && filename[aa] != fileDirectory[1][aa] && filename[aa] != fileDirectory[2][aa] && filename[aa] != fileDirectory[3][aa])
		{
			goto noDuplicate;
		}
	}
	if (aa == 8)
	{
		//duplicate file name exists in fileDirectory
		cout << "\nA file with that name already exists. Unable to write duplicate file.\n";
		return false;
	}

	noDuplicate:;
	//write file name into its new entry of the file directory
	for (int j = 0; j < 8; j++)
	{
		fileDirectory[empty][j] = filename[j];
	}

	//write date into directory[25:24]
	date = (((year - 1980) << 9) + (month << 5) + day); //date goes in [25:24]
	fileDirectory[i][25] = ((date & 0xFF00) >> 8);
	fileDirectory[i][24] = (date & 0x00FF);

	//write time into directory[23:22]
	time = ((hour << 11) + (minute << 5) + (second >> 1));
	fileDirectory[i][23] = ((time & 0xFF00) >> 8);
	fileDirectory[i][22] = (time & 0x00FF);

	//write first sector address into directory[27:26]
	fileDirectory[empty][27] = (firstClusterAddress >> 8);	
	fileDirectory[empty][26] = firstClusterAddress;

	//write file size into directory[31:28]
	fileDirectory[empty][31] = (numberBytes >> 24);
	fileDirectory[empty][30] = (numberBytes >> 16);
	fileDirectory[empty][29] = (numberBytes >> 8);
	fileDirectory[empty][28] = numberBytes;

	//write data[]
	unsigned char dataArray[1024];
	for (int a = 0; a < numberBytes; a++)
	{
		dataArray[a] = data[a];
	}

	return true;
};
void FileDirectory::printClusters(char filename[])
{
	/*
	Purpose:Print all the clusters of a file
	Input:	filename - the name of the file
	Output: prints all the clusters of a file
	*/
	//(1)	to check if the file to be printed; filename[]; is in the Directory.
	int i, j;
	unsigned short int date, time, firstClusterAddress;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (fileDirectory[i][j] != filename[j]) break;
		}
		if (j == 8) break;
	}
	if (i == 4)
	{
		cout << "File does not exist\n";
		goto here;
	}

	//(2)	use the file name to get the file information from the File Directory; including the first cluster address;

	if (fileDirectory[i][0] != '0')
	{
		//file name
		for (int j = 0; j < 8; j++)
		{
			fileDirectory[i][j];
		}

		//extension
		for (int j = 8; j < 11; j++)
		{
			fileDirectory[i][j];
		}
	}

	//date
	date = (fileDirectory[i][25] << 8) + fileDirectory[i][24]; //date is in [25:24]

	 //time
	time = (fileDirectory[i][23] << 8) + fileDirectory[i][22]; //time is in [23:22]

	//file size
	int size = (fileDirectory[i][31] << 24) + (fileDirectory[i][30] << 16) + (fileDirectory[i][29] << 8) + fileDirectory[i][28];

	//(3)	use the first cluster address to get all cluster addresses from the FAT - 16;
	firstClusterAddress = fileDirectory[i][26] + (fileDirectory[i][27] << 8);		// first sector address is located in [27:26]

	int numberOfClusters = (size / 4) + ((size % 4) ? 1 : 0);

	int temp = firstClusterAddress;
	
	while (FAT16[temp] != 255)
	{
		cout << temp;
		cout << "->";
		temp = FAT16[temp];
	}
		cout << temp;
		cout << "\n\n";

	here:;
};
void FileDirectory::printDirectory()
{
	/*
	Purpose:Prints all the files of the directory
	Input:	none
	Output: prints all the files of the directory
	*/
	int i;
	unsigned short int date, time, firstClusterAddress;

	if (fileDirectory[0][0] == 0 && fileDirectory[1][0] == 0 && fileDirectory[2][0] == 0 && fileDirectory[3][0] == 0)
	{
		cout << "There are no files in the directory.\n";
		goto here;
	}

	cout << "FileName.ext\tDate Created\tTime Created\tFile Size\tFirst Cluster Address\n";
	for (i = 0; i < 4; i++)
	{
		if (fileDirectory[i][0] != 0)
		{
			//print file name
			for (int j = 0; j < 8; j++)
			{
				cout << fileDirectory[i][j];
			}
			cout << ".";
			//print extension
			for (int j = 8; j < 11; j++)
			{
				cout << fileDirectory[i][j];
			}

			cout << "\t";
			//print date
			date = (fileDirectory[i][25] << 8) + fileDirectory[i][24]; //date is in [25:24]
			cout << ((date & 0x01E0) >> 5) << "/";						//output month
			cout << setfill('0') << setw(2) << (date & 0x001F) << "/";	//output day
			cout << (1980 + ((date & 0xFE00) >> 9));					//output year;	& is used to mask bits

			cout << "\t";
			//print time
			time = (fileDirectory[i][23] << 8) + fileDirectory[i][22]; //time is in [23:22]
			cout << setfill('0') << setw(2) << ((time & 0xF800) >> 11);			//output hour
			cout << ':';
			cout << setfill('0') << setw(2) << ((time & 0x07E0) >> 5);			//output minute
			cout << ':';
			cout << setfill('0') << setw(2) << ((time & 0x001F) << 1);			//output second/2
			cout << "\t";

			//print file size
			cout << (fileDirectory[i][31] << 24) + (fileDirectory[i][30] << 16) + (fileDirectory[i][29] << 8) + fileDirectory[i][28];

			//print first cluster address
			firstClusterAddress = fileDirectory[i][26] + (fileDirectory[i][27] << 8);		// first sector address is located in [27:26]
			
			cout << "\t\t" << firstClusterAddress;
			cout << "\n";
		}
	}
	here:;
	cout << "\n";
}

void FileDirectory::printData(char filename[])
{
	/*
	Purpose:Print the data of the file
	Input:	filename - the name of the file
	Output: prints the data of the file
	*/
	//(1)	use the file name to get the file information from the File Directory; including the first cluster address;
	int i, j;
	unsigned short int date, time, firstClusterAddress, ClusterAddress, storeClusterLocation[256];
	unsigned char fileData[1024];

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (fileDirectory[i][j] != filename[j]) break;
		}
		if (j == 8) break;
	}
	if (i == 4)
	{
		cout << "File does not exist\n";
		goto here;
	}

	if (fileDirectory[i][0] != '0')
	{
		//file name
		for (int j = 0; j < 8; j++)
		{
			fileDirectory[i][j];
		}

		//extension
		for (int j = 8; j < 11; j++)
		{
			fileDirectory[i][j];
		}
	}

	//date
	date = (fileDirectory[i][25] << 8) + fileDirectory[i][24]; //date is in [25:24]

	//time
	time = (fileDirectory[i][23] << 8) + fileDirectory[i][22]; //time is in [23:22]

	//file size
	int size = (fileDirectory[i][31] << 24) + (fileDirectory[i][30] << 16) + (fileDirectory[i][29] << 8) + fileDirectory[i][28];

	//(2)	use the first cluster address to get all cluster addresses from the FAT - 16;
	firstClusterAddress = (fileDirectory[i][27] << 8) + fileDirectory[i][26];		// first sector address is located in [27:26]
	ClusterAddress = firstClusterAddress;
	for (int k = 0; k < 256 && ClusterAddress < 0xFFF8; k++)
	{
		storeClusterLocation[k] = ClusterAddress;
		ClusterAddress = FAT16[ClusterAddress];
	}

	//(3)	use cluster address to read the data of the file. Use the file length to print these data in hexadecimal format.

	int counter = 0;

	for (int count = 0; count < 256; count++)
	{
		if (storeClusterLocation[count] != 0)
		{
			counter++;
		}
	}
	cout << "File contains " << (counter - 1) * 4 << " Bytes of data.\n";
	for (int count = 0; count < counter * 4; count++)
	{
		fileData[count] = data[count];
		cout << fileData[count];
	}
	cout << "\n\n";
here:;
}