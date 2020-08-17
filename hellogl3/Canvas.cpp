#include "Canvas.h"
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

    // 斜椭圆
//    QLine temp_line(max.x(), min.y(), (min.x() + max.x())/2, (min.y() + max.y())/2);

    // 平椭圆
    QLine temp_line(max.x(), (min.y() + max.y())/2, (min.x() + max.x())/2, (min.y() + max.y())/2);

    temp_vector.push_back(temp_line);

    // 琦角的线
    float tolal_length = 0;
    for (int i = 0; i < draw_stack[1].size() - 1; i+=2){

        QVector2D point = QVector2D(draw_stack[1][i], draw_stack[1][i+1]);

        corner_line.push_back(point);
    }

    for (int i = 0; i < corner_line.size() ; i++) {
        int i_1 = (i+1)%corner_line.size();
        tolal_length += sqrt(pow(corner_line[i_1].x() - corner_line[i].x(),2) + pow(corner_line[i_1].y() - corner_line[i].y(),2));
    }

    float height = tolal_length / 2;

    float step =  height / (corner_line.size()/2);

    QPoint p1 = QPoint((min.x() + max.x())/2, (min.y() + max.y())/2);
    QPoint p2 = QPoint(max.x(), (min.y() + max.y())/2);

    // 向量 P1_p2
    QPoint p1_p2 = QPoint(p2.x() - p1.x(), p2.y() - p1.y());

    // 向量 P1_p2 的垂直向量
    QPoint p1_p3 = QPoint(p2.y() - p1.y(), -(p2.x() - p1.x()));

    QPoint p3 = QPoint((min.x() + max.x())/2 + (p2.y() - p1.y()), ((min.y() + max.y())/2 - (p2.x() - p1.x())) / step );

    temp_vector.push_back(QLine(p1,p3));

//    for (int i = 0; i < corner_line.size()/2; i++) {

//    }

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



