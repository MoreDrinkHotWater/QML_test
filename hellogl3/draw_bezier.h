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

    static QStack<QVector<float>> bezierCurve_stack;

public slots:
    void receiver_bezierSlot(QStack<QVector<QVector3D>> draw_stack);

private:
    static void draw_bezier(void);

    static void winReshapeFcn(int newWidth, int newHeight);

    static QStack<QVector<QVector3D>> draw_stack;

    static void bezier(wcPt3D *ctrlPts, GLint nCtrlPts, GLint nBezCurvePts);

    static void binomialCoeffs(GLint n, GLint *C);

    static void spline_subdivision(QVector<QVector3D> draw_vector);

    static void computeBezpt(GLfloat u, wcPt3D *bezPt, GLint nCtrlPts, wcPt3D *ctrlPts, GLint *C);

    static void keyboard(unsigned char key, int x, int y);

    static QVector<float> bezierCurve_vector;
};

#endif // DRAW_BEZIER_H
