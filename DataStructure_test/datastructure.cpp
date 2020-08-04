#include "datastructure.h"

//  CSNode 模板类的实现
template<class T>
CSNode<T>::CSNode():
    firstchild(nullptr),
    nextsibling(nullptr)
{ }

template<class T>
CSNode<T>::CSNode(CSNode* fchild,CSNode* nextsibling,T newdata):
    firstchild(fchild),
    nextsibling(nextsibling),
    data(newdata)
{ }

template<class T>
CSNode<T>* CSNode<T>::getFirstChild()
{
    return firstchild;
}

template<class T>
CSNode<T>* CSNode<T>::getNextSibling()// 模板类：凡是包含模板类名的都要加<>，无论是在返回值还是在作用域
{						// 模板类中函数：在类外实现的时候，要有个冒	template<class T>  表示这是个模板类的函数，区分与不同函数
    return nextsibling;
}

template<class T>
T CSNode<T>::getData()
{
    return data;
}

template<class T>
void CSNode<T>::setFirstChild(T newData)
{
    firstchild->data = newData;
}

template<class T>
void CSNode<T>::setNextsibling(T newData)
{
    nextsibling->data = newData;
}

template<class T>
void CSNode<T>::showInputChild()
{
    cout<<"请输入"<<data<<"的孩子:";
}



//  CSTree 模板类的实现
template<class T>
CSTree<T>::CSTree():
    root(nullptr) // 注意，这里构造函数要为root赋值为空，否则在之后和空比对的时候，可能会出错
{ }

template<class T>
CSTree<T>::~CSTree()
{
    if (root!=nullptr)
    {
        int num=0;
        destory(root,num);
        InitCSTree();
    }
}

template<class T>
void CSTree<T>::createCSTree()
{
    create(root);
}

template<class T>
void CSTree<T>::create(CSNode<T>*& p)
{
    // p->data 类型: QStack<QVector<float>>
//    _root->data = 0;
//    _root->firstchild = nullptr;
//    _root->nextsibling = nullptr;

//    T stack;
//    QVector<float> vec1,vec2;
//    for(int i = 0; i < 10; i++)
//    {
//        vec1.push_back(i);
//        vec2.push_back(i);
//    }
//    stack.push_back(vec1);
//    stack.push_back(vec2);

//    // 根结点
//    CSNode<int>* root = _root;

//    // 第二层的孩子结点
//    T child = stack;

//    // 定义一个队列
//    int front=0;
//    int rear=0;
//    CSNode<T>* pNode = p;
//    CSNode<T>* queue[MaxCSTreeSize];

//    queue[(rear++)%MaxCSTreeSize] = root;// 根入队

    T parent;
    T child;
    // 定义一个队列
    int front=0;
    int rear=0;
    CSNode<T>* pNode = p;
    CSNode<T>* queue[MaxCSTreeSize];
    cin>>parent>>child;
    while(child!='#')
    {
        CSNode<T>* node = new CSNode<T>;
        node->data = child;
        node->firstchild = nullptr;
        node->nextsibling = nullptr;
        // #a 只有 a 入队
        if (parent=='#')
        {
            p = node;// 为根
            queue[(rear++)%MaxCSTreeSize]=node;// 根入队
        }
        // ab
        else
        {
            pNode = queue[front];// 读取队头元素
            while(pNode->data!=parent) // 找自己父亲节点的指针
            {
                front=(front+1)%MaxCSTreeSize;
                pNode = queue[front];// 读取队头元素---不能写成pNode = queue[front++]啊，因为在找到父亲的时候，front又执行了++，父亲就直接出队列了，当父亲后来的孩子来的时候就找不到父亲而出错了！
            }
            if (pNode->data==parent)// 是儿子
            {
                if (pNode->firstchild==nullptr)
                {
                    pNode->firstchild = node;
                }
                else
                {
                    pNode = pNode->firstchild;
                    while (pNode->nextsibling)
                    {
                        pNode=pNode->nextsibling;
                    }
                    pNode->nextsibling= node;
                }
            }
            if ((rear+1)%MaxCSTreeSize!=front)// 队不满，则入队
            {
                queue[(rear++)%MaxCSTreeSize]=node;// 把自己的儿子放到队列中
            }
        }
        cin>>parent>>child;
    }


}

