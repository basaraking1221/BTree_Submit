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


        };//第二次加的，刚搞明白不能用路径来写。。
        //中间节点
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
        //叶子节点
        struct leaves{

            int offset;
            int father;
            int prev,next;
            int pairnum;
            //你敢信我写到最后了才发现，这个Value_type跟我写的不一样？？？？
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
        //根节点？也不算吧，那就定名为索引。
        struct indexs{
            int head;
            int tail;
            int root;
            int size;
            int eof;

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
        FILE *txt;//txt文本
        bool whetheropen;//打不打开
        indexs catalogue;//我英语很棒了
        bool whetherexist;//竟然还有文件原来已经存在这一说。。
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

        //进行一些文件操作，本来想直接open等但是不如写成函数来的快--------下面进行第二次调试及更改
        void openfile(){

            whetherexist=1;
            if(whetheropen==0)
            {
                txt= fopen(txtname.str,"rb+");

                if(txt== nullptr)
                {
                    whetherexist=false;//鬼知道还会打开失败歪日，你试试第一次运行就没打开的心态？
                    txt = fopen(txtname.str,"w");
                    fclose(txt);
                    txt =fopen(txtname.str,"rb+");


                }



                else{
                    readfile(&catalogue,0,1, sizeof(indexs));
                }

                whetheropen=1;
            }
        }
        void closefile(){
            if(whetheropen==true)
                fclose(txt);
            whetheropen= false;
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
//第一次就写这几个应该够了吧

        void makeatree()
        {
            //先开始操作目录
            catalogue.size=0;
            catalogue.eof= sizeof(indexs);
            //根节点和叶子节点建立
            midroot root;
            leaves leaf;
            //一步步的摆正目录、各个节点的位置（position）
            catalogue.root=root.offset=catalogue.eof;
            catalogue.eof+= sizeof(midroot);
            catalogue.head=catalogue.tail=leaf.offset=catalogue.eof;
            catalogue.eof+= sizeof(leaves);
            //挨个初始化
            root.father=0;
            root.num=0;
            root.type=true;
            root.children[0]=leaf.offset;
            leaf.father=root.offset;
            leaf.next=leaf.prev=0;
            leaf.pairnum=0;
            //全扔进文件里，写tmd
            writefile(&catalogue,0,1, sizeof(indexs));
            writefile(&root,root.offset,1, sizeof(midroot));
            writefile(&leaf,leaf.offset,1, sizeof(leaves));
        }
        // Default Constructor and Copy Constructor
        BTree() {
            // Todo Default
            //第二次写添加，所有有关存在的都是，不再赘述
            txt=NULL;
            openfile();

            if(whetherexist==0)
                makeatree();

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
        int findleaves(Key key,int offset)
        {
            midroot p;

            readfile(&p,offset, 1, sizeof(midroot));

            if(p.type==1)  //恭喜你有儿子了
            {

                int pos=0;
                for(pos=0;pos<p.num;pos++)
                {
                    if(key<p.key[pos])  break;//找到真爱
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
        //插入之前应该先会查找吧，不然插个龟龟
        iterator find(const Key& key) {
            size_t leafpos = findleaves(key,catalogue.root);//挖他祖坟
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
            size_t leafpos = findleaves(key,catalogue.root);//挖他祖坟
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
        //同样重新写，先插入叶子节点
        pair<iterator,OperationResult> insertleaf(leaves &leaf,Key key,Value value)
        {
            iterator ret;
            int pos=0;

            for(pos=0;pos<leaf.pairnum;pos++)
            {

                if(key<leaf.k[pos]) break;
            }
            //可能插入第一个数据的时候，叶子节点还是空的，所以要处理下
            if(leaf.pairnum==0)
            {
                leaf.k[0]=key;
                leaf.v[0]=value;

                leaf.pairnum=1;
                ++catalogue.size;

                ret.bpt=this;
                ret.pairpos=pos;
                ret.leafoffset=leaf.offset;

                writefile(&leaf,leaf.offset,1,sizeof(leaves));

                return pair<iterator,OperationResult > (ret,Success);
            }
            //否则就技能疯狂后摇
            for(int i=leaf.pairnum-1;i>=pos;--i)
            {
                leaf.k[i+1]=leaf.k[i];
                leaf.v[i+1]=leaf.v[i];
            }

            leaf.k[pos]=key;
            leaf.v[pos]=value;

            ++leaf.pairnum;
            ++catalogue.size;

            ret.bpt=this;
            ret.pairpos=pos;
            ret.leafoffset=leaf.offset;
            if(leaf.pairnum<=Lmax)
                writefile(&leaf,leaf.offset,1,sizeof(leaves));
            else
                splitleaf(leaf,ret,key);

            return pair<iterator,OperationResult > (ret,Success);

        }
//终于到了激动人心的分裂，我还没怎么搞懂的地方
        void splitleaf (leaves &leaf, iterator & it, Key &key)
        {
            leaves new_leaf;

            new_leaf.pairnum=leaf.pairnum-leaf.pairnum/2;
            leaf.pairnum/=2;

            new_leaf.offset=catalogue.eof;

            for(int i=0;i<new_leaf.pairnum;i++)
            {
                new_leaf.k[i]=leaf.k[leaf.pairnum+i];
                new_leaf.v[i]=leaf.v[leaf.pairnum+i];

//注意这个iterator
                if(new_leaf.k[i]==key)
                {
                    it.leafoffset=new_leaf.offset;
                    it.pairpos=i;
                }
            }
            //分裂完了就插入进去
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
            //目录也要改变
            if(catalogue.tail==leaf.offset)  catalogue.tail=new_leaf.offset;
            catalogue.eof+= sizeof(leaves);
            //他爹也是哦宝贝 （怎么就这么多我吐了）
            new_leaf.father=leaf.father;

            writefile(&leaf,leaf.offset,1, sizeof(leaves));
            writefile(&new_leaf,new_leaf.offset,1, sizeof(leaves));
            writefile(&catalogue,0,1, sizeof(indexs));


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

            ++cur.num;


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
            new_node.offset=catalogue.eof;
            catalogue.eof+= sizeof(midroot);

            for(int i=0;i<new_node.num;i++)
            {
                new_node.key[i]=node.key[i+node.num+1];             }

            for(int i=0;i<=new_node.num;i++)
            {
                new_node.children[i]=node.children[i+node.num+1];
            }
            new_node.type=node.type;

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
            //还要考虑他爹是不是根，杀了我吧
            if(node.offset == catalogue.root)
            {

                midroot new_root;
                new_root.father=0;
                new_root.type=0;
                new_root.offset=catalogue.eof;
                catalogue.eof+= sizeof(midroot);
                new_root.num=1;
                //输入新的呢！
                new_root.key[0]=node.key[node.num];
                new_root.children[0]=node.offset;
                new_root.children[1]=new_node.offset;
                //更新他的papa
                node.father=new_root.offset;
                new_node.father=new_root.offset;
                //还有个目录（最后了吧我哭）
                catalogue.root=new_root.offset;

                //写入文件
                writefile(&catalogue,0,1,sizeof(indexs));
                writefile(&node,node.offset,1,sizeof(midroot));
                writefile(&new_node,new_node.offset,1, sizeof(midroot));
                writefile(&new_root,new_root.offset,1,sizeof(midroot));


            }
            else
            {
                new_node.father=node.father;
                writefile(&catalogue,0,1,sizeof(indexs));
                writefile(&node,node.offset,1,sizeof(midroot));
                writefile(&new_node,new_node.offset,1, sizeof(midroot));

                midroot father;
                readfile(&father,node.father,1, sizeof(midroot));
                insertnode(father,node.key[node.num],new_node.offset);
            }
        }
//大家好，我们终于可以开始插入了
        pair<iterator, OperationResult> insert(const Key& key, const Value& value) {

            if(catalogue.size==0)
            {
                midroot root;
                leaves leaf_right;

                readfile(&root,catalogue.root,1, sizeof(midroot));
                readfile(&leaf_right,catalogue.tail,1, sizeof(leaves));

                root.key[0]=key;
                leaf_right.k[0]=key;
                leaf_right.v[0]=value;

                catalogue.size=1;
                root.num=1;
                leaf_right.pairnum=1;

                iterator p;
                p.bpt=this;
                p.leafoffset=leaf_right.offset;
                p.pairpos=0;


                writefile(&root,catalogue.root,1, sizeof(midroot));
                writefile(&leaf_right,catalogue.tail,1,sizeof(leaves));

                return pair<iterator,OperationResult >(p,Success);

            }


            int cur_leaf_node_offset=findleaves(key,catalogue.root);

            leaves new_node;

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

            int leafpos = findleaves(key,catalogue.root);
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

    };
}  // namespace sjtu
