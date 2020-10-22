#include "gen_model.h"
#include "common.h"
#include "recognize_cup.h"
#include "recognize_desklamp.h"
#include "recognize_stool.h"

#include <iostream>
#include <QVector>
#include <QVector3D>
#include <QLineF>
#include <math.h>

// 产生随机数据
#include <cstdlib>
#include <ctime>
#define random(a,b) (rand()%(b-a)+a)

gen_Model::gen_Model():
    offset_cup(false),
    offset_deskLamp(false),
    offset_stool(false)
{
    common = Common::getInstance();
    recognizeCup = Recognize_cup::getInstance();
    recognizeDeskLamp = Recognize_deskLamp::getInstance();
    recognizeStool = Recognize_stool::getInstance();
}

gen_Model *gen_Model::getInstance(){
    static gen_Model _instance;
    return &_instance;
}

// 花生
void gen_Model::genPeanut(QVector<float> &vec, QVector<QVector2D> line_path, QVector3D offset)
{
    std::cout<<"====================花生====================="<<std::endl;

    int initSize = vec.size();

    std::cout<<"line_path.size: "<<line_path.size()<<std::endl;

    // 拉伸y
//    common->mapEllipseToCircle(line_path);

    // 保存最短路径 p1_p2 的数组
    QVector<QLineF> shotest_path_vector;
    // 保存中心线的数组
    QVector<QPointF> centerPoint_vector;

    QPointF p1, p2;
    QVector<QPointF> line_vec;

    // 取样
    for (int i = 0; i < line_path.size(); i++) {

        if(i == 0)
        {

            p1 = QPointF(line_path[0].x(), line_path[0].y());

            p2 = QPointF(line_path[line_path.size() - 1].x(), line_path[line_path.size() - 1].y());

            line_vec.push_back(p1);

            shotest_path_vector.push_back(QLineF(p1,p2));

            centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

            continue;
        }

        if(i % 5 == 0)
            line_vec.push_back(QVector2D(line_path[i]).toPointF());
    }

    QVector<QPointF>::iterator it_first = line_vec.begin();
    QVector<QPointF>::iterator it_end = line_vec.end();

    // 获取线段的中点
    do{
        auto first_next = it_first + 1;
        auto end_next = it_end - 1;
        float first_end_next = QVector2D(QPointF(it_first->x(),it_first->y()) - QPointF(end_next->x(),end_next->y())).length();
        float first_next_end = QVector2D(QPointF(first_next->x(),first_next->y()) - QPointF(it_end->x(),it_end->y())).length();

        if(first_end_next < first_next_end)
            it_end -= 1;
        else
            it_first += 1;

        p1 = QPointF(it_first->x(), it_first->y());
        p2 = QPointF(it_end->x(), it_end->y());

        shotest_path_vector.push_back(QLineF(p1,p2));

        centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

    }while(it_first != it_end);

    std::cout<<"shotest_path_vector.size: "<<shotest_path_vector.size()<<std::endl;

    std::cout<<"centerPoint_vector.size: "<<centerPoint_vector.size()<<std::endl;

    // 保存圆轨迹的数组
    QVector<QVector<QVector3D>> draw_circle_vector;

    QVector3D first_circle_center;

    // shotest_path_vector.size()
    for (int i = 0; i < shotest_path_vector.size() - 1; i++) {

        p1 = shotest_path_vector[i].p1();

        p2 = shotest_path_vector[i].p2();

        float radis = QVector2D(p2 - p1).length() / 2;

        QVector<QVector2D> circle_vector;

        QVector3D center(centerPoint_vector[i].x(), centerPoint_vector[i].y(), centerPoint_vector[i].y());

        if(i == 0)
            first_circle_center = QVector3D(centerPoint_vector[i].x(), 0, centerPoint_vector[i].y());

        QVector<QVector3D> temp_circle;

        // 构建圆的坐标
        float t = 0.0;
        for (int j = 0; j <= 360 ; j++) {

            // i 是弧度，需要转成角度 t
            t = j * 2 * M_PI / 360;

            // 改变圆心位置，只用给 x,y 加入固定的数值即可。
            circle_vector.push_back(QVector2D(radis * cos(t), radis * sin(t)));

            temp_circle.push_back(QVector3D(circle_vector[j].x(), circle_vector[j].y(), 0));
        }

        // 求旋转角度
        float cos_angle = (p2.x() - p1.x()) / sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));

        float sin_angle = (p2.y() - p1.y()) / sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));

        QVector<QVector3D> rotate_new_circle;

        // 逆时针旋转
        for (int j = 0; j < temp_circle.size() ; j++) {

            float x = cos_angle * temp_circle[j].x() +  centerPoint_vector[i].x();

            float y = temp_circle[j].y();

            float z = sin_angle * temp_circle[j].x() +  centerPoint_vector[i].y();

            rotate_new_circle.push_back(QVector3D(x, y, z));
        }

        draw_circle_vector.push_back(rotate_new_circle);
    }

    for (int i = 0; i < draw_circle_vector.size() - 1; i++)
    {
        int i_1 = (i + 1) % draw_circle_vector.size();

        for(int j = 0; j < draw_circle_vector[i].size(); j++)
        {
            int j_1 = (j + 1) % draw_circle_vector[i].size();

            QVector3D temp1 = QVector3D(draw_circle_vector[i][j].x(), draw_circle_vector[i][j].y(), draw_circle_vector[i][j].z());
            QVector3D temp3 = QVector3D(draw_circle_vector[i][j_1].x(), draw_circle_vector[i][j_1].y(), draw_circle_vector[i][j_1].z());

            // 首
            if(i == 1)
                common->genTriangle(vec, temp1, temp3, first_circle_center);

            QVector3D temp2 = QVector3D(draw_circle_vector[i_1][j].x(), draw_circle_vector[i_1][j].y(), draw_circle_vector[i_1][j].z());
            QVector3D temp4 = QVector3D(draw_circle_vector[i_1][j_1].x(), draw_circle_vector[i_1][j_1].y(), draw_circle_vector[i_1][j_1].z());

            common->genTriangle(vec,temp3,temp1,temp4);
            common->genTriangle(vec,temp1,temp2,temp4);

            // 尾
            if(i == draw_circle_vector[i].size() - 1)
                common->genTriangle(vec, temp2, temp4,QVector3D(centerPoint_vector[i_1].x(), centerPoint_vector[i_1].y(), draw_circle_vector[i_1][j_1].z()));  //bottom

        }
    }

    std::cout<<"first_circle_center.z(): "<<first_circle_center.z()<<std::endl;

    // cup 的偏移值
    if(offset_cup)
    {
        // 中心的 y 值
        std::cout<<"recognizeCup->cylinder_center.y: "<<recognizeCup->cylinder_center.y()<<std::endl;

        float peanut_offset_y = recognizeCup->cylinder_center.y();

        offset = QVector3D(0,peanut_offset_y, 0);
        offset_cup = false;

    }
    else if(offset_deskLamp)
    {
        // 中心的 y 值
        std::cout<<"cylinder_center.y: "<<recognizeDeskLamp->cylinder_center.y()<<std::endl;

        float peanut_offset_y = recognizeDeskLamp->cylinder_center.y();
        offset = QVector3D(0, peanut_offset_y, 0);
        offset_deskLamp = false;
    }
    // 凳子
    else if(offset_stool)
    {
        // 中心的 y 值
        std::cout<<"cylinder_center.y: "<<recognizeStool->cylinder_center.y()<<std::endl;

        float peanut_offset_y = recognizeStool->cylinder_center.y();
        offset = QVector3D(0 + 0.1, peanut_offset_y + offset.y(), -(first_circle_center.z() - centerBottom.z()));
//        offset = QVector3D(0, 0,0);
    }
    else
        offset = QVector3D(0, 0, 0);

    std::cout<<"offset: "<<offset.x()<< " "<< offset.y() << " " << offset.z()<<std::endl;

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}

