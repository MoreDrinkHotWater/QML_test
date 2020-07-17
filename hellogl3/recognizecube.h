#ifndef RECOGNIZECUBE_H
#define RECOGNIZECUBE_H

#include <QOpenGLFunctions>

class RecognizeCube
{
public:
    explicit RecognizeCube() = default;
    ~RecognizeCube() = default;

    bool recognize_cube();
    void draw_cube();

    QVector<GLfloat> cube_vector;
//    QVector<GLfloat> cube_up;
//    QVector<GLfloat> cube_down;
};

#endif // RECOGNIZECUBE_H
