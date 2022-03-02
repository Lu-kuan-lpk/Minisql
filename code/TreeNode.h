#ifndef TreeNode_H
#define TreeNode_H

#include<vector>
#include<string>
#include<iostream>
#include<sstream>
#include<string.h>
#define BlockSize 4000
//#include"TreeNode.h"
using namespace std;
//extern BufferManager buffermanager;


// a node of the bplus tree
template <typename T>
class TreeNode {
public:
	//the num of key in the node
    unsigned int KeyNum;
	//pointer to the parent
    TreeNode* parent;
	//vector for key
    vector <T> keys;
	//pointers to the childs
    vector <TreeNode*> childs;
    //vector for vals
    vector <int> vals;
	//pointers for next leaf
    TreeNode* NextLeafNode;
    //0 not ,1 is
    int isLeaf;
	//determine the degree of the tree
    int degree;

public:
	//
    TreeNode(int degree, int Leaf = 0);
	//
    ~TreeNode();
    //judge if it is the root point
    bool Root();
	//find whether the key is in the node, if is then return the index
    //if not find then return the closest point
    bool FindKey(T key,int &index);
    //输出：TreeNode指针
    //功能：将一个结点分裂成两个结点(与B+树的分裂有关)
	//新结点为本结点的下一个结点
    //同时功过key引用返回去到上层的key值
    TreeNode* SplitNode(T& key);
	//for the branch point only add the key
    int addKey(T &key);
	//for the leaf point add the key and value
    int addKey(T &key, int val);
	//delete the key in the index
    bool deleteKeyByIndex(int index);
    //return the pointer of the next leaf
    TreeNode* nextLeaf();

    
    void printl();
};



#include"TreeNode.h"


template<class T>
TreeNode<T>::TreeNode(int degree,int leaf) : KeyNum(0),parent(NULL),NextLeafNode(NULL),isLeaf(leaf),degree(degree){
    childs.resize(degree+1);
    keys.resize(degree+1);
    vals.resize(degree+1);
    for(int i=0;i<degree+1;i++){
        childs[i]=NULL;
        //keys[i]=NULL;
        //vals[i]=NULL;
    }
}

// use the default destructor
template<class T>
TreeNode<T>::~TreeNode(){
}

//return the index of the seq
template<class T>
bool TreeNode<T>::FindKey(T key,int &index){
    if(KeyNum==0){
        index=0; return false;
    }
    else if(keys[KeyNum-1]<key){
        index=KeyNum;
        return false;
    }
    else if(this->keys[0]>key){
        index=0;
        return false;
    }
    else{
        //scan the node sequencially
        for(int i=0;i<KeyNum;i++){
            if(keys[i]==key){
                index=i; return true;
            }
            //in increasing order
            //miss the correct
            else if(keys[i]>key){
                index=i; return false;
            }
            else continue;
        }
    }
    return false;
}

template<class T>
bool TreeNode<T>::Root(){
    return (parent==NULL) ? true:false;
}

template<class T>
TreeNode<T>* TreeNode<T>::nextLeaf(){
    return NextLeafNode;
}

//key return the key to return to the higher level
//confirm the degree is even
// degree key, (degree+1) child
// degree is 奇数
// not the leaf --->  a a
//leaf ---> a+1 a
template<class T>
TreeNode<T>* TreeNode<T>::SplitNode(T& key){
    TreeNode *tmp=new TreeNode(this->degree,this->isLeaf);

    //永远在原来的节点中最小数的节点
    int minnum=(this->degree-1)/2;

    //the leaf situation
    if(isLeaf==0){
        key=this->keys[minnum];


        for(int i=minnum+1;i<degree;i++){
            tmp->keys[i-minnum-1]=this->keys[i];
            this->keys[i]=T();
        }

        for(int i=minnum+1;i<degree+1;i++){
            tmp->childs[i-minnum-1]=this->childs[i];
            //change the parent pointer
            tmp->childs[i-minnum-1]->parent=tmp;
            this->childs[i]=NULL;
        }

        this->keys[minnum]=T(); //delete the key
        tmp->parent=this->parent;

        tmp->KeyNum=minnum;
        this->KeyNum=minnum;
    }
    else{
        int newnum=minnum+1;
        key=this->keys[newnum];
        for(int i=newnum;i<degree;i++){
            tmp->keys[i-newnum]=this->keys[i];
            tmp->vals[i-newnum]=this->vals[i];
            this->keys[i]=T();
            this->vals[i]=int();
        }
        // node relation adjust
        tmp->NextLeafNode=this->NextLeafNode;
        this->NextLeafNode=tmp;
        
		tmp->parent=this->parent;

        this->KeyNum=newnum;
        tmp->KeyNum=minnum;
    }
    return tmp;
}


//return the index of the added point
template<class T>
int TreeNode<T>::addKey(T &key){
    if(this->KeyNum==0){
        this->keys[0]=key;
        this->KeyNum++;
        return 0;
    }else{
        int index=0;
        bool same=this->FindKey(key,index);
        if(same) throw; //present the wrong definition of the primary key of the unique condition
        else{
            for(int i=this->KeyNum;i>index;i--){
                this->keys[i]=this->keys[i-1];
            }
            this->keys[index]=key;

            for(int i=this->KeyNum+1;i>index+1;i--){
                this->childs[i]=this->childs[i-1];
            }
            this->childs[index+1]=NULL;
            this->KeyNum++;
			
			return index;
        }
        
    }

}


//return the position of the added index
template<class T>
int TreeNode<T>::addKey(T &key,int val){
    if(this->KeyNum==0){
        this->keys[0]=key;
        this->vals[0]=val;
        this->KeyNum++;
        return 0;
    }else{
        int index=0;
        bool same=this->FindKey(key,index);
        if(same) throw;
        else{
            for(int i=this->KeyNum ; i>index;i--){
                this->keys[i]=this->keys[i-1];
                this->vals[i]=this->vals[i-1];
            }
            this->keys[index]=key;
            this->vals[index]=val;
            this->KeyNum++;
        }
        return index;
    }
}


//we assume the index is less than the num
template<class T>
bool TreeNode<T>::deleteKeyByIndex(int index){
    if(this->isLeaf==0){
        for(int i=index;i<this->KeyNum-1;i++){
            this->keys[i]=this->keys[i+1];
            this->childs[i+1]=this->childs[i+2];
        }
        this->keys[this->KeyNum-1]=T();
        this->childs[this->KeyNum]=NULL;
    }
    else if(this->isLeaf==1){
        for(int i=index;i<this->KeyNum-1;i++){
            this->keys[i]=this->keys[i+1];
            this->vals[i]=this->vals[i+1];
        }
        this->keys[this->KeyNum-1]=T();
        this->vals[this->KeyNum-1]=int();
    }
    else{}
    this->KeyNum--;
    return true;
}

template <class T>
void TreeNode<T>::printl(){
    for(int i=0;i<this->KeyNum;i++){
        cout<<'('<<this->keys[i]<<','<<this->vals[i]<<")->";
    }
    cout<<endl;
}





#endif