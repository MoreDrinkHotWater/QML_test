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

    auto judge_parallel = [](int c1, int c2,int c3, int c4, QVector<QVector2D> cube){
        QVector2D p(cube[c1].x()-cube[c2].x(), cube[c1].y()-cube[c2].y());

        QVector2D q(cube[c3].x()-cube[c4].x(), cube[c3].y()-cube[c4].y());

        float dotproduct = p.x()*q.x() + p.y()*q.y();

        float length = sqrt(pow(p.x(),2)+pow(p.y(),2)) * sqrt(pow(q.x(),2)+pow(q.y(),2));

        // cos threa
        std::cout<<"cos： "<<dotproduct/length<<std::endl;

        return dotproduct/length;
    };

    if(abs(1 - judge_parallel(2,1,3,0,cube)) < 0.1 && abs(1 - judge_parallel(2,3,1,0,cube)) < 0.1)
    {
        std::cout<<"up is 平行四边形"<<std::endl;
    }

    if(abs(1 - judge_parallel(3,0,4,5,cube)) < 0.1 && abs(1 - judge_parallel(3,4,0,5,cube)) < 0.1)
    {
        std::cout<<"front is 平行四边形"<<std::endl;
    }

    if(abs(1 - judge_parallel(1,0,6,5,cube)) < 0.1 && abs(1 - judge_parallel(1,6,0,5,cube)) < 0.1)
    {
        std::cout<<"right is 平行四边形"<<std::endl;
    }

    // 计算长宽高
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

    cube_length = sqrt(pow(cube[3].x()-cube[0].x(),2) + pow(cube[3].y()-cube[0].y(),2) + pow(z2-0,2));

    cube_width = sqrt(pow(cube[1].x()-cube[0].x(),2) + pow(cube[1].y()-cube[0].y(),2) + pow(z1-0,2));

    cube_height = sqrt(pow(cube[5].x()-cube[0].x(),2) + pow(cube[5].y()-cube[0].y(),2) + pow(z3-0,2));

    std::cout<<"cube_length: "<<cube_length<<" cube_width: "<<cube_width<<" cube_height: "<<cube_height<<std::endl;


    // 构建立方体顶点数组
    cube_vector =
    {
        // up
        cube_length/2,0,-cube_width/2,
        cube_length/2,0,cube_width/2,
        -cube_length/2,0,cube_width/2,
        -cube_length/2,0,-cube_width/2,
        cube_length/2,0,-cube_width/2,
        -cube_length/2,0,cube_width/2,

        // left
        -cube_length/2,cube_height,cube_width/2,
        -cube_length/2,cube_height,-cube_width/2,
        -cube_length/2,0,-cube_width/2,
        -cube_length/2,0,cube_width/2,
        -cube_length/2,cube_height,-cube_width/2,

        // down
        -cube_length/2,cube_height,cube_width/2,
        cube_length/2,cube_height,cube_width/2,
        cube_length/2,cube_height,-cube_width/2,
        -cube_length/2,cube_height,-cube_width/2,
        cube_length/2,cube_height,cube_width/2,

        // right
        cube_length/2,cube_height,-cube_width/2,
        cube_length/2,0,-cube_width/2,
        cube_length/2,0,cube_width/2,
        cube_length/2,cube_height,cube_width/2,
        cube_length/2,0,-cube_width/2,

        // front
        -cube_length/2,cube_height,-cube_width/2,

        // rear
        -cube_length/2,0,cube_width/2,
        cube_length/2,cube_height,cube_width/2,

    };

    return true;
}

// 识别 cube 成功后，构建 cube_vector 用于渲染
void RecognizeCube::draw_cube()
{
    QVector<GLfloat> temp;
    cube_vector = temp;
}
