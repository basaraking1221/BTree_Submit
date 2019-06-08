#include "utility.hpp"
#include <iostream>
#include <functional>
#include <cstddef>
#include <string>
#include "exception.hpp"
#include <fstream>
#include <cstring>
const  int Mmax=2000;
const  int Lmax=300;
namespace sjtu {
    template <class Key, class Value, class Compare = std::less<Key> >
    class BTree {
    public:
        typedef pair<const Key, Value> value_type;

        class const_iterator;
        class iterator {
            friend class BTree;

        private:
            size_t leafposition;
            size_t pairposition;
            BTree *bplustree;
        public:
            iterator() {
                // TODO Default Constructor
                bplustree=NULL;
                leafposition=0;
                pairposition=0;
            }
            iterator(const iterator& other) {
                // TODO Copy Constructor
                bplustree=other.bplustree;
                leafposition=other.leafposition;
                pairposition=other.pairposition;
            }
            iterator operator++(int) {
                // Todo iterator++

            }
            iterator& operator++() {
                // Todo ++iterator
            }
            iterator operator--(int) {
                // Todo iterator--
            }
            iterator& operator--() {
                // Todo --iterator
            }
            // Overloaded of operator '==' and '!='
            // Check whether the iterators are same
            bool operator==(const iterator& rhs) const {
                // Todo operator ==
            }
            bool operator==(const const_iterator& rhs) const {
                // Todo operator ==
            }
            bool operator!=(const iterator& rhs) const {
                // Todo operator !=
            }
            bool operator!=(const const_iterator& rhs) const {
                // Todo operator !=
            }
        };

        class const_iterator {
            friend class BTree;

        private:
            size_t leafposition;
            size_t pairposition;
            BTree *bplustree;
        public:
            const_iterator() {
                // TODO Default Constructor
                bplustree=NULL;
                leafposition=0;
                pairposition=0;
            }
            const_iterator(const iterator& other) {
                // TODO Copy Constructor
                bplustree=other.bplustree;
                leafposition=other.leafposition;
                pairposition=other.pairposition;
            }
            const_iterator(const const_iterator& other) {
                // TODO Copy Constructor
                bplustree=other.bplustree;
                leafposition=other.leafposition;
                pairposition=other.pairposition;
            }
        };
    private:

        //根节点？也不算吧，那就定名为索引。
        struct indexs{
            ssize_t head;
            ssize_t tail;
            ssize_t root;
            ssize_t  length;
            ssize_t endd;
            indexs(){
                head=0;
                tail=0;
                root=0;
                length=0;
                endd=0;
            }
        };
        //中间节点
        struct midroot{
            ssize_t parent;
            ssize_t children[Mmax+1];
            bool type;
            Key key[Mmax];
            size_t num;
            ssize_t position;
            midroot(){
                parent=0;
                memset(children,0,Mmax+1);
                num=0;
                type= false;
                position=0;
            }

        };
        //叶子节点
        struct leaves{
            ssize_t parent;
            ssize_t prev,next;
            size_t pairnum;
            //你敢信我写到最后了才发现，这个Value_type跟我写的不一样？？？？
            Key datak[Lmax+1];
            Value datav[Lmax+1];
            ssize_t position;
            leaves(){
                parent=0;
                prev=0;
                next=0;
                pairnum=0;
                position=0;
            }
        };
        struct filename{
            char *str;
            filename(){str=new char [300];}
            ~filename(){if(!str) delete str;}
        };//第二次加的，刚搞明白不能用路径来写。。
        FILE *txt;//txt文本
        bool whetheropen=false;//打不打开
        indexs catalogue;//我英语很棒了
        bool whetherexist=false;//竟然还有文件原来已经存在这一说。。
        filename txtname;
    public:
        //进行一些文件操作，本来想直接open等但是不如写成函数来的快--------下面进行第二次调试及更改
        void openfile(){
            whetherexist=true;
            if(whetheropen == false){
                txt=fopen(txtname.str,"rb+");
            }
            if(txt==NULL){
                whetherexist= false;//鬼知道还会打开失败歪日，你试试第一次运行就没打开的心态？
                txt=fopen(txtname.str,"w");
                fclose(txt);
                txt=fopen(txtname.str,"rb+");
            }
            else readfile(&catalogue,0,1, sizeof(indexs));
            whetheropen=true;
        }
        void closefile(){
            if(whetheropen==true)
                fclose(txt);
            whetheropen= false;
        }
        void readfile(void *place,size_t pos,size_t num, size_t size)const{
            fseek(txt,pos,SEEK_SET);
            fread(place,size,num,txt);
        }
        void writefile(void *place,size_t pos,size_t num,size_t size){
            fseek(txt,pos,SEEK_SET);
            fwrite(place,size,num,txt);
        }
        //第一次就写这几个应该够了吧