// 琦角
void gen_Model::genMarch_Angle(QVector<float> &vec, QVector<QVector2D> head_path, QVector<QVector2D> line_path, QVector3D offset)
{
    std::cout<<"====================琦角====================="<<std::endl;

    QVector2D min,max;
    common->findMinMax(head_path, min,max);

    QVector2D center((max.x() + min.x())/2, (max.y() + min.y())/2);

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    // 椭圆的中心
    QVector3D centerTop(center.x(), center.y(), 0);

    // R0: 上椭圆的半径
    float proportion = center.x() - min.x();

    int initSize = vec.size();

    std::cout<<"head_path.size: "<<head_path.size()<<std::endl;

    std::cout<<"line_path.size: "<<line_path.size()<<std::endl;

    // 保存最短路径 p1_p2 的数组
    QVector<QLineF> shotest_path_vector;
    // 保存中心线的数组
    QVector<QPointF> centerPoint_vector;

    QPointF p1, p2;
    QVector<QPointF> line_vec;
    float temp_proportion;

    // 取样
    for (int i = 0; i < line_path.size(); i++) {

        if(i == 0)
        {

            p1 = QPointF(min.x(), center.y());

            p2 = QPointF(max.x(), center.y());

            line_vec.push_back(p1);

            shotest_path_vector.push_back(QLineF(p1,p2));

            centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

            continue;
        }

        if(i % 5 == 0)
            line_vec.push_back(QVector2D(line_path[i]).toPointF());

    }

    QVector<QPointF>::iterator it_first = line_vec.begin();
    QVector<QPointF>::iterator it_end = line_vec.end();

    // 获取线段的中点
    do{
        auto first_next = it_first + 1;
        auto end_next = it_end - 1;
        float first_end_next = QVector2D(QPointF(it_first->x(),it_first->y()) - QPointF(end_next->x(),end_next->y())).length();
        float first_next_end = QVector2D(QPointF(first_next->x(),first_next->y()) - QPointF(it_end->x(),it_end->y())).length();

        if(first_end_next < first_next_end)
            it_end -= 1;
        else
            it_first += 1;

        p1 = QPointF(it_first->x(), it_first->y());
        p2 = QPointF(it_end->x(), it_end->y());

        shotest_path_vector.push_back(QLineF(p1,p2));

        centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

    }while(it_first != it_end);

    // draw
    QVector<QVector3D> origin_circle;

    // 设定 Z 值
    float z = centerPoint_vector[0].y();

    for(int i = 0; i < shotest_path_vector.size() - 1; i++)
    {
        centerTop.setZ(z);

        std::cout<<"=========================================="<<std::endl;
        if(i == 0)
        {
            std::cout<<"centerPoint_vector[i].x: "<< centerPoint_vector[i].x() << " centerPoint_vector[i].y: "<< centerPoint_vector[i].y() <<std::endl;

            for(int j = 0; j < head_path.size(); j++)
            {
                int j_1 = (j + 1) % head_path.size();

                // 所有点坐标向 y 轴负半轴平移 centerTop.y() 个单位， 使椭圆的中心坐标的 y 值为 0
                QVector3D p0 = QVector3D(head_path[j].x(),head_path[j].y() - centerTop.y(),z);
                QVector3D p1 = QVector3D(head_path[j_1].x(),head_path[j_1].y() - centerTop.y(),z);

                std::cout<<"p0.x: "<<p0.x()<<" p0.y: "<<p0.y()<<" p0.z: "<<p0.z()<<std::endl;

                origin_circle.push_back(p0);
                common->genTriangle(vec,p0,p1,QVector3D(centerPoint_vector[i].x(),0,z)); //top
            }
            continue;
        }

        p1 = shotest_path_vector[i].p1();

        p2 = shotest_path_vector[i].p2();

        std::cout<<"p1: ("<<p1.x()<<","<<p1.y()<<")"<<std::endl;

        std::cout<<"p2: ("<<p2.x()<<","<<p2.y()<<")"<<std::endl;

        float radis = QVector2D(p2 - p1).length() / 2;

        std::cout<<"radis: "<<radis<<std::endl;

        QVector<QVector3D> new_circle;

        // 构建平行于 X 轴的椭圆轨迹
        for(int j = 0; j < head_path.size(); j++){

            temp_proportion =  radis / proportion;

            // 放缩比例后的椭圆坐标
            QVector3D p0((head_path[j].x() - centerTop.x()) * temp_proportion, (head_path[j].y() - centerTop.y()) * temp_proportion, 0);

            new_circle.push_back(p0);
        }

        std::cout<<"new_circle[0].x: "<<new_circle[0].x()<<" new_circle[0].y: "<<new_circle[0].y()<<" new_circle[0].z: "<<new_circle[0].z()<<std::endl;

        std::cout<<"new_circle.size: "<<new_circle.size()<<std::endl;

        // 求旋转角度
        float cos_angle = (p2.x() - p1.x()) / sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));

        float sin_angle = (p2.y() - p1.y()) / sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));

        std::cout << "cos_angle:" << cos_angle << "     " <<  sin_angle << std::endl;

        QVector<QVector3D> rotate_new_circle;

        // 逆时针旋转
        for (int j = 0; j < new_circle.size() ; j++) {

            float x = cos_angle * new_circle[j].x() +  centerPoint_vector[i].x();

            float y = new_circle[j].y();

            float z = sin_angle * new_circle[j].x() +  centerPoint_vector[i].y();

            rotate_new_circle.push_back(QVector3D(x, y, z));
        }

        std::cout<<"rotate_new_circle[0].x: "<<rotate_new_circle[0].x()<<" rotate_new_circle[0].y: "<<rotate_new_circle[0].y()<<" rotate_new_circle[0].z: "<<rotate_new_circle[0].z()<<std::endl;

        std::cout<<"rotate_new_circle.size: "<<rotate_new_circle.size()<<std::endl;

        // 拉竖条
        for (int i = 0; i < origin_circle.size(); i++) {
            int i_1 = (i + 1) % origin_circle.size();
            QVector3D temp1 = QVector3D(origin_circle[i].x(), origin_circle[i].y(), origin_circle[i].z());
            QVector3D temp3 = QVector3D(origin_circle[i_1].x(), origin_circle[i_1].y(), origin_circle[i_1].z());

            QVector3D temp2 = QVector3D(rotate_new_circle[i].x(), rotate_new_circle[i].y(), rotate_new_circle[i].z());
            QVector3D temp4 = QVector3D(rotate_new_circle[i_1].x(), rotate_new_circle[i_1].y(), rotate_new_circle[i_1].z());

            common->genTriangle(vec,temp3,temp1,temp4);
            common->genTriangle(vec,temp1,temp2,temp4);

        }

        origin_circle.clear();

        origin_circle = rotate_new_circle;

        // 未得到正确结果, 此处可能存在鲁棒性， 也可能需要再加一个 y 轴的偏移
        if(i == shotest_path_vector.size() - 2)
        {
            float new_minZ = origin_circle[0].z(), new_maxZ = new_minZ, center_z;

            float new_minX = origin_circle[0].x(), new_maxX = new_minX, center_x;

            float new_minY = origin_circle[0].y(), new_maxY = new_minY, center_y;

            for (int k = 0; k < origin_circle.size(); k++) {
                new_minZ = qMin(new_minZ, origin_circle[k].z());

                new_maxZ = qMin(new_maxZ, origin_circle[k].z());

                new_minX = qMin(new_minX, origin_circle[k].x());

                new_maxX = qMin(new_maxX, origin_circle[k].x());

                new_minY = qMin(new_minY, origin_circle[k].y());

                new_maxY = qMin(new_maxY, origin_circle[k].y());
            }

            center_x = (new_minX + new_maxX) / 2 ;

            center_y = (new_minY + new_maxY) / 2 ;

            center_z = (new_minZ + new_maxZ) / 2 ;

            std::cout<<"center_x: "<< center_x << " center_y: "<< center_y << " center_z: "<<center_z<<std::endl;

            std::cout<<"centerPoint_vector[i].x: "<< centerPoint_vector[i].x() << " centerPoint_vector[i].y: "<< centerPoint_vector[i].y() <<std::endl;

            for(int j = 0; j < origin_circle.size(); j++)
            {
                int j_1 = (j + 1) % origin_circle.size();
                QVector3D p0 = QVector3D(origin_circle[j].x(), origin_circle[j].y() + centerPoint_vector[i].y(), origin_circle[j].z());
                QVector3D p1 = QVector3D(origin_circle[j_1].x(), origin_circle[j_1].y() + centerPoint_vector[i].y(), origin_circle[j_1].z());

                common->genTriangle(vec,p0,p1,QVector3D(centerPoint_vector[i].x(), centerPoint_vector[i].y(), center_z));  //bottom
            }
        }

    }

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}

