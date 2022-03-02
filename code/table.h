#ifndef _TABLE_
#define _TABLE_
#include <vector>
#include <cstring>
#include "BufferManager.h"

using namespace std;

enum attrType
{
    INT_TPYE,
    FLOAT_TYPE,
    CHAR_TYPE
};
class attribute
{
public:
    char name[40]; //the name of the attribute
    attrType type;
    int charLength; //the length of the char
    bool isPrimary;
    bool isUnique;
    bool isIndex;
    char index_name[30]; //the name of the index
    attribute(const char n[],
              attrType t, int cl,
              bool p, bool u, bool i, const char in[])
    {
        strcpy(name, n);
        type = t;
        charLength = cl;
        isPrimary = p;
        isUnique = u;
        isIndex = i;
        strcpy(index_name, in);
    }
    int getTypeSize() //get the size of corresponding data
    {
        switch (type)
        {
        case INT_TPYE:
            return sizeof(int);
            break;
        case FLOAT_TYPE:
            return sizeof(double);
            break;
        case CHAR_TYPE:
            return sizeof(char) * charLength;
            break;
        default:
            break;
        }
        return -1;
    }
};
typedef vector<attribute> attrVec;

typedef vector<char *> paleTuple;

class oneTuple
{
public:
    paleTuple dataPtr;
    attrVec columns;
    int getTupleSize() //return the size of the tuple
    {
        int tupleSize = 0;
        for (auto &i : columns)
        {
            tupleSize += i.getTypeSize();
        }
        return tupleSize;
    }
};

class oneCondi
{
public:
    int attrPos; //the relative postion of the attribute
    string op;   //the operator
    string a;    //operand
    oneCondi(int pos, string op, string a) : attrPos(pos), op(op), a(a) {}
    bool compare(string &b, attrType type) //compare with data b, return true if pass
    {
        switch (type)
        {
        case INT_TPYE:
            int ia, ib;
            ia = stoi(a);
            ib = stoi(b);
            if (op == ">")
            {
                return ib > ia;
            }
            else if (op == "<")
            {
                return ib < ia;
            }
            else if (op == "=")
            {
                return ib == ia;
            }
            else if (op == "!=")
            {
                return ib != ia;
            }
            else if (op == ">=")
            {
                return ib >= ia;
            }
            else if (op == "<=")
            {
                return ib <= ia;
            }
            break;
        case FLOAT_TYPE:
            double da, db;
            da = stod(a);
            db = stod(b);
            if (op == ">")
            {
                return db > da;
            }
            else if (op == "<")
            {
                return db < da;
            }
            else if (op == "=")
            {
                return db == da;
            }
            else if (op == "!=")
            {
                return db != da;
            }
            else if (op == ">=")
            {
                return db >= da;
            }
            else if (op == "<=")
            {
                return db <= da;
            }
            break;
        case CHAR_TYPE:
            if (op == ">")
            {
                return b > a;
            }
            else if (op == "<")
            {
                return b < a;
            }
            else if (op == "=")
            {
                return b == a;
            }
            else if (op == "!=")
            {
                return b != a;
            }
            else if (op == ">=")
            {
                return b >= a;
            }
            else if (op == "<=")
            {
                return b <= a;
            }
            break;

        default:
            break;
        }
        return false;
    }
};

class condition
{
public:
    vector<oneCondi> condVec;
    bool test(string b, int pos, attrType type, bool def) //test if the tuple is ok
    {
        for (auto &i : condVec)
        {
            if (i.attrPos == pos)
            {
                return i.compare(b, type);
            }
        }
        return def;
    }
};

#endif