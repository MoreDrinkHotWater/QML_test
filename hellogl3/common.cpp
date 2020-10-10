#include "common.h"

#include <math.h>
#include <iostream>
#include <QVector2D>
#include <QVector>
#include <random>
#include <algorithm>
#include <QVector3D>

Common::Common()
{

}

Common *Common::getInstance(){
    static Common _instance;
    return &_instance;
}

// 计算方差
float Common::variance (QVector<float> vector){

    double sum = std::accumulate(std::begin(vector) + 1, std::end(vector) - 1, 0.0);
    double mean =  sum / vector.size(); //均值

    double accum  = 0.0;
    std::for_each (std::begin(vector), std::end(vector), [&](const double d) {
        accum  += (d - mean)*(d - mean);
    });

    float stdev = sqrt(accum/(vector.size()-1)); //方差

    return stdev;
};

// 计算斜率的方差
float Common::calculate_k(QVector<QVector2D> vec)
{
    // 保存斜率的数组
    QVector<float> k_vec;

    int n = vec.size()-1;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dist_int(1, n);

    // 计算 100 组斜率，比较方差
    for(int i = 0; i < 100; i++)
    {
        int first_sub = dist_int(gen), end_sub = dist_int(gen);
        QVector2D first(QVector2D(vec[first_sub].x(), vec[first_sub].y()));
        QVector2D end(QVector2D(vec[end_sub].x(), vec[end_sub].y()));
        float  k = (first.y()-end.y())/(first.x() - end.x());

        //        std::cout<<"k: "<<k<<std::endl;

        // 处理掉无效数据
        if(!std::isnan(k) && !std::isinf(k))
            k_vec.push_back(k);
    }

    //    std::cout<< "k_vec size: "<<k_vec.size()<<std::endl;

    float vari = variance(k_vec);

    //    std::cout<< "vari: "<<vari<<std::endl;

    return vari;
}

// 计算斜率的均值
float Common::calculate_meanK(QVector<QVector2D> vec, int &num)
{
    // 保存斜率的数组
    QVector<float> k_vec;
    float sum = 0;

    int n = vec.size()-1;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dist_int(1, n);

    // 计算 100 组斜率，比较方差
    for(int i = 0; i < num; i++)
    {
        int first_sub = dist_int(gen), end_sub = dist_int(gen);
        QVector2D first(QVector2D(vec[first_sub].x(), vec[first_sub].y()));
        QVector2D end(QVector2D(vec[end_sub].x(), vec[end_sub].y()));
        float  k = (first.y()-end.y())/(first.x() - end.x());

        // 处理掉无效数据
        if(!std::isnan(k) && !std::isinf(k))
        {
            //            std::cout<<"k: "<<k<<std::endl;

            sum+=k;

            k_vec.push_back(k);
        }
    }

    float meank = sum/k_vec.size();

    num = k_vec.size();

//    std::cout<<"k_vec.size: "<<k_vec.size()<<std::endl;

    //    std::cout<<"meanK: "<<meank<<std::endl;

    return meank;
}

// 屏幕坐标转世界坐标
QStack<QVector<float>> Common::coordinate_transformation(QStack<QVector<float>> draw_stack)
{

    QStack<QVector<float>> draw_coorstack;

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

    return draw_coorstack;
}

// 查找最大，最小值
void Common::findMinMax(QVector<QVector2D> head_path, QVector2D &min,QVector2D &max){
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

// 计算倾斜度 return  1/cos()
float Common::mapEllipseToCircle(QVector<QVector2D> &head_path){

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

// 计算多边形面积
float Common::calculateArea(QVector<QVector2D> &vec)
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


// 三角化
void Common::genTriangle(QVector<float> &vec,QVector3D p0,QVector3D p1,QVector3D p2){
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
void Common::genRectangleZ(QVector<float> &vec,QVector3D p0,QVector3D p1){
    QVector3D p2 = QVector3D(p1.x(),p1.y(),p0.z());
    QVector3D p3 = QVector3D(p0.x(),p0.y(),p1.z());
    // 定义的顺序是逆时针！ 要逆时针画鸭鸭鸭！
    genTriangle(vec,p0,p3,p2);
    genTriangle(vec,p2,p3,p1);
}


