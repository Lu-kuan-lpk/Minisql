#ifndef BPlus_H
#define BPlus_H

#include"TreeNode.h"
#include"BufferManager.h"
using namespace std;

extern BufferManager buffer;
//extern BufferManager buffer;
//------------------------------------------------------------------
//模板类BPlusTree，用于对整个B+树的操作
//由此模块操作TreeNode模块，并且由此模块与buffer_manager模块进行交互
//完成文件的读写操作
template <typename T>
class BPlusTree {
private:
    string IndexName;
    //表示总信息储存的地方
    int wholemessage;
    //vector<int> leafid;
    int firstleaf;
	// for a special type 
    typedef TreeNode<T>* Tree;

	//find the position of the node which we find by the FindKey function
    struct Nodemin {
        Tree Node; //包含对应key的结点指针
        int index; //key在结点中的index
        bool Found; //是否找到该key
    };

public:
	//the file to store the tree
    string FileName;
	//pointer of root
    Tree Root;
	//pointer of leafHead
    Tree leafHead;
	//num of key
    unsigned int KeyNum;
	//height of level
    unsigned int level;
	//number of root
    unsigned int NodeNum;
    //fileNode* file; // the filenode of this tree
	//每个key值的size，对于一颗树来说，所有key的size应是相同的
    int KeySize;
	//树的度
    int degree;

public:
	//constructor
    BPlusTree(string IndexName,int key_size, int degree);
	//destructor
    ~BPlusTree();

	//return value for a special key
    int searchVal(T &key);

	//insert (key,value) in tree
    bool InsertKey(T &key, int val);

	//delete key
    bool deleteKey(T &key);

	//drop the whole tree
    void dropTree(Tree node);
    
    //write the data of the whole tree to the disk
    void WriteToDisk();
    
    //write a string to the pointer p
    void  WriteString(char *p,int &index, T s);
    
    //read the data of one block to the tree
    void ReadFromDisk();
    
    void ReadBlockDisk();

    void WriteBlockDisk();

    void printleaf();

private:
	//init the root 
    void TreeInit();

	//adjust the tree after insert the key
    bool AdjustInsert(Tree Node);
	
    //adjust the tree after delete the key
    bool AdjustDelete(Tree pNode);
	
    //find the leaf which has a specified key 
    //and return a structure of Nodemin containing the message of the node
    void FindLeaf(Tree Node, T key, Nodemin &snp);

    void ReadWholeMessage();
};


#include"BPlusTree.h"

template<class T>
BPlusTree<T>::BPlusTree(string IndexName,int KeySize, int degree):
Root(NULL),KeySize(KeySize),leafHead(NULL),degree(degree),IndexName(IndexName),firstleaf(-1){
    TreeInit();
}

//read the data of the tree and insert to the new tree
template <class T>
void BPlusTree<T>::ReadFromDisk(){

	T data;
	int value;
	char *p;  //the pointer to the block
	int i;
    int nextleaf;
    int nowleaf=this->firstleaf;
    
    if(nowleaf==-1) return ;

    p=buffer.getBlock(this->firstleaf);
    for(int k=0;;k++){
        char s[100];
        for(i=0;i<BlockSize;i++){
            if(p[i]!='*') break;
            else{
                i=i+2;
                int j;
                for(j=0;i<BlockSize;i++,j++){
                    if(p[i]==' ') break;
                    s[j]=p[i];
                }s[j]='\0';
                string S(s);
                stringstream stream(S);
                stream>>data;
                
                i++;
                memset(s,0,sizeof(s));
                
                for(j=0;i<BlockSize;i++,j++){
                    if(p[i]==' ') break;
                    s[j]=p[i];
                }s[j]='\0';
                string SS(s);
                stringstream streamm(SS);
                streamm>>value;
                
                this->InsertKey(data,value);
            }
        }
        //
        i++;
        int j;
        for(j=0;i<BlockSize;i++,j++){
            if(p[i]=='\r'||p[i]==' '||p[i]=='N') break;
            s[j]=p[i];
        }s[j]='\0';
        buffer.DeleteFileBlock(nowleaf);
        if(j==0) break;
        string S(s);
        stringstream stream(S);
        stream>>nextleaf;

        p=buffer.getBlock(nextleaf);
    }	
	
}

