﻿#include "Canvas.h"
#include "glwidget.h"

#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <iostream>
#include <math.h>

Canvas::Canvas(QWidget *parent):
    QWidget(parent)
{
    // keyPressEvent 函数没有响应键盘事件的解决方法
    setFocusPolicy(Qt::ClickFocus);

}

void Canvas::paintEvent(QPaintEvent *event){
    //        qDebug() << "================================paintEvent" << draw_lines_vector.size();

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.fillRect(event->rect(), QBrush(Qt::blue));


    if(draw_lines_vector.size() >= 2)
        drawPoint(painter);

    drawLines(painter);

    if(draw_stack.size() == 2)
    {
        draw_centerLine(painter);
    }

//    QPainter p;
//    QPixmap map(108*2, 192*2-100);
//    map.fill(Qt::transparent);
//    p.begin(&map);

//    p.setPen(QPen(QColor("red")));

//    if(draw_stack.size() > 0)
//        {
//            for (int i = 0; i < draw_stack.size(); i++) {
//                QPointF points[draw_stack[i].size()];
//                for (int j = 0; j < draw_stack[i].size(); j+=2) {
//                    points->setX(draw_stack[i][j]);
//                    points->setY(draw_stack[i][j+1]);
//                }

//                p.drawPolyline(points,draw_stack[i].size());
//            }
//        }

//    p.end();
//    map.save("/home/damon/Desktop/image/image.png", "PNG");

}

