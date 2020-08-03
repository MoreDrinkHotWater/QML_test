#include "recognizecylinder.h"

#include <iostream>
#include <QVector2D>
#include <QVector>

#include <math.h>

#include <random>
#include<algorithm>

Recognizecylinder::Recognizecylinder()
{

}

// 计算方差
float Recognizecylinder::variance (QVector<float> vector){

    double sum = std::accumulate(std::begin(vector) + 1, std::end(vector) - 1, 0.0);
    double mean =  sum / vector.size(); //均值

    double accum  = 0.0;
    std::for_each (std::begin(vector), std::end(vector), [&](const double d) {
        accum  += (d - mean)*(d - mean);
    });

    float stdev = sqrt(accum/(vector.size()-1)); //方差

    return stdev;
};

bool Recognizecylinder::recognize_cylinder_shape(QStack<QVector<float>> draw_coorstack)
{

//    QVector<QString> init = QVector<QString>{"0","1","2","3","类型"};
//    vec.push_back(init);

    // 组合问题
    for(int i = 0; i < draw_coorstack.size(); i++)
    {
        bool is_cylinder = recognize_cylinder(draw_coorstack[i]);
        bool is_curveLine = recognize_curveLine(draw_coorstack[i]);
        bool is_straightLine = recognize_straightLine(draw_coorstack[i]);

        QVector<QString> row_vec(draw_coorstack.size());

        if(i == draw_coorstack.size() - 1)
        {
            QVector<QString> temp_vec(draw_coorstack.size() + 1);

            if(is_cylinder)
            {

                temp_vec[temp_vec.size()-1].push_back("椭圆");
                vec.push_back(temp_vec);
            }
            else if(is_curveLine)
            {
                temp_vec[temp_vec.size()-1].push_back("曲线");
                vec.push_back(temp_vec);
            }
            else
            {
                temp_vec[temp_vec.size()-1].push_back("直线");
                vec.push_back(temp_vec);
            }
            continue;
        }

        for(int j = i+1 ; j < draw_coorstack.size(); j++)
        {
            // 椭圆
            if(is_cylinder)
            {
                std::cout<<"第"<<i<<"条线段是: 椭圆"<<std::endl;

                if(recognize_straightLine(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 直线"<<std::endl;
                    str_1 = "cylinder";
                    str_2 = "straightLine";
                    // 关系？ 相连？ 垂直？
                    if(join(str_1, str_2))
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the cylinder join with straightLine"<<std::endl;
                        // 垂直
                        if(verticality(str_1, str_2))
                        {
                            row_vec[j].clear();
                            row_vec[j].push_back("join and vertical");
                            std::cout<<"the cylinder vertical with straightLine"<<std::endl;
                        }
                        else
                        {
                            std::cout<<"the cylinder vertical without straightLine"<<std::endl;
                        }
                    }
                    else
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the cylinder join without straightLine"<<std::endl;
                    }
                }

                else
                {
                    if(recognize_curveLine(draw_coorstack[j]))
                        std::cout<<"第"<<j<<"条线段是: 曲线"<<std::endl;
                    // 关系？ 相离？
                    str_1 = "cylinder";
                    str_2 = "curveLine";
                    if(separation(str_1,str_2))
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the cylinder separate with curveLine"<<std::endl;
                    }
                    else
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the cylinder separate without curveLine"<<std::endl;
                    }
                }
                if(j == draw_coorstack.size()-1)
                {
                    row_vec.push_back("椭圆");
                    vec.push_back(row_vec);
                }
            }
            // 直线
            else if(is_straightLine)
            {
                std::cout<<"第"<<i<<"条线段是: 直线"<<std::endl;
                if(recognize_cylinder(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 椭圆"<<std::endl;
                    // 关系？
                    // 关系？ 相连？ 垂直？
                    str_1 = "cylinder";
                    str_2 = "straightLine";
                    if(join(str_1, str_2))
                    {
                        std::cout<<"the cylinder join with straightLine"<<std::endl;

                        row_vec[j].push_back("join");

                        // 垂直
                        if(verticality(str_1, str_2))
                        {
                            row_vec[j].clear();
                            row_vec[j].push_back("join and vertical");
                            std::cout<<"the cylinder vertical with straightLine"<<std::endl;
                        }
                        else
                        {
                            std::cout<<"the cylinder vertical without straightLine"<<std::endl;
                        }
                    }
                    else
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the cylinder join without straightLine"<<std::endl;
                    }
                }
                else if(recognize_straightLine(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 直线"<<std::endl;
                    str_1 = "straightLine";
                    str_2 = "straightLine";
                    if(parallel(draw_coorstack[i],draw_coorstack[j]))
                    {
                        std::cout<<"the line1 parallel line2"<<std::endl;

                        row_vec[j].push_back("parallel");

                        if(equilong(draw_coorstack[i],draw_coorstack[j]))
                        {
                            row_vec[j].clear();
                            row_vec[j].push_back("parallel and equal");
                            std::cout<<"the line1.length equal to line2.length"<<std::endl;
                        }
                        else
                        {
                            std::cout<<"the line1.length unequal to line2.length"<<std::endl;
                        }
                    }
                    else
                    {
                        row_vec[j].push_back("isnot parallel");
                        std::cout<<"the line1 不平行 line2"<<std::endl;
                    }

                }
                else
                {
                    if(recognize_curveLine(draw_coorstack[j]))
                        std::cout<<"第"<<j<<"条线段是: 曲线"<<std::endl;
                    str_1 = "straightLine";
                    str_2 = "curveLine";
                    // 关系？ 相连？
                    if(join(str_1, str_2))
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the straightLine join with curveLine"<<std::endl;
                    }
                    else
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the straightLine join without curveLine"<<std::endl;
                    }
                }

                if(j == draw_coorstack.size()-1)
                {
                    row_vec.push_back("直线");
                    vec.push_back(row_vec);
                }

            }
            else if(is_curveLine)
            {
                std::cout<<"第"<<i<<"条线段是: 曲线"<<std::endl;
                if(recognize_cylinder(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 椭圆"<<std::endl;
                    // 关系？
                    str_1 = "cylinder";
                    str_2 = "curveLine";
                    if(separation(str_1,str_2))
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the cylinder separate with curveLine"<<std::endl;
                    }
                    else
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the cylinder separate without curveLine"<<std::endl;
                    }
                }
                else
                {
                    if(recognize_straightLine(draw_coorstack[j]))
                        std::cout<<"第"<<j<<"条线段是: 直线"<<std::endl;
                    // 关系？
                    str_1 = "straightLine";
                    str_2 = "curveLine";
                    // 关系？ 相连？
                    if(join(str_1, str_2))
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the straightLine join with curveLine"<<std::endl;
                    }
                    else
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the straightLine join without curveLine"<<std::endl;
                    }
                }

                if(j == draw_coorstack.size()-1)
                {
                    row_vec.push_back("曲线");
                    vec.push_back(row_vec);
                }
            }
            else
            {
                std::cout<<"=============================="<<std::endl;
            }
        }
    }

    int flag = 0;

    for(int i = 0; i < vec.size(); i++)
    {
        std::cout<<"row_vec["<<i<<"] ";
        for(int j = 0; j < vec[i].size(); j++)
        {
            if(vec[i][j].isEmpty())
                std::cout<<"[     ]     ";
            else
            {
                std::cout<<vec[i][j].toStdString()<<"     ";

                if(vec[i][j] == "join and vertical")
                {
                    if((vec[i][vec[i].size()-1] == "椭圆" && vec[j][vec[i].size()-1] == "直线")
                            || (vec[i][vec[i].size()-1] == "直线" && vec[j][vec[i].size()-1] == "椭圆"))
                    {
                        flag+=1;
                    }
                }
                else if(vec[i][j] == "parallel and equal")
                {
                    if((vec[i][vec[i].size()-1] == "直线" && vec[j][vec[i].size()-1] == "直线")
                            || (vec[i][vec[i].size()-1] == "直线" && vec[j][vec[i].size()-1] == "直线"))
                    {
                        flag+=1;
                    }
                }
                else if(vec[i][j] == "join")
                {
                    if((vec[i][vec[i].size()-1] == "直线" && vec[j][vec[i].size()-1] == "曲线")
                            || (vec[i][vec[i].size()-1] == "曲线" && vec[j][vec[i].size()-1] == "直线"))
                    {
                        flag+=1;
                    }
                }
                else if(vec[i][j] == "separation")
                {
                    if((vec[i][vec[i].size()-1] == "椭圆" && vec[j][vec[i].size()-1] == "曲线")
                            || (vec[i][vec[i].size()-1] == "曲线" && vec[j][vec[i].size()-1] == "椭圆"))
                    {
                        flag+=1;
                    }
                }
            }

            if(j == vec[i].size() - 1)
            {
                // 换行
                std::cout<<""<<std::endl;
//                // 保存类型
                type_vec.push_back(vec[i][j]);
            }
        }
    }

