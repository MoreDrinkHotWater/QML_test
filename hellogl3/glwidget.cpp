/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include "recognizecube.h"
#include "recognizecylinder.h"
#include "recognizecorner.h"
#include "common.h"
#include "mainwindow.h"
#include "identification_type.h"

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <random>
#include <algorithm>
#include <iostream>
#include <window.h>
#include <fstream>
#include <QMessageBox>
#include <QTimer>

// 产生随机数据
#include <cstdlib>
#include <ctime>
#define random(a,b) (rand()%(b-a)+a)


bool GLWidget::m_transparent = false;

extern bool flag;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      m_program(0),
      appendingFlag(false),
      m_cameraZ(-15),
      recognizeCube(new RecognizeCube()),
      recognize_cube(false),
      recognizecylinder(new Recognizecylinder()),
      recognizecorner(new Recognizecorner()),
      common(new Common()),
      identificationtypes(new Identification_type()),
      off_var(0),
      peanut_offset_x(0)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.

    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }

    srand((int)time(nullptr));  // 产生随机种子
}

GLWidget::~GLWidget()
{
    cleanup();
}

// 设定画布缩放的最小尺寸
QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

// 设定画布的初始大小
QSize GLWidget::sizeHint() const
{
    return QSize(500, 500);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::cleanup()
{
    if (m_program == nullptr)
        return;
    makeCurrent();
    m_logoVbo.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

// 顶点着色器（指定几何形状的顶点） 内置变量： gl_Position = projMatrix * mvMatrix * vertex;
//            变换后的位置 = 投影矩阵 × 模型视图变换矩阵 × 顶点的坐标

static const char *vertexShaderSourceCore =
        "#version 150\n"
        "in vec4 vertex;\n"
        "in vec3 normal;\n"
        "out vec3 vert;\n"
        "out vec3 vertNormal;\n"
        "uniform mat4 projMatrix;\n"
        "uniform mat4 mvMatrix;\n"
        "uniform mat3 normalMatrix;\n"
        "void main() {\n"
        "   vert = vertex.xyz;\n"
        "   vertNormal = normalMatrix * normal;\n"
        "   gl_Position = projMatrix * mvMatrix * vertex;\n"
        "}\n";

static const char *vertexShaderSource =
        "#version 150\n"
        "attribute vec4 vertex;\n"
        "attribute vec3 normal;\n"
        "varying vec3 vert;\n"
        "varying vec3 vertNormal;\n"
        "uniform mat4 projMatrix;\n"
        "uniform mat4 mvMatrix;\n"
        "uniform mat3 normalMatrix;\n"
        "void main() {\n"
        "   vert = vertex.xyz;\n"
        "   vertNormal = normalMatrix * normal;\n"
        "   gl_Position = projMatrix * mvMatrix * vertex;\n"
        "}\n";


// 片元着色器（指定每个顶点的着色） 内置颜色变量： fragColor = vec4(col, 1.0)

static const char *fragmentShaderSourceCore =
        "#version 150\n"
        "in highp vec3 vert;\n"
        "in highp vec3 vertNormal;\n"
        "out highp vec4 fragColor;\n"
        "uniform highp vec3 lightPos;\n"
        "void main() {\n"
        "   highp vec3 L = normalize(lightPos - vert);\n"
        "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
        "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
        "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
        "   fragColor = vec4(1.0,0,0, 1.0); \n"
        "}\n";

static const char *fragmentShaderSource =
        "#version 150\n"
        "varying highp vec3 vert;\n"
        "varying highp vec3 vertNormal;\n"
        "uniform highp vec3 lightPos;\n"

        "void main() {\n"
        "   highp vec3 L = normalize(lightPos - vert);\n"
        "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
        "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
        "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
        "   gl_FragColor = vec4(col, 1.0); // vec4(1.0,0,0, 1.0);\n"
        "}\n";

// 设置OpenGL资源和状态
void GLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.

    // loader data
    // loader_data();

    // flag 决定是否渲染加载的数据
    // true 即 渲染三角面
    // false 即 渲染线
    flag = false;

    appendingFlag = true;

    //    recognition();

    //    draw_cylinder();

    // 调用 recognizeCube 时，只要开启注释就可啦

    //        if(recognizeCube->recognize_cube())
    //        {
    //            recognize_cube = true;
    //        }

    // OpenGL的状态机： 设置上下文（context）
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    // 初始化状态设置函数，这类函数将会改变上下文
    initializeOpenGLFunctions();

    // 设置背景色
    //    glClearColor(0, 0, 0, m_transparent ? 0 : 1);
    glClearColor(0, 0.5, 1, 1);


    // 创建 OpenGL 程序
    m_program = new QOpenGLShaderProgram;

    // 装载顶点着色器和片元着色器，从source. 此处 m_core = false
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);

    //    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, textureShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);

    // 将属性名称绑定到指定位置。这个函数可以在程序链接之前或之后调用。
    // 在链接程序时没有显式绑定的任何属性将自动分配位置
    // （当该函数在程序链接后被调用时，需要重新链接该程序以使更改生效）
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);

    // 链接程序
    m_program->link();

    // 相当于在programId()上调用glUseProgram()，如果程序被成功绑定，则返回true;否则错误。
    // 如果着色程序还没有被链接，或者需要重新链接，这个函数将调用link()。
    m_program->bind();

    // 设置着色程序的变量
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");

    // 创建顶点数组对象
    //    if(m_vao.isCreated())
    //        QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    //    else
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // 设置顶点缓冲区对象
    m_logoVbo.create();
    m_logoVbo.bind();

    // 分配内存
    if(temp.count() != 0 && temp_onlyVertex.count() != 0)
    {
        // GL_TRIANGLES(loader)
        if(flag)
        {
            m_logoVbo.allocate(temp.constData(), temp.count() * sizeof(GLfloat));
        }
        // GL_LINES(loader)
        else
        {
            m_logoVbo.allocate(temp_onlyVertex.constData(), temp_onlyVertex.count() * sizeof(GLfloat));
        }

    }
    else if(recognize_cube)
    {
        m_logoVbo.allocate(recognizeCube->cube_vector.constData(), recognizeCube->cube_vector.count() * sizeof(GLfloat));
    }
    // GL_TRIANGLES(origin_data)
    else
    {
        m_logoVbo.allocate(m_logo.constData(), m_logo.count() * sizeof(GLfloat));
    }

    // 设定顶点的属性
    setupVertexAttribs();

    // 设置相机，且相机为单位矩阵（Our camera never changes in this example.）
    m_camera.setToIdentity();
    m_camera.translate(0, 0, -15);

    // 固定灯光 （Light position is fixed.）
    //  函数 setUniformValue()：为上下文的变量赋值
    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 70));

    // 释放活动的着色程序。这相当于调用glUseProgram(0)。
    m_program->release();
}

void GLWidget::allocate_vector()
{
    // 将与此对象关联的缓冲区绑定到当前OpenGL上下文。不加这个无法分配成功
    m_logoVbo.bind();

    m_logoVbo.allocate(cylinder_vector.constData(), cylinder_vector.count() * sizeof(GLfloat));

    m_logoVbo.release();

    this->appendingFlag = true;

    setupVertexAttribs();

}