//find the blockid of this index by the indexname
//we store the message by the format of 
// * IndexName the first blockid
template<class T>
void BPlusTree<T>::ReadBlockDisk(){
    char *p;
    int tmp=1; //store the message of the whole Index
    string name;
    int id;
    //this->wholemessage=buffer.getNewBlock();
    
    for(int i=0;i<BlockSize;i++){
        p=buffer.getBlock(this->wholemessage);

        char s[100];
		if(p[i]!='*') return;
		else{
			i=i+2;
			int j;
			for(j=0;i<BlockSize;i++,j++){
				if(p[i]==' ') break;
				s[j]=p[i];
			}s[j]='\0';
			string S(s);
			stringstream stream(S);
			stream>>name;
            
            i++;
			memset(s,0,sizeof(s));
			
			for(j=0;i<BlockSize;i++,j++){
				if(p[i]==' ') break;
				s[j]=p[i];
			}s[j]='\0';
			string SS(s);
			stringstream streamm(SS);
			streamm>>id;

			if(name.compare(this->IndexName)==0){
                this->firstleaf=id;
            }
		}
    }
}

template <class T>
void  BPlusTree<T>::WriteString(char *p,int &index, T s){
	stringstream ss;
	//change the type of data to the type of string
	ss<<s;
	string S=ss.str();
	//copy the string to the pointer
	for(int i=0;i<S.length();i++){
		p[index++]=S[i];
	}
}


template<class T>
void BPlusTree<T>::ReadWholeMessage(){
    ifstream ifs;
    //记录全局的文件的存放位置，开始的时候存放-1然后将wholemessage进行写入的操作
    //之后在每次读取的时候就进行读取就可以得到wholemessage的地址
    ifs.open("store\\a.txt"); 
    ifs>>this->wholemessage;
    ifs.close();

    char *p;
    if(this->wholemessage==-1){
        this->wholemessage=buffer.getNewBlock(p);
    }

    ofstream ofs;
    ofs.open("store\\a.txt");
    remove("store\\a.txt");
    ofs<<this->wholemessage;
    ofs.close();

    return;
}



template<class T>
void BPlusTree<T>::TreeInit(){
    //buffer.init();
    this->Root=new TreeNode<T>(degree,1);
    char *p;
    this->ReadWholeMessage();
    //this->wholemessage=buffer.getNewBlock(p);
    //this->KeySize=0;
    this->level=1;
    this->NodeNum=1;
    this->leafHead=this->Root;
    this->ReadBlockDisk();
    this->ReadFromDisk();
}

// template<class T>
// void BPlusTree<T>::TreeInit()


template <class T>
void BPlusTree<T>::FindLeaf(Tree Node, T key, Nodemin &nodem){
    int index=0;
    bool find=Node->FindKey(key,index);
    //findkey would find the position which the positio- key is bigger than key
    if(find){ //find the key
        if(Node->isLeaf){
            nodem.Node=Node;
            nodem.index=index;
            nodem.Found=true;
        }
        else{// not the leaf node
            Node=Node->childs[index+1];
            while(Node->isLeaf==0){
                Node=Node->childs[0];
            }

            nodem.Node=Node;
            nodem.index=0;
            nodem.Found=true;
        }
    }else{
        if(Node->isLeaf==1){
            nodem.Node=Node;
            nodem.index=index;
            nodem.Found=false;
        }
        else{
            FindLeaf(Node->childs[index],key,nodem);
        }
    }
    return ;
}

template<class T>
bool BPlusTree<T>::InsertKey(T &key, int val){
    Nodemin nodem;
    if(this->Root==NULL) this->TreeInit();
    
    
    this->FindLeaf(Root,key,nodem);

    if(nodem.Found) return false;
    else{  //this key is not here and allow to insert
        nodem.Node->addKey(key,val);

        if(nodem.Node->KeyNum==degree){
            AdjustInsert(nodem.Node);
        }

        this->KeyNum++;
        return true;
    }
    return false;
}

//if the node is full then adjust the node
template <class T>
bool BPlusTree<T>::AdjustInsert(Tree Node){
    T extrakey;
    Tree newnode=Node->SplitNode(extrakey); //key has the value of the higer level
	this->NodeNum++;
	
    if(Node->Root()){
        Tree root= new TreeNode<T>(degree,false);

        this->level++;
        //this->NodeNum++;
        this->Root=root;
 		
        Node->parent=root;
        newnode->parent=root;
		
		root->addKey(extrakey);
		root->childs[0]=Node;
        root->childs[1]=newnode;
		return true;
    }
    else{// not the root pointer
        Tree pa=Node->parent;
        int index=pa->addKey(extrakey);

        pa->childs[index+1]=newnode;
        newnode->parent=pa;

        if(pa->KeyNum==degree) return AdjustInsert(pa);
        return true;
    }
    return false;
}


