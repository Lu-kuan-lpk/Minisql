#ifndef _BufferManager_h
#define _BufferManager_h

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<cstdio>
#include<list>
#include<string.h>
using namespace std;

#include"BufferBlock.h"

#define MAXBLKNUM 256 //number of blocks in the buffer

class BufferManager {
public:
	BufferManager();
	~BufferManager();
	//////////INTERFACE//////////
	bool init();
	int getNewBlock(char* &data);
	char* getBlock(int block_index);
	bool writeBlock(int block_index, char* data);
	bool DeleteFileBlock(int block_index);
	bool LockBlock(int block_index);
	bool UnlockBlock(int block_index);
	bool close();
	////////////////////////////
private:
	int total_index;
	list<BufferBlock> buffer_blocks;
	bool CreateBufferBlock(int block_index, char* data);
	list<BufferBlock>::iterator FindBufferBlock(int block_index);
	bool UpdateBufferBlock(BufferBlock* blk);
	bool DeleteBufferBlock();
	bool BuffertoFile(BufferBlock* blk);
};



#endif // !_BufferManager_h

