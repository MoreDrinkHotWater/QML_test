#ifndef DRAW_BEZIER_H
#define DRAW_BEZIER_H

#include <QObject>
#include <QVector3D>
#include <GL/glut.h>

class Draw_bezier : public QObject
{
    Q_OBJECT
public:
    explicit Draw_bezier() = default;

    ~Draw_bezier();

    static Draw_bezier *getInstance();

    QVector<float> bezierCurve_vector;

public slots:
    void receiver_bezierSlot(QVector<QVector3D> draw_vector);

private:
    static void draw_bezier(void);

    static void winReshapeFcn(int newWidth, int newHeight);

    static QVector<QVector3D> draw_vector;

    static void spline_subdivision(QVector<QVector3D> draw_vector);
};

#endif // DRAW_BEZIER_H
