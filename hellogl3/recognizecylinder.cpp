#include "recognizecylinder.h"
#include "identification_relation.h"
#include "identification_type.h"

#include <iostream>
#include <QVector2D>
#include <QVector>
#include <math.h>
#include <random>
#include<algorithm>

Recognizecylinder::Recognizecylinder()
{
    identification_relation = Identification_relation::getInstance();

    identification_type = Identification_type::getInstance();
}

bool Recognizecylinder::recognize_cylinder_shape(QStack<QVector<float>> draw_coorstack)
{
    // 清除
    vec.clear();
    type_vec.clear();

    // 组合问题
    for(int i = 0; i < draw_coorstack.size(); i++)
    {
        // 识别类型
        bool is_cylinder = identification_type->recognize_cylinder(draw_coorstack[i]);
        bool is_curveLine = identification_type->recognize_curveLine(draw_coorstack[i]);
        bool is_straightLine = identification_type->recognize_straightLine(draw_coorstack[i]);
        // 波浪线
        bool is_wavyLine = identification_type->recognize_wavyLine(draw_coorstack[i]);

        std::cout<<"is_cylinder: "<<is_cylinder<<std::endl;
        std::cout<<"is_straightLine: "<<is_straightLine<<std::endl;
        std::cout<<"is_wavyLine: "<<is_wavyLine<<std::endl;
        std::cout<<"is_curveLine: "<<is_curveLine<<std::endl;

        // 保存类型和关系的数组
        QVector<QString> row_vec(draw_coorstack.size());

        if(i == draw_coorstack.size() - 1)
        {

            QVector<QString> temp_vec(draw_coorstack.size() + 1);

            if(is_cylinder)
            {
                temp_vec[temp_vec.size()-1].push_back("椭圆");
                vec.push_back(temp_vec);
            }
            else if(is_straightLine)
            {
                temp_vec[temp_vec.size()-1].push_back("直线");
                vec.push_back(temp_vec);
            }
            else if(is_wavyLine)
            {
                temp_vec[temp_vec.size()-1].push_back("波浪线");
                vec.push_back(temp_vec);
            }
            else
            {
                temp_vec[temp_vec.size()-1].push_back("曲线");
                vec.push_back(temp_vec);
            }
            continue;
        }

        for(int j = i+1 ; j < draw_coorstack.size(); j++)
        {

            // =======================================================椭圆===============================================================================================================
            if(is_cylinder)
            {
                std::cout<<"第"<<i<<"条线段是: 椭圆"<<std::endl;

                if(identification_type->recognize_straightLine(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 直线"<<std::endl;
                    // 关系？ 相连？ 垂直？
                    str_1 = "cylinder";
                    str_2 = "straightLine";
                    if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the cylinder join with straightLine"<<std::endl;
                        // 垂直
                        if(identification_relation->verticality(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
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
                else if(identification_type->recognize_wavyLine(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 波浪线"<<std::endl;
                    // 关系？ 相连？
                    str_1 = "cylinder";
                    str_2 = "wavyLine";
                    if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the cylinder join with wavyLine"<<std::endl;
                    }
                    else
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the cylinder join without wavyLine"<<std::endl;
                    }
                }
                else
                {
                    if(identification_type->recognize_curveLine(draw_coorstack[j]))
                    {
                        std::cout<<"第"<<j<<"条线段是: 曲线"<<std::endl;
                        // 关系？ 相离？
                        str_1 = "cylinder";
                        str_2 = "curveLine";
                        if(identification_relation->separation(str_1,str_2,draw_coorstack[i], draw_coorstack[j]))
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
                }


                if(j == draw_coorstack.size()-1)
                {
                    row_vec.push_back("椭圆");
                    vec.push_back(row_vec);
                }
            }
            // ============================================================ 直线==========================================================================================================
            else if(is_straightLine)
            {
                std::cout<<"第"<<i<<"条线段是: 直线"<<std::endl;

                if(identification_type->recognize_cylinder(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 椭圆"<<std::endl;

                    // 关系？ 相连？ 垂直？
                    str_1 = "straightLine";
                    str_2 = "cylinder";
                    if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                    {
                        std::cout<<"the cylinder join with straightLine"<<std::endl;

                        row_vec[j].push_back("join");

                        // 垂直
                        if(identification_relation->verticality(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
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
                else if(identification_type->recognize_straightLine(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 直线"<<std::endl;
                    // 关系？ 相连？ 垂直？
                    str_1 = "straightLine";
                    str_2 = "straightLine";
                    if(identification_relation->parallel(draw_coorstack[i],draw_coorstack[j]))
                    {
                        std::cout<<"the line1 parallel line2"<<std::endl;

                        row_vec[j].push_back("parallel");

                        if(identification_relation->equilong(draw_coorstack[i],draw_coorstack[j]))
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
                else if(identification_type->recognize_wavyLine(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 波浪线"<<std::endl;

                    str_1 = "straightLine";
                    str_2 = "wavyLine";
                    // 关系？  相连？
                    if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the straightLine join with wavyLine"<<std::endl;
                    }
                    else
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the straightLine join without wavyLine"<<std::endl;
                    }
                }
                else
                {
                    if(identification_type->recognize_curveLine(draw_coorstack[j]))
                    {
                        std::cout<<"第"<<j<<"条线段是: 曲线"<<std::endl;
                        str_1 = "straightLine";
                        str_2 = "curveLine";
                        // 关系？ 相连？
                        if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                        {
                            row_vec[j].push_back("join");
                            std::cout<<"the straightLine join with curveLine"<<std::endl;
                        }
                        else
                        {
                            row_vec[j].push_back("separation");
                            std::cout<<"the straightLine separation with curveLine"<<std::endl;
                        }
                    }
                }

                if(j == draw_coorstack.size()-1)
                {
                    row_vec.push_back("直线");
                    vec.push_back(row_vec);
                }
            }

            // ==========================================================波浪线==================================================================================================
            else if(is_wavyLine)
            {
                std::cout<<"第"<<i<<"条线段是: 波浪线"<<std::endl;
                if(identification_type->recognize_cylinder(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 椭圆"<<std::endl;
                    // 关系？ 相连？
                    str_1 = "wavyLine";
                    str_2 = "cylinder";
                    if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the wavyLine join with cylinder"<<std::endl;
                    }
                    else
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the wavyLine join without cylinder"<<std::endl;
                    }
                }
                else if(identification_type->recognize_straightLine(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 直线"<<std::endl;
                    str_1 = "wavyLine";
                    str_2 = "straightLine";
                    // 关系？  相连？
                    if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the straightLine join with wavyLine"<<std::endl;
                    }
                    else
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the straightLine separation with wavyLine"<<std::endl;
                    }
                }
                else if(identification_type->recognize_wavyLine(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 波浪线"<<std::endl;
                    // 关系？ 相离？
                    str_1 = "wavyLine";
                    str_2 = "wavyLine";
                    if(identification_relation->separation(str_1,str_2,draw_coorstack[i],draw_coorstack[j]))
                    {

                        row_vec[j].push_back("separation");
                        std::cout<<"the wavyLine separate with wavyLine"<<std::endl;
                    }
                    else
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the wavyLine separate without wavyLine"<<std::endl;
                    }
                }
                else
                {
                    if(identification_type->recognize_curveLine(draw_coorstack[j]))
                    {
                        std::cout<<"第"<<j<<"条线段是: 曲线"<<std::endl;
                        // 关系？ 相连？
                        str_1 = "wavyLine";
                        str_2 = "curveLine";

                        if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                        {
                            row_vec[j].push_back("join");
                            std::cout<<"the wavyLine join with curveLine"<<std::endl;
                        }
                        else
                        {
                            row_vec[j].push_back("separation");
                            std::cout<<"the wavyLine join without curveLine"<<std::endl;
                        }
                    }
                }
                if(j == draw_coorstack.size()-1)
                {
                    row_vec.push_back("波浪线");
                    vec.push_back(row_vec);
                }

            }
            // ==============================================================曲线========================================================================================================
            else if(is_curveLine)
            {
                std::cout<<"第"<<i<<"条线段是: 曲线"<<std::endl;
                if(identification_type->recognize_cylinder(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 椭圆"<<std::endl;
                    // 关系？ 相离?
                    str_1 = "curveLine";
                    str_2 = "cylinder";
                    if(identification_relation->separation(str_1,str_2, draw_coorstack[i], draw_coorstack[j]))
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
                else if(identification_type->recognize_wavyLine(draw_coorstack[j]))
                {
                    std::cout<<"第"<<j<<"条线段是: 波浪线"<<std::endl;
                    str_1 = "curveLine";
                    str_2 = "wavyLine";
                    // 关系？ 相连？
                    if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                    {
                        row_vec[j].push_back("join");
                        std::cout<<"the curveLine join with wavyLine"<<std::endl;
                    }
                    else
                    {
                        row_vec[j].push_back("separation");
                        std::cout<<"the curveLine join without wavyLine"<<std::endl;
                    }
                }
                else
                {
                    if(identification_type->recognize_straightLine(draw_coorstack[j]))
                    {
                        std::cout<<"第"<<j<<"条线段是: 直线"<<std::endl;
                        // 关系？ 相连？
                        str_1 = "curveLine";
                        str_2 = "straightLine";
                        if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                        {
                            row_vec[j].push_back("join");
                            std::cout<<"the curveLine join with straightLine"<<std::endl;
                        }
                        else
                        {
                            row_vec[j].push_back("separation");
                            std::cout<<"the curveLine join without straightLine"<<std::endl;
                        }
                    }
                }
                if(j == draw_coorstack.size()-1)
                {
                    row_vec.push_back("曲线");
                    vec.push_back(row_vec);
                }
            }

            // ==========================================================其他====================================================================================================
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
                    if(vec[i][vec[i].size()-1] == "直线" && vec[j][vec[i].size()-1] == "直线")
                    {
                        flag+=1;
                    }
                }
                else if(vec[i][j] == "join")
                {
                    if((vec[i][vec[i].size()-1] == "直线" && vec[j][vec[i].size()-1] == "曲线")
                            || (vec[i][vec[i].size()-1] == "曲线" && vec[j][vec[i].size()-1] == "直线")
                            || (vec[i][vec[i].size()-1] == "椭圆" && vec[j][vec[i].size()-1] == "波浪线")
                            || (vec[i][vec[i].size()-1] == "波浪线" && vec[j][vec[i].size()-1] == "椭圆")
                            || (vec[i][vec[i].size()-1] == "波浪线" && vec[j][vec[i].size()-1] == "曲线")
                            || (vec[i][vec[i].size()-1] == "曲线" && vec[j][vec[i].size()-1] == "波浪线")
                            || (vec[i][vec[i].size()-1] == "波浪线" && vec[j][vec[i].size()-1] == "直线")
                            || (vec[i][vec[i].size()-1] == "直线" && vec[j][vec[i].size()-1] == "波浪线"))
                    {
//                        std::cout<<"vec[i][vec[i].size()-1]: "<<vec[i][vec[i].size()-1].toStdString()<<std::endl;
//                        std::cout<<"vec[j][vec[i].size()-1]: "<<vec[j][vec[i].size()-1].toStdString()<<std::endl;

                        flag+=1;
                    }
                }
                else if(vec[i][j] == "separation")
                {
                    if((vec[i][vec[i].size()-1] == "椭圆" && vec[j][vec[i].size()-1] == "曲线")
                            || (vec[i][vec[i].size()-1] == "曲线" && vec[j][vec[i].size()-1] == "椭圆")
                            || (vec[i][vec[i].size()-1] == "直线" && vec[j][vec[i].size()-1] == "波浪线")
                            || (vec[i][vec[i].size()-1] == "波浪线" && vec[j][vec[i].size()-1] == "直线")
                            || (vec[i][vec[i].size()-1] == "波浪线" && vec[j][vec[i].size()-1] == "波浪线")
                            || (vec[i][vec[i].size()-1] == "椭圆" && vec[j][vec[i].size()-1] == "直线"))
                    {
//                        std::cout<<"vec[i][vec[i].size()-1]: "<<vec[i][vec[i].size()-1].toStdString()<<std::endl;
//                        std::cout<<"vec[j][vec[i].size()-1]: "<<vec[j][vec[i].size()-1].toStdString()<<std::endl;

                        flag+=1;
                    }
                }
            }

            if(j == vec[i].size() - 1)
            {
                // 换行
                std::cout<<""<<std::endl;
                // 保存类型
                type_vec.push_back(vec[i][j]);
            }
        }
    }

    std::cout<<"flag: "<<flag<<std::endl;

    if(flag == 6)
    {
        this -> height_1 = identification_relation->height_1;
        this -> height_2 = identification_relation->height_2;
        this -> radius = identification_type->radius;
        return true;
    }
    else
        return false;
}
