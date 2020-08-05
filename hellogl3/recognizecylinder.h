#ifndef RECOGNIZECYLINDER_H
#define RECOGNIZECYLINDER_H

#include <QStack>
#include <QVector2D>

class Common;

class Recognizecylinder
{
public:
    Recognizecylinder();

public:
    bool recognize_cylinder_shape(QStack<QVector<float>> draw_coorstack);

    QStack<QVector<float>> draw_coorstack;

    float radius;
    float height;

    // 识别类型
    bool recognize_cylinder(QVector<float> vec);
    bool recognize_straightLine(QVector<float> vec);
    bool recognize_curveLine(QVector<float> vec);

    // 查找结点
    void find_cylinderNode(QVector<float> vec);
    void find_straightLineNode(QVector<float> vec);
    void find_curveLineNode(QVector<float> vec);

    // 记录结点的坐标值
    QVector2D cylinder_left,cylinder_right;
    QVector2D straightLine_first, straightLine_end;
    QVector2D curveLine_first, curveLine_end;

    // 记录类型
    QString str_1, str_2;

//    bool parallel(QVector<float> _vec1, QVector<float> _vec2);
//    bool equilong(QVector<float> _vec1, QVector<float> _vec2);

//    bool join(QString str_1, QString str_2);
//    bool separation(QString str_1, QString str_2);
//    bool verticality(QString str_1, QString str_2);

    // 相互关系: 平行，相连, 相离，垂直,等长
    bool parallel(QVector<float> _vec1, QVector<float> _vec2);
    bool equilong(QVector<float> _vec1, QVector<float> _vec2);

    bool join(QString str_1, QString str_2, QVector<float> _vec1, QVector<float> _vec2);
    bool separation(QString str_1, QString str_2, QVector<float> _vec1, QVector<float> _vec2);
    bool verticality(QString str_1, QString str_2, QVector<float> _vec1, QVector<float> _vec2);

    // 记录关系的表结构
    QVector<QVector<QString>> vec;

    // 保存类型
    QVector<QString> type_vec;

private:
    Common *common;

};

#endif // RECOGNIZECYLINDER_H
