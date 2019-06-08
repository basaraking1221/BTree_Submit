#include "utility.hpp"
#include <functional>
#include <cstddef>
#include "exception.hpp"
#include <fstream>
const int Mmax=1000;
const int Lmax=400;
namespace sjtu {
    template <class Key, class Value, class Compare = std::less<Key> >
    class BTree {
    public:
        typedef pair<const Key, Value> value_type;

    private:

        struct filename {
            char *str;

            filename() {str=new char[30];strcpy(str,"bplustree.txt");}
            ~filename(){ if(!str) delete str;}


        };

        struct midroot{

            int offset;   //自己的偏移量
            int father;   //爸爸的偏移量
            int num; //已存的key的个数
            int type;     //type=0 则它的儿子是internal_node

            int children[Mmax+1];  //设计每个中间结点存M个孩子  数组开M+1 大小
            Key key[Mmax];   //设计每个中间结点可以放M-1个key  但数组要开M个 因为要是已有M-1个key插入一个key 则先存M个再分裂


            midroot()
            {
                offset=0;
                father=0;
                num=0;
                type=0;
                memset(children,0,Mmax+1);

            }

        };

        struct leaves{

            int offset;
            int father;
            int prev,next;
            int pairnum;

            //value_type data[L+1];   //每个叶子结点放L个数据 数组开L+1 给插入留余量   我去pair只能读不能写？？？！！！！
            Key k[Lmax+1];
            Value v[Lmax+1];

            leaves(){
                offset=0;
                father=0;
                pairnum=0;
                prev=0;
                next=0;
            }

        };

        struct indexs{
            int head;   //第一个叶子结点的位置
            int tail;   //最后一个叶子结点的位置
            int root;   //根结点的位置   一开始应该在base的后面
            int size;
            int eof;    //文件末尾

            indexs()
            {
                head=0;
                tail=0;
                root=0;
                size=0;
                eof=0;
            }
        };

    private:
        FILE *txt;
        bool whetheropen;
        indexs base;
        bool whetherexist;
        filename txtname;




    public:

        class const_iterator;
        class iterator {

            friend class BTree;

        private:
            // Your private members go here
            int leafoffset;
            int pairpos;
            BTree *bpt;

        public:
            bool modify(const Value& value){

            }
            iterator() {
                // TODO Default Constructor
                bpt=NULL;
                leafoffset=0;
                pairpos=0;
            }
            iterator(const iterator& other) {
                // TODO Copy Constructor
                bpt=other.bpt;
                leafoffset=other.leafoffset;
                pairpos=other.pairpos;
            }
            // Return a new iterator which points to the n-next elements
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
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // Your private members go here
            int leafoffset;
            int pairpos;
            BTree *bpt;
        public:
            const_iterator() {
                // TODO
                bpt=NULL;
                leafoffset=0;
                pairpos=0;
            }
            const_iterator(const const_iterator& other) {
                // TODO
                bpt=other.bpt;
                leafoffset=other.leafoffset;
                pairpos=other.pairpos;
            }
            const_iterator(const iterator& other) {
                // TODO
                bpt=other.bpt;
                leafoffset=other.leafoffset;
                pairpos=other.pairpos;
            }
            // And other methods in iterator, please fill by yourself.
        };


        void openfile(){

            whetherexist=1;
            if(whetheropen==0)
            {

                //如果这个文件已存在 并且 原来时关着的 就打开它 并且把信息读到base
                txt= fopen(txtname.str,"rb+");

                //打开成功fopen返回指针地址，否则返回NULL
                //然后检查一下文件是否打开成功  没打开成功说明文件不存在嗷
                if(txt== nullptr)
                {
                    whetherexist=false;
                    //w：只能向文件写数据，若指定的文件不存在则创建它，如果存在则先删除它再重建一个新文件
                    txt = fopen(txtname.str,"w");
                    //现在有文件啦  再以rw+打开  但是这个文件里没有bpt的基本信息嗷  待会用build_a_tree往里面写
                    fclose(txt);
                    txt =fopen(txtname.str,"rb+");


                }



                    //说明刚刚打开成功啦 已经有 bpt的信息啦 现在要读basic_info进内存
                else{
                    readfile(&base,0,1, sizeof(indexs));
                }

                whetheropen=1;
            }
        }

