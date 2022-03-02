#ifndef _INIT_
#define _INIT_

#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

//return the Init Block Index, if doesn't exist, return -1
int getInitBlockIndex();

//update the Init Block Index
void updateInitBlockIndex(int index);

#endif