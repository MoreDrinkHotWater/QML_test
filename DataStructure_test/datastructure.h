#ifndef DATASTRUCTURE_TEST_H
#define DATASTRUCTURE_TEST_H

#include <QStack>
#include <QVector2D>
#include <iostream>

using namespace std;
const int MaxCSTreeSize = 20;

template<class T>
class CSNode
{
public:
    T data;
    CSNode* firstchild;
    CSNode* nextsibling;
public:
    CSNode();
    CSNode(CSNode* fchild,CSNode* nextsibling,T newdata);
    CSNode* getFirstChild();
    CSNode* getNextSibling();
    T getData();
    void setFirstChild(T newData);
    void setNextsibling(T newData);
    void showInputChild();// 显示本节点并请求输入它的孩子
};

template<class T>
class CSTree
{
public:
    CSTree();
    CSTree(const CSTree<T>& csTree);
    ~CSTree();
    const CSTree<T>& operator=(const CSTree<T>& csTree);
    void  createCSTree(); // 按建立树
    void  InitCSTree();   // 初始化树
    void  destoryCSTree();// 销毁树
    bool  isEmptyCSTree();// 检查树是否为空
    void  preOrderTraverse();// 先序遍历
    void  postOrderTraverse(); // 后序遍历
    void  levelOrderTraverse();// 层序遍历
    int   heightCSTree();// 树高度
    int   widthCSTree(); // 树宽度
    int   getDegreeCSTree();// 树的度--树中所有结点度的最大值
    int   nodeCountCSTree();  // 树结点个数
    int   LeavesCountCSTree();// 树叶子个数
    int   nodeLevelCSTree(T item);// 结点item在的层次
    int   getChildrenCount(const CSNode<T>* p)const;// 返回结点孩子个数
    void  getAllParentCSTree(T item)const;// 找item的所有祖先
    void  longPathCSNode();// 输出从每个叶子结点到根结点的最长路径 ----- 未实现
    bool  findCSNode(const T item,CSNode<T>*& ret)const; // 查找结点
    bool  getParentCSTree(const T item,CSNode<T>*& ret)const;// 查找结点item的父亲结点
    bool  getleftChild(const CSNode<T>* p,CSNode<T>*& ret) const;   // 返回最左边的兄弟
    bool  getrightSibling(const CSNode<T>* p,CSNode<T>*& ret) const;  // 返回最右边的兄弟
    bool  getAllSibling(const T item) const; // 输出所有兄弟
    bool  getAllChildren(T item);// 输出所有的孩子
private:
    void create(CSNode<T>*& p); // 以p为根创建子树
    void copyTree(CSNode<T>*& copyTreeRoot,CSNode<T>* otherTreeRoot);
    // 把以otherTreeRoot为根节点的部分拷贝到copyTreeRoot为根节点的部分
    void  destory(CSNode<T>*& p,int& num);
    // 销毁以p为根节点的部分
    void  preOrder(CSNode<T>* p);
    // 先序遍历以p为根节点的部分
    void  postOrder(CSNode<T>* p);
    // 后序遍历以p为根节点的部分
    void  levelOrder(CSNode<T>* p);
    // 层次遍历以p为根节点的部分
    int   height(CSNode<T>* p);
    // 计算以p为根节点的高度
    int   width(CSNode<T>* p);
    // 计算以p为根子树的宽度
    int   nodeCount(CSNode<T>* p);
    // 计算以p为根节点的结点个数
    int   leavesCount(CSNode<T>* p);
    // 计算以p为根节点的叶子个数
    void  nodeLevel(T item,CSNode<T>* p,int level,int& nlevel);
    // 计算以p为根节点的中item所在层次，如有多个元素，则遇到第一个则返回（离根最近），如果没有出现，则返回0
    bool  find(CSNode<T>*p,const T item,bool& isFind,CSNode<T>*& cur)const;
    // 在p指向的树中，返回 值为item的指针
    bool  getParent(CSNode<T>*p,const T item,bool& isFind,bool& isFirst,CSNode<T>*& ret)const;
    // 在p指向子树中，找item的父亲
    bool getAllParent(T item,CSNode<T>* p,CSNode<T>* path[MaxCSTreeSize],int& seat,bool& isFind)const;
    // 找item的所有祖先，seat表示最后一个父亲的下标
    void longPath(CSNode<T>* p,int len,int& maxLen,CSNode<T>*& longNode);
    // 输出从每个叶子结点到根结点的最长路径
    int getDegree(CSNode<T>* p);
    // 输出树的度（书中结点的孩子最大值）
private:
    CSNode<T>* root;

    CSNode<int>* _root;
};

#endif //  DATASTRUCTURE_TEST_H