        void maketree(){
            //先开始操作目录
            catalogue.length=0;
            catalogue.endd= sizeof(indexs);
            //根节点和叶子节点建立
            midroot root;
            leaves leaf;
            //一步步的摆正目录、各个节点的位置（position）
            catalogue.root=root.position=catalogue.endd;
            catalogue.endd+= sizeof(midroot);
            catalogue.head=catalogue.tail=leaf.position=catalogue.endd;
            catalogue.endd+= sizeof(leaves);
            //挨个初始化
            root.parent=0;
            root.num=0;
            root.type=true;
            root.children[0]=leaf.position;
            leaf.parent=root.position;
            leaf.next=leaf.prev=0;
            leaf.pairnum=0;
            //全扔进文件里，写tmd
            writefile(&catalogue,0,1, sizeof(indexs));
            writefile(&root,root.position,1, sizeof(midroot));
            writefile(&leaf,leaf.position,1, sizeof(leaves));
        }
        BTree() {
            txt=NULL;
            openfile();
            if(whetherexist==false)//第二次写添加，所有有关存在的都是，不再赘述
                maketree();
        }
        BTree(const BTree& other) {
            //对不起这个我不会写
        }
        BTree& operator=(const BTree& other) {
            // Todo Assignment
        }
        ~BTree() {
            closefile();
        }