//    for(auto type: type_vec)
//    {
//        std::cout<<"type: "<<type.toStdString()<<std::endl;
//    }

    std::cout<<"flag: "<<flag<<std::endl;

    if(flag == 6)
        return true;
    else
        return false;
}

bool Recognizecylinder::recognize_cylinder(QVector<float> vec)
{
    QVector<QVector2D> head_circle;

    for (int i = 0; i < vec.size(); i+=2) {
        QVector2D temp(vec[i],vec[i+1]);
        head_circle.push_back(temp);
    }

    //    std::cout<<"head_circle size: "<<head_circle.size()<<std::endl;

    float maxX = head_circle[0].x(), minX = head_circle[0].x(), maxY = head_circle[0].y(), minY = head_circle[0].y();

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

    //    std::cout<<"maxX: "<<maxX<<" minX:"<<minX<<" maxY: "<<maxY<<" minY: "<<minY<<std::endl;

    // 记录长，短轴的距离
    float circle_width = (maxX - minX) / 2;
    float circle_height = (maxY - minY) / 2;

    //    std::cout<<"circle_width: "<<circle_width<<" circle_height:"<<circle_height<<std::endl;

    // 1.判断椭圆！（若为椭圆，找出长、短半轴的长度。为了方便我们默认长半轴为半径）
    QVector<float> step_vector;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dist_int(1, head_circle.size()-1);

    int num = 50;
    // 取50组测试数据
    for (int i = 0; i < num; i++) {

        QVector<QVector2D> OriginPoints_vector;
        QVector<int> rand_vector;

        // 随机取四个点数据
        for (int i = 0; i < 4; i++) {
            int rand = dist_int(gen);
            rand_vector.push_back(rand);
        }

        // 对随机数据排序
        std::sort(rand_vector.begin(), rand_vector.end());

        for (int j = 0; j < 4; j++) {
            QVector2D temp;
            temp.setX(head_circle[rand_vector[j]].x());
            temp.setY(head_circle[rand_vector[j]].y());
            OriginPoints_vector.push_back(temp);
        }

        float step = 1.0;

        // 托勒密定理 判断 四点共圆 (四边形要是有序的！)
        auto left = [](QVector<QVector2D> OriginPoints_vector,float step, bool flag){
            float AB = 0,CD = 0,BC = 0,AD = 0,AC = 0,BD = 0;

            //            std::cout<<"step: "<<step<<std::endl;

            if(flag)
            {
                AB =  sqrt(pow(OriginPoints_vector[1].y() * step - OriginPoints_vector[0].y() * step, 2) + pow(OriginPoints_vector[1].x() - OriginPoints_vector[0].x(), 2));
                CD =  sqrt(pow(OriginPoints_vector[3].y() * step - OriginPoints_vector[2].y() * step,2) + pow(OriginPoints_vector[3].x() - OriginPoints_vector[2].x(),2));
                BC =  sqrt(pow(OriginPoints_vector[2].y() * step - OriginPoints_vector[1].y() * step,2) + pow(OriginPoints_vector[2].x() - OriginPoints_vector[1].x(),2));
                AD =  sqrt(pow(OriginPoints_vector[3].y() * step - OriginPoints_vector[0].y() * step,2) + pow(OriginPoints_vector[3].x() - OriginPoints_vector[0].x(),2));
                AC =  sqrt(pow(OriginPoints_vector[2].y() * step - OriginPoints_vector[0].y() * step,2) + pow(OriginPoints_vector[2].x() - OriginPoints_vector[0].x(),2));
                BD =  sqrt(pow(OriginPoints_vector[3].y() * step - OriginPoints_vector[1].y() * step,2) + pow(OriginPoints_vector[3].x() - OriginPoints_vector[1].x(),2));
            }
            else
            {
                AB =  sqrt(pow(OriginPoints_vector[1].y()  - OriginPoints_vector[0].y(),2) + pow(OriginPoints_vector[1].x() * step - OriginPoints_vector[0].x() * step,2));
                CD =  sqrt(pow(OriginPoints_vector[3].y()  - OriginPoints_vector[2].y(),2) + pow(OriginPoints_vector[3].x() * step - OriginPoints_vector[2].x() * step,2));
                BC =  sqrt(pow(OriginPoints_vector[2].y()  - OriginPoints_vector[1].y(),2) + pow(OriginPoints_vector[2].x() * step - OriginPoints_vector[1].x() * step,2));
                AD =  sqrt(pow(OriginPoints_vector[3].y()  - OriginPoints_vector[0].y(),2) + pow(OriginPoints_vector[3].x() * step - OriginPoints_vector[0].x() * step,2));
                AC =  sqrt(pow(OriginPoints_vector[2].y()  - OriginPoints_vector[0].y(),2) + pow(OriginPoints_vector[2].x() * step - OriginPoints_vector[0].x() * step,2));
                BD =  sqrt(pow(OriginPoints_vector[3].y()  - OriginPoints_vector[1].y(),2) + pow(OriginPoints_vector[3].x() * step - OriginPoints_vector[1].x() * step,2));
            }

            if(AB*CD + BC*AD == AC *BD)
                //            if(abs((AB*CD + BC*AD) - AC *BD) < 0.001)
            {
                //                                std::cout<<"find the step"<<std::endl;
                return false;
            }
            return true;
        };

        bool flag;

        // 拉伸 y 值
        if(circle_width>circle_height)
        {
            flag=true;
            do
            {
                if(step == 5.0)
                {
                    //                                        std::cout<<"can't find the step "<<std::endl;
                }

                step+=0.0001;

            }while(left(OriginPoints_vector, step, flag) && step <= 5);
        }
        // 拉伸 x 值
        else
        {
            flag = false;
            do
            {
                if(step == 5.0)
                {
                    //                                        std::cout<<"can't find the step "<<std::endl;
                }

                step+=0.0001;

            }while(left(OriginPoints_vector,step,flag) && step <= 5);
        }

        step_vector.push_back(step);

        //        std::cout<<"the final step = "<<step<<std::endl;

    }

    // 匹配标准？
    std::sort(step_vector.begin(), step_vector.end());

    //    for(int i = 0 ; i < step_vector.size(); i++)
    //    {

    //        std::cout<<"step_vector: "<< step_vector[i] <<std::endl;
    //    }

    //    std::cout<<"step_vector的方差: "<< variance(step_vector) <<std::endl;

    int flag = 0;

    for(int i = 0 ; i < step_vector.size(); i++)
    {
        if(abs(step_vector[i]-5.00007) < 0.001)
        {
            flag+=1;
        }
    }

    if(flag>30)
        return false;
    else
    {
        float length = sqrt(pow(head_circle[0].x() - head_circle[head_circle.size()-1].x(),2) + pow(head_circle[0].y() - head_circle[head_circle.size()-1].y(),2));
        // 是椭圆，而且相连
        if(length < 0.1)
        {
            // 默认长半轴为半径
            radius = (maxX-minX)/2;

            // 记录椭圆的左右结点
            cylinder_left = QVector2D(minX, (maxY + minY) / 2);
            cylinder_right = QVector2D(maxX, (maxY + minY) / 2);

            return true;
        }
        else
        {
            return false;
        }
    }
}