template<class T>
void CSTree<T>::copyTree(CSNode<T>*& dstTreeRoot,CSNode<T>* srcTreeRoot)
{
    if (srcTreeRoot)
    {
        dstTreeRoot = new CSNode<T>;
        dstTreeRoot->data = srcTreeRoot->data;
        copyTree(dstTreeRoot->firstchild,srcTreeRoot->firstchild);
        copyTree(dstTreeRoot->nextsibling,srcTreeRoot->nextsibling);
    }
}

template<class T>
CSTree<T>::CSTree(const CSTree<T>& csTree)
{
    if (csTree.root==nullptr)
    {
        root = nullptr;
    }
    else
    {
        copyTree(this->root,csTree.root);
    }
}

template<class T>
const CSTree<T>& CSTree<T>::operator=(const CSTree<T>& csTree)
{
    if (this!=&csTree)// 避免自己赋值
    {
        if (root!=nullptr)//
        {
            int num=0;// num只是为了调试用
            destory(root,num);// 自己有成员，先销毁
        }
        if (csTree.root==nullptr)
        {
            root = nullptr;
        }
        else
        {
            copyTree(this->root,csTree.root);
        }
    }
    return *this;
}

template<class T>
void CSTree<T>::InitCSTree()
{
    root=nullptr;
}

template<class T>
void CSTree<T>::destoryCSTree()
{
    int num=0;
    destory(root,num);
    root=nullptr;
    cout<<"销毁了"<<num<<"个结点"<<endl;
}
/*使用后序销毁树--可以直接把该树看成一半的二叉树，见结点销毁即可*/
template<class T>
void CSTree<T>::destory(CSNode<T>*& p,int& num)
{
    if (!p)// 空节点
    {
        return;
    }
    destory(p->firstchild,num);
    destory(p->nextsibling,num);
    delete p;
    num++;
}

template<class T>
bool CSTree<T>::isEmptyCSTree()
{
    if (root==nullptr)
    {
        return true;
    }
    return false;
}

template<class T>
void CSTree<T>::preOrderTraverse()
{
    preOrder(root);
    cout<<endl;
}

/*树的先序遍历和转变成二叉树后的先序遍历是一样的*/
template<class T>
void CSTree<T>::preOrder(CSNode<T>* p)
{
    if (p)
    {
        cout<<p->getData();
        preOrder(p->firstchild);
        preOrder(p->nextsibling);
    }
}

template<class T>
void CSTree<T>::postOrderTraverse()
{
    postOrder(root);
    cout<<endl;
}

/*树的后序遍历和转变成二叉树后的中序遍历是一样的*/
template<class T>
void CSTree<T>::postOrder(CSNode<T>* p)
{
    if (p)
    {
        postOrder(p->firstchild);
        cout<<p->getData();
        postOrder(p->nextsibling);
    }
}

template<class T>
void CSTree<T>::levelOrderTraverse()
{
    levelOrder(root);
    cout<<endl;
}
/*树的层次遍历和二叉树的层次遍历思路是一样的，入队的都是该结点的所有孩子*/
template<class T>
void CSTree<T>::levelOrder(CSNode<T>* p)
{
    // 定义一个队列
    int front=0;
    int rear=0;
    CSNode<T>* queue[MaxCSTreeSize];
    if(!p)
    {
        return;
    }
    queue[(rear++)%MaxCSTreeSize]=p;
    while(front<rear)
    {
        CSNode<T>* t = queue[(front++)%MaxCSTreeSize];
        cout<<t->data;
        for(CSNode<T>* pNode=t->firstchild;pNode;pNode = pNode->nextsibling)
        {
            queue[(rear++)%MaxCSTreeSize]=pNode;
        }
    }
}