void GLWidget::setupVertexAttribs()
{
    // 着色管线装配： 将应用程序的数据与着色器程序的变量关联起来
    m_logoVbo.bind();
    // 获取此上下文的 QOpenGLFunctions 实例
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // 指定要启用（或禁用）的通用顶点属性的索引
    f->glEnableVertexAttribArray(0);

    if(flag && temp.count() != 0)
    {
        f->glEnableVertexAttribArray(1);

        // 顶点的偏移量
        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

        // 法向量的偏移量
        f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    }

    else if (flag == false && temp_onlyVertex.count() != 0)
    {
        // 顶点的偏移量
        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    }

    // set draw_vector
    if(appendingFlag)
    {
        f->glEnableVertexAttribArray(1);

        // 顶点的偏移量
        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

        // 法向量的偏移量
        f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

        //        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    }
    else
    {
        // set cube_vector
        if(recognize_cube)
        {
            f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        }
    }

    m_logoVbo.release();

}

// 渲染OpenGL场景
void GLWidget::paintGL()
{
    // 清空缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    // 开启背面剔除
    glEnable(GL_CULL_FACE);

    glEnable(GL_LINE_SMOOTH); //开启线的反走样

    // 控制旋转
    m_world.setToIdentity();
    m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

    // 绑定 OpenGL 顶点数组对象
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);

    m_camera.setToIdentity();
    m_camera.translate(0, 0, m_cameraZ);

    m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    if(temp.count() != 0 && temp_onlyVertex.count() != 0)
    {

        // GL_POINTS ： 传入渲染管线的一系列顶点单独进行绘制。

        // GL_LINES : 传入渲染管线的一系列顶点按照顺序两两组织成线段进行绘制，若顶点个数为奇数，管线会自动忽略最后一个顶点。
        // GL_LINE_STRIP : 传入渲染管线的一系列顶点按照顺序依次组织成线段进行绘制。
        // GL_LINE_LOOP ： 传入渲染管线的一系列顶点按照顺序依次组织成线段进行绘制，最后一个顶点与第一个顶点相连。

        // GL_TRIANGLES ： 传入渲染管线的一系列顶点按照每3个组织成一个三角形进行绘制。
        // GL_TRIANGLES_STRIP ： 传入渲染管线的一系列顶点按照依次组织成三角形进行绘制，最后实际形成的是一个三角形条带。
        // GL_TRIANGLES_FAN ： 传入渲染管线的一系列顶点中第一个顶点作为中心点，其他顶点作为边缘点绘制出一系列形成扇形的相邻三角形。

        // 采用索引法进行绘制时可以有效地减少重复顶点数据，有重复时只需要提供重复的索引号就可
        // 顶点法： glDrawArrays 索引法： glDrawElements

        if(flag)
        {
            glDrawArrays(GL_TRIANGLES, 0, temp.count() / 6 );
        }

        else
        {
            glDrawArrays(GL_LINES, 0, temp_onlyVertex.count() / 3 );
        }

    }

    else if(appendingFlag)
    {
        //        std::cout<<"=====================test3=================="<<std::endl;
        //        glDrawArrays(GL_LINES, 0, cylinder_vector.count() / 3 );

//        std::cout<<"draw cylinder_vector.size: "<<cylinder_vector.size()<<std::endl;

        glDrawArrays(GL_TRIANGLES, 0, cylinder_vector.count() / 6 );
    }

    else if(recognize_cube)
    {
        glDrawArrays(GL_LINE_STRIP, 0, recognizeCube->cube_vector.count() / 3 );
    }

    else
    {
        // 渲染对象
        glDrawArrays(GL_TRIANGLES, 0, m_logo.vertexCount());
    }

    // 从当前的 QOpenGLContext 中释放活动的着色程序。这相当于调用glUseProgram(0)。
    m_program->release();
}

// 设置OpenGL视口、投影等
void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    // 获得鼠标点击的坐标
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    // 鼠标移动触发的事件处理
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);

    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }

    m_lastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0){                    // 当滚轮远离使用者时
        m_cameraZ += 0.5;                // 进行放大
        update();
    }else{                                     // 当滚轮向使用者方向旋转时
        m_cameraZ -= 0.5;            // 进行缩小
        update();
    }
}

void GLWidget::loader_data()
{
    // cube circle snake cylinder
    QString filePath = "/home/damon/Qt_Project/hellogl2_build/cylinder.stl";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "warning", "Can't open file!");
        return ;
    }

    QString lineString;
    QTextStream in(&file);

    QString headStr = in.readLine();

    if (headStr == "")
        return;

    // ReadASCII
    if (headStr[0] == 's')
    {
        qDebug() <<"ReadASCII";
    }

    // ReadBinary
    else
    {
        qDebug() <<"ReadBinary";

        std::ifstream in(filePath.toStdString(), std::ifstream::in | std::ifstream::binary);

        char str[80];

        in.read(str, 80);

        //number of triangles
        int triangles;
        in.read((char*)&triangles, sizeof(int));

        if (triangles == 0)
            return;

        for (int i = 0; i < triangles; i++)
        {
            float coorXYZ[12];
            in.read((char*)coorXYZ, 12 * sizeof(float));

            for (int j = 1; j < 4; j++)
            {
                // 顶点
                temp.push_back(coorXYZ[j * 3] );
                temp.push_back(coorXYZ[j * 3 + 1] );
                temp.push_back(coorXYZ[j * 3 + 2] );

                // 法向量
                for(int j = 0; j < 3; j++)
                {
                    temp.push_back(coorXYZ[j]);
                }
            }

            int j = 1;
            temp_onlyVertex.push_back(coorXYZ[j * 3]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 1]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 2]);

            temp_onlyVertex.push_back(coorXYZ[j * 3 + 3]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 4]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 5]);

            temp_onlyVertex.push_back(coorXYZ[j * 3 + 3]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 4]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 5]);

            temp_onlyVertex.push_back(coorXYZ[j * 3 + 6]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 7]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 8]);

            temp_onlyVertex.push_back(coorXYZ[j * 3 + 6]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 7]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 8]);

            temp_onlyVertex.push_back(coorXYZ[j * 3]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 1]);
            temp_onlyVertex.push_back(coorXYZ[j * 3 + 2]);

            in.read((char*)coorXYZ, 2);

        }

        in.close();
    }

    std::cout<<"temp size: "<<temp.size()<<std::endl;

    file.close();
};

float GLWidget::calculateArea(QVector<QVector2D> &vec)
{

    int point_num = vec.size();

    if(point_num < 3)return 0.0;

    float s = vec[0].y() * (vec[point_num-1].x() - vec[1].x());

    for(int i = 1; i < point_num; ++i)
        s += vec[i].y() * (vec[i-1].x() - vec[(i+1)%point_num].x());

    // 顺时针 > 0, 逆时针 < 0
    std::cout<<"area: "<< s/2.0 <<std::endl;

    return s/2.0;
}

// 屏幕坐标转世界坐标
void GLWidget::coordinate_transformation(QStack<QVector<float>> draw_stack)
{
    // 用于 test
    draw_coorstack.clear();

    // 坐标转换
    QVector<float> temp_vector;

    for(auto it = draw_stack.begin(); it != draw_stack.end(); ++it)
    {
        std::cout<< "it->size: "<<it->size()<<std::endl;

        temp_vector.clear();

        for (int j = 0; j < it->size(); j+=2) {
            float x = ( it->data()[j]  - 602 / 2 ) / (602 / 2 );
            float y = ( it->data()[j+1]   - 612 / 2 ) / (612 / 2 );

            temp_vector.push_back(x);
            temp_vector.push_back(y);
        }

        draw_coorstack.push_back(temp_vector);
    }
}

