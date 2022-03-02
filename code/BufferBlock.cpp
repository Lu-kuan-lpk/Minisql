#include "BufferBlock.h"

BufferBlock::BufferBlock(int index) {
	blk_index = index;
	locked = false;
	modified = true;
}

BufferBlock::~BufferBlock() {
	;
}

bool BufferBlock::operator==(const BufferBlock& b) {
	if (this->blk_index == b.blk_index) return true;
	else return false;
}

void BufferBlock::Lock() {
	this->locked = true;
}

void BufferBlock::Unlock() {
	this->locked = false;
}