// 任意
void gen_Model::genArbitrary(QVector<float> &vec,QVector<QVector2D> head_path, QVector<QVector2D> line_path_1, QVector<QVector2D> line_path_2, float height_1, float height_2, QVector3D offset)
{
    std::cout<<"====================任意====================="<<std::endl;

    // 高度比 (左右线段的 Z 值偏移角度)
    float heightRatio = common->mapEllipseToCircle(head_path);

    std::cout<<"heightRatio: "<< heightRatio <<std::endl;

    QVector2D min,max;
    common->findMinMax(head_path, min,max);

    QVector2D center((max.x() + min.x())/2, (max.y() + min.y())/2);

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    std::cout<<"origin height_1: "<<abs(line_path_1[line_path_1.size()-1].y()-line_path_1[0].y())<<std::endl;
    std::cout<<"origin height_2: "<<abs(line_path_2[line_path_2.size()-1].y()-line_path_2[0].y())<<std::endl;

    int initSize = vec.size();

    // 缩放变换
    auto Scale = [](QVector<QVector2D> &line_vector,float Scale_Ratio){
        QVector<QVector2D> temp_vector;
        for (int i = 0; i < line_vector.size(); i++) {
            QVector2D temp = QVector2D(line_vector[i].x(), line_vector[i].y() * Scale_Ratio);
            temp_vector.push_back(temp);
        }
        line_vector = temp_vector;
    };

    // 平移变换
    auto  Translate = [](QVector<QVector2D> &line_vector,float Translate_Ratio){
        QVector<QVector2D> temp_vector;
        for (int i = 0; i < line_vector.size(); i++) {
            QVector2D temp = QVector2D(line_vector[i].x(), line_vector[i].y() + Translate_Ratio);
            temp_vector.push_back(temp);
        }
        line_vector = temp_vector;
    };

    float Scale_Ratio, Translate_Ratio;
    // 记录最终的高度值,后面计算划分比例需要用到
    float height;
    QString str;
    // 参照高的线段进行缩放变换
    if(height_1 > height_2)
    {
        // 缩放 line_2
        Scale_Ratio = height_1/height_2;
        Scale(line_path_2, Scale_Ratio);
        height = height_1;
    }
    else
    {
        // 缩放 line_1
        Scale_Ratio = height_2/height_1;
        Scale(line_path_1, Scale_Ratio);
        height = height_2;
        str = "line_1";
    }

    if(abs((line_path_2[line_path_2.size()-1].y()-line_path_2[0].y()) - (line_path_1[line_path_1.size()-1].y()-line_path_1[0].y())) < 0.01)
        std::cout<<"拉伸成功!"<<std::endl;
    else
        std::cout<<"拉伸失败!"<<std::endl;

    std::cout<<"new height_1: "<<abs(line_path_1[line_path_1.size()-1].y()-line_path_1[0].y())<<std::endl;
    std::cout<<"new height_2: "<<abs(line_path_2[line_path_2.size()-1].y()-line_path_2[0].y())<<std::endl;

    if(str == "line_1")
    {
        Translate_Ratio = line_path_2[0].y() - line_path_1[0].y();
        Translate(line_path_1, Translate_Ratio);
    }
    else
    {
        Translate_Ratio = line_path_1[0].y() - line_path_2[0].y();
        Translate(line_path_2, Translate_Ratio);
    }

    if(abs(line_path_1[0].y() - line_path_2[0].y()) < 0.001 && abs(line_path_1[line_path_1.size()-1].y() - line_path_2[line_path_2.size()-1].y()) < 0.01 )
        std::cout<<"平移成功!"<<std::endl;
    else
        std::cout<<"平移失败!"<<std::endl;

    std::cout<<"line_path_1[0].x: "<<line_path_1[0].x()<<std::endl;
    std::cout<<"line_path_1[0].y: "<<line_path_1[0].y()<<std::endl;
    std::cout<<"line_path_2[0].x: "<<line_path_2[0].x()<<std::endl;
    std::cout<<"line_path_2[0].y: "<<line_path_2[0].y()<<std::endl;

    std::cout<<"line_path_1[line_path_1.size()-1].x: "<<line_path_1[line_path_1.size()-1].x()<<std::endl;
    std::cout<<"line_path_1[line_path_1.size()-1].y: "<<line_path_1[line_path_1.size()-1].y()<<std::endl;
    std::cout<<"line_path_2[line_path_2.size()-1].x: "<<line_path_2[line_path_2.size()-1].x()<<std::endl;
    std::cout<<"line_path_2[line_path_2.size()-1].y: "<<line_path_2[line_path_2.size()-1].y()<<std::endl;

    int Divide_size = 100;

    // 等分
    float Divide_Ratio = height / Divide_size;

    std::cout<<"Translate_Ratio: "<<Translate_Ratio<<std::endl;
    std::cout<<"Divide_Ratio: "<<Divide_Ratio<<std::endl;

    if(abs(line_path_1.end()->y() - line_path_1[0].y() + ((Divide_size - 1) * Divide_Ratio)) < 0.01)
        std::cout<<"Divide success! "<<std::endl;
    else
        std::cout<<"Divide failed! "<<std::endl;

    QVector<QVector2D> line1_vec, line2_vec, center_vec;

    // 根据划分
    for(int i = 0; i < Divide_size; i++)
    {
        for(int j = 0; j < line_path_1.size(); j++)
        {
            if(abs(line_path_1[j].y() - (line_path_1[0].y() - Divide_Ratio * i)) < 0.01)
            {
                line1_vec.push_back(QVector2D(line_path_1[j].x(), line_path_1[j].y()));
                break;
            }
        }

        for(int j = 0; j < line_path_2.size(); j++)
        {
            if(abs(line_path_2[j].y() - (line_path_2[0].y() - Divide_Ratio * i)) < 0.01)
            {
                line2_vec.push_back(QVector2D(line_path_2[j].x(), line_path_2[j].y()));
                break;
            }
        }
    }

    // 取最小的划分尺寸
    int size = qMin(line1_vec.size(), line2_vec.size());

    for(int i = 0; i < size; i++)
    {
        QVector2D temp_center = QVector2D((line1_vec[i].x()+line2_vec[i].x())/2, line1_vec[i].y());
        center_vec.push_back(temp_center);
    }

    std::cout<<"center_vec.size: "<<center_vec.size()<<std::endl;

    int flag_1 = 0, flag_2 = 0;

    // R0: 上椭圆的半径
    float proportion = center.x() - min.x();

    QVector3D centerTop(center.x(), center.y(), center.y());

    QVector3D centerBottom;

    QVector<QVector2D> head_path_bottom;


    for(int i = 0; i < head_path.size(); i++){

        int i_1 = (i + 1)%head_path.size();

        QVector3D temp1,temp3,temp2,temp4;
        float  temp_proportion,temp_proportion_1;

        for(int j = 0; j < size - 1; j++)
        {
            // R1: 要缩放的椭圆半径
            // 缩放比例 R = R1/R0
            temp_proportion =  abs(center_vec[0].x() - line1_vec[j].x()) / proportion;
            temp_proportion_1 =  abs(center_vec[0].x() - line1_vec[j+1].x()) / proportion;

            //  y轴作一个平移操作, x和z轴保持不变.
            if(j == 0)
            {
                QVector3D p0((head_path[i].x() - center_vec[j].x()) * temp_proportion, (head_path[i].y() - center_vec[j].y()) * temp_proportion + center_vec[j].y(), centerTop.z());
                QVector3D p1((head_path[i_1].x() - center_vec[j].x()) * temp_proportion, (head_path[i_1].y() - center_vec[j].y()) * temp_proportion + center_vec[j].y(), centerTop.z());

                common->genTriangle(vec,p0,p1,centerTop); // top
            }

            float z = (j) * (height / size) * heightRatio + centerTop.z();
            temp1 = QVector3D((head_path[i].x() - center_vec[j].x()) * temp_proportion, (head_path[i].y() - center_vec[j].y()) * temp_proportion + center_vec[j].y(), z);
            temp3 = QVector3D((head_path[i_1].x() - center_vec[j].x()) * temp_proportion, (head_path[i_1].y() - center_vec[j].y()) * temp_proportion + center_vec[j].y(), z);

            z = (j+1) * (height / size) * heightRatio + centerTop.z();
            temp2 = QVector3D((head_path[i].x() - center_vec[j+1].x()) * temp_proportion_1, (head_path[i].y() - center_vec[j+1].y()) * temp_proportion_1 + center_vec[j+1].y(), z);
            temp4 = QVector3D((head_path[i_1].x() - center_vec[j+1].x()) * temp_proportion_1, (head_path[i_1].y() - center_vec[j+1].y()) * temp_proportion_1 + center_vec[j+1].y(), z);

            common->genTriangle(vec,temp3,temp1,temp4); //竖条
            common->genTriangle(vec,temp1,temp2,temp4);

            // 记录底的中心坐标
            if(j == size - 2)
            {
                for(int i = 0; i < head_path.size(); i++)
                {
                    QVector2D temp_vector((head_path[i].x() - center_vec[j+1].x()) * temp_proportion_1, (head_path[i].y() - center_vec[j+1].y()) * temp_proportion_1 + center_vec[j+1].y());
                    head_path_bottom.push_back(temp_vector);
                }

                common->findMinMax(head_path_bottom, min, max);

                QVector2D center_bottom((max.x() + min.x())/2, (max.y() + min.y())/2);
                centerBottom.setX(center_bottom.x());
                centerBottom.setY(center_bottom.y());
                centerBottom.setZ(height * heightRatio);
            }
        }

        // 补最后一段三角面片
        temp1.setZ((size-1) * (height / size) * heightRatio);
        temp3.setZ((size-1) * (height / size) * heightRatio);

        temp2.setZ((size) * (height / size) * heightRatio);
        temp4.setZ((size) * (height / size) * heightRatio);

        common->genTriangle(vec,temp3,temp1,temp4); //竖条
        common->genTriangle(vec,temp1,temp2,temp4);

        common->genTriangle(vec,temp4,temp2,centerBottom); //bottom

    }

    std::cout<<"flag_1: "<<flag_1<<std::endl;
    std::cout<<"flag_2: "<<flag_2<<std::endl;

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }

