#ifndef RECOGNIZE_STOOL_H
#define RECOGNIZE_STOOL_H

#include <QVector>
#include <QStack>
#include <QVector2D>

class Common;

class Recognizecylinder;

class Recognizecorner;

class Identification_type;

class Recognize_stool
{
public:
    Recognize_stool();

    ~Recognize_stool() = default;

    static Recognize_stool *getInstance();

    bool recognize_stool(QStack<QVector<float>> draw_stack);

    QStack<QVector<float>> draw_coorstack;

private:
    Common *common;

    Recognizecylinder *recognizecylinder;

    Recognizecorner *recognizecorner;

    Identification_type *identificationtypes;

public:

    float radius,height;

    // 圆柱体的中心(2D和3D一样不会改变)
    QVector2D  cylinder_center;

    // 底角
    QStack<QVector<QVector2D>> stool_bottom_stack;

};

#endif // RECOGNIZE_STOOL_H
