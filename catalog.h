#ifndef _CATALOG_
#define _CATALOG_
#include "table.h"
#include <iostream>
#include "BufferManager.h"
#include <iomanip>

using namespace std;

extern BufferManager buffer;

class tableCatalog
{
public:
    char name[50]; //the name of the table
    attrVec columns;
    int catalogIndex; //the block stored the information of the table
    void extract()    //extract the information from the block
    {
        char *blockPtr;
        blockPtr = buffer.getBlock(catalogIndex);
        for (int i = 0; i < 50; i++)
        {
            name[i] = blockPtr[i];
        }
        int numOfAttr;
        numOfAttr = *(int *)(blockPtr + 50);
        columns.clear();
        columns.reserve(numOfAttr);
        for (int i = 0; i < numOfAttr; i++)
        {
            columns.push_back(
                *(attribute *)(blockPtr + 50 + sizeof(int) + sizeof(attribute) * i));
        }
    }
    void create() //create a new block
    {
        char *blockPtr;
        int blockIndex;
        blockIndex = buffer.getNewBlock(blockPtr);
        for (int i = 0; i < 50; i++)
        {
            blockPtr[i] = (*this).name[i];
        }
        *(int *)(blockPtr + 50) = (*this).columns.size();
        for (int i = 0; i < (*this).columns.size(); i++)
        {
            *(attribute *)(blockPtr + 50 + sizeof(int) + sizeof(attribute) * i) =
                (*this).columns[i];
        }
        buffer.writeBlock(blockIndex, blockPtr);
        catalogIndex = blockIndex;
    }
    void drop() //drop the catalog
    {
        buffer.DeleteFileBlock(catalogIndex);
    }
    void display() //display the attribute
    {
        cout << setfill('_') << setw((11) * columns.size() + 1);
        cout << '\n';
        for (auto &i : columns)
        {
            cout << '|';
            cout << setw(10);
            cout << setfill(' ');
            cout << i.name;
        }
        cout << '|' << endl;
    }
    int getPos(string s) //get the pos of an attribute named string s
    {
        for (int i = 0; i < columns.size(); i++)
        {
            if (string(columns[i].name) == s)
            {
                return i;
            }
        }
        return 0;
    }
};

#endif