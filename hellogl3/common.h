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

};

#endif // COMMON_H
