#include "record.h"

//create a tuple block, return its block index
int createTupleBlock(int tupleSize, char *&blockPtr)
{
    int blockIndex;
    blockIndex = buffer.getNewBlock(blockPtr);
    int slotSize;
    int tupleNum;
    slotSize = sizeof(int) + sizeof(bool) + tupleSize;
    tupleNum = (1024 * 4 - 2 * sizeof(int)) / (slotSize);
    *(int *)blockPtr = 0; //initialize the number of the tuple
    int offsetAddr;
    offsetAddr = sizeof(int) * 2;                  //the offset address of the first slot
    *(int *)(blockPtr + sizeof(int)) = offsetAddr; //the free slot address
    for (int i = 0; i < tupleNum - 1; i++)
    {
        //intialize the offset pointer and exist bool of each slot
        *(int *)(blockPtr + offsetAddr) = offsetAddr + slotSize; //point to next slot
        *(bool *)(blockPtr + offsetAddr + sizeof(int)) = false;
        offsetAddr += slotSize;
    }
    *(int *)(blockPtr + offsetAddr) = 0; //indicate this block is the end
    *(bool *)(blockPtr + offsetAddr) = false;
    buffer.writeBlock(blockIndex, blockPtr); //write back the data
    return blockIndex;
}

//create a record block, return its block index
int createRecordBlock(char *&blockPtr)
{
    int blockIndex;
    blockIndex = buffer.getNewBlock(blockPtr);
    *(int *)blockPtr = 0;                    //initailize the num of the tuple blocks
    buffer.writeBlock(blockIndex, blockPtr); //write back the data
    return blockIndex;
}

//add a new tuple block index to the record block
bool addTuple2Record(int tupleIndex, int recordIndex)
{
    char *recordPtr;
    recordPtr = buffer.getBlock(recordIndex);
    int tupleNum;
    tupleNum = *(int *)recordPtr;
    int offsetAddr;                                //offset address
    offsetAddr = sizeof(int) * (tupleNum + 1);     //go to the end of the record
    *(int *)(recordPtr + offsetAddr) = tupleIndex; //add new tuple block index
    (*(int *)recordPtr)++;                         //update the record number
    buffer.writeBlock(recordIndex, recordPtr);     //write back the data
    return true;
}

//add a new tuple to a tuple block, return 1 if success
bool addTuple2block(oneTuple &data, int blockIndex)
{
    int tupleSize;
    tupleSize = data.getTupleSize();
    char *blockPtr;
    blockPtr = buffer.getBlock(blockIndex);
    int offsetAddr;
    offsetAddr = *(int *)(blockPtr + sizeof(int));
    if (offsetAddr == 0) //the block is full
    {
        return false; //adding fails, need to create a new block
    }
    (*(int *)(blockPtr))++; //update the num of the tuples
    *(int *)(blockPtr + sizeof(int)) =
        *(int *)(blockPtr + offsetAddr); //update the freeBlock address
    *(bool *)(blockPtr + offsetAddr + sizeof(int)) = true;
    //update the existence status of slot
    offsetAddr += sizeof(int) + sizeof(bool); //reach the tuple part
    for (int i = 0; i < data.columns.size(); i++)
    {
        switch (data.columns[i].type)
        {
        case INT_TPYE:
            *(int *)(blockPtr + offsetAddr) = *(int *)data.dataPtr[i];
            break;
        case FLOAT_TYPE:
            *(double *)(blockPtr + offsetAddr) = *(double *)data.dataPtr[i];
            break;
        case CHAR_TYPE:
            for (int j = 0; j < data.columns[i].charLength; j++)
            {
                *(char *)(blockPtr + offsetAddr + j) = *((char *)data.dataPtr[i] + j);
            }
            break;

        default:
            break;
        }
        offsetAddr += data.columns[i].getTypeSize();
    }
    buffer.writeBlock(blockIndex, blockPtr); //write back the data
    return true;
}

//test if a tuple block is not full
bool isNotFull(int blockIndex)
{
    char *ptr;
    ptr = buffer.getBlock(blockIndex);
    bool flag;
    flag = (*(int *)(ptr + sizeof(int)) != 0) ? true : false;
    return flag;
}