#if 0
    // 将两边波浪线的高度补成一样!
    QVector2D line1_first = line_path_1[0],line1_end = line_path_1[line_path_1.size()-1];
    QVector2D line2_first = line_path_2[0],line2_end = line_path_2[line_path_2.size()-1];

    // 随机数
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<float> dist_float();

    // 比例
    float Ratio_1 = height_1/line_path_1.size(),Ratio_2= height_2/line_path_2.size();
    // 高度差
    float  diff_first, diff_end;
    // 需要补的点个数 = 高度差 / 比例
    int num_first,num_end;
    if(line1_first.y() < line2_first.y())
    {
        diff_first = line2_first.y() - line1_first.y();
        num_first = diff_first/Ratio_1;
        // 补line1的头
    }
    else
    {
        diff_first = line1_first.y() - line2_first.y();
        num_first = diff_first/Ratio_2;
        // 补line2的头
    }

    if(line1_end.y() > line2_end.y())
    {
        diff_end = line1_end.y() - line2_end.y();
        num_end = diff_end / Ratio_1;
        // 补line1的尾
    }
    else
    {
        diff_end = line2_end.y() - line1_end.y();
        num_end = diff_end / Ratio_2;
        // 补line2的尾
    }
#endif
}

// 对称
void gen_Model::genSymmetric(QVector<float> &vec,QVector<QVector2D> head_path, QVector<QVector2D> line_path, float height,QVector3D offset){

    std::cout<<"=========================对称====================="<<std::endl;

    // 高度比 (左右线段的 Z 值偏移角度)
    float heightRatio = common->mapEllipseToCircle(head_path);

    std::cout<<"heightRatio: "<< heightRatio <<std::endl;

    // 中心
    QVector2D min,max;

    common->findMinMax(head_path, min,max);

    QVector2D center((max.x() + min.x())/2, (max.y() + min.y())/2);

    float proportion = center.x() - min.x();

    std::cout<<"proportion: "<<proportion<<std::endl;

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    QVector3D centerTop(center.x(), center.y(), center.y());

    QVector<QVector2D> head_path_bottom;

    int initSize = vec.size();

    for(int i = 0; i < head_path.size(); i++){

        int i_1 = (i + 1)%head_path.size();

        QVector3D temp1,temp3,temp2,temp4;
        float  temp_proportion,temp_proportion_1;

        for(int j = 0; j < line_path.size() - 1; j++){

            // 我们最理想的情况是： line_path[0].x() == minX, 当用户画的不符合时，这里会有 bug
            // 缩放比例
            temp_proportion =  (center.x() - line_path[j].x()) / proportion;
            temp_proportion_1 =  (center.x() - line_path[j+1].x()) / proportion;

            float z = (j) * (height / line_path.size()) * heightRatio + centerTop.z();
            temp1 = QVector3D((head_path[i].x() - center.x()) * temp_proportion + center.x(), (head_path[i].y() - center.y()) * temp_proportion + center.y(), z);
            temp3 = QVector3D((head_path[i_1].x() - center.x()) * temp_proportion + center.x(), (head_path[i_1].y() - center.y()) * temp_proportion + center.y(), z);

            z = (j+1) * (height / line_path.size()) * heightRatio + centerTop.z();
            temp2 = QVector3D((head_path[i].x() - center.x()) * temp_proportion_1 + center.x(), (head_path[i].y() - center.y()) * temp_proportion_1 + center.y(), z);
            temp4 = QVector3D((head_path[i_1].x() - center.x()) * temp_proportion_1 + center.x(), (head_path[i_1].y() - center.y()) * temp_proportion_1 + center.y(), z);

            common->genTriangle(vec,temp3,temp1,temp4); //竖条
            common->genTriangle(vec,temp1,temp2,temp4);

            if(j == 0)
            {
                QVector3D p0((head_path[i].x() - center.x()) * temp_proportion + center.x(), (head_path[i].y() - center.y()) * temp_proportion + center.y(),  + centerTop.z());
                QVector3D p1((head_path[i_1].x() - center.x()) * temp_proportion + center.x(), (head_path[i_1].y() - center.y()) * temp_proportion + center.y(),  + centerTop.z());

                common->genTriangle(vec,p0,p1,centerTop); // top
            }

            // 记录底的中心坐标
            if(j == line_path.size()-2)
            {
                for(int i = 0; i < head_path.size(); i++)
                {
                    QVector2D temp_vector((head_path[i].x() - center.x()) * temp_proportion_1 + center.x(), (head_path[i].y() - center.y()) * temp_proportion_1 + center.y());
                    head_path_bottom.push_back(temp_vector);
                }

                common->findMinMax(head_path_bottom, min, max);

                QVector2D center_bottom((max.x() + min.x())/2, (max.y() + min.y())/2);
                centerBottom.setX(center_bottom.x());
                centerBottom.setY(center_bottom.y());
                centerBottom.setZ(height * heightRatio + centerTop.z());
            }
        }

        // 补最后一段三角面片
        temp1.setZ((line_path.size()-1) * (height / line_path.size()) * heightRatio   + centerTop.z());
        temp3.setZ((line_path.size()-1) * (height / line_path.size()) * heightRatio  + centerTop.z());

        temp2.setZ((line_path.size()) * (height / line_path.size()) * heightRatio  + centerTop.z());
        temp4.setZ((line_path.size()) * (height / line_path.size()) * heightRatio  + centerTop.z());

        common->genTriangle(vec,temp3,temp1,temp4); //竖条
        common->genTriangle(vec,temp1,temp2,temp4);

        common->genTriangle(vec,temp4,temp2,centerBottom); //bottom

    }

    std::cout<<"centerBottom.z: "<< centerBottom.z() <<std::endl;

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }

}

