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

//    QStack<QVector<QVector2D>> coorstack;

    float radius;
    float height;

    // 计算方差
    float variance (QVector<float> vector);
    // 计算斜率的方差
    float calculate_k(QVector<QVector2D> vec);
    // 计算斜率的均值
    float calculate_meanK(QVector<QVector2D> vec, int &num);


    // 识别类型
    bool recognize_cylinder(QVector<float> vec);
    bool recognize_straightLine(QVector<float> vec);
    bool recognize_curveLine(QVector<float> vec);

    // 记录结点的坐标值
    QVector2D cylinder_left,cylinder_right;
    QVector2D straightLine_first, straightLine_end;
    QVector2D curveLine_first, curveLine_end;

    // 记录类型
    QString str_1, str_2;

    // 相互关系: 平行，相连, 相离，垂直,等长
    bool parallel(QVector<float> _vec1, QVector<float> _vec2);
    bool equilong(QVector<float> _vec1, QVector<float> _vec2);

    bool join(QString str_1, QString str_2);
    bool separation(QString str_1, QString str_2);
    bool verticality(QString str_1, QString str_2);

    // 记录关系的表结构
    QVector<QVector<QString>> vec;

};

#endif // RECOGNIZECYLINDER_H
