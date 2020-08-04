#include "common.h"
#include <math.h>
#include <iostream>
#include <QVector2D>
#include <QVector>

#include <random>
#include<algorithm>

Common::Common()
{

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

    std::cout<<"k_vec.size: "<<k_vec.size()<<std::endl;

    //    std::cout<<"meanK: "<<meank<<std::endl;

    return meank;
}