        //貌似查找直接遍历不行。。。所以我们现在在叶子节点操作下
        size_t  findleaves(Key key,size_t position){
            midroot tmp;
            readfile(&tmp,position,1, sizeof(midroot));
            if(tmp.type==true)//恭喜你有儿子了
            {
                size_t i=0;
                for(;i<tmp.num;i++){
                    if(key<tmp.key[i]) break;//找到真爱
                }
                return tmp.children[i];
            }
            else {
                size_t i=0;
                for(;i<tmp.num;i++){
                    if(key<tmp.key[i]) break;
                    if(key==tmp.key[i]){
                        i++;
                        break;
                    }
                }
                return findleaves(key,tmp.children[i]);
            }
        }
        //插入之前应该先会查找吧，不然插个龟龟
        iterator find(const Key& key) {
            size_t leafpos = findleaves(key,catalogue.root);//挖他祖坟
            leaves leaf;
            readfile(&leaf,leafpos,1, sizeof(leaves));
            for(size_t i=0;i<leaf.pairnum;i++){
                //我怎么觉得数据类型不太对。。。好奇怪 列为一个bug点
                if(leaf.datak[i]==key){
                    iterator ret;
                    ret.bplustree=this;
                    ret.leafposition=leafpos;
                    ret.pairposition=i;
                    return ret;
                }
            }

        }
        const_iterator find(const Key& key) const {//没错我直接复制下来的
            size_t leafpos = findleaves(key,catalogue.root);//挖他祖坟
            leaves leaf;
            readfile(&leaf,leafpos,1, sizeof(leaves));
            for(size_t i=0;i<leaf.pairnum;i++){
                //我怎么觉得数据类型不太对。。。好奇怪 列为一个bug点
                if(leaf.datak[i]==key){
                    iterator ret;
                    ret.bplustree=this;
                    ret.leafposition=leafpos;
                    ret.pairposition=i;
                    return ret;
                }
            }
        }
        //同样重新写，先插入叶子节点
        pair<iterator,OperationResult > insertleaf(leaves &leaf,Key key,Value value ){
            iterator ret;
            size_t i=0;
            for(;i<leaf.pairnum;i++){
                if(key<leaf.datak[i]) break;
            }
            //可能插入第一个数据的时候，叶子节点还是空的，所以要处理下
            if(leaf.pairnum==0){
                leaf.datak[0]=key;
                leaf.datav[0]=value;
                leaf.pairnum=1;
                catalogue.length++;
                ret.bplustree=this;
                ret.pairposition=i;
                ret.leafposition=leaf.position;
                writefile(&leaf,leaf.position,1, sizeof(leaves));
                return  pair<iterator,OperationResult >(ret,Success);

            }
            //否则就疯狂后移
            for(size_t j =leaf.pairnum-1;j>=i;j--){
                leaf.datak[j+1]=leaf.datak[j];
                leaf.datav[j+1]=leaf.datav[j];
            }
            leaf.datak[i]=key;
            leaf.datav[i]=value;

            leaf.pairnum++;
            catalogue.length++;
            ret.bplustree=this;
            ret.pairposition=i;
            ret.leafposition=leaf.position;
            if(leaf.pairnum<=Lmax)
                writefile(&leaf,leaf.position,1, sizeof(leaves));
            else splitleaf(leaf,ret,key);

            return  pair<iterator,OperationResult >(ret,Success);
        }
        void insertnode(midroot & mid,Key key,size_t newleafpos){
            size_t i=0;
            for(;i<mid.num;i++){
                if(key<mid.key[i]) break;
            }
            for(int j=mid.num-1;j>=i;j--){
                mid.key[j+1]=mid.key[j];
            }
            for(int j=mid.num;j>=i+1;j--){
                mid.children[j+1]=mid.children[j];
            }
            mid.key[i]=key;
            mid.children[i+1]=newleafpos;//新生的孩子加个1

            mid.num++;

            if(mid.num<=Mmax-1) writefile( &mid,mid.position,1, sizeof(midroot));
            else splitnode(mid);
        }
        //终于到了激动人心的分裂，我还没怎么搞懂的地方
        void splitleaf(leaves &leaf, iterator & tmp, Key &key){
            leaves newleaf;
            newleaf.pairnum=leaf.pairnum/2;
            leaf.pairnum/=2;

            newleaf.position=catalogue.endd;
            for(size_t i=0;i<newleaf.pairnum;i++){
                newleaf.datak[i]=leaf.datak[leaf.pairnum+i];
                newleaf.datav[i]=leaf.datav[leaf.pairnum+i];
                //注意这个iterator
                if(newleaf.datak[i]==key){
                    tmp.leafposition=newleaf.position;
                    tmp.pairposition=i;
                }
            }
            //分裂完了就插入进去
            newleaf.next=leaf.next;
            newleaf.prev=leaf.position;

            if(leaf.next!=0){
                leaves nextleaf;
                readfile( &nextleaf,leaf.next,1, sizeof(leaves));
                nextleaf.prev=newleaf.position;
                writefile(&nextleaf,nextleaf.position,1, sizeof(leaves));
            }
            leaf.next=newleaf.position;

            //目录也要改变
            if(catalogue.tail==leaf.position) catalogue.tail=newleaf.position;
            catalogue.endd+= sizeof(leaves);

            //他爹也是哦宝贝 （怎么就这么多我吐了）
            newleaf.parent=leaf.parent;

            writefile(&leaf,leaf.position,1, sizeof(leaves));
            writefile(&newleaf,newleaf.position,1, sizeof(leaves));
            writefile(&catalogue,0,1, sizeof(indexs));

            midroot parent;
            readfile(&parent,leaf.parent,1, sizeof(midroot));
            insertnode(parent,newleaf.datak[0],newleaf.position);
        }
        void splitnode(midroot & node){
            midroot newnode;
            /*
             * 现在node的key的个数应该是m个
             * 我希望分裂后node 有M/2个key new_node有 M-M/2-1个key
             * 加起来有M-1个 中间一个key应该上移给node的爸爸
             * 这样node 有M/2+1个child  new_node有M-M/2个child
             */
            newnode.num=node.num/2-1;
            node.num/=2;
            newnode.position=catalogue.endd;
            catalogue.endd+= sizeof(midroot);
            for(size_t i=0;i<=newnode.num;i++){
                newnode.children[i]=node.children[i+node.num+1];
            }
            for(size_t i=0;i<newnode.num;i++){
                newnode.key[i]=node.key[i+node.num+1];
            }
            newnode.type=node.type;

            for(size_t i=0;i<=newnode.num;i++) {
                if (newnode.type == true) {
                    leaves leaf;
                    readfile(&leaf, newnode.children[i], 1, sizeof(leaves));
                    leaf.parent = newnode.position;
                    writefile(&leaf, leaf.position, 1, sizeof(leaves));
                } else {
                    midroot mid;
                    readfile(&mid, newnode.children[i], 1, sizeof(mid));
                    mid.parent = newnode.position;
                    writefile(&mid, mid.position, 1, sizeof(midroot));
                }
            }

            //还要考虑他爹是不是根，杀了我吧
            if(node.position==catalogue.root){
                //新建一个根节点
                midroot newroot;
                newroot.parent=0;
                newroot.type= false;
                newroot.position=catalogue.endd;
                catalogue.endd+= sizeof(midroot);
                newroot.num=1;

                //输入新的呢！
                newroot.key[0]=node.key[node.num];
                newroot.children[0]=node.position;
                newroot.children[1]=newnode.position;

                //更新他的papa
                node.parent=newroot.position;
                newnode.parent=newroot.position;

                //还有个目录（最后了吧我哭）
                catalogue.root=newroot.position;

                //写入文件
                writefile(&catalogue,0,1, sizeof(indexs));
                writefile(&node,node.position,1, sizeof(midroot));
                writefile(&newnode,newnode.position,1, sizeof(midroot));
                writefile(&newroot,newroot.position,1, sizeof(midroot));
            }
            else {
                newnode.parent=node.parent;
                writefile(&catalogue,0,1, sizeof(indexs));
                writefile(&node,node.position,1, sizeof(midroot));
                writefile(&newnode,newnode.position,1, sizeof(midroot));

                midroot parent;
                readfile(&parent,node.parent,1, sizeof(midroot));
                insertnode(parent,node.key[node.num],newnode.position);
            }
        }
        //大家好，我们终于可以开始插入了
        pair<iterator, OperationResult> insert(const Key& key, const Value& value) {
            if(catalogue.length==0){
                midroot root;
                leaves leave;
                readfile(&root,catalogue.root,1, sizeof(midroot));
                readfile(&leave,catalogue.tail,1, sizeof(leaves));

                root.key[0]=key;
                leave.datak[0]=key;
                leave.datav[0]=value;

                catalogue.length=1;
                root.num=1;
                leave.pairnum=1;

                iterator tmp;
                tmp.bplustree=this;
                tmp.leafposition=leave.position;
                tmp.pairposition=0;

                //写回文件

                writefile(&root,catalogue.root,1, sizeof(midroot));
                writefile(&leave,catalogue.tail,1, sizeof(leaves));
                return pair<iterator,OperationResult >(tmp,Success);
            }
            size_t thepos=findleaves(key,catalogue.root);

            leaves newnode;
            readfile(&newnode,thepos,1, sizeof(leaves));
            pair <iterator,OperationResult > ret=insertleaf(newnode,key,value);
            return ret;
        }
        // Erase: Erase the Key-Value
        // Return Success if it is successfully erased
        // Return Fail if the key doesn't exist in the database
        OperationResult erase(const Key& key) {
            // TODO erase function
            return Fail;  // If you can't finish erase part, just remaining here.
        }
        // Overloaded of []
        // Access Specified Element
        // return a reference to the first value that is mapped to a key equivalent to
        // key. Perform an insertion if such key does not exist.
        Value& operator[](const Key& key) {}
        // Overloaded of const []
        // Access Specified Element
        // return a reference to the first value that is mapped to a key equivalent to
        // key. Throw an exception if the key does not exist.
        const Value& operator[](const Key& key) const {}
        // Access Specified Element
        // return a reference to the first value that is mapped to a key equivalent to
        // key. Throw an exception if the key does not exist
        Value  at(const Key& key) {
            ssize_t leafpos = findleaves(key,catalogue.root);
            leaves leaf;
            readfile(&leaf,leafpos,1, sizeof(leaves));
            for(int i=0;i<leaf.pairnum;i++)
            {
                if(leaf.datak[i]==key)
                {
                    return leaf.datav[i];
                }
            }
        }
        // Overloaded of const []
        // Access Specified Element
        // return a reference to the first value that is mapped to a key equivalent to
        // key. Throw an exception if the key does not exist.
        const Value& at(const Key& key) const {

        }
        // Return a iterator to the beginning
        iterator begin() {}
        const_iterator cbegin() const {}
        // Return a iterator to the end(the next element after the last)
        iterator end() {}
        const_iterator cend() const {}
        // Check whether this BTree is empty
        bool empty() const {}
        // Return the number of <K,V> pairs
        size_t size() const {}
        // Clear the BTree
        void clear() {}
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key& key) const {}
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is
         * returned.
         */

    };
}  // namespace sjtu