// 圆柱体
void gen_Model::genCylinder(QVector<float> &vec,QVector2D cylinder_center,float r,float z,QVector3D offset){

    int n = 100;
    QVector<QVector2D> path;
    for(int i = 0; i < n; i++){
        path.append(QVector2D(r*cos(i*2*M_PI/n) + cylinder_center.x(),r*sin(i*2*M_PI/n) + cylinder_center.y()));
    }
    genIncline_Cylinder(vec,path,z,offset);

}

// 倾斜圆柱体
void gen_Model::genIncline_Cylinder(QVector<float> &vec,QVector<QVector2D> head_path,float z,QVector3D offset){

    std::cout<<"===================圆柱体=================="<<std::endl;

    // 高度比 (左右线段的 Z 值偏移角度)
    float heightRatio = common->mapEllipseToCircle(head_path);

    std::cout<<"heightRatio: "<< heightRatio <<std::endl;

    QVector2D min,max;

    common->findMinMax(head_path,min,max);

    std::cout<<"maxX: "<<max.x()<<" minX:"<<min.x()<<" maxY: "<<max.y()<<" minY: "<<min.y()<<std::endl;

    QVector2D center = QVector2D(min + max) /2;

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    QVector3D centerTop(center.x(), center.y(), center.y());

    // 由于和花生组合时会存在偏差，所以我们给它记录下来。
    centerBottom = QVector3D(center.x(), center.y(), z + center.y());

    std::cout<<"centerBottom.z: "<< centerBottom.z() <<std::endl;

    int initSize = vec.size();

    for(int i = 0; i < head_path.size(); i++){
        int i_1 = (i + 1)%head_path.size();

        QVector3D p0(head_path[i].x(),head_path[i].y(),center.y());
        QVector3D p1(head_path[i_1].x(),head_path[i_1].y(),center.y());

        common->genTriangle(vec,p0,p1,centerTop); //top

        p1.setZ(z + center.y());
        common->genRectangleZ(vec,p0,p1);       //竖条

        p0.setZ(z + center.y());
        common->genTriangle(vec,p1,p0,centerBottom); //bottom

    }

    // 偏移错误
    if(recognizeDeskLamp->offset_center != 0)
        offset = QVector3D(0,0,recognizeDeskLamp->offset_center);

    std::cout<<"offset: "<<offset.x()<< " "<< offset.y() << " " << offset.z()<<std::endl;

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}