template<class T>
int CSTree<T>::LeavesCountCSTree()
{
    return leavesCount(root);
}

/*如果一个节点的做左子树为空，则必为叶子节点*/
template<class T>
int CSTree<T>::leavesCount(CSNode<T>* p)
{
    int count = 0;
    if (!p)// 老忘 出口一
    {
        return 0;
    }
    if (!p->firstchild)
    {
        return 1;
    }
    for(CSNode<T>* pNode=p->firstchild;pNode;pNode = pNode->nextsibling)
    {
        count += leavesCount(pNode);
    }
    return count;
}
/*寻找item的结点*/
template<class T>
int CSTree<T>::nodeLevelCSTree(T item)
{
    int nlevel = 0;
    nodeLevel(item,root,1,nlevel);// 首先到第一层查找
    return nlevel;
}

/*
level参数表示现在已经查找到第几层
nlevel参数表示记录结点所在的最终的层次
*/
template<class T>
void CSTree<T>::nodeLevel(T item,CSNode<T>* p,int level,int& nlevel)
{
    if (nlevel) // 当层次非0时，表示已经找到 -- 出口一
    {
        return;
    }
    if (!p) // 出口二
    {
        return;
    }
    if (p->data == item)// 出口三
    {
        nlevel = level;
        return;
    }
    for (CSNode<T>*pNode = p->firstchild;pNode;pNode=pNode->nextsibling)// 在孩子处查找 --- 入口
    {
        nodeLevel(item,pNode,level+1,nlevel);
    }
}

template<class T>
int CSTree<T>::heightCSTree()
{
    return height(root);
}

/*把根和右孩子的深度比较，取最大值*/
template<class T>
int CSTree<T>::height(CSNode<T>* p)
{
    int maxHeight = 0;
    if (!p)// 因为在递归的时候不对空孩子进行判断，参数可能会有为空的情况，这时要专门对空进行处理
    {
        return 0;
    }
    if (!p->firstchild)// 结点是叶子结点的情况
    {
        return 1;
    }
    for (CSNode<T>* pNode = p->firstchild;pNode;pNode=pNode->nextsibling)// 结点是有多个孩子的情况
    {
        int h = height(pNode) + 1;
        if (h>maxHeight)
        {
            maxHeight = h;
        }
    }
    return maxHeight;
}


/*树的宽度*/
template<class T>
int CSTree<T>::widthCSTree()
{
    return width(root);
}

/*求宽度的函数总是想不出要引入last变量，一定要注意啊*/
template<class T>
int CSTree<T>::width(CSNode<T> *p)
{
    int w = 0;
    int maxWidth=0;
    int front=0;
    int rear = 0;
    int last=0;// last指向一层中最后那个元素
    CSNode<T>*queue[MaxCSTreeSize];
    // 根先入队
    if (!p)
    {
        maxWidth = 0;
    }
    else
    {
        queue[rear++]=p;
        while (front<=last)// 等号成立：最后那个元素仍然是本层的元素，要继续处理
        {
            CSNode<T>* cur = queue[front++];// 出队处理
            w++;
            for (CSNode<T>* pNode=cur->firstchild;pNode;pNode=pNode->nextsibling)
            {
                queue[rear++]=pNode;
            }
            if (front>last)
            {
                last = rear-1;// 因为rear总是指向本层元素的下一位
                if (w > maxWidth)
                {
                    maxWidth = w;
                }
                w=0;
            }
        }
    }
    return maxWidth;
}

template<class T>
int CSTree<T>::nodeCountCSTree()
{
    return nodeCount(root);
}
/*根据先序遍历得到*/
template<class T>
int CSTree<T>::nodeCount(CSNode<T>* p)
{
    int sum=0;
    if (!p)
    {
        return 0;
    }
    if (!p->firstchild)// 左孩子为空，该结点就为叶子结点
    {
        return 1;
    }
    for (CSNode<T>*pNode = p->firstchild;pNode;pNode=pNode->nextsibling)
    {
        sum+=nodeCount(pNode);
    }
    return sum + 1;// 1代表父亲本身，sum为孩子的总和
}



