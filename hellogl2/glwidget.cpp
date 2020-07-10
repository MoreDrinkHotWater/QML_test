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

#include <iostream>
#include "mainwindow.h"

#include <window.h>

#include <fstream>
#include <QMessageBox>

bool GLWidget::m_transparent = false;

extern bool flag;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      m_program(0)
    //      mainWindow(new MainWindow())
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



    //    test();

    // true 即 渲染三角面
    // false 即 渲染线
    flag = false;

    // 当 cylinder = true 时， 要取消调用上面的 test 函数 并且 flag = false 。
    cylinder = true;


    //    draw_cylinder();

    recognition();

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
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // 设置顶点缓冲区对象
    m_logoVbo.create();
    m_logoVbo.bind();

    // 分配内存
    if(temp.count() != 0 && temp_onlyVertex.count() != 0)
    {
        if(flag)
        {
            m_logoVbo.allocate(temp.constData(), temp.count() * sizeof(GLfloat));
        }
        else
        {
            m_logoVbo.allocate(temp_onlyVertex.constData(), temp_onlyVertex.count() * sizeof(GLfloat));
        }

    }
    else if(cylinder)
    {
        //        std::cout<<"=====================Test1==================="<<std::endl;

        m_logoVbo.allocate(cylinder_vector.constData(), cylinder_vector.count() * sizeof(GLfloat));
    }
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

    // set cylinder_vector
    if(cylinder)
    {
        //        std::cout<<"=====================Test2==================="<<std::endl;

        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
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

    else if(cylinder)
    {
        glDrawArrays(GL_LINE_STRIP, 1, cylinder_vector.count() / 3 );
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

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() & Qt::Key_A) {
        std::cout<<"test key A success"<<std::endl;
    }
}

void GLWidget::reviceVectorDataSlot(QVector<GLfloat> temp)
{
    this->temp = temp;

    qDebug()<<"revice vector size:"<< temp.size();

}

void GLWidget::test()
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

