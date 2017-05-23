#pragma once

class FileDirectory {
public:
	FileDirectory();

	bool create(char filename[], int numberBytes);
	bool deleteFile(char filename[]);
	bool read(char filename[]);
	bool write(char filename[], int numberBytes, char data[], int year, int month, int day, int hour, int minute, int second);
	void printClusters(char filename[]);
	void printDirectory();
	void printData(char filename[]);

private:
	unsigned char fileDirectory[4][32];		/* a max of 4 file directories. Each has 32 bytes.
											File Name[7:0] : 8-byte
											File Name Extension[8:10]: 3-byte
											Date[25:24] and Time[23:22] of file creating or modification
											16-bit First sector address[27:26]
											32-bit File size[31:28], for a file size up to 4 Gbytes
											The remaining bytes are for other File attributes  and future expansion. */
	unsigned short int FAT16[256]; // 256 clusters only
	unsigned char data[1024]; // 256 clusters * 4 bytes/cluster = 1024 bytes
};