        void readfile(void *place,int offset,int count,int size)
        {
            fseek(txt,offset,SEEK_SET);
            fread(place,size,count,txt);
        }

        void writefile(void *place, int offset, int count ,int size)
        {
            fseek(txt,offset,SEEK_SET);
            fwrite(place,size,count,txt);

        }


        //=======================================构造 析构======================================//
        void makeatree()
        {
            //先开始操作目录
            base.size=0;
            base.eof= sizeof(indexs);
            //根节点和叶子节点建立
            midroot root;
            leaves leaf;
            //一步步的摆正目录、各个节点的位置（position）
            base.root=root.offset=base.eof;
            base.eof+= sizeof(midroot);
            base.head=base.tail=leaf.offset=base.eof;
            base.eof+= sizeof(leaves);
            //挨个初始化
            root.father=0;
            root.num=0;
            root.type=true;
            root.children[0]=leaf.offset;
            leaf.father=root.offset;
            leaf.next=leaf.prev=0;
            leaf.pairnum=0;
            //全扔进文件里，写tmd
            writefile(&base,0,1, sizeof(indexs));
            writefile(&root,root.offset,1, sizeof(midroot));
            writefile(&leaf,leaf.offset,1, sizeof(leaves));
        }
        // Default Constructor and Copy Constructor
        BTree() {
            // Todo Default

            txt=NULL;
            openfile();

            if(whetherexist==0)
                makeatree();

        }
        BTree(const BTree& other) {
            // Todo Copy

        }
        BTree& operator=(const BTree& other) {
            // Todo Assignment
        }
        ~BTree() {
            // Todo Destructor

            if(whetheropen==1)
            {
                fclose(txt);
                whetheropen=0;
            }

        }

        int findleaves(Key key,int offset)
        {
            midroot p;

            readfile(&p,offset, 1, sizeof(midroot));

            if(p.type==1)   //说明它的下一个是叶子结点啦
            {

                int pos=0;
                for(pos=0;pos<p.num;pos++)
                {
                    if(key<p.key[pos])  break;
                }

                //注意孩子数比结点数多一个 pos++ 跳出循环后 也有孩子的
                return p.children[pos];
            }

            else{
                int pos=0;
                for(pos=0;pos<p.num;pos++)
                {
                    if(key<p.key[pos]) break;
                    if(key==p.key[pos])    //考虑等号？？？
                    {
                        pos++;
                        break;
                    }
                }

                return findleaves(key,p.children[pos]);
            }

        }

        pair<iterator,OperationResult> insertleaf(leaves &leaf,Key key,Value value)
        {
            iterator ret;
            int pos=0;

            for(pos=0;pos<leaf.pairnum;pos++)
            {

                if(key<leaf.k[pos]) break;
            }

            //刚开始插入数据的时候 第一次在根的左孩子里插入时 根的左孩子还没有数据
            if(leaf.pairnum==0)
            {
                leaf.k[0]=key;
                leaf.v[0]=value;

                //维护size!!!!!
                leaf.pairnum=1;
                ++base.size;

                //设置迭代器
                ret.bpt=this;
                ret.pairpos=pos;
                ret.leafoffset=leaf.offset;

                writefile(&leaf,leaf.offset,1,sizeof(leaves));

                return pair<iterator,OperationResult > (ret,Success);
            }
            //每个数据后移
            for(int i=leaf.pairnum-1;i>=pos;--i)
            {
                leaf.k[i+1]=leaf.k[i];
                leaf.v[i+1]=leaf.v[i];
            }

            //插入
            leaf.k[pos]=key;
            leaf.v[pos]=value;

            //维护size
            ++leaf.pairnum;
            ++base.size;

            //设置迭代器
            ret.bpt=this;
            ret.pairpos=pos;
            ret.leafoffset=leaf.offset;
            if(leaf.pairnum<=Lmax)
                writefile(&leaf,leaf.offset,1,sizeof(leaves));
            else
                splitleaf(leaf,ret,key);

            return pair<iterator,OperationResult > (ret,Success);




        }


