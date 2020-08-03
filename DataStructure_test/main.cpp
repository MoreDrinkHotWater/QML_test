#include <iostream>
#include "datastructure_test.h"
#include "datastructure_test.cpp"

#include <stdio.h>
#include <stdlib.h>
int main()
{
    /* ----测试赋值函数
    CSTree<char> csTree;
    CSTree<char> csTree1;
    std::cout<<"建立树，以##表示结束:"<<std::endl;
    csTree1.createCSTree();
    csTree = csTree1;//调用赋值函数
    */

    //测试赋值构造函数
    CSTree<char> csTree1;
    std::cout<<"建立树，以##表示结束:"<<std::endl;
    csTree1.createCSTree();
    CSTree<char> csTree = csTree1;//调用拷贝构造函数

    //---一般操作
    std::cout<<"先序遍历为:";
    csTree.preOrderTraverse();
    std::cout<<"后序遍历为:";
    csTree.postOrderTraverse();
    std::cout<<"层次遍历为:";
    csTree.levelOrderTraverse();
    std::cout<<"树的高度为："<<csTree.heightCSTree()<<std::endl;
    std::cout<<"树的宽度为："<<csTree.widthCSTree()<<std::endl;
    std::cout<<"树的度为："<<csTree.getDegreeCSTree()<<std::endl;
    std::cout<<"树的结点个数为："<<csTree.nodeCountCSTree()<<std::endl;
    std::cout<<"树的叶子结点为："<<csTree.LeavesCountCSTree()<<std::endl;
    char item='f';
    CSNode<char>* cur=nullptr;
    if (csTree.findCSNode(item,cur))
    {
        std::cout<<item<<"的层次："<<csTree.nodeLevelCSTree(item)<<std::endl;

        std::cout<<item<<"的祖先为:";
        csTree.getAllParentCSTree(item);

        CSNode<char>* ret=nullptr;
        if (csTree.getParentCSTree(item,ret))
        {
            std::cout<<item<<"的父亲为:"<<ret->getData()<<std::endl;
        }
        else
        {
            std::cout<<item<<"没有双亲！"<<std::endl;
        }
        if (csTree.getleftChild(cur,ret))
        {
            if (ret->data == cur->data)
            {
                std::cout<<item<<"的最左的兄弟为自己！"<<std::endl;
            }
            else
            {
                std::cout<<item<<"的最左的兄弟为:"<<ret->getData()<<std::endl;
            }
        }
        else
        {
            std::cout<<item<<"没有最左的兄弟！"<<std::endl;
        }
        if (csTree.getrightSibling(cur,ret))
        {
            if (ret->data == cur->data)
            {
                std::cout<<item<<"的最右的兄弟为自己！"<<std::endl;
            }
            else
            {
                std::cout<<item<<"的最右边的兄弟为:"<<ret->getData()<<std::endl;
            }
        }
        else
        {
            std::cout<<item<<"没有最右边的兄弟"<<std::endl;
        }
        std::cout<<item<<"的所有兄弟为:";
        bool isFind = csTree.getAllSibling(item);
        std::cout<<std::endl;
        if (!isFind)
        {
            std::cout<<item<<"没有兄弟！"<<std::endl;
        }
        std::cout<<item<<"的所有孩子为:";
        isFind =csTree.getAllChildren(item);
        std::cout<<std::endl;
        if (!isFind)
        {
            std::cout<<item<<"没有孩子！"<<std::endl;
        }
    }
    else
    {
        std::cout<<"结点"<<item<<"不存在！"<<std::endl;
    }
    csTree.destoryCSTree();
    system("pause");
    return 1;
}