template<class T>
bool CSTree<T>::findCSNode(const T item,CSNode<T>*& ret)const
{
    bool isFind = false;
    find(root,item,isFind,ret);
    return isFind;
}

/*使用深度遍历进行查找*/
template<class T>
bool CSTree<T>::find(CSNode<T>*p,const T item,bool& isFind,CSNode<T>*& cur)const
{
    if (isFind)// 找到直接返回
    {
        return isFind;
    }
    if (!p)// 结点为空，直接返回
    {
        cur = nullptr;
        return isFind;
    }
    if (p->data == item)// 找到
    {
        cur = p;
        isFind = true;
        return isFind;
    }// 没找到，继续递归
    for (CSNode<T>*pNode = p->firstchild;pNode;pNode=pNode->nextsibling)
    {
        find(pNode,item,isFind,cur);
        if (isFind)
        {
            return isFind;
        }
    }
    return isFind;
}

template<class T>
int CSTree<T>::getDegreeCSTree()
{
    return getDegree(root);
}

template<class T>
int CSTree<T>::getDegree(CSNode<T>* p)
{
    int num=0;
    if (!p)// 递归出口一 -- 结点为空
    {
        return 0;
    }
    if (!p->firstchild)// 递归出口二 -- 结点为叶子结点
    {
        return 1;
    }
    for (CSNode<T>* pNode = p->firstchild;pNode;pNode=pNode->nextsibling)// 处理自身，计算自己孩子的个数 -- 结点有孩子
    {
        num++;
    }
    for (CSNode<T>* pNode = p->firstchild;pNode;pNode=pNode->nextsibling)// 计算自己的孩子的度，并与自己的度相比较，返回最大度
    {
        int num1 = getDegree(pNode);
        if(num1>num)
        {
            num = num1;
        }
    }
    return num;
}
template<class T>
bool CSTree<T>::getleftChild(const CSNode<T>* p,CSNode<T>*& ret)const
{
    if (!p)
    {
        return false;
    }
    else
    {
        if(getParentCSTree(p->data,ret))// 找到p结点的父亲
        {
            ret = ret->firstchild;
            return true;
        }
        else
            return false;// 找不到父亲结点，表示这是个根节点
    }
}

template<class T>
bool CSTree<T>::getrightSibling(const CSNode<T>* p,CSNode<T>*& ret)const
{
    if(!p)
    {
        return false;
    }
    else
    {
        if(getParentCSTree(p->data,ret))// 找到p结点的父亲
        {
            CSNode<T>* node = ret->firstchild;
            while(node->nextsibling)
            {
                node = node->nextsibling;
            }
            ret = node;
            return true;
        }
        else
            return false;// 找不到父亲结点，表示这是个根节点
    }
}

template<class T>
bool CSTree<T>::getParentCSTree(const T item,CSNode<T>*& ret) const
{
    bool isFind = false;
    bool isFirst = true;
    getParent(root,item,isFind,isFirst,ret);
    return isFind;
}

/*使用层次遍历得到检查结点*/
template<class T>
bool CSTree<T>::getParent(CSNode<T>*p,const T item,bool& isFind,bool& isFirst,CSNode<T>*& ret)const
{
    if (isFind) // 出口一
    {
        return isFind;
    }
    if (!p)// p为空 //  出口二
    {
        return false;
    }
    if (p->data == item )// 检查自己 // 出口三
    {
        if (p==root)
        {
            cout<<"要查找元素为根！"<<endl;// 不存在父亲结点
            isFind = false;
            return isFind;
        }
        else
        {
            isFind = true;
            // 这里ret还不能赋值，只能在递归出去后才能继续得到父亲的指针
            return isFind;
        }
    }
    for (CSNode<T>* pNode = p->firstchild;pNode;pNode=pNode->nextsibling)// 出口五 + 入口
    {
        getParent(pNode,item,isFind,isFirst,ret); // 入口一
        if (isFind && isFirst)// 检查递归回来的结果，如果找到，就为ret赋值，因为递归在回退的时候，isFind仍也是true，不引入isFirst就还会对结点ret赋值，即一路回退一路赋值。如果要避免这种情况，则需要引入这个变量，只有第一次出现这个情况的时候才为其赋值
        {
            isFirst = false;
            ret = p;
            return isFind;
        }
    }
    return isFind;
}