void gen_Model::genLine(QVector<float> &vec, QVector<QVector2D> line_path, float width_var, QVector3D offset)
{
    std::cout<<"===============line==========="<<std::endl;

    int initSize = vec.size();

    std::cout<<"line_path.size: "<<line_path.size()<<std::endl;

    // 保存最短路径 p1_p2 的数组
    QVector<QLineF> shotest_path_vector;
    // 保存中心线的数组
    QVector<QPointF> centerPoint_vector;

    QPointF p1, p2;
    QVector<QPointF> line_vec;

    // 取样
    for(int i = 0; i < line_path.size(); i++)
    {
        QPointF p1 = QPointF(line_path[i].x(), line_path[i].y());
        QPointF p2 = QPointF(line_path[i+1].x(), line_path[i+1].y());

        // 以 p2 为中心 width 为半径画圆,求p3, p4;
        QVector<QPointF> path;
        for(int j = 0; j < 360; j++){
            path.append(QPointF(width_var * cos(j*2*M_PI/360) + p2.x(),width_var * sin(j*2*M_PI/360) + p2.y()));
        }

        QPointF p3(0,0), p4(0,0);

        for(auto point: path)
        {
            // 夹角
            QVector2D v1 = QVector2D(p1 - p2);
            QVector2D v2 = QVector2D(point - p2);
            float cos_a_b =(v1.x()*v2.x() + v1.y()*v2.y())/(v1.length()*v2.length());

            if(qAbs(cos_a_b) < 0.0001)
            {
                if(p3 == QPointF(0,0))
                    p3 = QPointF(point.x(), point.y());
                else
                    p4 = QPointF(point.x(), point.y());
            }
        }

        std::cout<<"======================================"<<std::endl;
        std::cout<<"p3: "<< p3.x() << " "<< p3.y() <<std::endl;
        std::cout<<"p4: "<< p4.x() << " "<< p4.y() <<std::endl;

        if(p3 != QPointF(0,0) && p4 != QPointF(0,0))
        {
            shotest_path_vector.push_back(QLineF(p3, p4));

            centerPoint_vector.push_back(p2);
        }

    }
    std::cout<<"centerPoint_vector[0]: "<<centerPoint_vector[0].x() << " "<<centerPoint_vector[0].y()<<std::endl;

    std::cout<<"centerPoint_vector[centerPoint_vector.size - 3]: "<<centerPoint_vector[centerPoint_vector.size() - 3].x() << " "<<centerPoint_vector[centerPoint_vector.size() - 3].y()<<std::endl;

    std::cout<<"centerPoint_vector[centerPoint_vector.size - 2]: "<<centerPoint_vector[centerPoint_vector.size() - 2].x() << " "<<centerPoint_vector[centerPoint_vector.size() - 2].y()<<std::endl;

    std::cout<<"centerPoint_vector[centerPoint_vector.size - 1]: "<<centerPoint_vector[centerPoint_vector.size() - 1].x() << " "<<centerPoint_vector[centerPoint_vector.size() - 1].y()<<std::endl;

    std::cout<<"shotest_path_vector.size: "<<shotest_path_vector.size()<<std::endl;

    std::cout<<"centerPoint_vector.size: "<<centerPoint_vector.size()<<std::endl;

    // 保存圆轨迹的数组
    QVector<QVector<QVector3D>> draw_circle_vector;

    QVector3D first_circle_center;

    // shotest_path_vector.size()
    for (int i = 0; i < shotest_path_vector.size(); i++) {

        p1 = shotest_path_vector[i].p1();

        p2 = shotest_path_vector[i].p2();

        float radis = QVector2D(p2 - p1).length() / 2;

        QVector<QVector2D> circle_vector;

        QVector3D center(centerPoint_vector[i].x(), centerPoint_vector[i].y(), centerPoint_vector[i].y());

        if(i == 0)
            first_circle_center = QVector3D(centerPoint_vector[i].x(), 0, centerPoint_vector[i].y());

        QVector<QVector3D> temp_circle;

        // 构建圆的坐标
        float t = 0.0;
        for (int j = 0; j <= 360 ; j++) {

            // i 是弧度，需要转成角度 t
            t = j * 2 * M_PI / 360;

            // 改变圆心位置，只用给 x,y 加入固定的数值即可。
            circle_vector.push_back(QVector2D(radis * cos(t), radis * sin(t)));

            temp_circle.push_back(QVector3D(circle_vector[j].x(), circle_vector[j].y(), 0));
        }

        // 求旋转角度
        float cos_angle = (p2.x() - p1.x()) / sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));

        float sin_angle = (p2.y() - p1.y()) / sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));

        QVector<QVector3D> rotate_new_circle;

        // 逆时针旋转
        for (int j = 0; j < temp_circle.size() ; j++) {

            float x = cos_angle * temp_circle[j].x() +  centerPoint_vector[i].x();

            float y = temp_circle[j].y();

            float z = sin_angle * temp_circle[j].x() +  centerPoint_vector[i].y();

            rotate_new_circle.push_back(QVector3D(x, y, z));
        }

        draw_circle_vector.push_back(rotate_new_circle);
    }

    std::cout<<"draw_circle_vector.size: "<<draw_circle_vector.size()<<std::endl;

    for (int i = 0; i < draw_circle_vector.size() - 1; i++)
    {
        int i_1 = (i + 1) % draw_circle_vector.size();

        for(int j = 0; j < draw_circle_vector[i].size(); j++)
        {
            int j_1 = (j + 1) % draw_circle_vector[i].size();

            QVector3D temp1 = QVector3D(draw_circle_vector[i][j].x(), draw_circle_vector[i][j].y(), draw_circle_vector[i][j].z());
            QVector3D temp3 = QVector3D(draw_circle_vector[i][j_1].x(), draw_circle_vector[i][j_1].y(), draw_circle_vector[i][j_1].z());

            // 首
            if(i == 1)
                common->genTriangle(vec, temp1, temp3, first_circle_center);

            QVector3D temp2 = QVector3D(draw_circle_vector[i_1][j].x(), draw_circle_vector[i_1][j].y(), draw_circle_vector[i_1][j].z());
            QVector3D temp4 = QVector3D(draw_circle_vector[i_1][j_1].x(), draw_circle_vector[i_1][j_1].y(), draw_circle_vector[i_1][j_1].z());

            common->genTriangle(vec,temp3,temp1,temp4);
            common->genTriangle(vec,temp1,temp2,temp4);

            // 尾
            if(i == draw_circle_vector.size() - 2)
            {
                QVector3D center_bottom = QVector3D(centerPoint_vector[i_1].x(), 0, draw_circle_vector[i_1][j_1].z());

                common->genTriangle(vec, temp4, temp2,center_bottom);  //bottom
            }
        }
    }

    std::cout<<"offset: "<<offset.x()<< " "<< offset.y() << " " << offset.z()<<std::endl;

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}