bool Recognizecylinder::recognize_straightLine(QVector<float> vec)
{
    // line
    QVector<QVector2D> line_vector;

    QVector<float> xcoor_vector,ycoor_vector;

    for (int var = 0; var < vec.size(); var+=2) {
        QVector2D temp(vec[var],vec[var+1]);
        xcoor_vector.push_back(vec[var]);
        ycoor_vector.push_back(vec[var+1]);
        line_vector.push_back(temp);
    }
    // 计算方差。
    if(variance(xcoor_vector) < 0.1)
    {

        // 记录直线首尾的结点
        straightLine_first = line_vector[0];
        straightLine_end = line_vector[line_vector.size()-1];

        return  true;
    }
    // 计算斜率的方差。
    else if(calculate_k(line_vector) < 0.2)
    {

        // 记录直线首尾的结点
        straightLine_first = line_vector[0];
        straightLine_end = line_vector[line_vector.size()-1];
        return  true;
    }
    else
    {
        //        std::cout<<"不是直线！"<<std::endl;
        return false;
    }
}

bool Recognizecylinder::recognize_curveLine(QVector<float> vec)
{
    QVector<QVector2D> head_circle;

    for (int i = 0; i < vec.size(); i+=2) {
        QVector2D temp(vec[i],vec[i+1]);
        head_circle.push_back(temp);
    }

    float maxX = head_circle[0].x(), minX = head_circle[0].x(), maxY = head_circle[0].y(), minY = head_circle[0].y();

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

    // 记录长，短轴的距离
    float circle_width = (maxX - minX) / 2;
    float circle_height = (maxY - minY) / 2;

    // 1.判断椭圆！（若为椭圆，找出长、短半轴的长度。为了方便我们默认长半轴为半径）
    QVector<float> step_vector;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dist_int(1, head_circle.size()-1);

    int num = 50;
    // 取50组测试数据
    for (int i = 0; i < num; i++) {

        QVector<QVector2D> OriginPoints_vector;
        QVector<int> rand_vector;

        // 随机取四个点数据
        for (int i = 0; i < 4; i++) {
            int rand = dist_int(gen);
            rand_vector.push_back(rand);
        }

        // 对随机数据排序
        std::sort(rand_vector.begin(), rand_vector.end());

        for (int j = 0; j < 4; j++) {
            QVector2D temp;
            temp.setX(head_circle[rand_vector[j]].x());
            temp.setY(head_circle[rand_vector[j]].y());
            OriginPoints_vector.push_back(temp);
        }

        float step = 1.0;

        // 托勒密定理 判断 四点共圆 (四边形要是有序的！)
        auto left = [](QVector<QVector2D> OriginPoints_vector,float step, bool flag){
            float AB = 0,CD = 0,BC = 0,AD = 0,AC = 0,BD = 0;

            //            std::cout<<"step: "<<step<<std::endl;

            if(flag)
            {
                AB =  sqrt(pow(OriginPoints_vector[1].y() * step - OriginPoints_vector[0].y() * step, 2) + pow(OriginPoints_vector[1].x() - OriginPoints_vector[0].x(), 2));
                CD =  sqrt(pow(OriginPoints_vector[3].y() * step - OriginPoints_vector[2].y() * step,2) + pow(OriginPoints_vector[3].x() - OriginPoints_vector[2].x(),2));
                BC =  sqrt(pow(OriginPoints_vector[2].y() * step - OriginPoints_vector[1].y() * step,2) + pow(OriginPoints_vector[2].x() - OriginPoints_vector[1].x(),2));
                AD =  sqrt(pow(OriginPoints_vector[3].y() * step - OriginPoints_vector[0].y() * step,2) + pow(OriginPoints_vector[3].x() - OriginPoints_vector[0].x(),2));
                AC =  sqrt(pow(OriginPoints_vector[2].y() * step - OriginPoints_vector[0].y() * step,2) + pow(OriginPoints_vector[2].x() - OriginPoints_vector[0].x(),2));
                BD =  sqrt(pow(OriginPoints_vector[3].y() * step - OriginPoints_vector[1].y() * step,2) + pow(OriginPoints_vector[3].x() - OriginPoints_vector[1].x(),2));
            }
            else
            {
                AB =  sqrt(pow(OriginPoints_vector[1].y()  - OriginPoints_vector[0].y(),2) + pow(OriginPoints_vector[1].x() * step - OriginPoints_vector[0].x() * step,2));
                CD =  sqrt(pow(OriginPoints_vector[3].y()  - OriginPoints_vector[2].y(),2) + pow(OriginPoints_vector[3].x() * step - OriginPoints_vector[2].x() * step,2));
                BC =  sqrt(pow(OriginPoints_vector[2].y()  - OriginPoints_vector[1].y(),2) + pow(OriginPoints_vector[2].x() * step - OriginPoints_vector[1].x() * step,2));
                AD =  sqrt(pow(OriginPoints_vector[3].y()  - OriginPoints_vector[0].y(),2) + pow(OriginPoints_vector[3].x() * step - OriginPoints_vector[0].x() * step,2));
                AC =  sqrt(pow(OriginPoints_vector[2].y()  - OriginPoints_vector[0].y(),2) + pow(OriginPoints_vector[2].x() * step - OriginPoints_vector[0].x() * step,2));
                BD =  sqrt(pow(OriginPoints_vector[3].y()  - OriginPoints_vector[1].y(),2) + pow(OriginPoints_vector[3].x() * step - OriginPoints_vector[1].x() * step,2));
            }


            if(AB*CD + BC*AD == AC *BD)
                //            if(abs((AB*CD + BC*AD) - AC *BD) < 0.001)
            {
                //                                std::cout<<"find the step"<<std::endl;
                return false;
            }

            return true;
        };

        bool flag;

        // 拉伸 y 值
        if(circle_width>circle_height)
        {
            flag=true;
            do
            {
                if(step == 5.0)
                {
                    //                                        std::cout<<"can't find the step "<<std::endl;
                }

                step+=0.0001;

            }while(left(OriginPoints_vector, step, flag) && step <= 5);
        }
        // 拉伸 x 值
        else
        {
            flag = false;
            do
            {
                if(step == 5.0)
                {
                    //                                        std::cout<<"can't find the step "<<std::endl;
                }

                step+=0.0001;

            }while(left(OriginPoints_vector,step,flag) && step <= 5);
        }

        step_vector.push_back(step);

        //        std::cout<<"the final step = "<<step<<std::endl;

    }

    // 匹配标准？
    std::sort(step_vector.begin(), step_vector.end());

    int flag = 0;

    for(int i = 0 ; i < step_vector.size(); i++)
    {
        if(abs(step_vector[i]-5.00007) < 0.001)
        {
            flag+=1;
        }
    }

    if(flag > 35)
        return false;
    else
    {
        float length = sqrt(pow(head_circle[0].x() - head_circle[head_circle.size()-1].x(),2) + pow(head_circle[0].y() - head_circle[head_circle.size()-1].y(),2));
        // 是椭圆，但是不相连
        if(length > 0.1)
        {
            // 记录直线首尾的结点
            curveLine_first = head_circle[0];
            curveLine_end = head_circle[head_circle.size()-1];

            return true;
        }
        else
        {
            return false;
        }

    }
}

