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

    auto compare_x = [](QPoint p1, QPoint p2)
    {
        return p1.x() < p2.x();
    };

    // 先把所有点对 x 坐标排序，如果有多个点的 x 坐标相同，再对 y 坐标排序。
    std::sort(randomPoint_vector.begin(), randomPoint_vector.end(), compare_x);

    // drawLine
    painter.drawLine(randomPoint_vector[0],randomPoint_vector[1]);
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
