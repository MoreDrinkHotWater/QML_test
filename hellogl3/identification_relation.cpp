#include "identification_relation.h"
#include "common.h"

#include <QVector2D>
#include <math.h>
#include <iostream>

Identification_relation::Identification_relation()
{
    common = Common::getInstance();
    height_1 = 0;
    height_2 = 0;
}

Identification_relation *Identification_relation::getInstance(){
    static Identification_relation _instance;
    return &_instance;
}

void Identification_relation::find_cylinderNode(QVector<float> vec)
{
    QVector<QVector2D> head_circle;

    for (int i = 0; i < vec.size(); i+=2) {
        QVector2D temp(vec[i],vec[i+1]);
        head_circle.push_back(temp);
    }

    float maxX = head_circle[0].x(), minX = maxX, maxY = head_circle[0].y(), minY = maxY;

    for(auto it = head_circle.begin(); it != head_circle.end(); it++)
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

    // 记录椭圆的左右结点
    cylinder_left = QVector2D(minX, (maxY + minY) / 2);
    cylinder_right = QVector2D(maxX, (maxY + minY) / 2);

    std::cout<<"cylinder_left.x: "<<cylinder_left.x()<<std::endl;
    std::cout<<"cylinder_left.y: "<<cylinder_left.y()<<std::endl;
}

void Identification_relation::find_straightLineNode(QVector<float> vec)
{
    // 记录直线首尾的结点
    straightLine_first = QVector2D(vec[0],vec[1]);
    straightLine_end = QVector2D(vec[vec.size()-2],vec[vec.size()-1]);

    std::cout<<"straightLine_first.x: "<<straightLine_first.x()<<std::endl;
    std::cout<<"straightLine_end.y: "<<straightLine_end.y()<<std::endl;

}

void Identification_relation::find_curveLineNode(QVector<float> vec)
{
    // 记录曲线首尾的结点
    curveLine_first = QVector2D(vec[0],vec[1]);
    curveLine_end = QVector2D(vec[vec.size()-2],vec[vec.size()-1]);

    std::cout<<"curveLine_first.x: "<<curveLine_first.x()<<std::endl;
    std::cout<<"curveLine_end.y: "<<curveLine_end.y()<<std::endl;
}

void Identification_relation::find_wavyLineNode(QVector<float> vec)
{
    // 记录波浪线首尾的结点
    wavyLine_first = QVector2D(vec[0],vec[1]);
    wavyLine_end = QVector2D(vec[vec.size()-2],vec[vec.size()-1]);

    std::cout<<"wavyLine_first.x: "<<wavyLine_first.x()<<std::endl;
    std::cout<<"wavyLine_end.y: "<<wavyLine_end.y()<<std::endl;
}

// 判断平行
bool Identification_relation::parallel(QVector<float> _vec1, QVector<float> _vec2)
{
    QVector<QVector2D> vec1,vec2;

    for(int i = 0; i < _vec1.size(); i+=2)
    {
        QVector2D temp = QVector2D(_vec1[i],_vec1[i+1]);
        vec1.push_back(temp);
    }

    for(int i = 0; i < _vec2.size(); i+=2)
    {
        QVector2D temp = QVector2D(_vec2[i],_vec2[i+1]);
        vec2.push_back(temp);
    }

    int num_1 = 100;
    int num_2 = 100;

    // 比较斜率的均值
    if(abs(common->calculate_meanK(vec1, num_1) - common->calculate_meanK(vec2, num_2)) < 0.3)
        return true;
    else if(num_1 < 80 && num_2 < 80)
        return true;
    else
        return false;

}

// 判断是否等长
bool Identification_relation::equilong(QVector<float> _vec1, QVector<float> _vec2)
{
    QVector<QVector2D> vec1,vec2;

    for(int i = 0; i < _vec1.size(); i+=2)
    {
        QVector2D temp = QVector2D(_vec1[i],_vec1[i+1]);
        vec1.push_back(temp);
    }

    for(int i = 0; i < _vec2.size(); i+=2)
    {
        QVector2D temp = QVector2D(_vec2[i],_vec2[i+1]);
        vec2.push_back(temp);
    }

    float vec1_length = sqrt(pow(vec1[0].x() - vec1[vec1.size()-1].x(),2) + pow(vec1[0].y() - vec1[vec1.size()-1].y(),2));
    float vec2_length = sqrt(pow(vec2[0].x() - vec2[vec2.size()-1].x(),2) + pow(vec2[0].y() - vec2[vec2.size()-1].y(),2));
    if(abs(vec1_length - vec2_length) < 0.1)
        return true;
    else
        return false;
}

