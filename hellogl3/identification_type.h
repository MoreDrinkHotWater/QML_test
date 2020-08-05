#ifndef IDENTIFICATION_TYPE_H
#define IDENTIFICATION_TYPE_H
#include <QVector2D>

class Common;

class Identification_type
{
public:
    Identification_type();
    ~Identification_type() = default;

    static Identification_type *getInstance();

    // 识别类型
    bool recognize_cylinder(QVector<float> vec);
    bool recognize_straightLine(QVector<float> vec);
    bool recognize_curveLine(QVector<float> vec);

    float radius;

    // 记录结点的坐标值
    QVector2D cylinder_left,cylinder_right;

    QVector2D straightLine_first, straightLine_end;

    QVector2D curveLine_first, curveLine_end;

private:
    Common *common;
};

#endif // IDENTIFICATION_TYPE_H