void Canvas::mousePressEvent(QMouseEvent *event){
    m_lastPos = event->pos();

    if (event->buttons() & Qt::LeftButton) {

        int dx = m_lastPos.x() ;
        int dy = m_lastPos.y() ;

        draw_lines_vector.push_back(dx);
        draw_lines_vector.push_back(dy);

        //        std::cout<<"dx: "<<dx<<" dy: "<<dy<<std::endl;

        update();
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event){
    m_lastPos = event->pos();

    if (event->buttons() & Qt::LeftButton) {

        int dx = m_lastPos.x() ;
        int dy = m_lastPos.y() ;

        draw_lines_vector.push_back(dx);
        draw_lines_vector.push_back(dy);

        //        std::cout<<"dx: "<<dx<<" dy: "<<dy<<std::endl;

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

    QVector<QLine> temp_vector;

    for(auto it = draw_stack.begin(); it != draw_stack.end(); ++it)
    {
        for (int last = it->size() - 1; last >= 3; last-=2) {

            QLine temp_line(it->data()[last-3],it->data()[last-2],it->data()[last-1],it->data()[last]);
            temp_vector.push_back(temp_line);
        }
    }

    painter.drawLines(temp_vector);

    update();
}

void Canvas::draw_centerLine(QPainter &painter)
{
    painter.setPen(Qt::green);

    QVector<QVector2D> head_circle, corner_line;
    for (int i = 0; i < draw_stack[0].size() - 1; i+=2){

        QVector2D point = QVector2D(draw_stack[0][i], draw_stack[0][i+1]);

        head_circle.push_back(point);
    }

    float maxX = head_circle[0].x(),minX = maxX,  maxY = head_circle[0].y(), minY = maxY;
    for(auto it = head_circle.begin(); it != head_circle.end(); it++)
    {
        maxX = qMax(maxX,it->x());
        minX = qMin(minX,it->x());
        maxY = qMax(maxY,it->y());
        minY = qMin(minY,it->y());

    }

    QVector2D min = QVector2D(minX,minY);
    QVector2D max = QVector2D(maxX,maxY);

    QVector<QLine> temp_vector;
    QVector<QPoint> centerPoint_vector;
    QVector<QPoint> temp_centerPoint_vector;

// 方法 0
#if 0
    for (int i = 0; i < draw_stack[1].size() - 1; i+=2){

        QVector2D point = QVector2D(draw_stack[1][i], draw_stack[1][i+1]);

        corner_line.push_back(point);
    }

    QVector<QVector2D>::iterator it_head = corner_line.begin();
    QVector<QVector2D>::iterator it_end = corner_line.end();

    for(int i = 0; i < (corner_line.size() / 2); i++)
    {
        it_head += 1;
        it_end -= 1;
        QPoint center = QPoint((it_head->x() + it_end->x()) / 2, (it_head->y() + it_end->y()) /2);

        temp_vector.push_back(QLine( QPoint(it_head->x(), it_head->y()), QPoint(it_end->x(), it_end->y())));

        centerPoint_vector.push_back(center);
    }

// 方法 2
#elif 1
    // 琦角的线
    float tolal_length = 0;
    for (int i = 0; i < draw_stack[1].size() - 1; i+=2){

        QVector2D point = QVector2D(draw_stack[1][i], draw_stack[1][i+1]);

        corner_line.push_back(point);
    }

    for (int i = 0; i < corner_line.size(); i++) {
        int i_1 = (i+1)%corner_line.size();
        tolal_length += sqrt(pow(corner_line[i_1].x() - corner_line[i].x(),2) + pow(corner_line[i_1].y() - corner_line[i].y(),2));
    }

    float height = tolal_length/2;

    // 竖直垂线的长度
//    float vertical_step =  height/(corner_line.size()/2);
    float vertical_step =  0.5;

    // 平行垂线的长度
    float parallel_step = 1;

    QPoint p1,p2,p3,p4,p1_p2,p3_p4;

    float p1_p2_length,p3_p4_length;

    for (int i = 1; i < corner_line.size()/2; i++) {

        if(i == 1)
        {
            p1 = QPoint( corner_line[0].x(), corner_line[0].y() );
            p2 = QPoint( corner_line[corner_line.size() - 1].x(), corner_line[corner_line.size() - 1].y());

            p1_p2_length = sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(), 2));
            // 向量 P1_p2
            p1_p2 = QPoint( p2.x() - p1.x(), p2.y() - p1.y());
            temp_vector.push_back(QLine(p1,p2));

            continue;
        }

        // 向量 P1_p2 的垂直向量 除以 (p1_p2_length/vertical_step) 是为了保证竖直垂线的长度一致
        p3_p4 = QPoint(-p1_p2.y()/(p1_p2_length/(vertical_step*i)), p1_p2.x()/(p1_p2_length/(vertical_step*i)));

        std::cout<<"p3_p4: ("<<p3_p4.x()<<","<<p3_p4.y()<<")"<<std::endl;

        p3 = QPoint((p1.x() + p2.x())/2, (p1.y() + p2.y())/2);

        std::cout<<"p3: ("<<p3.x()<<","<<p3.y()<<")"<<std::endl;

        //  把线段的中心点加入数组
        centerPoint_vector.push_back(p3);

        temp_centerPoint_vector.push_back(p3);

        p4 = QPoint(p3_p4.x() + p3.x(), p3_p4.y() + p3.y());

        std::cout<<"p4: ("<<p4.x()<<","<<p4.y()<<")"<<std::endl;

        temp_vector.push_back(QLine(p3,p4));

        p3_p4_length = sqrt(pow(p4.x() - p3.x(), 2) + pow(p4.y() - p3.y(), 2));

        p3_p4 = QPoint( p4.x() - p3.x(), p4.y() - p3.y());

#if 0
        // 连接两条线段的中点作延长线
        if(temp_centerPoint_vector.size() == 2)
        {

            for(auto it : temp_centerPoint_vector)
            {
                std::cout<<"it: ("<<it.x()<<","<<it.y()<<")"<<std::endl;
            }

            p3 = temp_centerPoint_vector[0];
            p4 = temp_centerPoint_vector[1];

            p3_p4_length = sqrt(pow(p4.x() - p3.x(), 2) + pow(p4.y() - p3.y(), 2));

            // 已知两点 p3, p4坐标,求两点连成的直线中的某一点坐标

            float x = 2 * p4.x() - p3.x();

            float y = 2 * p4.y() - p3.y();

            std::cout<<"p5: ("<<x<<","<<y<<")"<<std::endl;

            p3 = p4;

            p4 = QPoint(x,y);

            p3_p4 = QPoint( p4.x() - p3.x(), p4.y() - p3.y());

            temp_vector.push_back(QLine(p3,p4));

            temp_centerPoint_vector.pop_front();
        }

        // 已知一个直线方程  p3_p4: Ax + By + 1 = 0 ，求垂直于它的另一个直线方程
        float B = (p3.x() * p4.x() - p3.x())/(p3.x() * p4.y() - p3.x() * p4.x() * p4.y());

        float A = -(B * p3.y() +1)/p3.x();

        std::cout<< "A: " <<A<<std::endl;

        std::cout<< "B: " <<B<<std::endl;

        QVector<QPoint> line_vec;
        // 求向量 p3_p4 与琦角的交点
        for (int j = 0; j < corner_line.size() - 1; j++) {

            if(B * corner_line[j].x() - A * corner_line[j].y() + A * p4.y() - B * p4.x() == 0)
            {
                std::cout<<"corner_line[j]: ("<<corner_line[j].x()<<","<<corner_line[j].y()<<")"<<std::endl;

                line_vec.push_back(QPoint(corner_line[j].x(), corner_line[j].y()));
            }
        }

#elif 1

        QVector<QPoint> line_vec;
        // 求向量 p3_p4 与琦角的交点
        for (int j = 0; j < corner_line.size() - 1; j++) {

            if(p4.y() > corner_line[j].y()  && p4.y() < corner_line[j+1].y())
            {
                line_vec.push_back(QPoint(corner_line[j].x(), corner_line[j].y()));
            }
            if(p4.y() < corner_line[j].y()  && p4.y() > corner_line[j+1].y())
            {
                line_vec.push_back(QPoint(corner_line[j+1].x(), corner_line[j+1].y()));
            }
        }
#endif
        if(line_vec.size() == 2)
        {
            p1 = line_vec[0];
            p2 = line_vec[1];

            parallel_step = sqrt(pow(p2.x() - p1.x(), 2) + pow(p2.y() - p1.y(),2));

            // 向量 p3_p4 的垂直向量 除以 (p3_p4_length/parallel_step) 是为了保证水平垂线的长度一致
            p1_p2 = QPoint(p3_p4.y()/(p3_p4_length/parallel_step), -p3_p4.x()/(p3_p4_length/parallel_step));

            p1_p2_length = parallel_step;

            temp_vector.push_back(QLine(p1,p2));
        }
    }

#endif
    QVector<QLine> centerLine_vector;

    std::cout<<"centerPoint_vector.size(): "<<centerPoint_vector.size()<<std::endl;

    for(int i = 0; i < centerPoint_vector.size() - 1; i++)
    {
        QLine temp_line = QLine(centerPoint_vector[i], centerPoint_vector[i+1]);
        centerLine_vector.push_back(temp_line);
    }

    painter.drawLines(temp_vector);

    painter.setPen(Qt::red);

    painter.drawLines(centerLine_vector);

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