// 判断相连
bool Identification_relation::join(QString str_1, QString str_2, QVector<float> _vec1, QVector<float> _vec2)
{

    auto jundge = [](QVector2D first_1,QVector2D end_1,QVector2D first_2,QVector2D end_2){

        float first_first = sqrt(pow(first_1.x() - first_2.x(),2) + pow(first_1.y() - first_2.y(),2));
        float first_end = sqrt(pow(first_1.x() - end_2.x(),2) + pow(first_1.y() - end_2.y(),2));

        float end_first = sqrt(pow(end_1.x() - first_2.x(),2) + pow(end_1.y() - first_2.y(),2));
        float end_end = sqrt(pow(end_1.x() - end_2.x(),2) + pow(end_1.y() - end_2.y(),2));

        if(abs(first_first-0)<0.2 || abs(first_end-0)<0.2 || abs(end_first-0)<0.2 || abs(end_end-0)<0.2)
            return true;
        else
            return false;
    };

    // 椭圆和直线
    if(str_1 == "cylinder" && str_2 == "straightLine")
    {
        // 1.椭圆的左边和直线的头相连，或者尾相连
        // 2.椭圆的右边和直线的头相连，或者尾相连
        // 一共四种情况

        find_cylinderNode(_vec1);
        find_straightLineNode(_vec2);

        return jundge(cylinder_left, cylinder_right, straightLine_first, straightLine_end);
    }
    else if(str_1 == "straightLine" && str_2 == "cylinder")
    {
        find_straightLineNode(_vec1);
        find_cylinderNode(_vec2);

        return jundge(cylinder_left, cylinder_right, straightLine_first, straightLine_end);
    }
    // 直线和曲线
    else if(str_1 == "straightLine" && str_2 == "curveLine")
    {
        find_straightLineNode(_vec1);
        find_curveLineNode(_vec2);

        return jundge(straightLine_first, straightLine_end, curveLine_first, curveLine_end);
    }
    else if(str_1 == "curveLine" && str_2 == "straightLine")
    {
        find_curveLineNode(_vec1);
        find_straightLineNode(_vec2);

        return jundge(straightLine_first, straightLine_end, curveLine_first, curveLine_end);
    }
    // 椭圆和波浪线
    else if(str_1 == "cylinder" && str_2 == "wavyLine")
    {
        find_cylinderNode(_vec1);
        find_wavyLineNode(_vec2);

        if(height_1 == 0)
            // 给个高度值
            height_1 = abs(wavyLine_first.y() - wavyLine_end.y());
        else
            height_2 = abs(wavyLine_first.y() - wavyLine_end.y());

        return jundge(cylinder_left, cylinder_right, wavyLine_first, wavyLine_end);
    }
    else if(str_1 == "wavyLine" && str_2 == "cylinder")
    {
        find_wavyLineNode(_vec1);
        find_cylinderNode(_vec2);

        return jundge(cylinder_left, cylinder_right, wavyLine_first, wavyLine_end);
    }
    // 波浪线和曲线
    else if(str_1 == "wavyLine" && str_2 == "curveLine")
    {
        find_wavyLineNode(_vec1);
        find_curveLineNode(_vec2);

        return jundge(curveLine_first, curveLine_end, wavyLine_first, wavyLine_end);
    }
    else if(str_1 == "curveLine" && str_2 == "wavyLine")
    {
        find_wavyLineNode(_vec1);
        find_cylinderNode(_vec2);

        return jundge(curveLine_first, curveLine_end, wavyLine_first, wavyLine_end);
    }
    else
        return false;

}

