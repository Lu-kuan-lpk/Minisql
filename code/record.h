#ifndef _RECORD_
#define _RECORD_

#include "table.h"
#include <iostream>
#include <vector>
#include "catalog.h"
#include "BufferManager.h"
#include "IndexManager.h"
#include <set>
#include <iomanip>

using namespace std;

extern BufferManager buffer;

//create a tuple block, return its block index
int createTupleBlock(int tupleSize, char *&blockPtr);

//create a record block, return its block index
int createRecordBlock(char *&blockPtr);

//add a new tuple block index to the record block
bool addTuple2Record(int tupleIndex, int recordIndex);

//add a new tuple to a tuple block, return 1 if success
bool addTuple2block(oneTuple &data, int blockIndex);

//test if a tuple block is not full
bool isNotFull(int blockIndex);

class tmpTable
{
public:
    attrVec columns;            //attributes
    vector<paleTuple> tupleVec; //store all the tuples
    int blockIndex;             // the index of the block
    char *ptr;                  //pointer to the tuple block
    tmpTable(attrVec &attr) : columns(attr) {}
    tmpTable() {}
    void extract(int tupleIndex) //extract the data pointer in the block
    {
        blockIndex = tupleIndex;
        tupleVec.clear();
        int slotSize;
        slotSize = sizeof(int) + sizeof(bool);
        for (auto &i : columns)
        {
            slotSize += i.getTypeSize();
        }
        ptr = buffer.getBlock(blockIndex);
        int tupleNum;
        tupleNum = *(int *)ptr; //get the num of tuple
        int currGet;            //the number of the tuples gotten
        currGet = 0;
        for (int offset = 2 * sizeof(int); offset < 1024 * 4; offset += slotSize)
        {
            int tmpOffset;
            if (*(bool *)(ptr + offset + sizeof(int)) == true) //if the data exists in the slot
            {
                tmpOffset = sizeof(int) + sizeof(bool);
                currGet++; //update the currGet
                paleTuple tmpTuple;
                tmpTuple.clear();
                for (auto &i : columns) //get all the data in one tuple
                {
                    tmpTuple.push_back((char *)(ptr + offset + tmpOffset));
                    tmpOffset += i.getTypeSize();
                }
                tupleVec.push_back(tmpTuple);
                if (currGet >= tupleNum)
                {
                    break; //get all tuple, break
                }
            }
        }
    }
    void display() //display all the data
    {
        for (auto &i : tupleVec)
        {
            cout << setfill('_') << setw((11) * columns.size() + 1);
            cout << '\n';
            for (int j = 0; j < columns.size(); j++)
            {
                cout << '|';
                cout << setw(10);
                cout << setfill(' ');
                switch (columns[j].type)
                {
                case INT_TPYE:
                    cout << *(int *)(i[j]);
                    break;
                case FLOAT_TYPE:
                    cout << *(double *)(i[j]);
                    break;
                case CHAR_TYPE:
                    char tmpC[256];
                    for (int k = 0; k < columns[j].charLength; k++)
                    {
                        tmpC[k] = i[j][k];
                    }
                    cout << tmpC;
                    break;
                }
            }
            cout << '|' << endl;
        }
        cout << setfill('_') << setw((11) * columns.size() + 1);
        cout << '\n';
    }
    void initIndexI(Index<int> &index, int pos) //initialize an index on INT
    {
        for (auto &i : tupleVec)
        {
            index.InsertKey(*(int *)(i[pos]), blockIndex);
        }
    }
    void initIndexD(Index<double> &index, int pos) //initialize an index on FLOAT
    {
        for (auto &i : tupleVec)
        {
            index.InsertKey(*(double *)(i[pos]), blockIndex);
        }
    }
    void initIndexS(Index<string> &index, int pos) //initialize an index on CHAR
    {
        for (auto &i : tupleVec)
        {
            index.InsertKey(string(i[pos]), blockIndex);
        }
    }