template <class T>
int BPlusTree<T>::searchVal(T &key){
    if(this->Root==NULL) return -1;
    Nodemin nodem;

    this->FindLeaf(this->Root,key,nodem);
    if(nodem.Found==1) return nodem.Node->vals[nodem.index];
    else return -1;
}

template <class T>
bool BPlusTree<T>::deleteKey(T &key){
    Nodemin nodem;
    this->FindLeaf(this->Root,key,nodem);

    if(nodem.Node->Root()){
        this->KeyNum--;
        nodem.Node->deleteKeyByIndex(nodem.index);
        bool ans=this->AdjustDelete(nodem.Node);
        return ans;
    }else{
        if(nodem.index==0&&this->leafHead!=nodem.Node){
            //key is in the branch and need to update the higher
            int index;

            Tree newpa=nodem.Node->parent;
            // find the higher level which contain the key
            bool branch=newpa->FindKey(key,index);
            while(branch==false){
                if(newpa->parent) newpa=newpa->parent;
                else break;

                branch=newpa->FindKey(key,index);
            }

            newpa->keys[index]=nodem.Node->keys[1];

            nodem.Node->deleteKeyByIndex(nodem.index);
            this->KeyNum--;
            bool ans=this->AdjustDelete(nodem.Node);
            return ans;
        }else{  //only in the leaf node
            nodem.Node->deleteKeyByIndex(nodem.index);
            this->KeyNum--;
            bool ans=this->AdjustDelete(nodem.Node);
            return ans;
        }
    }
}


