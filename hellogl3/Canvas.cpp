#include "Canvas.h"
#include "glwidget.h"

#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <iostream>

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



