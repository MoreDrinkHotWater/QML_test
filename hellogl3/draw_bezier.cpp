#include "draw_bezier.h"

#include <GL/glut.h>
#include <iostream>
#include <QThread>
#include <QVector3D>

GLfloat xwcMin = -50.0, xwcMax = 50.0;
GLfloat ywcMin = -50.0, ywcMax = 50.0;

#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE 4

Draw_bezier::~Draw_bezier()
{
    std::cout<<"=================~Draw_bezier================="<<std::endl;
}

void processMenuEvents(int option) {
        //option，就是传递过来的value的值。
         switch (option) {
                 case RED :glutDestroyWindow(1);
                 case GREEN :
                 case BLUE :
                 case WHITE :break;
         }
}

void createGLUTMenus() {

    int menu;

    // 创建菜单并告诉GLUT，processMenuEvents处理菜单事件。
    menu = glutCreateMenu(processMenuEvents);

    //给菜单增加条目
    glutAddMenuEntry("Red",RED);
    glutAddMenuEntry("Blue",BLUE);
    glutAddMenuEntry("Green",GREEN);
    glutAddMenuEntry("White",WHITE);

    // 把菜单和鼠标右键关联起来。
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

QVector<QVector3D> Draw_bezier::draw_vector;

void Draw_bezier::receiver_bezierSlot(QVector<QVector3D> draw_vector)
{

    Draw_bezier::draw_vector = draw_vector;

    std::cout<<"Draw_bezier::draw_vector.size: "<<Draw_bezier::draw_vector.size()<<std::endl;

    std::cout << "Draw_bezier thread: " << QThread::currentThreadId() << std::endl;

    /*
     *  测试 glut 库
    */
    int argc = 0; // 这里不赋初值，程序会发生错误
    char **argv = nullptr;
    glutInit (&argc, argv); // glut 环境初始化
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); // 显示模式初始化
    glutInitWindowPosition(50,50);
    glutInitWindowSize(600, 600);
    int window = glutCreateWindow("Bezier Curve");

    std::cout<<"window: "<<window<<std::endl;

    // 背景设置成白色
    glClearColor(1.0, 1.0, 1.0, 0.0);

    //调用我们的函数来创建菜单
    createGLUTMenus();

    glutDisplayFunc(draw_bezier); // draw_bezier: 绘图函数
    glutReshapeFunc(winReshapeFcn); // winReshapeFcn: 世界坐标裁剪窗口

    glutMainLoop(); // 事件循环

}

void Draw_bezier::winReshapeFcn(int newWidth, int newHeight)
{
    glViewport(0,0, newHeight, newWidth);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(xwcMin, xwcMax, ywcMin, ywcMax);

    glClear(GL_COLOR_BUFFER_BIT);
}

void Draw_bezier::draw_bezier(void)
{
    glClear(GL_COLOR_BUFFER_BIT); // clear display window

    std::cout<<"============bezier============="<<std::endl;

    int multiple = 30;

#if 0
    QVector<QVector3D> draw_vector;

    QVector3D p1 = QVector3D(-40, -40, 0);
    QVector3D p2 = QVector3D(-10, 200, 0);
    QVector3D p3 = QVector3D(10, -200, 0);
    QVector3D p4 = QVector3D(40, 40, 0);

    draw_vector.push_back(p1);
    draw_vector.push_back(p2);
    draw_vector.push_back(p3);
    draw_vector.push_back(p4);

    multiple = 1;
#endif

    int row = draw_vector.size();

    std::cout<<"row: "<<row<<std::endl;

    int col = 3;

    GLfloat ctrlPts [row][col];

    for(int i = 0; i < row; i++)
    {
        ctrlPts[i][0] = multiple * draw_vector[i].x();
        ctrlPts[i][1] = multiple * draw_vector[i].y();
        ctrlPts[i][2] = draw_vector[i].z();
    }

    // 输出
    for(int j = 0; j < row; j++)
    {
        std::cout<<"j: "<<ctrlPts[j][0]<< " " << ctrlPts[j][1] << " "<< ctrlPts[j][2]<<std::endl;
    }

    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, row, *ctrlPts);
    glEnable(GL_MAP1_VERTEX_3);

    GLint k;

    glColor3f(0.0, 0.0, 1.0);  // line
    glMapGrid1f(50, 0.0, 1.0);
    glEvalMesh1(GL_LINE, 0, 50);

    glColor3f(1.0, 0.0, 0.0); // points
    glPointSize(row + 1);
    glBegin(GL_POINTS);
    for(k = 0; k < row; k++)
        glVertex3fv(&ctrlPts[k][0]);
    glEnd();

    std::cout<<"============bezier end============="<<std::endl;

    glFlush();
}
