#include"BplusTree.h"
#include<iostream>
using namespace std;

template<class T>
class Index{
    public:
        //constructor
        Index(string name);
        //destructor
        ~Index();
        //Insert key and the blockid
        bool InsertKey(T key,int blockid);
        //delete a key
        bool DeleteKey(T key);
        //find the block id of a key
        int FindKey(T key);
        //get the degree of the B+ tree
        int degree();
    private:
        BPlusTree<T> *tree;
        string index_name;

};

template<class T>
Index<T>::Index(string name){
    int degree=this->degree();
    tree=new BPlusTree<T>(name,sizeof(T),degree);
}

//use the default function
template <class T>
Index<T>::~Index(){
    delete tree;
}

template<class T>
int Index<T>::degree(){
    int degree=(BlockSize)/(sizeof(int)+sizeof(T));
    //we confirm that degree is odd number for the B+ tree
    if(degree%2==0) degree=degree-1;
    return degree;
}

template<class T>
bool Index<T>::InsertKey(T key,int blockid){
    return tree->InsertKey(key,blockid);
}

template <class T>
bool Index<T>::DeleteKey(T key){
    return tree->deleteKey(key);
}

template <class T>
int Index<T>::FindKey(T key){
    return tree->searchVal(key);
}