void GLWidget::reviceStackDataSlot(QStack<QVector<float>> draw_stack)
{
    this->draw_stack = draw_stack;

    coordinate_transformation(draw_stack);

#if 1
    // 识别椭圆
    if(recognizecylinder->recognize_cylinder_shape(draw_coorstack))
    {
        this->radius = recognizecylinder->radius;
        this->height_1 = recognizecylinder->height_1;
        this->height_2 = recognizecylinder->height_2;

        std::cout<<"radius: "<<radius<<std::endl;
        std::cout<<"height_1: "<<height_1<<std::endl;
        std::cout<<"height_2: "<<height_2<<std::endl;

        int cylinder_index = 0, line_index = 0;

        // head_vector
        QVector<QVector2D> head_vector, line_vector_1, line_vector_2;

        for(int i = 0; i < recognizecylinder->type_vec.size(); i++)
        {
            if(recognizecylinder->type_vec[i] == "椭圆")
            {
                cylinder_index = i;
                for (int var = 0; var < draw_coorstack[cylinder_index].size(); var+=2) {

                    QVector2D temp(draw_coorstack[cylinder_index][var],draw_coorstack[cylinder_index][var+1]);

                    head_vector.push_back(temp);
                }
            }
            // 没有区分左右
            else if(recognizecylinder->type_vec[i] == "波浪线")
            {
                line_index = i;
                if(line_vector_1.isEmpty())
                {
                    for (int var = 0; var < draw_coorstack[line_index].size(); var+=2) {

                        QVector2D temp(draw_coorstack[line_index][var],draw_coorstack[line_index][var+1]);

                        line_vector_1.push_back(temp);
                    }
                }
                else
                {
                    for (int var = 0; var < draw_coorstack[line_index].size(); var+=2) {

                        QVector2D temp(draw_coorstack[line_index][var],draw_coorstack[line_index][var+1]);

                        line_vector_2.push_back(temp);
                    }
                }
            }
        }

        // 计算多边形面积 -> 确定是顺时针还是逆时针。
        // 顺时针面积>0 逆时针面积<0
        // 统一为逆时针方向
        if(calculateArea(head_vector) > 0)
        {
            QVector<QVector2D> temp_vec;
            for(int var = head_vector.size()-1; var >=0 ; var--)
            {
                temp_vec.push_back(head_vector[var]);
            }
            head_vector = temp_vec;
        }

        // 由于只有这个函数会多次调用,所以我们写成 lambda 函数,后续有需要再拿出去
        // 统一竖直方向的顺序为: 从上到下
        auto  vertical_order = [] (QVector<QVector2D> &line_vector){
            if(line_vector.begin()->y() < line_vector.end()->y())
            {
                QVector<QVector2D> temp_vec;
                for(int var = line_vector.size()-1; var >=0 ; var--)
                {
                    temp_vec.push_back(line_vector[var]);
                }
                line_vector = temp_vec;
            }
        };

        if(!line_vector_1.isEmpty())
            vertical_order(line_vector_1);

        if(!line_vector_2.isEmpty())
            vertical_order(line_vector_2);

        QVector3D offset(off_var,off_var,off_var);

        off_var += 1;

        // 圆柱体
//        genCylinder(cylinder_vector, radius, height_1, offset);

        if(line_vector_1.isEmpty() && line_vector_2.isEmpty())
            // 顶部具有倾斜角度的圆柱体
            genCylinder(cylinder_vector, head_vector, height_1, offset);
        else if(!line_vector_1.isEmpty() && line_vector_2.isEmpty())
            // 右边和左边对称的立方体
            genCylinder(cylinder_vector, head_vector, line_vector_1, height_1, offset);
        else
        {
            // 左边和右边均可任意的立方体
            genCylinder(cylinder_vector, head_vector, line_vector_1, line_vector_2, height_1, height_2, offset);
        }

        allocate_vector();

        update();
    }

#elif 0
    // 识别琦角
    if(recognizecorner->recognize_corner_shape(draw_coorstack))
    {
        this->radius = recognizecorner->radius;

        std::cout<<"radius: "<<radius<<std::endl;

        int cylinder_index = 0, line_index = 0;

        // head_vector
        QVector<QVector2D> head_vector, line_vector;

        for(int i = 0; i < recognizecorner->type_vec.size(); i++)
        {
            if(recognizecorner->type_vec[i] == "椭圆")
            {
                cylinder_index = i;
                for (int var = 0; var < draw_coorstack[cylinder_index].size(); var+=2) {

                    QVector2D temp(draw_coorstack[cylinder_index][var],draw_coorstack[cylinder_index][var+1]);

                    head_vector.push_back(temp);
                }
            }
            else
            {
                line_index = i;
                for (int var = 0; var < draw_coorstack[line_index].size(); var+=2) {

                    QVector2D temp(draw_coorstack[line_index][var],draw_coorstack[line_index][var+1]);

                    line_vector.push_back(temp);
                }
            }
        }

        // 计算多边形面积 -> 确定是顺时针还是逆时针。
        // 顺时针面积>0 逆时针面积<0
        // 统一为逆时针方向
        if(calculateArea(head_vector) > 0)
        {
            QVector<QVector2D> temp_vec;
            for(int var = head_vector.size()-1; var >=0 ; var--)
            {
                temp_vec.push_back(head_vector[var]);
            }
            head_vector = temp_vec;
        }

        QVector3D offset(off_var,off_var,off_var);

        off_var += 1;

        genCylinder(cylinder_vector, radius, head_vector, line_vector, offset);

        allocate_vector();

        update();
    }

#elif 0
    // 画花生

    // line_vector
    QVector<QVector2D> line_vector;

    // 寻找 y 值最小的点, 如果有多个, 则取中间的一个。

    // 记录下标的数组
    QVector<int> min_coory_vector;

    int miny_sub = 0;

    float minY = draw_coorstack[0][1];

    // 找出最小 y 值坐标
    for (int var = 0; var < draw_coorstack[0].size(); var+=2) {

        QVector2D temp(draw_coorstack[0][var],draw_coorstack[0][var+1]);

        minY = qMin(minY, draw_coorstack[0][var+1]);

        line_vector.push_back(temp);
    }

    // 查找是否有多个
    for(int var = 0; var < draw_coorstack[0].size(); var+=2)
    {
        if(draw_coorstack[0][var+1] == minY)
            min_coory_vector.push_back(var / 2);
    }

    std::cout<< "min_coory_vector.size: "<< min_coory_vector.size() <<std::endl;

    if(min_coory_vector.size() == 1)
    {
        miny_sub = min_coory_vector[0];
    }
    else
    {
        for(auto miny: min_coory_vector)
        {
            miny_sub += miny;
        }
        miny_sub /= min_coory_vector.size();
    }

    std::cout<<"miny_sub: "<< miny_sub <<std::endl;

    // 变换后的数组
    QVector<QVector2D> temp = line_vector;

    QVector<QVector2D> first_line, second_line;

    line_vector.clear();

    for (int i = 0; i < temp.size(); i++) {
        if(i < miny_sub)
            second_line.push_back(temp[i]);
        else
            first_line.push_back(temp[i]);
    }

    line_vector = first_line;

    for (auto var: second_line) {
        line_vector.push_back(var);
    }

    QVector3D offset(off_var,off_var,off_var);

    off_var += 1;

    genCylinder(cylinder_vector, line_vector, offset);

    allocate_vector();

    update();
#endif
}

