#ifndef RECOGNIZECYLINDER_H
#define RECOGNIZECYLINDER_H

#include <QStack>
#include <QVector2D>

class Recognizecylinder
{
public:
    Recognizecylinder();

public:
    bool recognize_cylinder_shape(QStack<QVector<float>> draw_coorstack);

    QStack<QVector<float>> draw_coorstack;

    float radius;
    float height;

    // 计算方差
    float variance (QVector<float> vector);

    bool recognize_cylinder(QVector<float> vec);
    bool recognize_straightLine(QVector<float> vec);
    bool recognize_curveLine(QVector<float> vec);

    // 记录结点的坐标值
    QVector2D cylinder_left,cylinder_right;
};

#endif // RECOGNIZECYLINDER_H