    void select(condition &con) //display with condition
    {
        for (auto &i : tupleVec)
        {

            bool conFlag; //indicate continue
            conFlag = false;
            for (int j = 0; j < columns.size(); j++)
            {
                switch (columns[j].type)
                {
                case INT_TPYE:
                    if (!con.test(to_string(*(int *)(i[j])), j, columns[j].type, true))
                    {
                        conFlag = true;
                    }
                    break;
                case FLOAT_TYPE:
                    if (!con.test(to_string(*(double *)(i[j])), j, columns[j].type, true))
                    {
                        conFlag = true;
                    }
                    break;
                case CHAR_TYPE:
                    if (!con.test(string(i[j]), j, columns[j].type, true))
                    {
                        conFlag = true;
                    }
                    break;
                }
                if (conFlag)
                {
                    break;
                }
            }
            if (conFlag)
            {
                continue; //don't show this tuple
            }
            cout << setfill('_') << setw((11) * columns.size() + 1);
            cout << '\n';
            for (int j = 0; j < columns.size(); j++)
            {
                cout << '|';
                cout << setw(10);
                cout << setfill(' ');
                switch (columns[j].type)
                {
                case INT_TPYE:
                    cout << *(int *)(i[j]);
                    break;
                case FLOAT_TYPE:
                    cout << *(double *)(i[j]);
                    break;
                case CHAR_TYPE:
                    char tmpC[256];
                    for (int k = 0; k < columns[j].charLength; k++)
                    {
                        tmpC[k] = i[j][k];
                    }
                    cout << tmpC;
                    break;
                }
            }
            cout << '|' << endl;
        }
        cout << setfill('_') << setw((11) * columns.size() + 1);
        cout << '\n';
    }
    void deleteOne(char *tuplePtr) //delete one tuple
    {
        (*(int *)ptr)--; //update the number of tuples
        *(int *)(tuplePtr - sizeof(bool) - sizeof(int)) =
            *(int *)(ptr + sizeof(int));            //link the slot
        *(bool *)(tuplePtr - sizeof(bool)) = false; //update the existence
        *(int *)(ptr + sizeof(int)) =
            (tuplePtr - ptr - sizeof(int) - sizeof(bool)); //update the free slot addr
        buffer.writeBlock(blockIndex, ptr);                //write back the data
    }
    void deleteMore(condition &con) //delete with condition
    {
        vector<char *> deleList; // point to the tuple to be deleted
        for (auto &i : tupleVec)
        {
            bool conFlag; //indicate continue
            conFlag = false;
            for (int j = 0; j < columns.size(); j++)
            {
                switch (columns[j].type)
                {
                case INT_TPYE:
                    if (!con.test(to_string(*(int *)(i[j])), j, columns[j].type, true))
                    {
                        conFlag = true;
                    }
                    break;
                case FLOAT_TYPE:
                    if (!con.test(to_string(*(double *)(i[j])), j, columns[j].type, true))
                    {
                        conFlag = true;
                    }
                    break;
                case CHAR_TYPE:
                    if (!con.test(string(i[j]), j, columns[j].type, true))
                    {
                        conFlag = true;
                    }
                    break;
                }
                if (conFlag)
                {
                    break;
                }
            }
            if (!conFlag)
            {
                deleList.push_back(i[0]);
            }
        }
        cout << "Delete " << deleList.size() << " tuples" << endl;
        for (auto &d : deleList)
        {
            deleteOne(d);
        }
    }
};

