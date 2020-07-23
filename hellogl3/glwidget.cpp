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
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>

#include <random>
#include<algorithm>

#include <iostream>
#include "mainwindow.h"

#include <window.h>

#include <fstream>
#include <QMessageBox>

#include <QTimer>

#include "recognizecube.h"
#include "recognizecylinder.h"

bool GLWidget::m_transparent = false;

extern bool flag;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      appendingFlag(false),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      m_program(0),
      _step(0.0f),
      m_cameraZ(-15),
      recognizeCube(new RecognizeCube()),
      recognize_cube(false),
      recognizecylinder(new Recognizecylinder())
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.

    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }

    _qTimer.setInterval(1000); // 100 ms -> 10 Hz
    QObject::connect(&_qTimer, &QTimer::timeout,
                     this, &GLWidget::timeout);

}

void GLWidget::timeout()
{
    _step = fmod(_step + 0.1, 2 * 3.141);
    update(); // force redraw
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
    std::cout<<"=====================test1=================="<<std::endl;

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
        std::cout<<"=====================test2=================="<<std::endl;

        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
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

        glDrawArrays(GL_LINES, 0, cylinder_vector.count() / 3 );
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

    // 定时器
    //    _qTimer.start();

    // 将 update 方法加入事件循环中
    //    QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);

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

void GLWidget::reviceStackDataSlot(QStack<QVector<float>> draw_stack)
{
    this->draw_stack = draw_stack;

    // 用于 test
    draw_coorstack.clear();

    qDebug()<<"revice stack size: "<< draw_stack.size();

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
            //            temp_vector.push_back(-10);
        }

        draw_coorstack.push_back(temp_vector);
    }

    qDebug()<<"draw_coorstack size: "<< draw_coorstack.size();

    if(recognizecylinder->recognizecy_linder(draw_coorstack))
    {
        this->radius = recognizecylinder->radius;
        this->height = recognizecylinder->height;
    }

    //        draw_cylinder();

    //        draw_arbitrary();

    draw_arbitrary_line();

    allocate_vector();

    update();
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


void genTriangle(QVector<float> &vec,QVector3D p0,QVector3D p1,QVector3D p2){
        QVector3D n = QVector3D::normal(p0,p1,p2);
        vec.append(p0.x());vec.append(p0.y());vec.append(p0.z());
        vec.append(n.x());vec.append(n.y());vec.append(n.z());
        vec.append(p1.x());vec.append(p1.y());vec.append(p1.z());
        vec.append(n.x());vec.append(n.y());vec.append(n.z());
        vec.append(p2.x());vec.append(p2.y());vec.append(p2.z());
        vec.append(n.x());vec.append(n.y());vec.append(n.z());
}
//斜角点
void genRectangleZ(QVector<float> &vec,QVector3D p0,QVector3D p1){
    QVector3D p2 = QVector3D(p1.x(),p1.y(),p0.z());
    QVector3D p3 = QVector3D(p0.x(),p0.y(),p1.z());
    genTriangle(vec,p0,p2,p3);
    genTriangle(vec,p2,p1,p3);
}

void genCylinder(QVector<float> &vec,QVector<QVector2D>&path,float z,QVector3D offset){
    int n = 100;
    QVector3D centerTop(0,0,z);
    QVector3D centerBottom(0,0,0);
    int initSize = vec.size();
    for(int i = 0; i < path.size(); i++){
        int i_1 = (i + 1)%path.size();
        QVector3D p0(path[i].x(),path[i].y(),z);
        QVector3D p1(path[i_1].x(),path[i+1].y(),z);
        genTriangle(vec,p0,p1,centerTop);//top
        p1.setZ(0);
        genRectangleZ(vec,p0,p1);       //竖条
        p0.setZ(0);
        genTriangle(vec,p1,p0,centerBottom);//bottom
    }
    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}
void genCylinder(QVector<float> &vec,float r,float z,QVector3D offset){
    int n = 100;
    QVector<QVector2D>path;
    for(int i = 0; i < n; i++){
        path.append(QVector2D(r*cos(i*2*M_PI/n),r*sin(i*2*M_PI/n)));
    }
    genCylinder(vec,path,z,offset);

}


void genCylinder(QVector<float> &vec,QVector<QVector2D>path,float z,QVector3D offset){
    int n = 100;
    QVector3D centerTop(0,0,z);
    QVector3D centerBottom(0,0,0);
    int initSize = vec.size();
    for(int i = 0; i < path.size(); i++){
        int i_1 = (i + 1)%path.size();
        QVector3D p0(path[i].x(),path[i].y(),z);
        QVector3D p1(path[i_1].x(),path[i+1].y(),z);
        genTriangle(vec,p0,p1,centerTop);//top
        p1.setZ(0);
        genRectangleZ(vec,p0,p1);       //竖条
        p0.setZ(0);
        genTriangle(vec,p1,p0,centerBottom);//bottom
    }
    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}

