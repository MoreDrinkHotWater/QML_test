#include "recognize_cup.h"
#include "common.h"
#include "recognizecylinder.h"
#include "recognizecorner.h"
#include "identification_type.h"

#include <QVector>
#include <QStack>
#include <iostream>
#include <QMessageBox>
#include <random>

// 产生随机数据
#include <cstdlib>
#include <ctime>
#define random(a,b) (rand()%(b-a)+a)

Recognize_cup::Recognize_cup()
{
    common = new Common();
    recognizecylinder = new Recognizecylinder();
    recognizecorner = new Recognizecorner();
    identificationtypes = new Identification_type();
}

Recognize_cup *Recognize_cup::getInstance(){
    static Recognize_cup _instance;
    return &_instance;
}

bool Recognize_cup::recognize_cup(QStack<QVector<float> > draw_stack)
{
    std::cout<<"=========================start========================"<<std::endl;

    // 转换坐标
    this->draw_coorstack = common->coordinate_transformation(draw_stack);

    QStack<QVector<float>> cylinder;

    QVector<float> peanut;

    for (int i = 0; i < draw_coorstack.size(); i++) {

        if(i == draw_coorstack.size() - 1)
            peanut = draw_coorstack[i];
        else
            cylinder.push_back(draw_coorstack[i]);
    }

    // step1: 识别圆柱体
    if(recognizecylinder->recognize_cylinder_shape(cylinder))
    {
        this->radius = recognizecylinder->radius;
        this->height_1 = recognizecylinder->height_1;

        std::cout<<"radius: "<<radius<<std::endl;
        std::cout<<"height_1: "<<height_1<<std::endl;

        // 记录椭圆的中心坐标
        for(int i = 0; i < recognizecylinder->type_vec.size(); i++)
        {
            if(recognizecylinder->type_vec[i] == "椭圆")
            {
                QVector<QVector2D> cylinder_vec;
                for (int j = 0; j < cylinder.size() - 1; j+=2)
                    cylinder_vec.push_back(QVector2D(cylinder[i][j],cylinder[i][j+1]));

                QVector2D min,max;

                common->findMinMax(cylinder_vec, min,max);

                cylinder_center = QVector2D(min + max)/2;
            }
        }
    }
    else
    {
        QMessageBox::information(NULL, "Error", "识别圆柱体失败！");

        return false;
    }

    // step2: 识别花生
    if(identificationtypes->recognize_peanut(peanut))
    {
        // 寻找 y 值最小的点, 如果有多个, 则取中间的一个。

        // 记录下标的数组
        QVector<int> min_coory_vector;

        int miny_sub = 0;

        float minY = peanut[1];

        // 找出最小 y 值坐标
        for (int var = 0; var < peanut.size(); var+=2) {

            QVector2D temp(peanut[var],peanut[var+1]);

            minY = qMin(minY, peanut[var+1]);

            peanutLine_vector.push_back(temp);
        }

        // 查找是否有多个
        for(int var = 0; var < peanut.size(); var+=2)
        {
            if(peanut[var + 1] == minY)
                min_coory_vector.push_back(var / 2);
        }

        std::cout<< "min_coory_vector.size: "<< min_coory_vector.size() <<std::endl;

        if(min_coory_vector.size() == 1)
        {
            miny_sub = min_coory_vector[0];
        }
        else
        {
            for(auto miny: min_coory_vector)
            {
                miny_sub += miny;
            }
            miny_sub /= min_coory_vector.size();
        }

        std::cout<<"miny_sub: "<< miny_sub <<std::endl;

        // 变换后的数组
        QVector<QVector2D> temp = peanutLine_vector;

        QVector<QVector2D> first_line, second_line;

        for (int i = 0; i < temp.size(); i++) {
            if(i < miny_sub)
                second_line.push_back(temp[i]);
            else
                first_line.push_back(temp[i]);
        }

        peanutLine_vector = first_line;

        for (auto var: second_line) {
            peanutLine_vector.push_back(var);
        }
    }
    else
    {
        QMessageBox::information(NULL, "Error", "识别花生失败！");
        return false;
    }

    // step3: 识别相对位置
    int intersection_num = 0;

    for(int i = 0; i < recognizecylinder->type_vec.size(); i++)
    {
        //        std::cout<< "type["<<i<<"]: "<<recognizecylinder->type_vec[i].toStdString()<<std::endl;

        if(recognizecylinder->type_vec[i] == "直线")
        {

            QVector<QVector2D> straightLine_vector;

            for(int j = 0; j < cylinder[i].size() - 1; j+=2)
                straightLine_vector.push_back(QVector2D(cylinder[i][j], cylinder[i][j+1]));

            QVector2D p1 = straightLine_vector[random(1,straightLine_vector.size())];
            QVector2D p2 = straightLine_vector[random(1,straightLine_vector.size())];

            // 求解直线方程:  0 = Ax + By + C
            float A = p2.y() - p1.y();
            float B = p1.x() - p2.x();
            float C = p2.x() * p1.y() - p1.x() * p2.y();

            // 判断交点个数以及输出交点坐标

            for (auto peanut_point: peanutLine_vector)
            {

                if(abs(A * peanut_point.x() + B  * peanut_point.y() + C) < 0.001)
                {
                    intersection_num += 1;
                    intersection_vector.push_back(peanut_point);
                }
            }

            std::cout<<"intersection_num: "<< intersection_num <<std::endl;

        }
    }

    // 交点小于两个
    if(intersection_num < 2)
    {
        QMessageBox::information(NULL, "Error", "相对位置有误！");
        return false;
    }

    std::cout<<"radius: "<<radius<<std::endl;

    std::cout<<"peanutLine_vector.size: "<<peanutLine_vector.size()<<std::endl;

    return true;

}
