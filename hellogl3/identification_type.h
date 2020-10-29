#ifndef IDENTIFICATION_TYPE_H
#define IDENTIFICATION_TYPE_H
#include <QVector2D>
#include <QVector>

class Common;

class Identification_type
{
public:
    Identification_type();
    ~Identification_type() = default;

    static Identification_type *getInstance();

    int jundge_cylinder(QVector<QVector2D> head_circle);

    // 识别类型
    // 椭圆
    bool recognize_cylinder(QVector<float> vec);
    // 直线
    bool recognize_straightLine(QVector<float> vec);
    // 曲线
    bool recognize_curveLine(QVector<float> vec);
    // 波浪线
    bool recognize_wavyLine(QVector<float> vec);
    // 琦角
    bool recognize_corner(QVector<float> vec);
    // 花生
    bool recognize_peanut(QVector<float> vec);

    float radius;

    // 临时需要用的变量
    QVector<float> xcoor_vector,ycoor_vector;

private:
    Common *common;

    float circle_width, circle_height;
};

#endif // IDENTIFICATION_TYPE_H
