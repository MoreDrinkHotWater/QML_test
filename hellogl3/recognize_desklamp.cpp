#include "recognize_desklamp.h"
#include "common.h"
#include "recognizecylinder.h"
#include "recognizecorner.h"
#include "identification_type.h"

#include <iostream>
#include <QMessageBox>

Recognize_deskLamp::Recognize_deskLamp():
    offset_center(0)
{
    common = new Common();
    recognizecylinder = new Recognizecylinder();
    recognizecorner = new Recognizecorner();
    identificationtypes = new Identification_type();
}

Recognize_deskLamp *Recognize_deskLamp::getInstance(){
    static Recognize_deskLamp _instance;
    return &_instance;
}

bool Recognize_deskLamp::recognize_deskLamp(QStack<QVector<float>> draw_stack)
{
    std::cout<<"=========================start========================"<<std::endl;

    // 转换坐标
    this->draw_coorstack = common->coordinate_transformation(draw_stack);

    // 灯罩
    QStack<QVector<float>> deskLamp_top;
    // 柱子
    QVector<float> corner;
    // 底座
    QStack<QVector<float>> deskLamp_bottom;

    for (int i = 0; i < draw_coorstack.size(); i++) {

        if(i == 4)
            corner = draw_coorstack[i];
        else if( i < 4)
        {
            QVector<float> transfromation_y_vec;

            for(int j = 0; j < draw_coorstack[i].size(); j+=2)
            {
                transfromation_y_vec.push_back(draw_coorstack[i][j]);
                transfromation_y_vec.push_back(-draw_coorstack[i][j+1]);
            }

            deskLamp_top.push_back(transfromation_y_vec);
        }

        else
            deskLamp_bottom.push_back(draw_coorstack[i]);
    }

    if(recognizecylinder->recognize_cylinder_shape(deskLamp_top))
    {
        this->radius = recognizecylinder->radius;
        this->height_1 = recognizecylinder->height_1;
        this->height_2 = recognizecylinder->height_2;

        std::cout<<"radius: "<<radius<<std::endl;
        std::cout<<"height_1: "<<height_1<<std::endl;
        std::cout<<"height_2: "<<height_2<<std::endl;

        int cylinder_index = 0, line_index = 0;

        for(int i = 0; i < recognizecylinder->type_vec.size(); i++)
        {
            if(recognizecylinder->type_vec[i] == "椭圆")
            {
                cylinder_index = i;

                for (int var = 0; var < draw_coorstack[cylinder_index].size() - 1; var+=2) {

                    QVector2D temp(draw_coorstack[cylinder_index][var],draw_coorstack[cylinder_index][var+1]);

                    head_vector_top.push_back(temp);
                }

                // 记录椭圆的中心坐标
                QVector2D min,max;

                common->findMinMax(head_vector_top, min,max);

                cylinder_center = QVector2D(min + max)/2;
            }
            // 没有区分左右
            else if(recognizecylinder->type_vec[i] == "波浪线")
            {
                line_index = i;
                // 波浪线 1
                if(wavyline_vector_1.isEmpty())
                {
                    for (int var = 0; var < draw_coorstack[line_index].size(); var+=2) {

                        QVector2D temp(draw_coorstack[line_index][var],draw_coorstack[line_index][var+1]);

                        wavyline_vector_1.push_back(temp);
                    }
                }
                // 波浪线 2
                else
                {
                    for (int var = 0; var < draw_coorstack[line_index].size(); var+=2) {

                        QVector2D temp(draw_coorstack[line_index][var],draw_coorstack[line_index][var+1]);

                        wavyline_vector_2.push_back(temp);
                    }
                }
            }
        }

        // 计算多边形面积 -> 确定是顺时针还是逆时针。
        // 顺时针面积>0 逆时针面积<0
        // 统一为逆时针方向
        if(common->calculateArea(head_vector_top) > 0)
        {
            QVector<QVector2D> temp_vec;
            for(int var = head_vector_top.size()-1; var >=0 ; var--)
            {
                temp_vec.push_back(head_vector_top[var]);
            }
            head_vector_top = temp_vec;
        }

        // 由于只有这个函数会多次调用,所以我们写成 lambda 函数,后续有需要再拿出去
        // 统一竖直方向的顺序为: 从上到下
        auto  vertical_order = [] (QVector<QVector2D> &line_vector){
            if(line_vector.begin()->y() < line_vector.end()->y())
            {
                QVector<QVector2D> temp_vec;
                for(int var = line_vector.size()-1; var >=0 ; var--)
                {
                    temp_vec.push_back(line_vector[var]);
                }
                line_vector = temp_vec;
            }
        };

        if(!wavyline_vector_1.isEmpty())
            vertical_order(wavyline_vector_1);

    }
    else
    {
        QMessageBox::information(NULL, "Error", "识别椭圆立方体失败！");

        return false;
    }

    // step2: 识别柱子
    if(identificationtypes->recognize_corner(corner))
    {
        for(int i = 0; i < corner.size() - 1; i+=2)
        {
            peanutLine_vector.push_back(QVector2D(corner[i], corner[i+1]));
        }
    }

    // 识别底座
    if(recognizecylinder->recognize_cylinder_shape(deskLamp_bottom))
    {
        this->deskLamp_bottom_radius = recognizecylinder->radius;
        this->deskLamp_bottom_height = recognizecylinder->height_1;

        std::cout<<"deskLamp_bottom_radius: "<<deskLamp_bottom_radius<<std::endl;
        std::cout<<"deskLamp_bottom_height: "<<deskLamp_bottom_height<<std::endl;

        QVector<QVector2D> head_vector_bootom,line_vector_1, line_vector_2;

        for(int i = 0; i < recognizecylinder->type_vec.size(); i++)
        {
            if(recognizecylinder->type_vec[i] == "椭圆")
            {
                for (int var = 0; var < deskLamp_bottom[i].size() - 1; var+=2) {

                    QVector2D temp(deskLamp_bottom[i][var],deskLamp_bottom[i][var+1]);

                    head_vector_bootom.push_back(temp);
                }

                // 记录椭圆的中心坐标
                QVector2D min,max;

                common->findMinMax(head_vector_bootom, min,max);

                offset_center = min.y() - cylinder_center.y();
            }
        }

        // 计算多边形面积 -> 确定是顺时针还是逆时针。
        // 顺时针面积>0 逆时针面积<0
        // 统一为逆时针方向
        if(common->calculateArea(head_vector_bootom) > 0)
        {
            QVector<QVector2D> temp_vec;
            for(int var = head_vector_bootom.size()-1; var >=0 ; var--)
            {
                temp_vec.push_back(head_vector_bootom[var]);
            }
            head_vector_bootom = temp_vec;
        }

        // 由于只有这个函数会多次调用,所以我们写成 lambda 函数,后续有需要再拿出去
        // 统一竖直方向的顺序为: 从上到下
        auto  vertical_order = [] (QVector<QVector2D> &line_vector){
            if(line_vector.begin()->y() < line_vector.end()->y())
            {
                QVector<QVector2D> temp_vec;
                for(int var = line_vector.size()-1; var >=0 ; var--)
                {
                    temp_vec.push_back(line_vector[var]);
                }
                line_vector = temp_vec;
            }
        };

        if(!line_vector_1.isEmpty())
            vertical_order(line_vector_1);

        if(!line_vector_2.isEmpty())
            vertical_order(line_vector_2);
    }

    // 识别相对关系

    return true;
}
