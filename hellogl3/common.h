#ifndef COMMON_H
#define COMMON_H

#include <QStack>
#include <QVector2D>

class Common
{
public:
    Common();

    ~Common() = default;

    static Common *getInstance();

    // 计算方差
    float variance (QVector<float> vector);

    // 计算斜率的方差
    float calculate_k(QVector<QVector2D> vec);

    // 计算斜率的均值
    float calculate_meanK(QVector<QVector2D> vec, int &num);

    // 坐标转换
    QStack<QVector<float>> coordinate_transformation(QStack<QVector<float>> draw_stack);

    // 查找最大，最小值
    void findMinMax(QVector<QVector2D> head_path, QVector2D &min,QVector2D &max);

    // 计算倾斜度
    float mapEllipseToCircle(QVector<QVector2D> &head_path);

    // 计算多边形面积
    float calculateArea(QVector<QVector2D> &vec);

    // 三角化
    void genTriangle(QVector<float> &vec,QVector3D p0,QVector3D p1,QVector3D p2);

    // 斜角点
    void genRectangleZ(QVector<float> &vec,QVector3D p0,QVector3D p1);

};

#endif // COMMON_H