void gen_Model::genCircle(QVector<float> &vec, QVector<QVector2D> line_path, QVector3D offset)
{
    std::cout<<"==========================circle===================="<<std::endl;

    int initSize = vec.size();

    std::cout<<"line_path.size: "<<line_path.size()<<std::endl;

    // 求圆心坐标以及半径
    QVector2D min,max;
    common->findMinMax(line_path, min,max);

    QVector2D center((max.x() + min.x())/2, (max.y() + min.y())/2);

    float radis = qMax((max.x() - min.x())/2, (max.y() - min.y())/2);

    std::cout<<"center.x: "<<center.x()<<" center.y: "<<center.y()<<std::endl;

    std::cout<<"radis: "<<radis<<std::endl;

    QVector<QVector2D> circle_vector;

    // 构建圆的坐标
    float t = 0.0;
    for (int j = 0; j <= 1000 ; j++) {

        // i 是弧度，需要转成角度 t
        t = j * 2 * M_PI / 1000;

        // 改变圆心位置，只用给 x,y 加入固定的数值即可。
        circle_vector.push_back(QVector2D(radis * cos(t) + center.x(), radis * sin(t) + center.y()));
    }

    QVector2D min_point, max_point;

    common->findMinMax(circle_vector, min_point, max_point);

    std::cout<<"min_point.y: "<<min_point.y()<<std::endl;

    std::cout<<"max_point.y: "<<max_point.y()<<std::endl;

    float part = (max_point.y() - min_point.y()) / 50;

    std::cout<<"part: "<<part<<std::endl;

    QVector<QVector2D> dividecircle_vector;

    for(int i = 0; i <= 50; i++)
    {
        float y = min_point.y() + part * i;

        QVector<QVector2D> temp_vec;

        // 根据y值， 划分;
        for(auto point : circle_vector)
        {

            if(abs(point.y() - y) < 0.001)
            {
                if(temp_vec.size() == 0)
                {
                    temp_vec.push_back(point);
                }
                else
                {
                    if(temp_vec[0].y() == point.y())
                        temp_vec.push_back(point);
                }
            }

            if(temp_vec.size() == 2)
            {
                for(auto point : temp_vec)
                    dividecircle_vector.push_back(point);

                temp_vec.clear();
            }
        }
    }

    std::cout<<"dividecircle_vector.size: "<< dividecircle_vector.size() <<std::endl;

    // 划分圆的中心
    QVector<QVector2D> dividecircle_center;

    // 划分圆的半径
     QVector<float>  dividecircle_radis;

    for(int i = 0; i < dividecircle_vector.size() - 1; i+=2)
    {
        QVector2D left = dividecircle_vector[i];
        QVector2D right = dividecircle_vector[i+1];

        std::cout<<"==============="<<std::endl;

        std::cout<<"left: "<<left.x()<<" "<<left.y()<<std::endl;

        std::cout<<"right: "<<right.x()<<" "<<right.y()<<std::endl;

        QVector2D center = QVector2D(left + right)/2;

        float radis = abs(right.x() - left.x())/2;

        std::cout<<"center: "<<center.x()<<" "<<center.y()<<std::endl;

        std::cout<<"radis: "<<radis<<std::endl;

        dividecircle_center.push_back(center);

        dividecircle_radis.push_back(radis);
    }

    QVector<QVector<QVector2D>> all_dividecircle_circle;

    for(int i = 0; i < dividecircle_center.size(); i++)
    {

        QVector<QVector2D> dividecircle_circle;
        // 构建圆的坐标
        for (int j = 0; j <= 1000 ; j++) {

            // i 是弧度，需要转成角度 t
           float t = j * 2 * M_PI / 1000;

            // 改变圆心位置，只用给 x,y 加入固定的数值即可。
            dividecircle_circle.push_back(QVector2D(dividecircle_radis[i] * cos(t) + center.x(), dividecircle_radis[i] * sin(t) + center.y()));
        }

        all_dividecircle_circle.push_back(dividecircle_circle);
    }

    for(int i = 0; i < all_dividecircle_circle.size() - 1; i++)
    {
        int i_1 = (i + 1) % all_dividecircle_circle.size();

        // 缺一个 z 的偏移值
        for(int j = 0; j < all_dividecircle_circle[i].size(); j++)
        {
            int j_1 = (j + 1) % all_dividecircle_circle[i].size();

            QVector3D temp1 = QVector3D(all_dividecircle_circle[i][j].x(), all_dividecircle_circle[i][j].y(), dividecircle_center[i].y());
            QVector3D temp3 = QVector3D(all_dividecircle_circle[i][j_1].x(), all_dividecircle_circle[i][j_1].y(), dividecircle_center[i].y());

            if(i == 0)
                common->genTriangle(vec, temp1, temp3, QVector3D(center.x(),  center.y(), dividecircle_center[i].y()));

            QVector3D temp2 = QVector3D(all_dividecircle_circle[i_1][j].x(), all_dividecircle_circle[i_1][j].y(), dividecircle_center[i_1].y());
            QVector3D temp4 = QVector3D(all_dividecircle_circle[i_1][j_1].x(), all_dividecircle_circle[i_1][j_1].y(), dividecircle_center[i_1].y());

            common->genTriangle(vec,temp3,temp1,temp4);
            common->genTriangle(vec,temp1,temp2,temp4);

            if(i == all_dividecircle_circle.size() - 2)
                common->genTriangle(vec, temp4, temp2, QVector3D(center.x(), center.y(), dividecircle_center[i_1].y()));
        }
    }

    std::cout<<"offset: "<<offset.x()<< " "<< offset.y() << " " << offset.z()<<std::endl;

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }

}

