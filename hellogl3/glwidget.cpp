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
#include "mainwindow.h"

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <random>
#include<algorithm>
#include <iostream>
#include <window.h>
#include <fstream>
#include <QMessageBox>
#include <QTimer>


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
      off_var(0)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.

    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
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

void GLWidget::reviceStackDataSlot(QStack<QVector<float>> draw_stack)
{
    this->draw_stack = draw_stack;

    // 用于 test
    draw_coorstack.clear();

    qDebug()<<"revice stack size: "<< draw_stack.size();

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

    qDebug()<<"draw_coorstack size: "<< draw_coorstack.size();

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

        QVector3D offset(off_var,off_var,off_var);

        off_var += 1;

        // 画椭圆
//        genCylinder(cylinder_vector, radius, height, offset);

        std::cout<<"line_vector_1 size: "<<line_vector_1.size()<<std::endl;

        if(line_vector_1.isEmpty() && line_vector_2.isEmpty())
            genCylinder(cylinder_vector, head_vector, height_1, offset);
        else if(!line_vector_1.isEmpty() && line_vector_2.isEmpty())
            genCylinder(cylinder_vector, head_vector, line_vector_1, height_1, offset);
        else
        {
            genCylinder(cylinder_vector, head_vector, line_vector_1, line_vector_2, height_1, height_2, offset);
        }

        allocate_vector();

        update();
    }
}

// 三角化
void GLWidget::genTriangle(QVector<float> &vec,QVector3D p0,QVector3D p1,QVector3D p2){
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

void GLWidget::genCylinder(QVector<float> &vec,QVector<QVector2D> head_path, QVector<QVector2D> line_path_1, QVector<QVector2D> line_path_2, float height_1, float height_2, QVector3D offset)
{
    std::cout<<"====================start====================="<<std::endl;

    QVector2D min,max;
    findMinMax(head_path, min,max);
    QVector2D center((max.x() + min.x())/2, (max.y() + min.y())/2);

    // 高度比
    float heightRatio = mapEllipseToCircle(head_path);

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    std::cout<<"height_1/line_path_1: "<<height_1/line_path_1.size()<<std::endl;

    std::cout<<"height_2/line_path_2: "<<height_2/line_path_2.size()<<std::endl;

    QVector3D centerTop(center.x(), center.y(), 0);

    int initSize = vec.size();

    for(int i = 0; i < head_path.size(); i++){

        int i_1 = (i + 1)%head_path.size();

        QVector3D p0(head_path[i].x(),head_path[i].y(), 0);
        QVector3D p1(head_path[i_1].x(),head_path[i_1].y(), 0);

        genTriangle(vec,p0,p1,centerTop); // top


    }

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }

}

// draw_arbitrary_line
void GLWidget::genCylinder(QVector<float> &vec,QVector<QVector2D> head_path, QVector<QVector2D> line_path, float height,QVector3D offset){
    // 中心
    QVector2D min,max;
    findMinMax(head_path, min,max);

    QVector2D center((max.x() + min.x())/2, (max.y() + min.y())/2);

    // 高度比
    float heightRatio = mapEllipseToCircle(head_path);

    float proportion = center.x() - min.x();

    std::cout<<"proportion: "<<proportion<<std::endl;

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    QVector3D centerTop(center.x(), center.y(), 0);
    QVector3D centerBottom;

    QVector<QVector2D> head_path_bottom;

    int initSize = vec.size();

    for(int i = 0; i < head_path.size(); i++){

        int i_1 = (i + 1)%head_path.size();

        QVector3D p0(head_path[i].x(),head_path[i].y(), 0);
        QVector3D p1(head_path[i_1].x(),head_path[i_1].y(), 0);

        genTriangle(vec,p0,p1,centerTop); // top

        QVector3D temp1,temp3,temp2,temp4;
        float  temp_proportion,temp_proportion_1;

        for(int j = 0; j < line_path.size() - 1; j++){

            // 我们最理想的情况是： line_path[0].x() == minX, 当用户画的不符合时，这里会有 bug
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

// draw_cylinder
void GLWidget::genCylinder(QVector<float> &vec,float r,float z,QVector3D offset){
    int n = 100;
    QVector<QVector2D> path;
    for(int i = 0; i < n; i++){
        path.append(QVector2D(r*cos(i*2*M_PI/n),r*sin(i*2*M_PI/n)));
    }
    genCylinder(vec,path,z,offset);

}

// draw_arbitrary
void GLWidget::genCylinder(QVector<float> &vec,QVector<QVector2D> head_path,float z,QVector3D offset){
    float maxX = head_path[0].x(), minX = head_path[0].x(), maxY = head_path[0].y(), minY = head_path[0].y();
    for(auto it = head_path.begin(); it != head_path.end(); it++)
    {
        if(it ->x() > maxX)
            maxX = it->x();

        if(it ->x() < minX)
            minX = it->x();

        if(it ->y() > maxY)
            maxY = it->y();

        if(it ->y() < minY)
            minY = it->y();
    }

    std::cout<<"maxX: "<<maxX<<" minX:"<<minX<<" maxY: "<<maxY<<" minY: "<<minY<<std::endl;

    QVector2D center((maxX + minX)/2, (maxY + minY)/2);

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    // 拉伸y
    mapEllipseToCircle(head_path);

    QVector3D centerTop(center.x(), center.y(), 0);
    QVector3D centerBottom(center.x(), center.y(), z);

    int initSize = vec.size();

    for(int i = 0; i < head_path.size(); i++){
        int i_1 = (i + 1)%head_path.size();

        QVector3D p0(head_path[i].x(),head_path[i].y(),0);
        QVector3D p1(head_path[i_1].x(),head_path[i_1].y(),0);

        genTriangle(vec,p0,p1,centerTop);//top

        p1.setZ(z);
        genRectangleZ(vec,p0,p1);       //竖条

        p0.setZ(z);
        genTriangle(vec,p1,p0,centerBottom);//bottom

    }

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}