// 判断相离
bool Identification_relation::separation(QString str_1, QString str_2, QVector<float> _vec1, QVector<float> _vec2)
{
    auto jundge = [](QVector2D first_1,QVector2D end_1,QVector2D first_2,QVector2D end_2){

        float first_first = sqrt(pow(first_1.x() - first_2.x(),2) + pow(first_1.y() - first_2.y(),2));
        float first_end = sqrt(pow(first_1.x() - end_2.x(),2) + pow(first_1.y() - end_2.y(),2));

        float end_first = sqrt(pow(end_1.x() - first_2.x(),2) + pow(end_1.y() - first_2.y(),2));
        float end_end = sqrt(pow(end_1.x() - end_2.x(),2) + pow(end_1.y() - end_2.y(),2));

        if(abs(first_first-0)>0.2 || abs(first_end-0)>0.2 || abs(end_first-0)>0.2 || abs(end_end-0)>0.2)
            return true;
        else
            return false;
    };

    // 椭圆和曲线
    if(str_1 == "cylinder" && str_2 == "curveLine")
    {
        find_cylinderNode(_vec1);
        find_curveLineNode(_vec2);
        return jundge(cylinder_left, cylinder_right, curveLine_first, curveLine_first);
    }
    else if(str_1 == "curveLine" && str_2 == "cylinder")
    {
        find_curveLineNode(_vec1);
        find_cylinderNode(_vec2);
        return jundge(cylinder_left, cylinder_right, curveLine_first, curveLine_first);
    }
    // 直线和波浪线
    else if(str_1 == "straightLine" && str_2 == "wavyLine")
    {
        find_straightLineNode(_vec1);
        find_wavyLineNode(_vec2);

        return jundge(straightLine_first, straightLine_end, wavyLine_first, wavyLine_end);
    }
    else if(str_1 == "wavyLine" && str_2 == "straightLine")
    {
        find_wavyLineNode(_vec1);
        find_straightLineNode(_vec2);

        return jundge(straightLine_first, straightLine_end, wavyLine_first, wavyLine_end);
    }
    // 波浪线和波浪线
    else if(str_1 == "wavyLine" && str_2 == "wavyLine")
    {
        find_wavyLineNode(_vec1);
        QVector2D wavyLine_first_1 = wavyLine_first, wavyLine_end_1 = wavyLine_end;
        find_wavyLineNode(_vec2);

        return jundge(wavyLine_first_1, wavyLine_end_1, wavyLine_first, wavyLine_end);
    }
    else
        return false;
}

// 判断垂直
bool Identification_relation::verticality(QString str_1, QString str_2, QVector<float> _vec1, QVector<float> _vec2)
{
    auto jundge = [](QVector2D first_1,QVector2D end_1,QVector2D first_2,QVector2D end_2,float &height_1){
        // 向量
        float end_1_first_1_x = end_1.x() - first_1.x();
        float end_1_first_1_y = end_1.y() - first_1.y();

        float first_1_end_1_x = first_1.x() - end_1.x();
        float first_1_end_1_y = first_1.y() - end_1.y();
        // 向量
        float first_2_first_1_x = first_2.x() - first_1.x();
        float first_2_first_1_y = first_2.y() - first_1.y();

        float end_2_first_1_x = end_2.x() - first_1.x();
        float end_2_first_1_y = end_2.y() - first_1.y();

        // 以 75度 为标准
        if((end_1_first_1_x * first_2_first_1_x + end_1_first_1_y * first_2_first_1_y) < 0.25882)
        {
            std::cout<<"the line is left!"<<std::endl;

            height_1 = end_2_first_1_y;
            //            height = first_2_first_1_y;
            std::cout<<"height_1: "<<height_1<<std::endl;

            return true;
        }
        else if((end_1_first_1_x * end_2_first_1_x + end_1_first_1_y * end_2_first_1_y) < 0.25882)
        {
            std::cout<<"the line is left!"<<std::endl;

            height_1 = first_2_first_1_y;
            //            height = end_2_first_1_y;
            std::cout<<"height_1: "<<height_1<<std::endl;

            return true;
        }
        else if((first_1_end_1_x * first_2_first_1_x + first_1_end_1_y * first_2_first_1_y) < 0.25882)
        {
            std::cout<<"the line is right!"<<std::endl;

            height_1 = end_2_first_1_y;
            //            height = first_2_first_1_y;
            std::cout<<"height_1: "<<height_1<<std::endl;

            return true;
        }
        else if((first_1_end_1_x * first_1_end_1_y + end_2_first_1_x * end_2_first_1_y) < 0.25882)
        {
            std::cout<<"the line is right!"<<std::endl;

            height_1 = first_2_first_1_y;
            //            height = end_2_first_1_y;
            std::cout<<"height: "<<height_1<<std::endl;

            return true;
        }
        else
            return  false;
    };

    if(str_1 == "cylinder" && str_2 == "straightLine")
    {
        find_cylinderNode(_vec1);
        find_straightLineNode(_vec2);

        return jundge(cylinder_left, cylinder_right, straightLine_first, straightLine_end, height_1);
    }
    else if(str_1 == "straightLine" && str_2 == "cylinder")
    {
        find_straightLineNode(_vec1);
        find_cylinderNode(_vec2);

        return jundge(cylinder_left, cylinder_right, straightLine_first, straightLine_end, height_1);
    }
    else
        return false;
}
