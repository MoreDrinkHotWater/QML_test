#ifndef RECOGNIZE_DESKLAMP_H
#define RECOGNIZE_DESKLAMP_H

#include <QVector>
#include <QStack>
#include <QVector2D>

class Common;

class Recognizecylinder;

class Recognizecorner;

class Identification_type;

class Recognize_deskLamp
{
public:
    Recognize_deskLamp();

    ~Recognize_deskLamp() = default;

    static Recognize_deskLamp *getInstance();

    bool recognize_deskLamp(QStack<QVector<float>> draw_stack);

    QStack<QVector<float>> draw_coorstack;

private:
    Common *common;

    // 识别椭圆
    Recognizecylinder *recognizecylinder;

    Recognizecorner *recognizecorner;

    Identification_type *identificationtypes;

public:

    // 台灯的底座高度 deskLamp_bottom_height
    float deskLamp_bottom_radius,deskLamp_bottom_height;

    // 底座和灯罩的偏移值
    float offset_center;

    QVector2D cylinder_center;

    float radius,height_1,height_2;

    // 识别灯罩
    QVector<QVector2D> head_vector_top, wavyline_vector_1, wavyline_vector_2;

    // 保存琦角数据的数组
    QVector<QVector2D> cornerLine_vector;
};

#endif // RECOGNIZE_DESKLAMP_H