class tableRecord
{
private:
    template <class T>
    void addToIndex(string indexName, T data, int blockIndex)
    {
        Index<T> *bIndex = new Index<T>(indexName);
        bIndex->InsertKey(data, blockIndex);
    }

public:
    int recordIndex;
    vector<int> tupleIndexVec;
    attrVec columns;
    void create(attrVec &attr)
    {
        columns = attr;
        char *p;
        recordIndex = createRecordBlock(p);
    }
    void extract() //extract all the tupleIndex in the record block
    {
        tupleIndexVec.clear();
        char *ptr;
        ptr = buffer.getBlock(recordIndex);
        int blockNum; //the number of tuple blocks
        blockNum = *(int *)ptr;
        int offset = sizeof(int);
        for (int i = 0; i < blockNum; i++) //extract all the index of tuple Block
        {
            tupleIndexVec.push_back(*(int *)(ptr + offset));
            offset += sizeof(int);
        }
    }
    void addOneTuple(oneTuple &data) //add one tuple to the record
    {
        int tupleIndex;
        extract();
        if (!tupleIndexVec.empty() && isNotFull(tupleIndexVec.back()))
        {
            tupleIndex = tupleIndexVec.back();
        }
        else
        {
            char *blockPtr;
            int tupleSize;
            tupleSize = 0;
            for (auto &i : columns)
            {
                tupleSize += i.getTypeSize();
            }
            tupleIndex = createTupleBlock(tupleSize, blockPtr);
            addTuple2Record(tupleIndex, recordIndex); //add it to record block
        }
        addTuple2block(data, tupleIndex); //add one tuple to the block
        for (int i = 0; i < columns.size(); i++)
        {
            if (columns[i].isIndex)
            {
                switch (columns[i].type)
                {
                case INT_TPYE:
                    addToIndex(string(columns[i].index_name),
                               *(int *)data.dataPtr[i], tupleIndex);
                    break;
                case FLOAT_TYPE:
                    addToIndex(string(columns[i].index_name),
                               *(double *)data.dataPtr[i], tupleIndex);
                    break;
                case CHAR_TYPE:
                    addToIndex(string(columns[i].index_name),
                               string(data.dataPtr[i]), tupleIndex);
                    break;

                default:
                    break;
                }
            }
        }
    }
    void display() //display all the tuple in the record
    {
        extract();
        tmpTable tmpT(columns);
        for (auto &i : tupleIndexVec)
        {
            tmpT.tupleVec.clear();
            tmpT.extract(i);
            tmpT.display();
        }
    }
    void drop() //delete all
    {
        extract();
        for (auto &i : tupleIndexVec)
        {
            buffer.DeleteFileBlock(i);
        }
        buffer.DeleteFileBlock(recordIndex);
    }
    void initIndexI(Index<int> &index, int pos) //initialize an index on INT
    {
        extract();
        tmpTable tmpT(columns);
        for (auto &i : tupleIndexVec)
        {
            tmpT.tupleVec.clear();
            tmpT.extract(i);
            tmpT.initIndexI(index, pos);
        }
    }
    void initIndexD(Index<double> &index, int pos) //initialize an index on FLOAT
    {
        extract();
        tmpTable tmpT(columns);
        for (auto &i : tupleIndexVec)
        {
            tmpT.tupleVec.clear();
            tmpT.extract(i);
            tmpT.initIndexD(index, pos);
        }
    }
    void initIndexS(Index<string> &index, int pos) //intialize an index on CHAR
    {
        extract();
        tmpTable tmpT(columns);
        for (auto &i : tupleIndexVec)
        {
            tmpT.tupleVec.clear();
            tmpT.extract(i);
            tmpT.initIndexS(index, pos);
        }
    }
    void select(condition &con) //select
    {
        bool indexFlag = 0; //indicate to use the index or not
        set<int> blockSet;
        for (auto &c : con.condVec)
        {
            if (c.op == "=" && columns[c.attrPos].isIndex)
            {
                if (columns[c.attrPos].type == INT_TPYE)
                {
                    Index<int> *bIndex =
                        new Index<int>(string(columns[c.attrPos].index_name));
                    if (bIndex->FindKey(stoi(c.a)) == -1)
                    {
                        continue;
                    }
                    indexFlag = 1;
                    blockSet.insert(bIndex->FindKey(stoi(c.a)));
                    delete bIndex;
                }
                else if (columns[c.attrPos].type == FLOAT_TYPE)
                {
                    Index<double> *dIndex =
                        new Index<double>(string(columns[c.attrPos].index_name));
                    if (dIndex->FindKey(stod(c.a)) == -1)
                    {
                        continue;
                    }
                    indexFlag = 1;
                    blockSet.insert(dIndex->FindKey(stod(c.a)));
                    delete dIndex;
                }
                else if (columns[c.attrPos].type == CHAR_TYPE)
                {
                    Index<string> *sIndex =
                        new Index<string>(string(columns[c.attrPos].index_name));
                    if (sIndex->FindKey(c.a) == -1)
                    {
                        continue;
                    }
                    indexFlag = 1;
                    blockSet.insert(sIndex->FindKey(c.a));
                    delete sIndex;
                }
            }
        }

        extract();
        if (indexFlag)
        {
            tmpTable tmpT(columns);
            for (auto &i : blockSet)
            {
                cout << "hi" << endl;
                cout << i << endl;
                tmpT.tupleVec.clear();
                tmpT.extract(i);
                tmpT.select(con);
            }
        }
        else
        {
            tmpTable tmpT(columns);
            for (auto &i : tupleIndexVec)
            {
                tmpT.tupleVec.clear();
                tmpT.extract(i);
                tmpT.select(con);
            }
        }
    }
    void dele(condition &con) //delete
    {
        extract();
        tmpTable tmpT(columns);
        for (auto &i : tupleIndexVec)
        {
            tmpT.tupleVec.clear();
            tmpT.extract(i);
            tmpT.deleteMore(con);
        }
    }
};