// 识别杯子
void GLWidget::Recognize_cup(QStack<QVector<float>> draw_stack)
{
    std::cout<<"=========================start========================"<<std::endl;

    // 转换坐标
    coordinate_transformation(draw_stack);

    QStack<QVector<float>> cylinder;

    QVector<float> peanut;

    for (int i = 0; i < draw_coorstack.size(); i++) {

        if(i == draw_coorstack.size() - 1)
            peanut = draw_coorstack[i];
        else
            cylinder.push_back(draw_coorstack[i]);
    }

    // step1: 识别圆柱体
    if(recognizecylinder->recognize_cylinder_shape(cylinder))
    {
        this->radius = recognizecylinder->radius;
        this->height_1 = recognizecylinder->height_1;

        std::cout<<"radius: "<<radius<<std::endl;
        std::cout<<"height_1: "<<height_1<<std::endl;

        // 记录椭圆的中心坐标
        for(int i = 0; i < recognizecylinder->type_vec.size(); i++)
        {
            if(recognizecylinder->type_vec[i] == "椭圆")
            {
                QVector<QVector2D> cylinder_vec;
                for (int j = 0; j < cylinder.size() - 1; j+=2)
                    cylinder_vec.push_back(QVector2D(cylinder[i][j],cylinder[i][j+1]));

                QVector2D min,max;

                findMinMax(cylinder_vec, min,max);

                cylinder_maxX_2D = max.x();

                cylinder_center = QVector2D(min + max)/2;
            }
        }

    }
    else
    {
        QMessageBox::information(NULL, "Error", "识别圆柱体失败！");

        return;
    }

    // 保存花生数据的数组
    QVector<QVector2D> peanutLine_vector;

    // step2: 识别花生
    if(identificationtypes->recognize_peanut(peanut))
    {
        // 寻找 y 值最小的点, 如果有多个, 则取中间的一个。

        // 记录下标的数组
        QVector<int> min_coory_vector;

        int miny_sub = 0;

        peanut_minX_2D = peanut[0];

        float minY = peanut[1];

        // 找出最小 y 值坐标
        for (int var = 0; var < peanut.size(); var+=2) {

            QVector2D temp(peanut[var],peanut[var+1]);

            minY = qMin(minY, peanut[var+1]);

            peanut_minX_2D = qMin(peanut_minX_2D, peanut[var]);

            peanutLine_vector.push_back(temp);
        }

        // 查找是否有多个
        for(int var = 0; var < peanut.size(); var+=2)
        {
            if(peanut[var + 1] == minY)
                min_coory_vector.push_back(var / 2);
        }

        std::cout<< "min_coory_vector.size: "<< min_coory_vector.size() <<std::endl;

        if(min_coory_vector.size() == 1)
        {
            miny_sub = min_coory_vector[0];
        }
        else
        {
            for(auto miny: min_coory_vector)
            {
                miny_sub += miny;
            }
            miny_sub /= min_coory_vector.size();
        }

        std::cout<<"miny_sub: "<< miny_sub <<std::endl;

        // 变换后的数组
        QVector<QVector2D> temp = peanutLine_vector;

        QVector<QVector2D> first_line, second_line;

        peanutLine_vector.clear();

        for (int i = 0; i < temp.size(); i++) {
            if(i < miny_sub)
                second_line.push_back(temp[i]);
            else
                first_line.push_back(temp[i]);
        }

        peanutLine_vector = first_line;

        for (auto var: second_line) {
            peanutLine_vector.push_back(var);
        }
    }
    else
    {
        QMessageBox::information(NULL, "Error", "识别花生失败！");
        return;
    }

    // step3: 识别相对位置
    int intersection_num = 0;

    for(int i = 0; i < recognizecylinder->type_vec.size(); i++)
    {
//        std::cout<< "type["<<i<<"]: "<<recognizecylinder->type_vec[i].toStdString()<<std::endl;

        if(recognizecylinder->type_vec[i] == "直线")
        {

            QVector<QVector2D> straightLine_vector;

            for(int j = 0; j < cylinder[i].size() - 1; j+=2)
                straightLine_vector.push_back(QVector2D(cylinder[i][j], cylinder[i][j+1]));

            QVector2D p1 = straightLine_vector[random(1,straightLine_vector.size())];
            QVector2D p2 = straightLine_vector[random(1,straightLine_vector.size())];

            // 求解直线方程:  0 = Ax + By + C
            float A = p2.y() - p1.y();
            float B = p1.x() - p2.x();
            float C = p2.x() * p1.y() - p1.x() * p2.y();

            // 判断交点个数以及输出交点坐标

            for (auto peanut_point: peanutLine_vector)
            {
//                std::cout<<"y: "<< abs(A * peanut_point.x() + B  * peanut_point.y() + C) << std::endl;

                if(abs(A * peanut_point.x() + B  * peanut_point.y() + C) < 0.001)
                {
                    intersection_num += 1;
                    intersection_vector.push_back(peanut_point);
                }
            }

            std::cout<<"intersection_num: "<< intersection_num <<std::endl;

//            for(auto intersection_point: intersection_vector)
//                 std::cout<<"intersection_point: "<< intersection_point.x()<< " "<<intersection_point.y()<<std::endl;

        }
    }

    // 交点小于两个
    if(intersection_num < 2)
    {
        QMessageBox::information(NULL, "Error", "相对位置有误！");
        return;
    }


    // step4: draw 模型

    // offset(0,0,0)
    QVector3D offset(off_var,off_var,off_var);

    // 圆柱体
    genCylinder(cylinder_vector,cylinder_center, radius, height_1, offset);

    // 花生
    genCylinder(cylinder_vector, peanutLine_vector, offset);

    allocate_vector();

    update();
}

void GLWidget::Recognize_deskLamp(QStack<QVector<float>> draw_stack)
{
    std::cout<<"======================start===================="<<std::endl;
}

// 三角化
void GLWidget::genTriangle(QVector<float> &vec,QVector3D p0,QVector3D p1,QVector3D p2){
    // 单位化法向量
    QVector3D n = QVector3D::normal(p0,p1,p2);

    vec.append(p0.x());vec.append(p0.y());vec.append(p0.z());
    vec.append(n.x());vec.append(n.y());vec.append(n.z());
    vec.append(p1.x());vec.append(p1.y());vec.append(p1.z());
    vec.append(n.x());vec.append(n.y());vec.append(n.z());
    vec.append(p2.x());vec.append(p2.y());vec.append(p2.z());
    vec.append(n.x());vec.append(n.y());vec.append(n.z());
}

// 斜角点
void GLWidget::genRectangleZ(QVector<float> &vec,QVector3D p0,QVector3D p1){
    QVector3D p2 = QVector3D(p1.x(),p1.y(),p0.z());
    QVector3D p3 = QVector3D(p0.x(),p0.y(),p1.z());
    // 定义的顺序是逆时针！ 要逆时针画鸭鸭鸭！
    genTriangle(vec,p0,p3,p2);
    genTriangle(vec,p2,p3,p1);
}

void GLWidget::findMinMax(QVector<QVector2D> head_path, QVector2D &min,QVector2D &max){
    float maxX = head_path[0].x(),minX = maxX,  maxY = head_path[0].y(), minY = maxY;
    for(auto it = head_path.begin(); it != head_path.end(); it++)
    {
        maxX = qMax(maxX,it->x());
        minX = qMin(minX,it->x());
        maxY = qMax(maxY,it->y());
        minY = qMin(minY,it->y());

    }
    min = QVector2D(minX,minY);
    max = QVector2D(maxX,maxY);
};

// return  1/cos()
float GLWidget::mapEllipseToCircle(QVector<QVector2D> &head_path){

    QVector2D min,max;
    findMinMax(head_path,min,max);
    QVector2D center = (min + max)/2;

    float ratio = (max.x() - min.x())/(max.y() - min.y());
    for(int i = 0; i < head_path.size(); i++){
        // 拉伸y
        head_path[i].setY((head_path[i].y() - center.y())*ratio + center.y());
    }

    return 1/sqrt(1 - (1/ratio)*(1/ratio));
}

