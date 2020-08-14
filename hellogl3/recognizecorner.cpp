#include "recognizecorner.h"
#include "identification_relation.h"
#include "identification_type.h"

#include <iostream>

Recognizecorner::Recognizecorner()
{
    identification_relation = Identification_relation::getInstance();

    identification_type = Identification_type::getInstance();
}

bool Recognizecorner::recognize_corner_shape(QStack<QVector<float>> draw_coorstack)
{
    // 清除
    vec.clear();
    type_vec.clear();

    // 组合问题
    for(int i = 0; i < draw_coorstack.size(); i++)
    {
        // 识别类型
        bool is_cylinder = identification_type->recognize_cylinder(draw_coorstack[i]);
        bool is_corner = identification_type->recognize_corner(draw_coorstack[i]);

        std::cout<<"is_cylinder: "<<is_cylinder<<std::endl;
        std::cout<<"is_corner: "<<is_corner<<std::endl;

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
            else
            {
                temp_vec[temp_vec.size()-1].push_back("琦角");
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

                if(identification_type->recognize_corner(draw_coorstack[j]))
                {
                     std::cout<<"第"<<j<<"条线段是: 琦角"<<std::endl;
                     str_1 = "cylinder";
                     str_2 = "corner";
                     // 关系? 相连?
                     if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                     {
                         row_vec[j].push_back("join");
                         std::cout<<"the cylinder join with corner"<<std::endl;
                     }
                     else
                     {
                         row_vec[j].push_back("separation");
                         std::cout<<"the cylinder join without corner"<<std::endl;
                     }
                }

                if(j == draw_coorstack.size()-1)
                {
                    row_vec.push_back("椭圆");
                    vec.push_back(row_vec);
                }
            }
// =======================================================琦角===============================================================================================================
            else if(is_corner)
            {
                std::cout<<"第"<<i<<"条线段是: 琦角"<<std::endl;

                if(identification_type->recognize_cylinder(draw_coorstack[j]))
                {
                     std::cout<<"第"<<j<<"条线段是: 椭圆"<<std::endl;
                     str_1 = "corner";
                     str_2 = "cylinder";

                     // 关系? 相连?
                     if(identification_relation->join(str_1, str_2, draw_coorstack[i], draw_coorstack[j]))
                     {
                         row_vec[j].push_back("join");
                         std::cout<<"the corner join with cylinder"<<std::endl;
                     }
                     else
                     {
                         row_vec[j].push_back("separation");
                         std::cout<<"the corner join without cylinder"<<std::endl;
                     }
                }

                if(j == draw_coorstack.size()-1)
                {
                    row_vec.push_back("椭圆");
                    vec.push_back(row_vec);
                }
            }
// =======================================================其他===============================================================================================================
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

                if(vec[i][j] == "join")
                {
                    if((vec[i][vec[i].size()-1] == "椭圆" && vec[j][vec[i].size()-1] == "琦角")
                            || (vec[i][vec[i].size()-1] == "琦角" && vec[j][vec[i].size()-1] == "椭圆"))
                    {

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

    if(flag == 1)
    {
        this -> radius = identification_type->radius;
        return true;
    }
    else
        return false;


}