float Recognizecylinder::calculate_k(QVector<QVector2D> vec)
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

float Recognizecylinder::calculate_meanK(QVector<QVector2D> vec, int &num)
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

// 判断平行
bool Recognizecylinder::parallel(QVector<float> _vec1, QVector<float> _vec2)
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

    // 比较斜率
    if(abs(calculate_meanK(vec1, num_1)-calculate_meanK(vec2, num_2)) < 0.3)
        return true;
    else if(num_1 < 80 && num_2 < 80)
        return true;
    else
        return false;

}

// 判断是否等长
bool Recognizecylinder::equilong(QVector<float> _vec1, QVector<float> _vec2)
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
bool Recognizecylinder::join(QString str_1, QString str_2)
{

    auto jundge = [](QVector2D first_1,QVector2D end_1,QVector2D first_2,QVector2D end_2){

        float first_first = sqrt(pow(first_1.x() - first_2.x(),2) + pow(first_1.y() - first_2.y(),2));
        float first_end = sqrt(pow(first_1.x() - end_2.x(),2) + pow(first_1.y() - end_2.y(),2));

        float end_first = sqrt(pow(end_1.x() - first_2.x(),2) + pow(end_1.y() - first_2.y(),2));
        float end_end = sqrt(pow(end_1.x() - end_2.x(),2) + pow(end_1.y() - end_2.y(),2));

        std::cout<<"cylinder_left: ("<<first_1.x()<<","<<first_1.y()<<") "<<std::endl;
        std::cout<<"cylinder_right: ("<<end_1.x()<<","<<end_1.y()<<") "<<std::endl;
        std::cout<<"straightLine_first: ("<<first_2.x()<<","<<first_2.y()<<") "<<std::endl;
        std::cout<<"straightLine_end: ("<<end_2.x()<<","<<end_2.y()<<") "<<std::endl;

        std::cout<<"first_first: "<<first_first<<std::endl;

        std::cout<<"end_first: "<<end_first<<std::endl;

        std::cout<<"first_end: "<<first_end<<std::endl;

        std::cout<<"end_end: "<<end_end<<std::endl;


        if(abs(first_first-0)<0.2 || abs(first_end-0)<0.2 || abs(end_first-0)<0.2 || abs(end_end-0)<0.2)
            return true;
        else
            return false;
    };

    if(str_1 == "cylinder" && str_2 == "straightLine")
    {
        // 1.椭圆的左边和直线的头相连，或者尾相连
        // 2.椭圆的右边和直线的头相连，或者尾相连
        // 一共四种情况
        return jundge(cylinder_left, cylinder_right, straightLine_first, straightLine_end);
    }
    else if(str_1 == "straightLine" && str_2 == "curveLine")
    {
        return jundge(straightLine_first, straightLine_end, curveLine_first, curveLine_end);
    }
    else
        return false;

}