// 画花生
void GLWidget::genCylinder(QVector<float> &vec, QVector<QVector2D> line_path, QVector3D offset)
{
    std::cout<<"====================花生====================="<<std::endl;

    int initSize = vec.size();

    std::cout<<"line_path.size: "<<line_path.size()<<std::endl;

    // 拉伸y
//    mapEllipseToCircle(line_path);

    // 保存最短路径 p1_p2 的数组
    QVector<QLineF> shotest_path_vector;
    // 保存中心线的数组
    QVector<QPointF> centerPoint_vector;

    QPointF p1, p2;
    QVector<QPointF> line_vec;

    // 取样
    for (int i = 0; i < line_path.size(); i++) {

        if(i == 0)
        {

            p1 = QPointF(line_path[0].x(), line_path[0].y());

            p2 = QPointF(line_path[line_path.size() - 1].x(), line_path[line_path.size() - 1].y());

            line_vec.push_back(p1);

            shotest_path_vector.push_back(QLineF(p1,p2));

            centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

            continue;
        }

        if(i % 5 == 0)
            line_vec.push_back(QVector2D(line_path[i]).toPointF());
    }

    QVector<QPointF>::iterator it_first = line_vec.begin();
    QVector<QPointF>::iterator it_end = line_vec.end();

    // 获取线段的中点
    do{
        auto first_next = it_first + 1;
        auto end_next = it_end - 1;
        float first_end_next = QVector2D(QPointF(it_first->x(),it_first->y()) - QPointF(end_next->x(),end_next->y())).length();
        float first_next_end = QVector2D(QPointF(first_next->x(),first_next->y()) - QPointF(it_end->x(),it_end->y())).length();

        if(first_end_next < first_next_end)
            it_end -= 1;
        else
            it_first += 1;

        p1 = QPointF(it_first->x(), it_first->y());
        p2 = QPointF(it_end->x(), it_end->y());

        shotest_path_vector.push_back(QLineF(p1,p2));

        centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

    }while(it_first != it_end);

    std::cout<<"shotest_path_vector.size: "<<shotest_path_vector.size()<<std::endl;

    std::cout<<"centerPoint_vector.size: "<<centerPoint_vector.size()<<std::endl;

    // 保存圆轨迹的数组
    QVector<QVector<QVector3D>> draw_circle_vector;

    QVector3D first_circle_center;

    // shotest_path_vector.size()
    for (int i = 0; i < shotest_path_vector.size() - 1; i++) {

        p1 = shotest_path_vector[i].p1();

        p2 = shotest_path_vector[i].p2();

        float radis = QVector2D(p2 - p1).length() / 2;

        QVector<QVector2D> circle_vector;

        QVector3D center(centerPoint_vector[i].x(), centerPoint_vector[i].y(), centerPoint_vector[i].y());

        if(i == 0)
            first_circle_center = QVector3D(centerPoint_vector[i].x(), 0, centerPoint_vector[i].y());

        QVector<QVector3D> temp_circle;

        // 构建圆的坐标
        float t = 0.0;
        for (int j = 0; j <= 360 ; j++) {

            // i 是弧度，需要转成角度 t
            t = j * 2 * M_PI / 360;

            // 改变圆心位置，只用给 x,y 加入固定的数值即可。
            circle_vector.push_back(QVector2D(radis * cos(t), radis * sin(t)));

            temp_circle.push_back(QVector3D(circle_vector[j].x(), circle_vector[j].y(), 0));
        }

        // 求旋转角度
        float cos_angle = (p2.x() - p1.x()) / sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));

        float sin_angle = (p2.y() - p1.y()) / sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));

        QVector<QVector3D> rotate_new_circle;

        // 逆时针旋转
        for (int j = 0; j < temp_circle.size() ; j++) {

            float x = cos_angle * temp_circle[j].x() +  centerPoint_vector[i].x();

            float y = temp_circle[j].y();

            float z = sin_angle * temp_circle[j].x() +  centerPoint_vector[i].y();

            rotate_new_circle.push_back(QVector3D(x, y, z));
        }

        draw_circle_vector.push_back(rotate_new_circle);
    }

    for (int i = 0; i < draw_circle_vector.size() - 1; i++)
    {
        int i_1 = (i + 1) % draw_circle_vector.size();

        for(int j = 0; j < draw_circle_vector[i].size(); j++)
        {
            int j_1 = (j + 1) % draw_circle_vector[i].size();

            QVector3D temp1 = QVector3D(draw_circle_vector[i][j].x(), draw_circle_vector[i][j].y(), draw_circle_vector[i][j].z());
            QVector3D temp3 = QVector3D(draw_circle_vector[i][j_1].x(), draw_circle_vector[i][j_1].y(), draw_circle_vector[i][j_1].z());

            // 首
            if(i == 1)
                genTriangle(vec, temp1, temp3, first_circle_center);

            QVector3D temp2 = QVector3D(draw_circle_vector[i_1][j].x(), draw_circle_vector[i_1][j].y(), draw_circle_vector[i_1][j].z());
            QVector3D temp4 = QVector3D(draw_circle_vector[i_1][j_1].x(), draw_circle_vector[i_1][j_1].y(), draw_circle_vector[i_1][j_1].z());

            genTriangle(vec,temp3,temp1,temp4);
            genTriangle(vec,temp1,temp2,temp4);

            // 尾
            if(i == draw_circle_vector[i].size() - 1)
                genTriangle(vec, temp2, temp4,QVector3D(centerPoint_vector[i_1].x(), centerPoint_vector[i_1].y(), draw_circle_vector[i_1][j_1].z()));  //bottom

        }
    }

    peanut_minX_3D = vec[0];

    float peanut_maxY_3D = vec[1];

    for(int i = 0; i < vec.size() - 1; i+=2)
    {
        peanut_minX_3D = qMin(peanut_minX_3D, vec[i]);

        peanut_maxY_3D = vec[i + 1];
    }

    std::cout<<"cylinder_maxX_2D: "<<cylinder_maxX_2D<<std::endl;
    std::cout<<"peanut_minX_2D: "<<peanut_minX_2D<<std::endl;

    std::cout<<"cylinder_maxX_3D: "<<cylinder_maxX_3D<<std::endl;
    std::cout<<"peanut_minX_3D: "<<peanut_minX_3D<<std::endl;

    peanut_offset_x = cylinder_maxX_3D - cylinder_maxX_2D;

    std::cout<<"peanut_offset_x: "<<peanut_offset_x<<std::endl;

    // 中心的 y 值
    std::cout<<"cylinder_center.y: "<<cylinder_center.y()<<std::endl;

    float peanut_offset_y = cylinder_center.y() - peanut_maxY_3D;

    // 花生的 maxX 加偏移后的 y 值
    std::cout<<"peanut.y: "<<peanut_maxY_3D + peanut_offset_y<<std::endl;

    offset = QVector3D(0, peanut_offset_y, 0);

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}