void GLWidget::draw_arbitrary()
{
    // 画第一个圆  center->0->10->center->10->20...
    float  all_x = 0, all_y = 0;

    std::cout<< "draw_coorstack[0].size(): "<<draw_coorstack[0].size()<<std::endl;
    // 画上面的部分
    for (int i = 0; i < draw_coorstack[0].size(); i+=2) {

        float x = draw_coorstack[0][i];
        float y = draw_coorstack[0][i+1];

        all_x += x;
        all_y += y;

        cylinder_vector.push_back(x);
        cylinder_vector.push_back(y);
        cylinder_vector.push_back(0);

        if(i!=0 && i!=draw_coorstack[0].size()-2)
        {
            cylinder_vector.push_back(x);
            cylinder_vector.push_back(y);
            cylinder_vector.push_back(0);
        }

    }

    for (int i = 0; i < draw_coorstack[0].size(); i+=2) {

        // 重心
        if(i%30 == 0)
        {
            cylinder_vector.push_back(all_x/draw_coorstack[0].size());
            cylinder_vector.push_back(all_y/draw_coorstack[0].size());
            cylinder_vector.push_back(0);

            float x = draw_coorstack[0][i];
            float y = draw_coorstack[0][i+1];

            cylinder_vector.push_back(x);
            cylinder_vector.push_back(y);
            cylinder_vector.push_back(0);
        }

    }

    QVector2D first;
    //画中间区域
    for (int i = 0; i < draw_coorstack[0].size(); i+=2)
    {

        float x = draw_coorstack[0][i];
        float y = draw_coorstack[0][i+1];

        if(i==0)
        {
            first.setX(x);
            first.setY(y);
        }

        if(i!=0 && i%30 == 0)
        {
            cylinder_vector.push_back(x);
            cylinder_vector.push_back(y);
            cylinder_vector.push_back(0);
        }

        if(i%30 == 0)
        {
            cylinder_vector.push_back(x);
            cylinder_vector.push_back(y);
            cylinder_vector.push_back(0);

            cylinder_vector.push_back(x);
            cylinder_vector.push_back(y);
            cylinder_vector.push_back(height);

            cylinder_vector.push_back(x);
            cylinder_vector.push_back(y);
            cylinder_vector.push_back(height);

            // 连接 i=0 时的线
            if(draw_coorstack[0].size() - i < 30)
            {
                cylinder_vector.push_back(first.x());
                cylinder_vector.push_back(first.y());
                cylinder_vector.push_back(0);
            }
        }
    }

    all_x = 0;
    all_y = 0;

    // 画下面的部分
    for (int i = 0; i < draw_coorstack[0].size(); i+=2) {

        float x = draw_coorstack[0][i];
        float y = draw_coorstack[0][i+1];

        all_x += x;
        all_y += y;

        cylinder_vector.push_back(x);
        cylinder_vector.push_back(y);
        cylinder_vector.push_back(height);

        if(i!=0 && i!=draw_coorstack[0].size() - 2)
        {
            cylinder_vector.push_back(x);
            cylinder_vector.push_back(y);
            cylinder_vector.push_back(height);
        }
    }

    for (int i = 0; i < draw_coorstack[0].size(); i+=2) {

        // 重心
        if(i%30 == 0)
        {

            cylinder_vector.push_back(all_x/draw_coorstack[0].size());
            cylinder_vector.push_back(all_y/draw_coorstack[0].size());
            cylinder_vector.push_back(height);

            float x = draw_coorstack[0][i];
            float y = draw_coorstack[0][i+1];

            cylinder_vector.push_back(x);
            cylinder_vector.push_back(y);
            cylinder_vector.push_back(height);
        }
    }

}

