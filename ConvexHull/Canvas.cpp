#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <iostream>
#include <math.h>

#include "Canvas.h"
#include "widget.h"

Canvas::Canvas(QWidget *parent):
    QWidget(parent)
{
    // keyPressEvent 函数没有响应键盘事件的解决方法
    setFocusPolicy(Qt::ClickFocus);
}

void Canvas::paintEvent(QPaintEvent *event){

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.fillRect(event->rect(), QBrush(Qt::blue));

    if(draw_lines_vector.size() >= 2)
        drawPoint(painter);

    drawLines(painter);

    // draw RandomPoints
    if(randomPoint_vector.size() != 0)
    {
        drawRandomPoints(painter);
    }
}

void Canvas::drawRandomPoints(QPainter &painter)
{

    painter.setPen(Qt::green);

    painter.drawPoints(randomPoint_vector);

    painter.setPen(Qt::red);

    auto compare_x = [](QPoint p1, QPoint pn)
    {
        return p1.x() < pn.x();
    };

    // 先把所有点对 x 坐标排序，如果有多个点的 x 坐标相同，再对 y 坐标排序。
    std::sort(randomPoint_vector.begin(), randomPoint_vector.end(), compare_x);

    for (int i = 0; i < 20; i++) {
        if(randomPoint_vector[i].x() == randomPoint_vector[i+1].x())
        {
//            std::cout<<"randomPoint_vector["<<i<<"].x == randomPoint_vector["<<i+1<<"].x"<<std::endl;
            if(randomPoint_vector[i].y() > randomPoint_vector[i+1].y())
            {
                QPoint temp;
                temp = randomPoint_vector[i];
                randomPoint_vector[i] = randomPoint_vector[i+1];
                randomPoint_vector[i+1] = temp;
            }
        }
    }

    // drawLine
    // 1.连接 p1->pn : 确认上凸包
    QPoint p1 = randomPoint_vector[0], pn = randomPoint_vector[19];

    painter.drawLine(p1,pn);

    QVector<QPoint> up_points_vector, down_points_vector;

    for(auto point: randomPoint_vector)
    {
        // 若点位于直线的左侧， 则认为是上凸包的点， 这里没考虑与直线共线的点
        int Tmp = (p1.y() - pn.y()) * point.x() + (pn.x() - p1.x()) * point.y() + p1.x() * pn.y()- pn.x() * p1.y();
        if(Tmp <= 0)
        {
//            painter.drawLine(p1,point);
            up_points_vector.push_back(point);
        }
        else
        {
//            painter.drawLine(pn,point);
            down_points_vector.push_back(point);
        }
    }

    // 2.遍历上凸包： 如果连接的 p1->p2 可以使得上凸包其余所有点都落在它的右边， 就连接起来
    for(int i = 0; i < up_points_vector.size(); i++)
    {
        QVector<QPoint> other_point_vector;

        QPoint p2 = up_points_vector[i];

        // 记录落在直线左侧的点的个数
        int left_num = 0;

        // 保存其他点的数组
        for(int j = 0; j < up_points_vector.size(); j++)
        {
            if(up_points_vector[j] != p2)
                other_point_vector.push_back(up_points_vector[j]);
        }
        for(int k = 0; k < other_point_vector.size(); k++)
        {
            int Tmp = (p1.y() - p2.y()) * other_point_vector[k].x() + (p2.x() - p1.x()) * other_point_vector[k].y() + p1.x() * p2.y()- p2.x() * p1.y();

            if(Tmp > 0)
                left_num += 1;
        }

        if(left_num == 0)
        {
            std::cout<< "left_num: " << left_num <<std::endl;

            painter.drawLine(p1,p2);

            p1 = p2;
        }

    }

}

void Canvas::mousePressEvent(QMouseEvent *event){
    m_lastPos = event->pos();

    if (event->buttons() & Qt::LeftButton) {

        float dx = m_lastPos.x();
        float dy = m_lastPos.y();

        draw_lines_vector.push_back(dx);
        draw_lines_vector.push_back(dy);

        std::cout<<"dx: "<<dx<<" dy: "<<dy<<std::endl;

        update();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event){
    m_lastPos = event->pos();

    if (event->buttons() & Qt::LeftButton) {

        float dx = m_lastPos.x() ;
        float dy = m_lastPos.y() ;

        draw_lines_vector.push_back(dx);
        draw_lines_vector.push_back(dy);

        std::cout<<"dx: "<<dx<<" dy: "<<dy<<std::endl;

        update();
    }

}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{

    if (event->KeyRelease && event->button() == Qt::LeftButton)
    {
        // 不加入这个函数无法触发 mouseReleaseEvent ！
        releaseMouse();

        draw_stack.push_back(draw_lines_vector);

        std::cout<<"draw_stack size: "<<draw_stack.size()<<std::endl;

        draw_lines_vector.clear();

        update();
    }


}

void Canvas::drawPoint(QPainter &painter)
{

    painter.setPen(Qt::green);

    for (int last = draw_lines_vector.size() - 1; last >= 1; last-=2) {
        painter.drawPoint(draw_lines_vector[last-1],draw_lines_vector[last]);
    }

    update();
}

void Canvas::drawLines(QPainter &painter)
{
    painter.setPen(Qt::red);

    QVector<QLineF> temp_vector;

    for(auto it = draw_stack.begin(); it != draw_stack.end(); ++it)
    {
        for (int last = it->size() - 1; last >= 3; last-=2) {

            QLineF temp_line(it->data()[last-3],it->data()[last-2],it->data()[last-1],it->data()[last]);
            temp_vector.push_back(temp_line);
        }
    }

    painter.drawLines(temp_vector);

    update();
}

void Canvas::keyPressEvent(QKeyEvent *event)
{

    if (event->modifiers() ==  Qt::ControlModifier && event->key() == Qt::Key_Z)
    {
        if(draw_stack.size() != 0)
        {
            draw_stack.pop();

            std::cout<<"draw_stack size: "<<draw_stack.size()<<std::endl;

            update();
        }
        else
        {
            std::cout<<"draw_stack size is zero!"<<std::endl;
        }
    }
}

void Canvas::receive_randomPoint_vector(QVector<QPoint> randomPoint_vector)
{
    std::cout<< "randomPoint_vector.size: "<< randomPoint_vector.size() <<std::endl;

    this->randomPoint_vector = randomPoint_vector;
}
