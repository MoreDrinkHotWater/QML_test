#include "widget.h"
#include "ui_widget.h"

#include <iostream>
#include <random>
#include <algorithm>

// 产生随机数据
#include <cstdlib>
#include <ctime>
#define random(a,b) (rand()%(b-a)+a)

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

// 生成随机点数据
void Widget::on_generateButton_clicked()
{
    srand((int)time(nullptr));  // 产生随机种子

    for (int i = 0; i < 20; i++) {
        int x = random(100,500), y = random(100,500);
        QPoint point(x,y);

        randomPoint_vector.push_back(point);
    }

//    for (int i = 0; i < 5; i++)
//        std::cout<<"randomPoint_vector["<<i<<"] "<<randomPoint_vector[i].x()<<" "<<randomPoint_vector[i].y()<<std::endl;

    ui->canvas->receive_randomPoint_vector(randomPoint_vector);
}

void Widget::on_cleanButton_clicked()
{
    ui->canvas->randomPoint_vector.clear();

    randomPoint_vector.clear();
}
