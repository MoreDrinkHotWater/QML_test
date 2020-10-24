#include "draw_bezier.h"

#include <GL/glut.h>
#include <iostream>
#include <QThread>
#include <QVector3D>
#include <math.h>

GLfloat xwcMin = -50.0, xwcMax = 50.0;
GLfloat ywcMin = -50.0, ywcMax = 50.0;

Draw_bezier *Draw_bezier::getInstance(){
    static Draw_bezier _instance;
    return &_instance;
}

Draw_bezier::~Draw_bezier()
{
    std::cout<<"=================~Draw_bezier================="<<std::endl;
}

QVector<QVector3D> Draw_bezier::draw_vector;

QVector<float> Draw_bezier::bezierCurve_vector;

// 画点
void plotPoint(wcPt3D bzeCurvePt)
{
    glBegin(GL_POINTS);
        glVertex2f(bzeCurvePt.x, bzeCurvePt.y);
    glEnd();
}

// 计算二次项系数
void Draw_bezier::binomialCoeffs(GLint n, GLint *C)
{
    GLint k,j;
    for(k = 0; k <= n; k++)
    {
        C[k]=1;
        for(j = n; j >= k+1; j--)
            C[k] *= j;
        for(j = n-k; j>=2; j--)
            C[k] /= j;
    }
}

// 计算曲线路径坐标
void Draw_bezier::computeBezpt(GLfloat u, wcPt3D *bezPt, GLint nCtrlPts, wcPt3D *ctrlPts, GLint *C)
{
    GLint k,n = nCtrlPts -1;
    GLfloat bezBlendFcn;

    bezPt->x = bezPt->y = bezPt->z = 0.0;

    for(k = 0; k < nCtrlPts; k++)
    {
        bezBlendFcn = C[k] * pow(u,k) * pow(1-u, n-k);
        bezPt->x += ctrlPts[k].x * bezBlendFcn;
        bezPt->y += ctrlPts[k].y * bezBlendFcn;
        bezPt->z += ctrlPts[k].z * bezBlendFcn;
    }

    std::cout<<"bezPt: "<<bezPt->x/50.0<<" "<<bezPt->y/50.0<<" "<<bezPt->z<<std::endl;
}

// 曲线公式计算
void Draw_bezier::bezier(wcPt3D *ctrlPts, GLint nCtrlPts, GLint nBezCurvePts)
{
    wcPt3D bezCurvePt;
    GLfloat u;
    GLint *C, k;

    C = new GLint[nCtrlPts];

    binomialCoeffs(nCtrlPts-1, C);

    glBegin(GL_LINE_STRIP);
    for(k = 0; k <= nBezCurvePts; k++)
    {
        u = GLfloat(k)/GLfloat(nBezCurvePts);
        glEvalCoord1f(u);
        computeBezpt(u, &bezCurvePt, nCtrlPts, ctrlPts, C);

        if(k != nBezCurvePts)
        {
            bezierCurve_vector.push_back(bezCurvePt.x/50.0);
            bezierCurve_vector.push_back(- bezCurvePt.y/50.0);
        }
//        plotPoint(bezCurvePt);
    }
    glEnd();
    delete  [] C;
}

// 样条细分
void Draw_bezier::spline_subdivision(QVector<QVector3D> draw_vector)
{
    int row = draw_vector.size();

    std::cout<<"spline_subdivision row: "<<row<<std::endl;

    wcPt3D ctrlPts[row];

    GLfloat test_ctrlPts [row][3];

    int multiple = 50;

    for(int i = 0; i < row; i++)
    {
        ctrlPts[i].x = multiple * draw_vector[i].x();
        ctrlPts[i].y = multiple * draw_vector[i].y();
        ctrlPts[i].z = draw_vector[i].z();

        test_ctrlPts[i][0] = multiple * draw_vector[i].x();
        test_ctrlPts[i][1] = multiple * draw_vector[i].y();
        test_ctrlPts[i][2] = draw_vector[i].z();
    }

    GLint nCtrlPts = row, nBezCurvePts = 1000;

    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, row, *test_ctrlPts);
    glEnable(GL_MAP1_VERTEX_3);

    // 反走样
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);  // Antialias the lines
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(0.5);
    glColor3f(0.0, 0.0, 1.0);

    bezier(ctrlPts, nCtrlPts, nBezCurvePts);

    glFlush();
}

void Draw_bezier::winReshapeFcn(int newWidth, int newHeight)
{
    glViewport(0,0, newHeight, newWidth);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(xwcMin, xwcMax, ywcMin, ywcMax);

    glClear(GL_COLOR_BUFFER_BIT);
}

void Draw_bezier::keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:   // ESC键
        exit(0);
        break;
    default:
        break;
    }
}

// 绘图函数
void Draw_bezier::draw_bezier(void)
{
    glClear(GL_COLOR_BUFFER_BIT); // clear display window

    int row = draw_vector.size();

    std::cout<<"row: "<<row<<std::endl;

    for(int i = 0; i < row; i+=3)
    {
        std::cout<<"i: "<<i<<std::endl;

        QVector<QVector3D> temp_vector;

        std::cout<<"================================"<<std::endl;

        // 均匀分段
        int num = abs(row - i);
        if(num >= 4)
        {
            temp_vector.push_back(draw_vector[i]);
            temp_vector.push_back(draw_vector[i+1]);
            temp_vector.push_back(draw_vector[i+2]);
            temp_vector.push_back(draw_vector[i+3]);
        }
        // 多余分段
        else
        {
            for(int j = 0; j < num; j++)
                temp_vector.push_back(draw_vector[i + j]);
        }

        for(auto point : temp_vector)
            std::cout<<"point: "<<point.x()<<" "<<point.y()<<" "<<point.z()<<std::endl;

        // 样条细分
        if(temp_vector.size() > 0)
            spline_subdivision(temp_vector);
    }
}

void Draw_bezier::receiver_bezierSlot(QVector<QVector3D> draw_vector)
{
    Draw_bezier::draw_vector = draw_vector;

    std::cout << "Draw_bezier thread: " << QThread::currentThreadId() << std::endl;

    /*
     *  测试 glut 库
    */
    int argc = 0; // 这里不赋初值，程序会发生错误
    char **argv = nullptr;
    glutInit (&argc, argv); // glut 环境初始化
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); // 显示模式初始化
    glutInitWindowPosition(650,150);
    glutInitWindowSize(602, 612);
    glutCreateWindow("Bezier Curve");

    glClearColor(1.0, 1.0, 1.0, 0.0);  // 背景设置成白色

    glutDisplayFunc(draw_bezier); // draw_bezier: 绘图函数
    glutReshapeFunc(winReshapeFcn); // winReshapeFcn: 世界坐标裁剪窗口
    glutKeyboardFunc(keyboard);

    glutMainLoop(); // 事件循环
}