void GLWidget::recognition()
{
    QVector<QVector2D> head_circle;
    QVector2D center(0,0);
    head_circle.push_back(center);

    cylinder_vector.push_back(center.x());
    cylinder_vector.push_back(center.y());
    cylinder_vector.push_back(0);

    QVector<QVector2D> left_line;
    QVector2D l_line;
    QVector<QVector2D> right_line;
    QVector2D r_line;
    QVector<QVector2D> end_circle;

    // 生成椭圆的随机宽度和高度
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(1.0, 5.0);
    std::uniform_real_distribution<> dis_coords(0.0, 0.1);

    GLfloat circle_width = dis(gen);
    GLfloat circle_height = dis(gen);

    std::cout<<"circle_width: "<<circle_width<<" circle_height: "<<circle_height<<std::endl;


    // 画上面的椭圆
    float t = 0.0;
    int n = 1000;
    for (int i = 0; i <= n ; i++) {

        // i 是弧度，需要转成角度 t
        t = i * 2 * M_PI / n;

        // 改变圆心位置，只用给 x,y 加入固定的数值即可。

//        if( i % 10 == 0)
//            cylinder_vector.push_back(circle_width * cos(t) + dis_coords(gen));
//        else
            cylinder_vector.push_back(circle_width * cos(t));
        cylinder_vector.push_back(circle_height * sin(t));
        cylinder_vector.push_back(0);

        QVector2D temp(cylinder_vector[cylinder_vector.size()-3], cylinder_vector[cylinder_vector.size()-2]);

        // 保存上椭圆的 2D 坐标
        head_circle.push_back(temp);

        // 记录左边线段的坐标的(x, y)坐标
        if(i == n/2)
        {
            l_line.setX(cylinder_vector[cylinder_vector.size()-3]);
            l_line.setY(cylinder_vector[cylinder_vector.size()-2]);
            left_line.push_back(l_line);
            std::cout<<cylinder_vector[cylinder_vector.size()-3]<<" "<<cylinder_vector[cylinder_vector.size()-2]<<" "<<cylinder_vector[cylinder_vector.size()-1]<<std::endl;
        }

        // 记录右边线段的坐标的(x, y)坐标
        if(i == n)
        {
            r_line.setX(cylinder_vector[cylinder_vector.size()-3]);
            r_line.setY(cylinder_vector[cylinder_vector.size()-2]);
            right_line.push_back(r_line);
        }
    }

    // 画右边的线段
    cylinder_vector.push_back(r_line.x());
    cylinder_vector.push_back(r_line.y());
    cylinder_vector.push_back(5);


    // 画下面椭圆的半弧
    for (int i = n; i >= n/2 ; i--) {

        t = i * 2 * M_PI / n;
        if( i % 10 == 0)
            cylinder_vector.push_back(circle_width*cos(t) + dis_coords(gen));
        else
            cylinder_vector.push_back(circle_width*cos(t));
        cylinder_vector.push_back(circle_height*sin(t));
        cylinder_vector.push_back(5);
    }

    // 画左边的线段
    cylinder_vector.push_back(l_line.x());
    cylinder_vector.push_back(l_line.y());
    cylinder_vector.push_back(0);

    QVector<float> step_vector;
    std::uniform_int_distribution<> dist_int(1, 1000);

    QVector<QVector2D> Points_vector;
    QVector<QVector2D> OriginPoints_vector;

    // 取10组测试数据
    //    for (int i = 0; i < 10; i++) {
    // 随机取四个点数据
    for (int i = 0; i < 4; i++) {
        QVector2D temp;
        temp.setX(head_circle[dist_int(gen)].x());
        temp.setY(head_circle[dist_int(gen)].y());
        Points_vector.push_back(temp);
        OriginPoints_vector.push_back(temp);
    }

    for (int i = 0; i < 4; i++) {

        std::cout<<OriginPoints_vector[i].x()<<" "<<OriginPoints_vector[i].y()<<std::endl;

    }

    float AB =  sqrt(pow(Points_vector[1].y() - Points_vector[0].y(),2) + pow(Points_vector[1].x() - Points_vector[0].x(),2));
    std::cout<<"AB: "<<AB<<std::endl;

    float CD =  sqrt(pow(Points_vector[3].y() - Points_vector[2].y(),2) + pow(Points_vector[3].x() - Points_vector[2].x(),2));
    std::cout<<"CD: "<<CD<<std::endl;

    float BC =  sqrt(pow(Points_vector[2].y() - Points_vector[1].y(),2) + pow(Points_vector[2].x() - Points_vector[1].x(),2));
    std::cout<<"BC: "<<BC<<std::endl;

    float AD =  sqrt(pow(Points_vector[3].y() - Points_vector[0].y(),2) + pow(Points_vector[3].x() - Points_vector[0].x(),2));
    std::cout<<"AD: "<<AD<<std::endl;

    float AC =  sqrt(pow(Points_vector[2].y() - Points_vector[0].y(),2) + pow(Points_vector[2].x() - Points_vector[0].x(),2));
    std::cout<<"AC: "<<AC<<std::endl;

    float BD =  sqrt(pow(Points_vector[3].y() - Points_vector[1].y(),2) + pow(Points_vector[3].x() - Points_vector[1].x(),2));
    std::cout<<"BD: "<<BD<<std::endl;

    float step = 1.0;
    // 托勒密定理 判断 四点共圆
    auto left = [](QVector<QVector2D> Points_vector){

        float AB =  sqrt(pow(Points_vector[1].y() - Points_vector[0].y(),2) + pow(Points_vector[1].x() - Points_vector[0].x(),2));
        float CD =  sqrt(pow(Points_vector[3].y() - Points_vector[2].y(),2) + pow(Points_vector[3].x() - Points_vector[2].x(),2));
        float BC =  sqrt(pow(Points_vector[2].y() - Points_vector[1].y(),2) + pow(Points_vector[2].x() - Points_vector[1].x(),2));
        float AD =  sqrt(pow(Points_vector[3].y() - Points_vector[0].y(),2) + pow(Points_vector[3].x() - Points_vector[0].x(),2));

        float AC =  sqrt(pow(Points_vector[2].y() - Points_vector[0].y(),2) + pow(Points_vector[2].x() - Points_vector[0].x(),2));
        float BD =  sqrt(pow(Points_vector[3].y() - Points_vector[1].y(),2) + pow(Points_vector[3].x() - Points_vector[1].x(),2));

        if(abs(AB*CD + BC*AD - AC *BD) < 0.000001)
        {
            std::cout<<"find the step"<<std::endl;
            return false;
        }

        return true;
    };

    // 拉伸 y 值
    if(circle_width>circle_height)
    {
        do
        {
            if(step == 2.0)
            {
                std::cout<<"can't find the step "<<std::endl;
            }

            for (int i = 0; i < 4; i++) {
                Points_vector[i].setY(OriginPoints_vector[i].y() * step);
            }

            step+=0.000001;

        }while(left(Points_vector) && step <= 2);

    }
    // 拉伸 x 值
    else
    {
        do
        {
            if(step == 2.0)
            {
                std::cout<<"can't find the step "<<std::endl;
            }

            for (int i = 0; i < 4; i++) {
                Points_vector[i].setX(OriginPoints_vector[i].x() * step);
            }

            step+=0.000001;

        }while(left(Points_vector) && step <= 2);

    }

    step_vector.push_back(step-0.00001);

    std::cout<<"the final step = "<<step<<std::endl;

    //    }



    //        QMap<float,int> num_map;
    //        for (int i = 0; i < 10 ; i++) {
    //            num_map[step_vector[i]]++;
    //        }

    //        int max = 0;
    //        float ans;
    //        for(auto it = num_map.begin();it != num_map.end();it++){
    //            if(it.value() > max){
    //                max = it.value();//后面的元素 value
    //                ans = it.key();//前面的元素 key
    //            }
    //        }

    //        std::cout<<ans<<std::endl;

    //        for (auto it = num_map.begin(); it!=num_map.end();it++)
    //        {
    //            std::cout<<"it.key: "<<it.key()<<" it.value: "<<it.value()<<std::endl;
    //        }

    // 已知椭圆上两点和圆心坐标(0,0)，求长短轴
    //    float x1 = head_circle[32].x(), y1 = head_circle[32].y();
    //    float x2 = head_circle[33].x(), y2 = head_circle[33].y();

    //    float b = sqrt((pow(x1,2)*pow(y2,2) - pow(x2,2)*pow(y1,2))/(pow(x1,2)-pow(x2,2)));
    //    float a = sqrt(pow(b,2)*pow(x1,2)/(pow(b,2)-pow(y1,2)));

    //    std::cout<<"a: "<<a<<" b: "<<b<<std::endl;

    //    int num = head_circle.size() - 1;

    //    float maxX = 0.0, minX = 0.0, maxY = 0.0, minY = 0.0;
    //    for(auto it = head_circle.begin(); it != head_circle.end(); it++)
    //    {
    //        if(it ->x() > maxX)
    //            maxX = it->x();

    //        if(it ->x() < minX)
    //            minX = it->x();

    //        if(it ->y() > maxY)
    //            maxY = it->y();

    //        if(it ->y() < minY)
    //            minY = it->y();

    //    }

    //    std::cout<<"maxX: "<<maxX<<" minX:"<<minX<<" maxY"<<maxY<<" minY"<<minY<<std::endl;

    // 利用长短轴画椭圆, 对比原来的椭圆数据，看匹配度

    //    cylinder_vector.clear();

    //    QVector<QVector2D> new_vector2D;

    //    for (int i = 0; i <= 360 ; i++) {

    //        t = i * 2 * M_PI / 360;
    //        QVector2D temp(maxX * cos(t), maxY * sin(t));
    //        new_vector2D.push_back(temp);

    //    }

}

