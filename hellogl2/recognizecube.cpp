#include "recognizecube.h"

#include <iostream>
#include <QOpenGLFunctions>
#include <QVector2D>

#include <random>

// 构建数据， 并识别是否是 cube
bool RecognizeCube::recognize_cube()
{
    //    std::cout<<"=============test recognize_cube function============="<<std::endl;

    QVector<QVector2D> up_cube;

    // 矩形的中心
    QVector2D center(0, 0);
    cube_vector.push_back(center.x());
    cube_vector.push_back(center.y());
    cube_vector.push_back(0);

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(1.0, 5.0);
    // 偏差
    std::uniform_real_distribution<> dis_coords(0.0, 0.1);

    // 立方体的长宽高
    GLfloat cube_length = dis(gen);
    GLfloat cube_width = dis(gen);
    GLfloat cube_height = dis(gen);

    std::cout<<"cube_length: "<<cube_length<<" cube_width: "<<cube_width<<" cube_height: "<<cube_height<<std::endl;


    QVector<QVector2D> cube;
    float x = 8.472, y = 12.944;
    cube.push_back(QVector2D(8.472 - x,12.944 - y));
    cube.push_back(QVector2D(9.639 - x,11.778 - y));
    cube.push_back(QVector2D(5.236 - x,10.819 - y));
    cube.push_back(QVector2D(3.833 - x,11.736 - y));
    cube.push_back(QVector2D(4.000 - x,13.931 - y));
    cube.push_back(QVector2D(8.306 - x,15.278 - y));
    cube.push_back(QVector2D(9.403 - x,13.986 - y));

//    4.000 + (5.236-3.833)
    // x4 + (x2-x3), y4 + (y2-y3), z2
    cube.push_back(QVector2D(4.000 + abs(5.236-3.833) - x,13.931 + (10.819 - 11.778) - y));

    float z1,z2,z3;
    float n1 = cube[1].x()*cube[3].x() + cube[1].y()*cube[3].y();
    float n2 = cube[1].x()*cube[5].x() + cube[1].y()*cube[5].y();
    float n3 = cube[3].x()*cube[5].x() + cube[3].y()*cube[5].y();

    z2 = sqrt(n1*n2/(-n3));

    z1 = (-n1)/z2;

    z3 = (-n2)/z2;

    if(z1*z2 + n1 == 0.0)
    {
        std::cout<<"z1: "<<z1<<" z2: "<<z2<<" z3: "<<z3<<std::endl;
    }

    // 6,7,4,5 ->5,8,9(z2)
    int draw_index[10]={0,1,2,3,0,5,6,7,4,5};

    for(int i = 0; i < 10; i++)
    {
        cube_vector.push_back(cube[draw_index[i]].x());
        cube_vector.push_back(cube[draw_index[i]].y());
        if(i==0||i==3||i==4||i==5||i==8||i==9)
            cube_vector.push_back(z2);
        else
            cube_vector.push_back(z1);
    }

    // 画二维
//    for(int i = 0; i < 4; i++)
//    {
//        cube_vector.push_back(cube[i].x());
//        cube_vector.push_back(cube[i].y());
//        cube_vector.push_back(0);
//    }

//    cube_vector.push_back(cube[0].x());
//    cube_vector.push_back(cube[0].y());
//    cube_vector.push_back(0);

//    cube_vector.push_back(cube[5].x());
//    cube_vector.push_back(cube[5].y());
//    cube_vector.push_back(0);

//    cube_vector.push_back(cube[4].x());
//    cube_vector.push_back(cube[4].y());
//    cube_vector.push_back(0);

//    cube_vector.push_back(cube[3].x());
//    cube_vector.push_back(cube[3].y());
//    cube_vector.push_back(0);

//    cube_vector.push_back(cube[0].x());
//    cube_vector.push_back(cube[0].y());
//    cube_vector.push_back(0);

//    cube_vector.push_back(cube[1].x());
//    cube_vector.push_back(cube[1].y());
//    cube_vector.push_back(0);

//    cube_vector.push_back(cube[6].x());
//    cube_vector.push_back(cube[6].y());
//    cube_vector.push_back(0);

//    cube_vector.push_back(cube[5].x());
//    cube_vector.push_back(cube[5].y());
//    cube_vector.push_back(0);

    // 画三维
    // 画上矩形
    //    GLfloat step = -cube_width/2;
    //    do
    //    {
    //        cube_vector.push_back(center.x() + cube_length/2);
    //        cube_vector.push_back(center.y() + step);
    //        cube_vector.push_back(0);

    //        QVector2D temp(center.x() + cube_length/2, center.y() + step);
    //        up_cube.push_back(temp);

    //        step+=0.01;
    //    }while(step<cube_width/2);

    //    step = cube_length/2;
    //    do
    //    {
    //        cube_vector.push_back(center.x() + step );
    //        cube_vector.push_back(center.y() + cube_width/2);
    //        cube_vector.push_back(0);

    //        QVector2D temp(center.x() + step, center.y() + cube_width/2);
    //        up_cube.push_back(temp);

    //        step-=0.01;
    //    }while(step>-cube_length/2);

    //    step = cube_width/2;
    //    do
    //    {
    //        cube_vector.push_back(center.x() - cube_length/2 );
    //        cube_vector.push_back(center.y() + step);
    //        cube_vector.push_back(0);

    //        QVector2D temp(center.x() - cube_length/2, center.y() + step);
    //        up_cube.push_back(temp);

    //        step-=0.01;
    //    }while(step>-cube_width/2);

    //    step = -cube_length/2;
    //    do
    //    {
    //        cube_vector.push_back(center.x() + step );
    //        cube_vector.push_back(center.y() - cube_width/2);
    //        cube_vector.push_back(0);

    //        QVector2D temp(center.x() + step, center.y() - cube_width/2);
    //        up_cube.push_back(temp);

    //        step+=0.01;
    //    }while(step<cube_length/2);

    //    cube_vector.push_back(center.x() + cube_length/2);
    //    cube_vector.push_back(center.y() - cube_width/2);
    //    cube_vector.push_back(cube_height);

    //    cube_vector.push_back(center.x() + cube_length/2);
    //    cube_vector.push_back(center.y() + cube_width/2);
    //    cube_vector.push_back(cube_height);

    //    cube_vector.push_back(center.x() + cube_length/2);
    //    cube_vector.push_back(center.y() + cube_width/2);
    //    cube_vector.push_back(0);

    //    // 多画两条
    //    cube_vector.push_back(center.x() + cube_length/2);
    //    cube_vector.push_back(center.y() - cube_width/2);
    //    cube_vector.push_back(0);

    //    cube_vector.push_back(center.x() + cube_length/2);
    //    cube_vector.push_back(center.y() - cube_width/2);
    //    cube_vector.push_back(cube_height);

    //    cube_vector.push_back(center.x() - cube_length/2);
    //    cube_vector.push_back(center.y() - cube_width/2);
    //    cube_vector.push_back(cube_height);

    //    cube_vector.push_back(center.x() - cube_length/2);
    //    cube_vector.push_back(center.y() - cube_width/2);
    //    cube_vector.push_back(0);

    return true;
}

// 识别 cube 成功后，构建 cube_vector 用于渲染
void RecognizeCube::draw_cube()
{
    QVector<GLfloat> temp;
    cube_vector = temp;
}
