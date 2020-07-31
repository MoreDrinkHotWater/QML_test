#ifndef DATASTRUCTURE_TEST_H
#define DATASTRUCTURE_TEST_H

#include <QStack>
#include <QVector2D>

typedef QStack<QVector<float>> stack;
typedef QVector<float> leaf;

typedef struct Root *PtrToNode;
typedef struct Stack *StackNode;
typedef struct Leaf *LeafNode;

struct Root
{

    PtrToNode FirstChild; // 根结点
    PtrToNode NextSibling;

    Root()
    {
        FirstChild = nullptr;
        FirstChild = nullptr;
    }
};

struct Stack
{
    stack data;
    StackNode firstChild;
    StackNode nextBrother;
};

struct Leaf
{
    LeafNode data;
};

// 椭圆
struct Cylinder
{
    leaf data;
};

// 直线
struct StraightLine
{

};

// 曲线
struct CurveLine
{

};

class DataStructure_test
{
public:
    DataStructure_test();
};

#endif // DATASTRUCTURE_TEST_H