// 判断相离
bool Recognizecylinder::separation(QString str_1, QString str_2)
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

    if(str_1 == "cylinder" && str_2 == "curveLine")
    {
        return jundge(cylinder_left, cylinder_right, curveLine_first, curveLine_end);
    }
    else
        return false;
}

// 判断垂直
bool Recognizecylinder::verticality(QString str_1, QString str_2)
{
    auto jundge = [](QVector2D first_1,QVector2D end_1,QVector2D first_2,QVector2D end_2,float &height){
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

            height = first_2_first_1_y;

            std::cout<<"height: "<<height<<std::endl;

            return true;
        }
        else if((end_1_first_1_x * end_2_first_1_x + end_1_first_1_y * end_2_first_1_y) < 0.25882)
        {
            std::cout<<"the line is left!"<<std::endl;

            height = end_2_first_1_y;

            std::cout<<"height: "<<height<<std::endl;

            return true;
        }
        else if((first_1_end_1_x * first_2_first_1_x + first_1_end_1_y * first_2_first_1_y) < 0.25882)
        {
            std::cout<<"the line is right!"<<std::endl;

            height = first_2_first_1_y;

            std::cout<<"height: "<<height<<std::endl;

            return true;
        }
        else if((first_1_end_1_x * first_1_end_1_y + end_2_first_1_x * end_2_first_1_y) < 0.25882)
        {
            std::cout<<"the line is right!"<<std::endl;

            height = end_2_first_1_y;

            std::cout<<"height: "<<height<<std::endl;

            return true;
        }
        else
            return  false;
    };

    if(str_1 == "cylinder" && str_2 == "straightLine")
    {
        return jundge(cylinder_left, cylinder_right, straightLine_first, straightLine_end,height);
    }
    else
        return false;
}

