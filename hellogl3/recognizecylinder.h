#ifndef RECOGNIZECYLINDER_H
#define RECOGNIZECYLINDER_H

#include <QStack>

class Recognizecylinder
{
public:
    Recognizecylinder();

public:
    bool recognizecy_linder(QStack<QVector<float>> draw_coorstack);

    QStack<QVector<float>> draw_coorstack;

    float radius;
    float height;
};

#endif // RECOGNIZECYLINDER_H
