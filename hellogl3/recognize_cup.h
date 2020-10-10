#ifndef RECOGNIZE_CUP_H
#define RECOGNIZE_CUP_H

#include <QVector>
#include <QStack>
#include <QVector2D>

class Common;

class Recognizecylinder;

class Recognizecorner;

class Identification_type;

class Recognize_cup
{
public:
    Recognize_cup();

    ~Recognize_cup() = default;

    static Recognize_cup *getInstance();

    bool recognize_cup(QStack<QVector<float>> draw_stack);

    QStack<QVector<float>> draw_coorstack;

private:
    Common *common;

    // 识别椭圆
    Recognizecylinder *recognizecylinder;

    Recognizecorner *recognizecorner;

    Identification_type *identificationtypes;

public:

    // 保存花生和直线交点的数组
    QVector<QVector2D> intersection_vector;

    // 圆柱体的中心(2D和3D一样不会改变)
    QVector2D  cylinder_center;

    float radius,height_1;

    // 保存花生数据的数组
    QVector<QVector2D> peanutLine_vector;
};

#endif // RECOGNIZE_CUP_H
