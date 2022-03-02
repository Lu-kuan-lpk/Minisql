#include "interface.h"

static bool initFlag;
static map<string, myTable> tableMap;
static int momIndex;
BufferManager buffer;

attrType getType(string &s)
{
    switch (s[0])
    {
    case 'i':
        return INT_TPYE;
        break;
    case 'f':
        return FLOAT_TYPE;
        break;
    default:
        return CHAR_TYPE;
        break;
    }
}

int getLen(string &s)
{
    if (s[0] != 'c')
    {
        return 0;
    }
    else
    {
        return stoi(s.substr(4));
    }
}

bool getPri(char k)
{
    return k == 'p';
}

bool getUni(char k)
{
    return k == 'u';
}

void interface(SQLCommand &com)
{
    if (initFlag == 0) //initialize the table map
    {
        buffer.init();
        initFlag = 1;
        momIndex = getInitBlockIndex();
        char *ptr;
        if (momIndex == -1)
        {
            momIndex = buffer.getNewBlock(ptr);
            *(int *)ptr = 0;
        }
        ptr = buffer.getBlock(momIndex);
        int tableNum;
        tableNum = *(int *)ptr;
        int offset;
        offset = sizeof(int);
        tableMap.clear();
        for (int i = 0; i < tableNum; i++)
        {
            myTable tmp;
            tmp.catalog.catalogIndex = *(int *)(ptr + offset);
            tmp.catalog.extract();
            tmp.pRecord.init(tmp.catalog.columns, *(int *)(ptr + offset + sizeof(int)));
            tableMap[string(tmp.catalog.name)] = tmp;
            offset += 2 * sizeof(int);
        }
    }
    if (com.command_type == 1) //create the table
    {
        attrVec columns;
        for (int i = 0; i < com.attri_name.size(); i++)
        {
            columns.push_back(
                attribute(com.attri_name[i].c_str(), getType(com.attri_datatype[i]),
                          getLen(com.attri_datatype[i]), getPri(com.attri_keytype[i]),
                          getUni(com.attri_keytype[i]), 0, "0"));
        }
        myTable table(columns);
        strcpy(table.catalog.name, com.table_name.c_str());
        table.catalog.create();
        tableMap[com.table_name] = table;
    }
    else if (com.command_type == 2) //drop the table
    {
        tableMap[com.table_name].drop();
        tableMap.erase(com.table_name); //delete its in the map
    }
    else if (com.command_type == 3) //create index
    {
        tableMap[com.table_name].createIndex(com.index_name, com.col_name);
    }
    else if (com.command_type == 4) //drop the index
    {
        tableMap[com.table_name].dropIndex(com.index_name, com.col_name);
    }
    else if (com.command_type == 5) //select
    {
        if (com.condition_num == 0)
        {
            tableMap[com.table_name].display();
            return;
        }
        condition con;
        for (int i = 0; i < com.condition_num; i++)
        {
            con.condVec.push_back(
                oneCondi(tableMap[com.table_name].catalog.getPos(com.condi_attri_name[i]),
                         com.condi_operator[i], com.condi_data[i]));
        }
        tableMap[com.table_name].select(con);
    }
    else if (com.command_type == 6) //insert tuple
    {
        oneTuple tmpTuple;
        tmpTuple.columns = tableMap[com.table_name].catalog.columns;
        for (int i = 0; i < com.value_num; i++)
        {
            char *ptr;
            ptr = new char[tmpTuple.columns[i].getTypeSize()];
            switch (tmpTuple.columns[i].type)
            {
            case INT_TPYE:
                *(int *)ptr = stoi(com.value[i]);
                break;
            case FLOAT_TYPE:
                *(double *)ptr = stod(com.value[i]);
                break;
            case CHAR_TYPE:
                strcpy(ptr, com.value[i].c_str());
                break;

            default:
                break;
            }
            tmpTuple.dataPtr.push_back(ptr);
        }
        tableMap[com.table_name].addOneTuple(tmpTuple);
    }
    else if (com.command_type == 7) //delete tuple
    {
        condition con;
        for (int i = 0; i < com.condition_num; i++)
        {
            con.condVec.push_back(
                oneCondi(tableMap[com.table_name].catalog.getPos(com.condi_attri_name[i]),
                         com.condi_operator[i], com.condi_data[i]));
        }
        tableMap[com.table_name].dele(con);
    }
    else if (com.command_type == 8) //quit
    {
        updateInitBlockIndex(momIndex);
        char *ptr;
        ptr = buffer.getBlock(momIndex);
        *(int *)ptr = tableMap.size();
        int offset;
        offset = sizeof(int);
        for (auto &i : tableMap)
        {
            *(int *)(ptr + offset) = i.second.catalog.catalogIndex;
            *(int *)(ptr + offset + sizeof(int)) = i.second.pRecord.index;
            offset += sizeof(int) * 2;
        }
        buffer.writeBlock(momIndex, ptr);
        buffer.close();
    }
}