template <class T>
bool BPlusTree<T>::AdjustDelete(Tree Node){

    int Minnum = (degree - 1) / 2;
	//三种不需要调整的情况
	//	1. the leaf and the num is larger than the minnum
	//	2. not the leaf and the num is larger than the minnum
	//	3. degree==3 ,not the leaf and the num is less than zero 
    if (((Node->isLeaf) && (Node->KeyNum >= Minnum)) ||((degree != 3) && (!Node->isLeaf) && (Node->KeyNum >= Minnum - 1)) ||((degree == 3) && (!Node->isLeaf) && (Node->KeyNum < 0))) {
        return  true;
    }
    if (Node->Root()) { //当前结点为根结点
        if (Node->KeyNum > 0) //不需要调整
            return true;
        else { //正常需要调整
            if (this->Root->isLeaf) { //将成为空树情况
                delete Node;
                this->Root = NULL;
                this->leafHead = NULL;
                this->level--;
                this->NodeNum--;
            }
            else { //根节点将成为左头部
                Root = Node->childs[0];
                Root->parent = NULL;
                delete Node;
                this->level--;
                this->NodeNum--;
            }
        }
    } else { //非根节点情况
        Tree pa= Node->parent, bro= NULL;
        if (Node->isLeaf) { //当前为叶节点
            int index = 0;
            pa->FindKey(Node->keys[0], index);

			//选择左兄弟
            if ((pa->childs[0] !=Node) && (index + 1 == pa->KeyNum)) {
                bro = pa->childs[index];
                if(bro->KeyNum > Minnum) {
                    for (int i = Node->KeyNum; i > 0; i--) {
                        Node->keys[i] = Node->keys[i-1];
                        Node->vals[i] = Node->vals[i-1];
                    }
                    Node->keys[0] = bro->keys[bro->KeyNum-1];
                    Node->vals[0] = bro->vals[bro->KeyNum-1];
                    bro->deleteKeyByIndex(bro->KeyNum-1);

                    Node->KeyNum++;
                    pa->keys[index] = Node->keys[0];
                    return true;

                } else {
                    pa->deleteKeyByIndex(index);

                    for (int i = 0; i < Node->KeyNum; i++) {
                        bro->keys[i+bro->KeyNum] = Node->keys[i];
                        bro->vals[i+bro->KeyNum] = Node->vals[i];
                    }
                    bro->KeyNum += Node->KeyNum;
                    bro->NextLeafNode = Node->NextLeafNode;

                    delete Node;
                    this->NodeNum--;

                    return AdjustDelete(pa);
                }

            } else {
                if(pa->childs[0] == Node)
                    bro= pa->childs[1];
                else
                    bro = pa->childs[index+2];
                if(bro->KeyNum > Minnum) {
                    Node->keys[Node->KeyNum] = bro->keys[0];
                    Node->vals[Node->KeyNum] = bro->vals[0];
                    Node->KeyNum++;
                    bro->deleteKeyByIndex(0);
                    if(pa->childs[0] == Node)
                        pa->keys[0] = bro->keys[0];
                    else
                        pa->keys[index+1] = bro->keys[0];
                    return true;

                } else {
                    for (int i = 0; i < bro->KeyNum; i++) {
                        Node->keys[Node->KeyNum+i] = bro->keys[i];
                        Node->vals[Node->KeyNum+i] = bro->vals[i];
                    }
                    if (Node == pa->childs[0])
                        pa->deleteKeyByIndex(0);
                    else
                        pa->deleteKeyByIndex(index+1);
                    Node->KeyNum+= bro->KeyNum;
                    Node->NextLeafNode = bro->NextLeafNode;
                    delete bro;
                    this->NodeNum--;

                    return AdjustDelete(pa);
                }
            }

        } else { //枝干节点情况
            int index = 0;
            pa->FindKey(Node->childs[0]->keys[0], index);
            if ((pa->childs[0] != Node) && (index + 1 == pa->KeyNum)) {
                bro = pa->childs[index];
                if (bro->KeyNum > Minnum - 1) {
                    Node->childs[Node->KeyNum+1] = Node->childs[Node->KeyNum];
                    for (int i = Node->KeyNum; i > 0; i--) {
                        Node->childs[i] = Node->childs[i-1];
                        Node->keys[i] = Node->keys[i-1];
                    }
                    Node->childs[0] = bro->childs[bro->KeyNum];
                    Node->keys[0] = pa->keys[index];
                    Node->KeyNum++;

                    pa->keys[index]= bro->keys[bro->KeyNum-1];

                    if (bro->childs[bro->KeyNum])
                        bro->childs[bro->KeyNum]->parent= Node;
                    bro->deleteKeyByIndex(bro->KeyNum-1);

                    return true;

                } else {
                    bro->keys[bro->KeyNum] = pa->keys[index];
                    pa->deleteKeyByIndex(index);
                    bro->KeyNum++;

                    for (int i = 0; i < Node->KeyNum; i++) {
                        bro->childs[bro->KeyNum+i] =Node->childs[i];
                        bro->keys[bro->KeyNum+i] = Node->keys[i];
                        bro->childs[bro->KeyNum+i]->parent= bro;
                    }
                    bro->childs[bro->KeyNum+Node->KeyNum] = Node->childs[Node->KeyNum];
                    bro->childs[bro->KeyNum+Node->KeyNum]->parent = bro;

                    bro->KeyNum += Node->KeyNum;

                    delete Node;
                    this->NodeNum--;

                    return AdjustDelete(pa);
                }

            } else {
                if (pa->childs[0] == Node)
                    bro = pa->childs[1];
                else
                    bro = pa->childs[index+2];
                if (bro->KeyNum > Minnum - 1) {

                    Node->childs[Node->KeyNum+1] = bro->childs[0];
                    Node->keys[Node->KeyNum] = bro->keys[0];
                    Node->childs[Node->KeyNum+1]->parent =Node;
                    Node->KeyNum++;

                    if (Node == pa->childs[0])
                        pa->keys[0] = bro->keys[0];
                    else
                        pa->keys[index+1] = bro->keys[0];

                    bro->childs[0] = bro->childs[1];
                    bro->deleteKeyByIndex(0);

                    return true;
                } else {

                    Node->keys[Node->KeyNum] = pa->keys[index];

                    if(Node == pa->childs[0])
                        pa->deleteKeyByIndex(0);
                    else
                        pa->deleteKeyByIndex(index+1);

                    Node->KeyNum++;

                    for (int i = 0; i < bro->KeyNum; i++) {
                        Node->childs[Node->KeyNum+i] = bro->childs[i];
                        Node->keys[Node->KeyNum+i] = bro->keys[i];
                        Node->childs[Node->KeyNum+i]->parent = Node;
                    }
                    Node->childs[Node->KeyNum+bro->KeyNum] = bro->childs[bro->KeyNum];
                    Node->childs[Node->KeyNum+bro->KeyNum]->parent = Node;

                    Node->KeyNum += bro->KeyNum;

                    delete bro;
                    this->NodeNum--;

                    return AdjustDelete(pa);
                }

            }

        }

    }

    return false;
}


//for checking the correctnes of the tree
template<class T>
void BPlusTree<T>::printleaf(){
    Tree tmp=this->leafHead;
    while(tmp){
        tmp->printl();
        tmp=tmp->nextLeaf();
    }
    return ;
}



template<class T>
void BPlusTree<T>:: dropTree(Tree node){
    if(node==NULL) return;
    if(node->isLeaf==0){
        for(int i=0;i<node->KeyNum;i++){
            this->dropTree(node->childs[i]);
            node->childs[i]=NULL;
        }
    }
    
    //node is the leaf
    delete node;
    this->NodeNum--;
    return;
}

