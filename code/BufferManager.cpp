#define _CRT_SECURE_NO_WARNINGS
#include "BufferManager.h"

BufferManager::BufferManager() {
	total_index = 0;
}

BufferManager::~BufferManager() {
	;
}

bool BufferManager::init() {
	//����������
	string file_name = "InitFile/BMI.dat";
	ifstream inFile(file_name, ios::in | ios::binary);
	if (inFile.is_open()) {
		ostringstream tmp;
		tmp << inFile.rdbuf();
		total_index = atoi((tmp.str()).c_str());
		return true;
	}//���ڳ�ʼ���ļ�
	else return false;
}

int BufferManager::getNewBlock(char* &data) {
	data = new char[1024 * 4];
	CreateBufferBlock(total_index++, data);
	return total_index - 1;
}
char* BufferManager::getBlock(int block_index) {
	char *data = new char[1024 * 4];
	list<BufferBlock>::iterator p = FindBufferBlock(block_index);
	if (p != buffer_blocks.end()) {
		data = p->blk_data;
		UpdateBufferBlock(&(*p));
		return data;
	}//���Ѵ�����buffer��
	else {
		string file_name = "BlockData/" + to_string(block_index) + ".dat";
		ifstream inFile(file_name, ios::in | ios::binary);
		if (inFile.is_open()) {
			inFile.read(data, 1024 * 4);
			CreateBufferBlock(block_index, data);
			buffer_blocks.back().modified = false;
			inFile.close();
			return data;
		}//���ڿ��ļ���������buffer��
		else return NULL;
	}
}

bool BufferManager::writeBlock(int block_index, char* data) {
	list<BufferBlock>::iterator p = FindBufferBlock(block_index);
	if (p != buffer_blocks.end()) {
		p->blk_data = data;
		p->modified = true;
		UpdateBufferBlock(&(*p));
		return true;
	}//���Ѵ�����buffer��
	else {
		string file_name = "BlockData/" + to_string(block_index) + ".dat";
		ifstream inFile(file_name, ios::in | ios::binary);
		if (inFile.is_open()) {
			inFile.read(data, 1024 * 4);
			CreateBufferBlock(block_index, data);
			buffer_blocks.back().modified = true;
			inFile.close();
			return true;
		}//���ڿ��ļ���������buffer��
		else return false;
	}
}

bool BufferManager::DeleteFileBlock(int block_index) {
	list<BufferBlock>::iterator p = FindBufferBlock(block_index);
	if (p != buffer_blocks.end()) {
		buffer_blocks.remove(*p);
	}//���Ѵ�����buffer��
	string file_name = "BlockData/" + to_string(block_index) + ".dat";
	remove(file_name.c_str());
	return true;
}

bool BufferManager::LockBlock(int block_index) {
	list<BufferBlock>::iterator p = FindBufferBlock(block_index);
	if (p != buffer_blocks.end()) {
		p->Lock();
		UpdateBufferBlock(&(*p));
		return true;
	}//���Ѵ�����buffer��
	else {
		string file_name = "BlockData/" + to_string(block_index) + ".dat";
		ifstream inFile(file_name, ios::in | ios::binary);
		if (inFile.is_open()) {
			char* data = new char[4*1024];
			inFile.read(data, 1024 * 4);
			CreateBufferBlock(block_index, data);
			buffer_blocks.back().modified = false;
			p->Lock();
			inFile.close();
			return true;
		}//���ڿ��ļ���������buffer��
		else return false;
	}
}

bool BufferManager::UnlockBlock(int block_index) {
	list<BufferBlock>::iterator p = FindBufferBlock(block_index);
	if (p != buffer_blocks.end()) {
		p->Unlock();
		return true;
	}//���Ѵ�����buffer��
	else return false;
}

bool BufferManager::close() {
	while (buffer_blocks.size()) {
		buffer_blocks.begin()->locked = false;
		DeleteBufferBlock();
	}
	//�򿪳�ʼ���ļ�
	string file_name = "InitFile/BMI.dat";
	ofstream outFile(file_name, ios::out | ios::binary);
	if (outFile.is_open()) {
		outFile.clear();
		string data = to_string(total_index);
		outFile.write(data.c_str(), data.size());
		outFile.close();
		return true;
	}
	else return false;
}

///////////////////////////////

bool BufferManager::CreateBufferBlock(int block_index, char* data) {
	BufferBlock *blk = new BufferBlock(block_index);
	if (!blk) return false;
	blk->blk_data = data;
	if (buffer_blocks.size() == MAXBLKNUM) DeleteBufferBlock();
	buffer_blocks.push_back(*blk);
	return true;
}

list<BufferBlock>::iterator BufferManager::FindBufferBlock(int block_index) {
	//�жϸñ�ŵ�block�Ƿ��Ѿ���buffer��
	list<BufferBlock>::iterator p;
	for (p = buffer_blocks.begin();p != buffer_blocks.end();p++) {
		if (p->blk_index == block_index) return p;
	}
	return p;
}

bool BufferManager::DeleteBufferBlock() {
	//�ҵ�ͷ���ڵ�(LRU)
	BufferBlock* blk = &(*buffer_blocks.begin());
	if (!blk) return false;
	if (!blk->locked) {
		if (blk->modified) BuffertoFile(blk);
		//����block��buffer���Ƴ�
		buffer_blocks.remove(*blk);
	}
	else {
		UpdateBufferBlock(blk);
		DeleteBufferBlock();
	}
	return true;
}

bool BufferManager::UpdateBufferBlock(BufferBlock* blk) {
	if (!blk) return false;
	//�����ʹ�õ�block������ĩ
	BufferBlock tmp = *blk;
	buffer_blocks.remove(*blk);
	buffer_blocks.push_back(tmp);
	return true;
}

bool BufferManager::BuffertoFile(BufferBlock* blk) {
	//�����ļ�
	string file_name = "BlockData/" + to_string(blk->blk_index) + ".dat";
	ofstream outFile(file_name, ios::out | ios::binary);
	//д������
	char* data = blk->blk_data;
	outFile.write(data, 4*1024);
	outFile.close();
	return true;
}