void gen_Model::genExtrude(QVector<float> &vec, QVector<QVector2D> line_path, float width_var, float up_var, float down_var, QVector3D offset)
{
    std::cout<<"===============extrude==========="<<std::endl;

    int initSize = vec.size();

    std::cout<<"line_path.size: "<<line_path.size()<<std::endl;

    QVector2D min, max;

    common->findMinMax(line_path, min, max);

    QVector2D extrude_center = QVector2D((min + max) / 2);

    std::cout<< "extrude_center: "<< extrude_center.x() << " " <<extrude_center.y() <<std::endl;

    for(int i = 0; i < line_path.size(); i++)
    {
        int i_1 = (i + 1) % line_path.size();

        QVector3D temp1 = QVector3D(line_path[i].x(), line_path[i].y(), extrude_center.y());
        QVector3D temp3 = QVector3D(line_path[i_1].x(), line_path[i_1].y(), extrude_center.y());

        // up
        // extrude_center.y() - 0.1: 凸
        // extrude_center.y() + 0.1: 凹
        common->genTriangle(vec, temp1, temp3, QVector3D(extrude_center.x(),  extrude_center.y(), extrude_center.y() + up_var));

        QVector3D temp2 = QVector3D(line_path[i].x(), line_path[i].y(), extrude_center.y() + width_var);
        QVector3D temp4 = QVector3D(line_path[i_1].x(), line_path[i_1].y(), extrude_center.y() + width_var);

        common->genTriangle(vec,temp3,temp1,temp4);
        common->genTriangle(vec,temp1,temp2,temp4);


//        std::cout<<"===================================="<<std::endl;
//        std::cout<<"temp1: "<<temp1.x()<<" "<<temp1.y()<<" "<<temp1.z()<<std::endl;
//        std::cout<<"temp2: "<<temp2.x()<<" "<<temp2.y()<<" "<<temp2.z()<<std::endl;

        // down
        // extrude_center.y() + width_var - 0.1: 凹
        // extrude_center.y() + width_var + 0.1: 凸
        common->genTriangle(vec, temp4, temp2, QVector3D(extrude_center.x(),  extrude_center.y(), extrude_center.y() + width_var + down_var));
    }

    std::cout<<"offset: "<<offset.x()<< " "<< offset.y() << " " << offset.z()<<std::endl;

    for(int i = initSize; i < vec.size(); i += 6){
        vec[i] += offset.x();
        vec[i+1] += offset.y();
        vec[i+2] += offset.z();
    }
}