template<class T>
int CSTree<T>::getChildrenCount(const CSNode<T>* p)const
{
    int count= 0;
    for (CSNode<T>* pNode = p->firstchild;pNode;pNode=pNode->nextsibling)// 计算本结点的度
    {
        count++;
    }
    return count;
}

template<class T>
void CSTree<T>::getAllParentCSTree(T item)const
{
    CSNode<T>* path[MaxCSTreeSize];// 存放树的结点
    int len=0;// 递归的层次
    int seat=0;// 结点有多少双亲
    bool isFind = false;// 是否已经找到
    getAllParent(item,root,path,seat,isFind);
    if (isFind)
    {
        for (int i=0;i<seat;i++)
        {
            cout<<path[i]->data;
        }
        cout<<endl;
    }
    else
    {
        cout<<"没有找到结点!"<<item<<endl;
    }
}
// 可以直接使用转换二叉树后的先序遍历。
/*思想:
使用一个数组保存双亲结点
双亲放入数组，之后当处理检查完自己和左子树的时候，如果没找到，则这个结点就不会再是其双亲结点，直接把该结点移除数组。
*/
template<class T>
bool CSTree<T>::getAllParent(T item,CSNode<T>* p,CSNode<T>* path[MaxCSTreeSize],int& seat,bool& isFind)const // 需要思考啊
{
    if (isFind)// 找到直接返回
    {
        return isFind;
    }
    if (!p)// 空子树不进数组
    {
        return false;
    }
    if (p->data==item)// 找着的这个数也不进入数组
    {
        isFind = true;
        return isFind;
    }
    else
    {
        path[seat++]=p;
        getAllParent(item,p->firstchild,path,seat,isFind);
        if (!isFind)
        {
            seat=seat-1;// 减1是因为：ath[seat++]=p;这里seat是指向下一个要存放的位置，这里当执行到这一步的时候，说明左面的兄弟没找到，因为他们两个是兄弟关系，这时要把最左边的孩子移除去，这里只需还让右孩子放到左孩子的位置直接覆盖就好
            getAllParent(item,p->nextsibling,path,seat,isFind);
        }
    }
    return isFind;
}

template<class T>
bool CSTree<T>::getAllSibling(const T item) const
{
    // 先找到自己的父亲
    bool isFind = false;
    CSNode<T>* ret = nullptr;
    isFind = getParentCSTree(item,ret);
    if (!isFind)
    {
        // 没找到父亲
        return false;
    }
    else
    {
        isFind=false;// 同时利用isFind表示是不是找到了自己的兄弟
        for (CSNode<T>*pNode = ret->firstchild;pNode;pNode=pNode->nextsibling)
        {
            if (pNode->data!=item)
            {
                isFind = true;
                cout<<pNode->data<<" ";// 兄弟从左到右依次为：
            }

        }
        return isFind;
    }
}

template<class T>
bool CSTree<T>::getAllChildren(T item)
{
    bool isFind = false;
    CSNode<T> *parent = nullptr;
    findCSNode(item,parent);
    for (CSNode<T>*pNode = parent->firstchild;pNode;pNode=pNode->nextsibling)
    {
        isFind = true;// 有孩子
        cout<<pNode->data<<" ";// 兄弟从左到右依次为：
    }
    return isFind;
}