class primeRecord
{
private:
    vector<int> recordIndexVec;
    bool isRecordFull(int recordIndex)
    {
        char *ptr;
        ptr = buffer.getBlock(recordIndex);
        int num; //number of the tuple index
        num = *(int *)ptr;
        return (num > 250) ? true : false;
    }

public:
    attrVec columns;
    int index;
    void create(attrVec &attr) //create a new prime record
    {
        columns = attr;
        char *ptr;
        index = createRecordBlock(ptr);
        tableRecord record;
        record.create(attr);
        addTuple2Record(record.recordIndex, index); //add the first record index
    }
    void init(attrVec &attr, int i) //initiliaze the data from block index
    {
        columns = attr;
        index = i;
    }
    void extract() //extract all the recordIndex in the primeRecord block
    {
        recordIndexVec.clear();
        char *ptr;
        ptr = buffer.getBlock(index);
        int blockNum; //the number of record blocks
        blockNum = *(int *)ptr;
        int offset = sizeof(int);
        for (int i = 0; i < blockNum; i++) //extract all the index of record Block
        {
            recordIndexVec.push_back(*(int *)(ptr + offset));
            offset += sizeof(int);
        }
    }
    void addOneTuple(oneTuple &data) //add one tuple
    {
        extract();
        if (isRecordFull(recordIndexVec.back()))
        {
            char *ptr;
            tableRecord record;
            record.create(data.columns); //create new record block
            addTuple2Record(record.recordIndex, index);
            record.addOneTuple(data);
        }
        else
        {
            tableRecord record;
            record.columns = columns;
            record.recordIndex = recordIndexVec.back();
            record.addOneTuple(data);
        }
    }
    void display() //display all the data
    {
        extract();
        tableRecord record;
        record.columns = columns;
        for (auto &i : recordIndexVec)
        {
            record.recordIndex = i;
            record.display();
        }
    }
    void drop() //delete all
    {
        extract();
        tableRecord record;
        record.columns = columns;
        for (auto &i : recordIndexVec)
        {
            record.recordIndex = i;
            record.drop();
        }
        buffer.DeleteFileBlock(index);
    }
    void initIndexI(Index<int> &index, int pos) //initialize an index on INT
    {
        tableRecord record;
        record.columns = columns;
        for (auto &i : recordIndexVec)
        {
            record.recordIndex = i;
            record.initIndexI(index, pos);
        }
    }
    void initIndexD(Index<double> &index, int pos) //initialize an index on DOUBLE
    {
        tableRecord record;
        record.columns = columns;
        for (auto &i : recordIndexVec)
        {
            record.recordIndex = i;
            record.initIndexD(index, pos);
        }
    }
    void initIndexS(Index<string> &index, int pos) //intiliaze an index on CHAR
    {
        tableRecord record;
        record.columns = columns;
        for (auto &i : recordIndexVec)
        {
            record.recordIndex = i;
            record.initIndexS(index, pos);
        }
    }
    void select(condition &con) //select
    {
        extract();
        tableRecord record;
        record.columns = columns;
        for (auto &i : recordIndexVec)
        {
            record.recordIndex = i;
            record.select(con);
        }
    }
    void dele(condition &con) //delete
    {
        extract();
        tableRecord record;
        record.columns = columns;
        for (auto &i : recordIndexVec)
        {
            record.recordIndex = i;
            record.dele(con);
        }
    }
};

class myTable
{
private:
    void initIndexI(Index<int> &index, int pos)
    {
        pRecord.initIndexI(index, pos);
    }
    void initIndexD(Index<double> &index, int pos)
    {
        pRecord.initIndexD(index, pos);
    }
    void initIndexS(Index<string> &index, int pos)
    {
        pRecord.initIndexS(index, pos);
    }

public:
    primeRecord pRecord;
    tableCatalog catalog;
    myTable() {}
    myTable(attrVec &attr)
    {
        pRecord.create(attr);
        catalog.columns = attr;
    }
    myTable(int catIndex, int pRIndex)
    {
        catalog.catalogIndex = catIndex;
        catalog.extract();
        pRecord.init(catalog.columns, pRIndex);
    }
    void addOneTuple(oneTuple &data) //insert
    {
        pRecord.addOneTuple(data);
    }
    void display() //display all tuples
    {
        catalog.display();
        pRecord.display();
    }
    void drop() //drop table
    {
        pRecord.drop();
        catalog.drop();
    }
    void select(condition &con) //select
    {
        catalog.display();
        pRecord.select(con);
    }
    void dele(condition &con) //delete
    {
        pRecord.dele(con);
    }
    void dropIndex(string indexName, string colName) {} //drop an index
    void createIndex(string indexName, string colName)  //create an index
    {
        int pos;
        pos = catalog.getPos(colName);
        catalog.columns[pos].isIndex = true;
        strcpy(catalog.columns[pos].index_name, colName.c_str());
        catalog.create();
        pRecord.columns = catalog.columns;
        if (catalog.columns[pos].type == INT_TPYE)
        {
            Index<int> *bIndex = new Index<int>(indexName);
            initIndexI(*bIndex, pos);
            delete bIndex;
        }
        else if (catalog.columns[pos].type == FLOAT_TYPE)
        {
            Index<double> *dIndex = new Index<double>(indexName);
            initIndexD(*dIndex, pos);
            delete dIndex;
        }
        else if (catalog.columns[pos].type == CHAR_TYPE)
        {
            Index<string> *sIndex = new Index<string>(indexName);
            initIndexS(*sIndex, pos);
            delete sIndex;
        }
    }
};

#endif