void GLWidget::draw_arbitrary_line()
{
    // 画第一个圆  center->0->10->center->10->20...
    float  all_x = 0, all_y = 0;

    std::cout<< "draw_coorstack[0].size(): "<<draw_coorstack[0].size()<<std::endl;

    // 画上面的部分
    for (int i = 0; i < draw_coorstack[0].size(); i+=2) {

        float x = draw_coorstack[0][i];
        float y = draw_coorstack[0][i+1];

        all_x += x;
        all_y += y;

        cylinder_vector.push_back(x);
        cylinder_vector.push_back(y);
        cylinder_vector.push_back(0);

        if(i!=0 && i!=draw_coorstack[0].size() - 2)
        {
            cylinder_vector.push_back(x);
            cylinder_vector.push_back(y);
            cylinder_vector.push_back(0);
        }

    }

    QVector<QVector2D> repeat_vector;

    for (int i = 0; i < draw_coorstack[0].size(); i+=2) {

        // 重心
        if(i%30 == 0)
        {
            cylinder_vector.push_back(all_x/draw_coorstack[0].size());
            cylinder_vector.push_back(all_y/draw_coorstack[0].size());
            cylinder_vector.push_back(0);

            float x = draw_coorstack[0][i];
            float y = draw_coorstack[0][i+1];

            cylinder_vector.push_back(x);
            cylinder_vector.push_back(y);
            cylinder_vector.push_back(0);

            QVector2D temp(x,y);
            repeat_vector.push_back(temp);
        }
    }

    int flag = 0;

    std::cout<<"repeat_vector size: "<<repeat_vector.size()<<std::endl;

//    do
//    {

        QVector2D first;
        //画中间区域
        for (int i = 0; i < draw_coorstack[1].size(); i+=2)
        {

            float x = draw_coorstack[1][i];
            float y = draw_coorstack[1][i+1];

            if(i==0)
            {
                first.setX(x);
                first.setY(y);

                std::cout<<"x: "<<x<<std::endl;
                std::cout<<"y: "<<y<<std::endl;
            }

            cylinder_vector.push_back(x);
            cylinder_vector.push_back(first.y());
            // 当 i = 0 时，z = 0;
            // 当 i != 0 时，z = y - first.y(), 实际上是平移其他点
            cylinder_vector.push_back(y - first.y());

            if(i!=0 && i!=draw_coorstack[1].size() - 2)
            {
                cylinder_vector.push_back(x);
                cylinder_vector.push_back(first.y());
                cylinder_vector.push_back(y - first.y());
            }
        }

        flag+=1;

//    }while(flag<repeat_vector.size());

}

void GLWidget::draw_cylinder()
{
    // 半径
    GLfloat r = radius;

    // 默认圆心
    GLfloat center[3] = {0, 0, 0};

    //    for ( int i = 0; i < 3; i++) {
    //        cylinder_vector.push_back(center[i]);
    //    }

    int n = 100;
    float t;

    // 画第一个圆  center->0->10->center->10->20...
    for (int i = 0; i <= n; i++) {
        t = i * 2 * M_PI / n;

        cylinder_vector.push_back(center[0] + r * cos(t));
        cylinder_vector.push_back(center[1] + r * sin(t));
        cylinder_vector.push_back(center[2] );

        if(i!=0 && i!= n)
        {
            cylinder_vector.push_back(center[0] + r * cos(t));
            cylinder_vector.push_back(center[1] + r * sin(t));
            cylinder_vector.push_back(center[2]);
        }


        if(i%10 == 0)
        {
            for ( int i = 0; i < 3; i++) {
                cylinder_vector.push_back(center[i]);
            }

            cylinder_vector.push_back(center[0] + r * cos(t));
            cylinder_vector.push_back(center[1] + r * sin(t));
            cylinder_vector.push_back(center[2] );
        }

    }

    // 画圆之间的区域
    for (int i = 0; i <= n; i+=10) {

        t = i * 2 * M_PI / n;

        if(i!=0 && i%10==0)
        {
            cylinder_vector.push_back(center[0] + r * cos(t));
            cylinder_vector.push_back(center[1] + r * sin(t));
            cylinder_vector.push_back(center[2]);
        }

        cylinder_vector.push_back(center[0] + r * cos(t));
        cylinder_vector.push_back(center[1] + r * sin(t) );
        cylinder_vector.push_back(center[2]);

        cylinder_vector.push_back(center[0] + r * cos(t));
        cylinder_vector.push_back(center[1] + r * sin(t));
        cylinder_vector.push_back(height);

        cylinder_vector.push_back(center[0] + r * cos(t));
        cylinder_vector.push_back(center[1] + r * sin(t));
        cylinder_vector.push_back(height);

    }

    // 画第二个圆

    for (int i = 0; i <= n; i++) {
        t = i * 2 * M_PI / n;

        cylinder_vector.push_back(center[0] + r * cos(t));
        cylinder_vector.push_back(center[1] + r * sin(t));
        cylinder_vector.push_back(height);

        cylinder_vector.push_back(center[0] + r * cos(t));
        cylinder_vector.push_back(center[1] + r * sin(t));
        cylinder_vector.push_back(height);

        if(i%10 == 0)
        {
            for ( int i = 0; i < 3; i++) {
                if(i == 2)
                    cylinder_vector.push_back(height);
                else
                    cylinder_vector.push_back(center[i]);
            }

            cylinder_vector.push_back(center[0] + r * cos(t));
            cylinder_vector.push_back(center[1] + r * sin(t));
            cylinder_vector.push_back(height);
        }

    }

    std::cout<<"the cylinder_vector size is: "<< cylinder_vector.size() <<std::endl;

}
