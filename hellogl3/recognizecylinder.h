#ifndef RECOGNIZECYLINDER_H
#define RECOGNIZECYLINDER_H

#include <QStack>
#include <QVector2D>

class Common;

class Identification_relation;

class Identification_type;

class Recognizecylinder
{
public:
    Recognizecylinder();

public:
    bool recognize_cylinder_shape(QStack<QVector<float>> draw_coorstack);

    QStack<QVector<float>> draw_coorstack;

    float radius;
    float height;

    // 记录结点的坐标值
    QVector2D cylinder_left,cylinder_right;
    QVector2D straightLine_first, straightLine_end;
    QVector2D curveLine_first, curveLine_end;

    // 记录类型
    QString str_1, str_2;

    // 记录关系的表结构
    QVector<QVector<QString>> vec;

    // 保存类型
    QVector<QString> type_vec;

private:
    Common *common;
    Identification_relation *identification_relation;
    Identification_type *identification_type;

};

#endif // RECOGNIZECYLINDER_H
