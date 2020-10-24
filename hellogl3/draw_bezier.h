#ifndef DRAW_BEZIER_H
#define DRAW_BEZIER_H

#include <QObject>
#include <QVector3D>
#include <GL/glut.h>

class wcPt3D
{
public:
    GLfloat x,y,z;
};

class Draw_bezier : public QObject
{
    Q_OBJECT
public:
    explicit Draw_bezier() = default;

    ~Draw_bezier();

    static Draw_bezier *getInstance();

    static QVector<float> bezierCurve_vector;

public slots:
    void receiver_bezierSlot(QVector<QVector3D> draw_vector);

private:
    static void draw_bezier(void);

    static void winReshapeFcn(int newWidth, int newHeight);

    static QVector<QVector3D> draw_vector;

    static void bezier(wcPt3D *ctrlPts, GLint nCtrlPts, GLint nBezCurvePts);

    static void binomialCoeffs(GLint n, GLint *C);

    static void spline_subdivision(QVector<QVector3D> draw_vector);

    static void computeBezpt(GLfloat u, wcPt3D *bezPt, GLint nCtrlPts, wcPt3D *ctrlPts, GLint *C);

    static void keyboard(unsigned char key, int x, int y);
};

#endif // DRAW_BEZIER_H
