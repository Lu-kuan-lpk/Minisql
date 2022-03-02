#ifndef _BufferBlock_h
#define _BufferBlock_h

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<cstdio>
#include<list>
using namespace std;

#define MAXBLKSIZE 4096 //size of a single block(4kb)

class BufferBlock {
public:
	BufferBlock(int index);
	~BufferBlock();
	bool operator==(const BufferBlock& b);
	void Lock();
	void Unlock();

	int blk_index;
	char* blk_data;
	bool modified;
	bool locked;
};

#endif