// 椭圆->琦角
void GLWidget::genCylinder(QVector<float> &vec, float r, QVector<QVector2D> head_path, QVector<QVector2D> line_path, QVector3D offset)
{
    std::cout<<"====================椭圆->琦角====================="<<std::endl;

    QVector2D min,max;
    findMinMax(head_path, min,max);

    QVector2D center((max.x() + min.x())/2, (max.y() + min.y())/2);

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    // 椭圆的中心
    QVector3D centerTop(center.x(), center.y(), 0);

    // R0: 上椭圆的半径
    float proportion = center.x() - min.x();

    int initSize = vec.size();

    std::cout<<"head_path.size: "<<head_path.size()<<std::endl;

    std::cout<<"line_path.size: "<<line_path.size()<<std::endl;

    // 保存最短路径 p1_p2 的数组
    QVector<QLineF> shotest_path_vector;
    // 保存中心线的数组
    QVector<QPointF> centerPoint_vector;

    QPointF p1, p2;
    QVector<QPointF> line_vec;
    float temp_proportion;

    // 取样
    for (int i = 0; i < line_path.size(); i++) {

        if(i == 0)
        {

            p1 = QPointF(min.x(), center.y());

            p2 = QPointF(max.x(), center.y());

            line_vec.push_back(p1);

            shotest_path_vector.push_back(QLineF(p1,p2));

            centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

            continue;
        }

        if(i % 5 == 0)
            line_vec.push_back(QVector2D(line_path[i]).toPointF());

    }

    QVector<QPointF>::iterator it_first = line_vec.begin();
    QVector<QPointF>::iterator it_end = line_vec.end();

    // 获取线段的中点
    do{
        auto first_next = it_first + 1;
        auto end_next = it_end - 1;
        float first_end_next = QVector2D(QPointF(it_first->x(),it_first->y()) - QPointF(end_next->x(),end_next->y())).length();
        float first_next_end = QVector2D(QPointF(first_next->x(),first_next->y()) - QPointF(it_end->x(),it_end->y())).length();

        if(first_end_next < first_next_end)
            it_end -= 1;
        else
            it_first += 1;

        p1 = QPointF(it_first->x(), it_first->y());
        p2 = QPointF(it_end->x(), it_end->y());

        shotest_path_vector.push_back(QLineF(p1,p2));

        centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

    }while(it_first != it_end);

    // draw
    QVector<QVector3D> origin_circle;

    // 设定 Z 值
    float z = centerPoint_vector[0].y();

    for(int i = 0; i < shotest_path_vector.size() - 1; i++)
    {
        centerTop.setZ(z);

        std::cout<<"=========================================="<<std::endl;
        if(i == 0)
        {
            std::cout<<"centerTop.x: "<<centerTop.x()<<" centerTop.y: "<<centerTop.y()<<" centerTop.z: "<<z<<std::endl;

            std::cout<<"centerPoint_vector[i].x: "<< centerPoint_vector[i].x() << " centerPoint_vector[i].y: "<< centerPoint_vector[i].y() <<std::endl;

            for(int j = 0; j < head_path.size(); j++)
            {
                int j_1 = (j + 1) % head_path.size();

                // 所有点坐标向 y 轴负半轴平移 centerTop.y() 个单位， 使椭圆的中心坐标的 y 值为 0
                QVector3D p0 = QVector3D(head_path[j].x(),head_path[j].y() - centerTop.y(),z);
                QVector3D p1 = QVector3D(head_path[j_1].x(),head_path[j_1].y() - centerTop.y(),z);

                std::cout<<"p0.x: "<<p0.x()<<" p0.y: "<<p0.y()<<" p0.z: "<<p0.z()<<std::endl;

                origin_circle.push_back(p0);
                genTriangle(vec,p0,p1,QVector3D(centerPoint_vector[i].x(),0,z)); //top
            }
            continue;
        }

        p1 = shotest_path_vector[i].p1();

        p2 = shotest_path_vector[i].p2();

        std::cout<<"p1: ("<<p1.x()<<","<<p1.y()<<")"<<std::endl;

        std::cout<<"p2: ("<<p2.x()<<","<<p2.y()<<")"<<std::endl;

        float radis = QVector2D(p2 - p1).length() / 2;

        std::cout<<"radis: "<<radis<<std::endl;

        QVector<QVector3D> new_circle;

        // 构建平行于 X 轴的椭圆轨迹
        for(int j = 0; j < head_path.size(); j++){

            temp_proportion =  radis / proportion;

            // 放缩比例后的椭圆坐标
            QVector3D p0((head_path[j].x() - centerTop.x()) * temp_proportion, (head_path[j].y() - centerTop.y()) * temp_proportion, 0);

            new_circle.push_back(p0);
        }

        std::cout<<"new_circle[0].x: "<<new_circle[0].x()<<" new_circle[0].y: "<<new_circle[0].y()<<" new_circle[0].z: "<<new_circle[0].z()<<std::endl;

        std::cout<<"new_circle.size: "<<new_circle.size()<<std::endl;

        // 求旋转角度
        float cos_angle = (p2.x() - p1.x()) / sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));

        float sin_angle = (p2.y() - p1.y()) / sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));

        std::cout << "cos_angle:" << cos_angle << "     " <<  sin_angle << std::endl;

        QVector<QVector3D> rotate_new_circle;

        // 逆时针旋转
        for (int j = 0; j < new_circle.size() ; j++) {

            float x = cos_angle * new_circle[j].x() +  centerPoint_vector[i].x();

            float y = new_circle[j].y();

            float z = sin_angle * new_circle[j].x() +  centerPoint_vector[i].y();

            rotate_new_circle.push_back(QVector3D(x, y, z));
        }

        std::cout<<"rotate_new_circle[0].x: "<<rotate_new_circle[0].x()<<" rotate_new_circle[0].y: "<<rotate_new_circle[0].y()<<" rotate_new_circle[0].z: "<<rotate_new_circle[0].z()<<std::endl;

        std::cout<<"rotate_new_circle.size: "<<rotate_new_circle.size()<<std::endl;

        // 拉竖条
        for (int i = 0; i < origin_circle.size(); i++) {
            int i_1 = (i + 1) % origin_circle.size();
            QVector3D temp1 = QVector3D(origin_circle[i].x(), origin_circle[i].y(), origin_circle[i].z());
            QVector3D temp3 = QVector3D(origin_circle[i_1].x(), origin_circle[i_1].y(), origin_circle[i_1].z());

            QVector3D temp2 = QVector3D(rotate_new_circle[i].x(), rotate_new_circle[i].y(), rotate_new_circle[i].z());
            QVector3D temp4 = QVector3D(rotate_new_circle[i_1].x(), rotate_new_circle[i_1].y(), rotate_new_circle[i_1].z());

            genTriangle(vec,temp3,temp1,temp4);
            genTriangle(vec,temp1,temp2,temp4);

        }

        origin_circle.clear();

        origin_circle = rotate_new_circle;

        // 未得到正确结果, 此处可能存在鲁棒性， 也可能需要再加一个 y 轴的偏移
        if(i == shotest_path_vector.size() - 2)
        {
            float new_minZ = origin_circle[0].z(), new_maxZ = new_minZ, center_z;

            float new_minX = origin_circle[0].x(), new_maxX = new_minX, center_x;

            float new_minY = origin_circle[0].y(), new_maxY = new_minY, center_y;

            for (int k = 0; k < origin_circle.size(); k++) {
                new_minZ = qMin(new_minZ, origin_circle[k].z());

                new_maxZ = qMin(new_maxZ, origin_circle[k].z());

                new_minX = qMin(new_minX, origin_circle[k].x());

                new_maxX = qMin(new_maxX, origin_circle[k].x());

                new_minY = qMin(new_minY, origin_circle[k].y());

                new_maxY = qMin(new_maxY, origin_circle[k].y());
            }

            center_x = (new_minX + new_maxX) / 2 ;

            center_y = (new_minY + new_maxY) / 2 ;

            center_z = (new_minZ + new_maxZ) / 2 ;

            std::cout<<"center_x: "<< center_x << " center_y: "<< center_y << " center_z: "<<center_z<<std::endl;

            std::cout<<"centerPoint_vector[i].x: "<< centerPoint_vector[i].x() << " centerPoint_vector[i].y: "<< centerPoint_vector[i].y() <<std::endl;

            for(int j = 0; j < origin_circle.size(); j++)
            {
                int j_1 = (j + 1) % origin_circle.size();
                QVector3D p0 = QVector3D(origin_circle[j].x(), origin_circle[j].y() + centerPoint_vector[i].y(), origin_circle[j].z());
                QVector3D p1 = QVector3D(origin_circle[j_1].x(), origin_circle[j_1].y() + centerPoint_vector[i].y(), origin_circle[j_1].z());

                genTriangle(vec,p0,p1,QVector3D(centerPoint_vector[i].x(), centerPoint_vector[i].y(), center_z));  //bottom
            }
        }

    }

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}