        void splitleaf (leaves &leaf, iterator & it, Key &key)
        {
            leaves new_leaf;

            //维护numOfpair
            new_leaf.pairnum=leaf.pairnum-leaf.pairnum/2;
            leaf.pairnum/=2;

            //维护new_leaf的offset
            new_leaf.offset=base.eof;

            for(int i=0;i<new_leaf.pairnum;i++)
            {
                new_leaf.k[i]=leaf.k[leaf.pairnum+i];
                new_leaf.v[i]=leaf.v[leaf.pairnum+i];


                //维护iterator  如果新插入数据被分到new_leaf
                if(new_leaf.k[i]==key)
                {
                    it.leafoffset=new_leaf.offset;
                    it.pairpos=i;
                }
            }

            //维护叶子结点 顺序  注意读入leaf.next 修改它的prev
            new_leaf.next=leaf.next;
            new_leaf.prev=leaf.offset;

            if(leaf.next!=0)
            {
                leaves leaf_next;
                readfile(&leaf_next,leaf.next,1, sizeof(leaves));
                leaf_next.prev=new_leaf.offset;
                writefile(&leaf_next,leaf_next.offset,1,sizeof(leaves));
            }
            leaf.next=new_leaf.offset;

            //维护base
            if(base.tail==leaf.offset)  base.tail=new_leaf.offset;
            base.eof+= sizeof(leaves);

            //维护爸爸
            new_leaf.father=leaf.father;

            writefile(&leaf,leaf.offset,1, sizeof(leaves));
            writefile(&new_leaf,new_leaf.offset,1, sizeof(leaves));
            writefile(&base,0,1, sizeof(indexs));


            midroot father;
            readfile(&father,leaf.father,1,sizeof(midroot));
            insertnode(father,new_leaf.k[0],new_leaf.offset);


        }


        void insertnode(midroot & cur,Key key,int new_leaf_offset)
        {

            int pos=0;
            for(pos=0;pos<cur.num;++pos)
            {
                if(key<cur.key[pos]) break;
            }

            for(int i=cur.num-1;i>=pos;i--)
            {
                cur.key[i+1]=cur.key[i];
            }

            for(int i=cur.num;i>=pos+1;i--)
            {
                cur.children[i+1]=cur.children[i];
            }

            cur.key[pos]=key;
            cur.children[pos+1]=new_leaf_offset;

            //维护numOfkey
            ++cur.num;


            // 写回去  考虑分裂node
            if(cur.num<=Mmax-1)
                writefile(&cur,cur.offset,1,sizeof(midroot));
            else
                splitmidroot(cur);



        }

        void splitmidroot(midroot & node)
        {

            midroot new_node;

            new_node.num=node.num-node.num/2-1;
            node.num/=2;
            new_node.offset=base.eof;
            base.eof+= sizeof(midroot);

            //更新new_node的数据
            for(int i=0;i<new_node.num;i++)
            {
                new_node.key[i]=node.key[i+node.num+1];  //从numOfkey+1开始抄  numOfkey号key应该写到爸爸里
            }

            for(int i=0;i<=new_node.num;i++)
            {
                new_node.children[i]=node.children[i+node.num+1];   //从numOfkey+1（下标）开始抄 numOfkey号（下标）放在node的最后一个
            }


            new_node.type=node.type;


            //更新新结点的孩子们的father


            for(int i=0;i<=new_node.num;i++)
            {
                if(new_node.type==1)
                {
                    leaves leaf;
                    readfile(&leaf,new_node.children[i],1,sizeof(leaves));
                    leaf.father=new_node.offset;
                    writefile(&leaf,leaf.offset,1,sizeof(leaves));
                }
                else{
                    midroot internal;
                    readfile(&internal,new_node.children[i],1,sizeof(midroot));
                    internal.father=new_node.offset;
                    writefile(&internal,internal.offset,1,sizeof(midroot));
                }

            }





            //把中间那个插到爸爸里  考虑爸爸是root
            if(node.offset == base.root)
            {

                //新建一个根
                midroot new_root;
                new_root.father=0;
                new_root.type=0;
                new_root.offset=base.eof;
                base.eof+= sizeof(midroot);
                new_root.num=1;

                new_root.key[0]=node.key[node.num];
                new_root.children[0]=node.offset;
                new_root.children[1]=new_node.offset;

                //更新node new_node的爸爸
                node.father=new_root.offset;
                new_node.father=new_root.offset;

                //更新base的root;
                base.root=new_root.offset;


                //写进文件
                writefile(&base,0,1,sizeof(indexs));
                writefile(&node,node.offset,1,sizeof(midroot));
                writefile(&new_node,new_node.offset,1, sizeof(midroot));
                writefile(&new_root,new_root.offset,1,sizeof(midroot));


            }
            else
            {
                new_node.father=node.father;
                writefile(&base,0,1,sizeof(indexs));
                writefile(&node,node.offset,1,sizeof(midroot));
                writefile(&new_node,new_node.offset,1, sizeof(midroot));

                midroot father;
                readfile(&father,node.father,1, sizeof(midroot));
                insertnode(father,node.key[node.num],new_node.offset);
            }



        }