void GLWidget::draw_cylinder()
{
    // 半径
    GLfloat r = 1.5;

    // 圆心
    GLfloat center[3] = {0.5, 0.4, 5.5};

    for ( int i = 0; i < 3; i++) {
        cylinder_vector.push_back(center[i]);
    }

    // 角度 (0-360) // 画第一个圆
    for (int i = 0; i <= 360; i++) {

        cylinder_vector.push_back(center[0] + r * cos(i*3.14/180));
        cylinder_vector.push_back(center[1] + r * sin(i*3.14/180));
        cylinder_vector.push_back(center[2] );

    }

    for (int i = 0; i <= 360; i++) {
        if(i == 45 ||  i == 90 ||  i == 135 || i == 180 || i == 225 || i == 270 || i == 315 ||  i == 360)
        {
            cylinder_vector.push_back(center[0] + r * cos(i*3.14/180));
            cylinder_vector.push_back(center[1] + r * sin(i*3.14/180));
            cylinder_vector.push_back(center[2] );
        }
    }

    // 360->45 多画了一次
    for (int i = 0; i <= 360; i++) {
        // 画大矩形
        if(i == 45 ||   i == 135 ||  i == 225 || i == 315 )
        {
            cylinder_vector.push_back(center[0] + r * cos(i*3.14/180));
            cylinder_vector.push_back(center[1] + r * sin(i*3.14/180));
            cylinder_vector.push_back(center[2] );

            if( i == 315 )
            {
                // 画大矩形的
                int j = 45;
                cylinder_vector.push_back(center[0] + r * cos(j*3.14/180));
                cylinder_vector.push_back(center[1] + r * sin(j*3.14/180));
                cylinder_vector.push_back(center[2] );

                // 画矩形的对角线
                int k = 225;
                cylinder_vector.push_back(center[0] + r * cos(k*3.14/180));
                cylinder_vector.push_back(center[1] + r * sin(k*3.14/180));
                cylinder_vector.push_back(center[2] );

                // 225->315->360
                cylinder_vector.push_back(center[0] + r * cos(315*3.14/180));
                cylinder_vector.push_back(center[1] + r * sin(315*3.14/180));
                cylinder_vector.push_back(center[2] );

                cylinder_vector.push_back(center[0] + r * cos(360*3.14/180));
                cylinder_vector.push_back(center[1] + r * sin(360*3.14/180));
                cylinder_vector.push_back(center[2] );
            }
        }
    }

    // 画第二个圆
    GLfloat center_2[3] = {0.5, 0.3, 2.5};

    for (int i = 0; i <= 360; i++) {

        cylinder_vector.push_back(center_2[0] + r * cos(i*3.14/180));
        cylinder_vector.push_back(center_2[1] + r * sin(i*3.14/180));
        cylinder_vector.push_back(center_2[2] );

    }

    // 画圆之间的区域
    float i = 0;
    do
    {
        cylinder_vector.push_back(center[0] + r * cos(i*3.14/180));
        cylinder_vector.push_back(center[1] + r * sin(i*3.14/180) );
        cylinder_vector.push_back(center[2] );

        // (i + 22.5)
        cylinder_vector.push_back(center_2[0] + r * cos(i*3.14/180));
        cylinder_vector.push_back(center_2[1] + r * sin(i*3.14/180));
        cylinder_vector.push_back(center_2[2] );

        i += 11.25;
    }while(i<=360);

    for (int i = 0; i <= 360; i++) {
        if(i == 45 ||  i == 90 ||  i == 135 || i == 180 || i == 225 || i == 270 || i == 315 ||  i == 360)
        {
            cylinder_vector.push_back(center_2[0] + r * cos(i*3.14/180));
            cylinder_vector.push_back(center_2[1] + r * sin(i*3.14/180));
            cylinder_vector.push_back(center_2[2] );
        }
    }

    // 360->315 多画了一次
    for (int i = 360; i >= 0; i--) {
        // 画大矩形
        if(i == 315 ||   i == 225 ||  i == 135 || i == 45 )
        {
            cylinder_vector.push_back(center_2[0] + r * cos(i*3.14/180));
            cylinder_vector.push_back(center_2[1] + r * sin(i*3.14/180));
            cylinder_vector.push_back(center_2[2] );

            if( i == 135 )
            {
                // 画矩形的对角线
                int k = 315;
                cylinder_vector.push_back(center_2[0] + r * cos(k*3.14/180));
                cylinder_vector.push_back(center_2[1] + r * sin(k*3.14/180));
                cylinder_vector.push_back(center_2[2] );

            }

            if(i==45)
            {
                int k = 135;
                cylinder_vector.push_back(center_2[0] + r * cos(k*3.14/180));
                cylinder_vector.push_back(center_2[1] + r * sin(k*3.14/180));
                cylinder_vector.push_back(center_2[2] );
            }
        }
    }

    std::cout<<"the cylinder_vector size is: "<< cylinder_vector.size() <<std::endl;

}
