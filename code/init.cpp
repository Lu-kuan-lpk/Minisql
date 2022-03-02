#include "init.h"

//return the Init Block Index, if doesn't exist, return -1
int getInitBlockIndex()
{
    ifstream ifs;
    ifs.open("store\\table.txt", ifstream::in);
    int index;
    ifs >> index;
    ifs.close();
    return index;
}

//update the Init Block Index
void updateInitBlockIndex(int index)
{
    ofstream ofs;
    ofs.open("store\\table.txt", ofstream::out | ofstream::trunc);
    ofs << index;
    ofs.close();
    return;
}

//test part

// int main()
// {
//     cout << getInitBlockIndex() << endl;
// }