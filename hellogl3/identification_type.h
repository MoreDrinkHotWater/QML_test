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
    bool recognize_wavyLine(QVector<float> vec);

    float radius;

private:
    Common *common;
};

#endif // IDENTIFICATION_TYPE_H
