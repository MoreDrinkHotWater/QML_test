#include "recognize_stool.h"
#include "common.h"
#include "recognizecylinder.h"
#include "recognizecorner.h"
#include "identification_type.h"

#include <iostream>
#include <QMessageBox>

Recognize_stool::Recognize_stool()
{
    common = new Common();
    recognizecylinder = new Recognizecylinder();
    recognizecorner = new Recognizecorner();
    identificationtypes = new Identification_type();
}

Recognize_stool *Recognize_stool::getInstance(){
    static Recognize_stool _instance;
    return &_instance;
}

bool Recognize_stool::recognize_stool(QStack<QVector<float>> draw_stack)
{
    std::cout<<"=========================start========================"<<std::endl;

    // 转换坐标
    this->draw_coorstack = common->coordinate_transformation(draw_stack);

    std::cout<<"this->draw_coorstack.size: "<<this->draw_coorstack.size()<<std::endl;

    // 灯罩
    QStack<QVector<float>> stool_top;

    // 底角
    QStack<QVector<float>> stool_bottom;

    for (int i = 0; i < draw_coorstack.size(); i++) {

        if( i < 4)
            stool_top.push_back(draw_coorstack[i]);
        else
            stool_bottom.push_back(draw_coorstack[i]);

    }

    // 识别凳子的桌面
    if(recognizecylinder->recognize_cylinder_shape(stool_top))
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
                for (int j = 0; j < stool_top.size() - 1; j+=2)
                    cylinder_vec.push_back(QVector2D(stool_top[i][j],stool_top[i][j+1]));

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

    // 识别凳子的角
    for(auto peanutLine: stool_bottom)
    {
        if(identificationtypes->recognize_corner(peanutLine))
        {
            QVector<QVector2D> temp_peanutLine;

            for(int i = 0; i < peanutLine.size() - 1; i+=2)
            {
                temp_peanutLine.push_back(QVector2D(peanutLine[i], peanutLine[i+1]));
            }

            stool_bottom_stack.push_back(temp_peanutLine);
        }
        else
            return false;
    }

    // 识别相对关系

    return true;
}