// 左边和右边均可任意的立方体
void GLWidget::genCylinder(QVector<float> &vec,QVector<QVector2D> head_path, QVector<QVector2D> line_path_1, QVector<QVector2D> line_path_2, float height_1, float height_2, QVector3D offset)
{
    std::cout<<"====================椭圆->波浪线->波浪线->曲线====================="<<std::endl;

    // 高度比 (左右线段的 Z 值偏移角度)
    float heightRatio = mapEllipseToCircle(head_path);

    QVector2D min,max;
    findMinMax(head_path, min,max);

    QVector2D center((max.x() + min.x())/2, (max.y() + min.y())/2);

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    int initSize = vec.size();

    // 缩放变换
    auto Scale = [](QVector<QVector2D> &line_vector,float Scale_Ratio){
        QVector<QVector2D> temp_vector;
        for (int i = 0; i < line_vector.size(); i++) {
            QVector2D temp = QVector2D(line_vector[i].x(), line_vector[i].y() * Scale_Ratio);
            temp_vector.push_back(temp);
        }
        line_vector = temp_vector;
    };

    // 平移变换
    auto  Translate = [](QVector<QVector2D> &line_vector,float Translate_Ratio){
        QVector<QVector2D> temp_vector;
        for (int i = 0; i < line_vector.size(); i++) {
            QVector2D temp = QVector2D(line_vector[i].x(), line_vector[i].y() + Translate_Ratio);
            temp_vector.push_back(temp);
        }
        line_vector = temp_vector;
    };

    float Scale_Ratio, Translate_Ratio;
    // 记录最终的高度值,后面计算划分比例需要用到
    float height;
    QString str;
    // 参照高的线段进行缩放变换
    if(height_1 > height_2)
    {
        // 缩放 line_2
        float Scale_Ratio = height_1/height_2;
        Scale(line_path_2, Scale_Ratio);
        height = height_1;
    }
    else
    {
        // 缩放 line_1
        Scale_Ratio = height_2/height_1;
        Scale(line_path_1, Scale_Ratio);
        height = height_2;
        str = "line_1";
    }

    if(abs((line_path_2[line_path_2.size()-1].y()-line_path_2[0].y()) - (line_path_1[line_path_1.size()-1].y()-line_path_1[0].y())) < 0.001)
    {
        std::cout<<"拉伸成功!"<<std::endl;
    }

    std::cout<<"height_1: "<<abs(line_path_1[line_path_1.size()-1].y()-line_path_1[0].y())<<std::endl;
    std::cout<<"height_2: "<<abs(line_path_2[line_path_2.size()-1].y()-line_path_2[0].y())<<std::endl;

    if(str == "line_1")
    {
        Translate_Ratio = line_path_2[0].y() - line_path_1[0].y();
        Translate(line_path_1, Translate_Ratio);
    }
    else
    {
        Translate_Ratio = line_path_1[0].y() - line_path_2[0].y();
        Translate(line_path_2, Translate_Ratio);
    }

    if( abs(line_path_1[0].y() - line_path_2[0].y()) < 0.001 && abs(line_path_1[line_path_1.size()-1].y() - line_path_2[line_path_2.size()-1].y()) < 0.001 )
    {
        std::cout<<"平移成功!"<<std::endl;
    }

    std::cout<<"line_path_1[0].x: "<<line_path_1[0].x()<<std::endl;
    std::cout<<"line_path_1[0].y: "<<line_path_1[0].y()<<std::endl;
    std::cout<<"line_path_2[0].x: "<<line_path_2[0].x()<<std::endl;
    std::cout<<"line_path_2[0].y: "<<line_path_2[0].y()<<std::endl;

    std::cout<<"line_path_1[line_path_1.size()-1].x: "<<line_path_1[line_path_1.size()-1].x()<<std::endl;
    std::cout<<"line_path_1[line_path_1.size()-1].y: "<<line_path_1[line_path_1.size()-1].y()<<std::endl;
    std::cout<<"line_path_2[line_path_2.size()-1].x: "<<line_path_2[line_path_2.size()-1].x()<<std::endl;
    std::cout<<"line_path_2[line_path_2.size()-1].y: "<<line_path_2[line_path_2.size()-1].y()<<std::endl;

    int Divide_size = 100;

    // 等分
    float Divide_Ratio = height / Divide_size;

    std::cout<<"Translate_Ratio: "<<Translate_Ratio<<std::endl;
    std::cout<<"Divide_Ratio: "<<Divide_Ratio<<std::endl;

    if(abs(line_path_1.end()->y() - line_path_1[0].y() + ((Divide_size - 1) * Divide_Ratio)) < 0.01)
    {
        std::cout<<"Divide success! "<<std::endl;
    }

    QVector<QVector2D> line1_vec, line2_vec, center_vec;

    for(int i = 0; i < Divide_size; i++)
    {

        for(int j = 0; j < line_path_1.size(); j++)
        {
            if(abs(line_path_1[j].y() - (line_path_1[0].y() - Divide_Ratio * i)) < 0.01)
            {
                line1_vec.push_back(QVector2D(line_path_1[j].x(), line_path_1[j].y()));
                break;
            }
        }

        for(int j = 0; j < line_path_2.size(); j++)
        {
            if(abs(line_path_2[j].y() - (line_path_2[0].y() - Divide_Ratio * i)) < 0.01)
            {
                line2_vec.push_back(QVector2D(line_path_2[j].x(), line_path_2[j].y()));
                break;
            }
        }
    }

    std::cout<<"line1_vec.size: "<<line1_vec.size()<<std::endl;
    std::cout<<"line2_vec.size: "<<line2_vec.size()<<std::endl;

    int size = qMin(line1_vec.size(), line2_vec.size());

    for(int i = 0; i < size; i++)
    {
        QVector2D temp_center = QVector2D((line1_vec[i].x()+line2_vec[i].x())/2, line1_vec[i].y());
        center_vec.push_back(temp_center);
    }

    std::cout<<"center_vec.size: "<<center_vec.size()<<std::endl;

    int flag_1 = 0, flag_2 = 0;

    // R0: 上椭圆的半径
    float proportion = center.x() - min.x();

    QVector3D centerTop(center.x(), center.y(), 0);

    QVector3D centerBottom;

    QVector<QVector2D> head_path_bottom;


    for(int i = 0; i < head_path.size(); i++){

        int i_1 = (i + 1)%head_path.size();

        QVector3D temp1,temp3,temp2,temp4;
        float  temp_proportion,temp_proportion_1;

        for(int j = 0; j < size - 1; j++)
        {
            // R1: 要缩放的椭圆半径
            // 缩放比例 R = R1/R0
            temp_proportion =  abs(center_vec[j].x() - line1_vec[j].x()) / proportion;
            temp_proportion_1 =  abs(center_vec[j+1].x() - line1_vec[j+1].x()) / proportion;

            //  x轴作一个平移操作, y和z轴保持不变.
            if(j == 0)
            {
                QVector3D p0((head_path[i].x() - center_vec[j].x()) * temp_proportion, (head_path[i].y() - center_vec[j].y()) * temp_proportion + center_vec[j].y(), 0);
                QVector3D p1((head_path[i_1].x() - center_vec[j].x()) * temp_proportion , (head_path[i_1].y() - center_vec[j].y()) * temp_proportion + center_vec[j].y(), 0);

                genTriangle(vec,p0,p1,centerTop); // top
            }

            float z = (j) * (height / size) * heightRatio;
            temp1 = QVector3D((head_path[i].x() - center_vec[j].x()) * temp_proportion, (head_path[i].y() - center_vec[j].y()) * temp_proportion + center_vec[j].y(), z);
            temp3 = QVector3D((head_path[i_1].x() - center_vec[j].x()) * temp_proportion, (head_path[i_1].y() - center_vec[j].y()) * temp_proportion + center_vec[j].y(), z);

            z = (j+1) * (height / size) * heightRatio;
            temp2 = QVector3D((head_path[i].x() - center_vec[j+1].x()) * temp_proportion_1, (head_path[i].y() - center_vec[j+1].y()) * temp_proportion_1 + center_vec[j+1].y(), z);
            temp4 = QVector3D((head_path[i_1].x() - center_vec[j+1].x()) * temp_proportion_1, (head_path[i_1].y() - center_vec[j+1].y()) * temp_proportion_1 + center_vec[j+1].y(), z);

            genTriangle(vec,temp3,temp1,temp4); //竖条
            genTriangle(vec,temp1,temp2,temp4);

            // 记录底的中心坐标
            if(j == size - 2)
            {
                for(int i = 0; i < head_path.size(); i++)
                {
                    QVector2D temp_vector((head_path[i].x() - center_vec[j+1].x()) * temp_proportion_1 , (head_path[i].y() - center_vec[j+1].y()) * temp_proportion_1 + center_vec[j+1].y());
                    head_path_bottom.push_back(temp_vector);
                }

                findMinMax(head_path_bottom, min, max);

                QVector2D center_bottom((max.x() + min.x())/2, (max.y() + min.y())/2);
                centerBottom.setX(center_bottom.x());
                centerBottom.setY(center_bottom.y());
                centerBottom.setZ(height * heightRatio);
            }
        }

        // 补最后一段三角面片
        temp1.setZ((size-1) * (height / size) * heightRatio);
        temp3.setZ((size-1) * (height / size) * heightRatio);

        temp2.setZ((size) * (height / size) * heightRatio);
        temp4.setZ((size) * (height / size) * heightRatio);

        genTriangle(vec,temp3,temp1,temp4); //竖条
        genTriangle(vec,temp1,temp2,temp4);

        genTriangle(vec,temp4,temp2,centerBottom); //bottom

    }

    std::cout<<"flag_1: "<<flag_1<<std::endl;
    std::cout<<"flag_2: "<<flag_2<<std::endl;

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }

#if 0
    // 将两边波浪线的高度补成一样!
    QVector2D line1_first = line_path_1[0],line1_end = line_path_1[line_path_1.size()-1];
    QVector2D line2_first = line_path_2[0],line2_end = line_path_2[line_path_2.size()-1];

    // 随机数
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<float> dist_float();

    // 比例
    float Ratio_1 = height_1/line_path_1.size(),Ratio_2= height_2/line_path_2.size();
    // 高度差
    float  diff_first, diff_end;
    // 需要补的点个数 = 高度差 / 比例
    int num_first,num_end;
    if(line1_first.y() < line2_first.y())
    {
        diff_first = line2_first.y() - line1_first.y();
        num_first = diff_first/Ratio_1;
        // 补line1的头
    }
    else
    {
        diff_first = line1_first.y() - line2_first.y();
        num_first = diff_first/Ratio_2;
        // 补line2的头
    }

    if(line1_end.y() > line2_end.y())
    {
        diff_end = line1_end.y() - line2_end.y();
        num_end = diff_end / Ratio_1;
        // 补line1的尾
    }
    else
    {
        diff_end = line2_end.y() - line1_end.y();
        num_end = diff_end / Ratio_2;
        // 补line2的尾
    }
#endif
}

// 右边和左边对称的立方体
void GLWidget::genCylinder(QVector<float> &vec,QVector<QVector2D> head_path, QVector<QVector2D> line_path, float height,QVector3D offset){

    std::cout<<"=========================椭圆->波浪线->直线->曲线====================="<<std::endl;

    // 高度比
    float heightRatio = mapEllipseToCircle(head_path);

    // 中心
    QVector2D min,max;
    findMinMax(head_path, min,max);

    QVector2D center((max.x() + min.x())/2, (max.y() + min.y())/2);

    float proportion = center.x() - min.x();

    std::cout<<"proportion: "<<proportion<<std::endl;

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    QVector3D centerTop(center.x(), center.y(), 0);
    QVector3D centerBottom;

    QVector<QVector2D> head_path_bottom;

    int initSize = vec.size();

    for(int i = 0; i < head_path.size(); i++){

        int i_1 = (i + 1)%head_path.size();

        //        QVector3D p0(head_path[i].x(),head_path[i].y(), 0);
        //        QVector3D p1(head_path[i_1].x(),head_path[i_1].y(), 0);

        //        genTriangle(vec,p0,p1,centerTop); // top

        QVector3D temp1,temp3,temp2,temp4;
        float  temp_proportion,temp_proportion_1;

        for(int j = 0; j < line_path.size() - 1; j++){

            // 我们最理想的情况是： line_path[0].x() == minX, 当用户画的不符合时，这里会有 bug
            // 缩放比例
            temp_proportion =  (center.x() - line_path[j].x()) / proportion;
            temp_proportion_1 =  (center.x() - line_path[j+1].x()) / proportion;

            float z = (j) * (height / line_path.size()) * heightRatio;
            temp1 = QVector3D((head_path[i].x() - center.x()) * temp_proportion + center.x(), (head_path[i].y() - center.y()) * temp_proportion + center.y(), z);
            temp3 = QVector3D((head_path[i_1].x() - center.x()) * temp_proportion + center.x(), (head_path[i_1].y() - center.y()) * temp_proportion + center.y(), z);

            z = (j+1) * (height / line_path.size()) * heightRatio;
            temp2 = QVector3D((head_path[i].x() - center.x()) * temp_proportion_1 + center.x(), (head_path[i].y() - center.y()) * temp_proportion_1 + center.y(), z);
            temp4 = QVector3D((head_path[i_1].x() - center.x()) * temp_proportion_1 + center.x(), (head_path[i_1].y() - center.y()) * temp_proportion_1 + center.y(), z);

            genTriangle(vec,temp3,temp1,temp4); //竖条
            genTriangle(vec,temp1,temp2,temp4);

            if(j == 0)
            {
                QVector3D p0((head_path[i].x() - center.x()) * temp_proportion + center.x(), (head_path[i].y() - center.y()) * temp_proportion + center.y(), 0);
                QVector3D p1((head_path[i_1].x() - center.x()) * temp_proportion + center.x(), (head_path[i_1].y() - center.y()) * temp_proportion + center.y(), 0);

                genTriangle(vec,p0,p1,centerTop); // top
            }

            // 记录底的中心坐标
            if(j == line_path.size()-2)
            {
                for(int i = 0; i < head_path.size(); i++)
                {
                    QVector2D temp_vector((head_path[i].x() - center.x()) * temp_proportion_1 + center.x(), (head_path[i].y() - center.y()) * temp_proportion_1 + center.y());
                    head_path_bottom.push_back(temp_vector);
                }

                findMinMax(head_path_bottom, min, max);

                QVector2D center_bottom((max.x() + min.x())/2, (max.y() + min.y())/2);
                centerBottom.setX(center_bottom.x());
                centerBottom.setY(center_bottom.y());
                centerBottom.setZ(height * heightRatio);
            }
        }

        // 补最后一段三角面片
        temp1.setZ((line_path.size()-1) * (height / line_path.size()) * heightRatio);
        temp3.setZ((line_path.size()-1) * (height / line_path.size()) * heightRatio);

        temp2.setZ((line_path.size()) * (height / line_path.size()) * heightRatio);
        temp4.setZ((line_path.size()) * (height / line_path.size()) * heightRatio);

        genTriangle(vec,temp3,temp1,temp4); //竖条
        genTriangle(vec,temp1,temp2,temp4);

        genTriangle(vec,temp4,temp2,centerBottom); //bottom

    }

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }

}

// 圆柱体
void GLWidget::genCylinder(QVector<float> &vec,QVector2D cylinder_center,float r,float z,QVector3D offset){

    int n = 100;
    QVector<QVector2D> path;
    for(int i = 0; i < n; i++){
        path.append(QVector2D(r*cos(i*2*M_PI/n) + cylinder_center.x(),r*sin(i*2*M_PI/n) + cylinder_center.y()));
    }
    genCylinder(vec,path,z,offset);
}

// 顶部具有倾斜角度的圆柱体
void GLWidget::genCylinder(QVector<float> &vec,QVector<QVector2D> head_path,float z,QVector3D offset){

    std::cout<<"===================圆柱体=================="<<std::endl;

    QVector2D min,max;

    findMinMax(head_path,min,max);

    std::cout<<"maxX: "<<max.x()<<" minX:"<<min.x()<<" maxY: "<<max.y()<<" minY: "<<min.y()<<std::endl;

    cylinder_maxX_3D = max.x();

    QVector2D center = QVector2D(min + max) /2;

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    // 拉伸y
    mapEllipseToCircle(head_path);

    QVector3D centerTop(center.x(), center.y(), center.y());

    std::cout<<"centerTop.z: "<< centerTop.z() <<std::endl;

    QVector3D centerBottom(center.x(), center.y(), z + center.y());

    int initSize = vec.size();

    for(int i = 0; i < head_path.size(); i++){
        int i_1 = (i + 1)%head_path.size();

        QVector3D p0(head_path[i].x(),head_path[i].y(),center.y());
        QVector3D p1(head_path[i_1].x(),head_path[i_1].y(),center.y());

        genTriangle(vec,p0,p1,centerTop);//top

        p1.setZ(z + center.y());
        genRectangleZ(vec,p0,p1);       //竖条

        p0.setZ(z + center.y());
        genTriangle(vec,p1,p0,centerBottom);//bottom

    }

    std::cout<<"centerBottom.z: "<< z + center.y() <<std::endl;

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}