        pair<iterator, OperationResult> insert(const Key& key, const Value& value) {

            if(base.size==0)
            {
                midroot root;
                leaves leaf_right;

                readfile(&root,base.root,1, sizeof(midroot));
                readfile(&leaf_right,base.tail,1, sizeof(leaves));

                root.key[0]=key;
                leaf_right.k[0]=key;
                leaf_right.v[0]=value;

                base.size=1;
                root.num=1;
                leaf_right.pairnum=1;

                iterator p;
                p.bpt=this;
                p.leafoffset=leaf_right.offset;
                p.pairpos=0;


                writefile(&root,base.root,1, sizeof(midroot));
                writefile(&leaf_right,base.tail,1,sizeof(leaves));

                return pair<iterator,OperationResult >(p,Success);

            }

            //找到在哪个叶子结点
            int cur_leaf_node_offset=findleaves(key,base.root);

            //在该叶子结点里面插入key value
            //先往new_node里面写 然后在write回去 覆盖掉
            leaves new_node;

            //<????>
            //先把文件里的数据写进new_node
            readfile(&new_node,cur_leaf_node_offset,1, sizeof(leaves));
            pair<iterator,OperationResult > ret =insertleaf(new_node,key,value);
            return ret;

        }
        // Erase: Erase the Key-Value
        // Return Success if it is successfully erased
        // Return Fail if the key doesn't exist in the database
        OperationResult erase(const Key& key) {
            // TODO erase function
            return Fail;  // If you can't finish erase part, just remaining here.
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
        // Return the value refer to the Key(key)
        Value at(const Key& key){

            int leafpos = findleaves(key,base.root);
            leaves leaf;
            readfile(&leaf,leafpos,1, sizeof(leaves));
            for(size_t i=0;i<leaf.pairnum;i++)
            {
                if(leaf.k[i]==key)
                {
                    return leaf.v[i];
                }
            }
            /*iterator it=find(key);
            leaf_node leaf;
            ReadFile(&leaf,it.leaf_offset,1, sizeof(leaf_node));
            return leaf.v[it.pair_pos];*/


        }
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
        iterator find(const Key& key) {
            size_t leafpos = findleaves(key,base.root);//挖他祖坟
            leaves leaf;
            readfile(&leaf,leafpos,1, sizeof(leaves));
            for(size_t i=0;i<leaf.pairnum;i++){
                //我怎么觉得数据类型不太对。。。好奇怪 列为一个bug点
                if(leaf.datak[i]==key){
                    iterator ret;
                    ret.bpt=this;
                    ret.leafoffset=leafpos;
                    ret.pairpos=i;
                    return ret;
                }
            }

        }
        const_iterator find(const Key& key) const {//没错我直接复制下来的
            size_t leafpos = findleaves(key,base.root);//挖他祖坟
            leaves leaf;
            readfile(&leaf,leafpos,1, sizeof(leaves));
            for(size_t i=0;i<leaf.pairnum;i++){
                //我怎么觉得数据类型不太对。。。好奇怪 列为一个bug点
                if(leaf.datak[i]==key){
                    iterator ret;
                    ret.bpt=this;
                    ret.leafoffset=leafpos;
                    ret.pairpos=i;
                    return ret;
                }
            }
        }

    };
}  // namespace sjtu