template<class T>
BPlusTree<T>:: ~BPlusTree(){
    this->WriteToDisk();
    this->WriteBlockDisk();
    this->dropTree(this->Root);
    this->KeyNum=0;
    this->Root=NULL;
    this->level=0;
}

//we store the data by the form of 
// * indexname number_of_firstblock
template <class T>
void BPlusTree<T>::WriteBlockDisk(){
    char *p;
    char *newp= new char[BlockSize];
    int tmp=1; //store the message of the whole Index
    string name;
    string num;
    vector<string> iname; //record the whole message of each block 
    vector<string> inum;
    int flag=0;
    iname.clear();
    inum.clear();
    p=buffer.getBlock(this->wholemessage);

    for(int i=0;i<BlockSize;i++){
        

        char s[100];
		if(p[i]!='*'&&flag==1) break;
        else if(p[i]!='*'&&flag==0){}
		else{
			i=i+2;
			int j;
			for(j=0;i<BlockSize;i++,j++){
				if(p[i]==' ') break;
				s[j]=p[i];
			}s[j]='\0';
			string S(s);
			stringstream stream(S);
			stream>>name;
            iname.push_back(name);			
            
            i++;
			memset(s,0,sizeof(s));
			for(j=0;i<BlockSize;i++,j++){
				if(p[i]==' ') break;
				s[j]=p[i];
			}s[j]='\0';
			string SS(s);
			stringstream streamm(SS);
			streamm>>num;
            if(name.compare(this->IndexName)==0){
                flag=1;
                stringstream ss;
                ss<<this->firstleaf;
                num=ss.str();
            }
			inum.push_back(num);
		}
    }

    if(flag==0){
        iname.push_back(this->IndexName);
    	stringstream stream;
		stream<<this->firstleaf;
        inum.push_back(stream.str());
        flag=1;
    }

    //write the new data
    int index=0;
	memset(newp,0,BlockSize);
	for(int j=0;j<iname.size();j++){
		newp[index++]='*';
		newp[index++]=' ';
		//this->WriteString(newp,index,iname[j]);
        for(int k=0;k<iname[j].size();k++) newp[index++]=iname[j][k];
		newp[index++]=' ';
		//this->WriteString<std::string>(newp,index,inum[j]);
        for(int k=0;k<inum[j].size();k++) newp[index++]=inum[j][k];
		newp[index++]=' ';
		}
	newp[index]='\0';

    buffer.writeBlock(this->wholemessage,newp);

    //buffer.close();
}

// we set the size of the block to 4kb which equal to the 40000
//write each node of the tree to the buffer
template <class T>
void BPlusTree<T>::WriteToDisk(){
	//FILE *p=fopen(file_name,"w+");
	//sequencially transform each point
	Tree tmp=this->leafHead;
    char *newp;
    int nextleaf;
    char *p;
    int thisleaf=buffer.getNewBlock(p); //p point to the new space
	this->firstleaf=thisleaf;

	for(int i=0;tmp!=NULL;i++){
        //char *p=getBlock(blockid+i);
        if(i!=0){
            p=newp;
            thisleaf=nextleaf;
        }
        memset(p,0,sizeof(p));
        //this->leafid.push_back(thisleaf);
        int index=0;
		//memset(p,0,BlockSize);
        stringstream S;
		for(int j=0;j<tmp->KeyNum;j++){
			p[index++]='*';
			p[index++]=' ';
            stringstream S,SS;
			S<<tmp->keys[j];
            string skey=S.str();
            for(int k=0;k<skey.size();k++) p[index++]=skey[k];
			//this->WriteString(p,index,tmp->keys[j]);
			p[index++]=' ';
            SS<<tmp->vals[j];
            string sval=SS.str();
            for(int k=0;k<sval.size();k++) p[index++]=sval[k];
			//this->WriteString(p,index,tmp->vals[j]);
			p[index++]=' ';
		}
        p[index++]='#';
        if(tmp->nextLeaf()){
            int nextleaf=buffer.getNewBlock(newp);
            stringstream S;
            S<<nextleaf;
            string sleaf=S.str();
            for(int k=0;k<sleaf.size();k++) p[index++]=sleaf[k];
            //this->WriteString(p,index,nextleaf);
        }else{
            p[index++]='N';
        }
        p[index++]=' ';
		p[index]='\0';
		
        buffer.writeBlock(thisleaf,p);
        //writeBlock(blockid+i,p);
		tmp=tmp->nextLeaf();
        //cout<<i<<endl;
	}

    //buffer.close();
	
}


#endif