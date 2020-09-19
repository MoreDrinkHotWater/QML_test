#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <iostream>
#include <math.h>

#include "Canvas.h"
#include "glwidget.h"

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

    if(draw_stack.size() == 1)
        // 首尾按 5 为比例拉
        draw_centerLine2(painter);

    if(draw_stack.size() == 2)
    {
#if 0
        // 延长线拉
        draw_centerLine(painter);
#elif 1
        // 首尾按 5 为比例拉
        draw_centerLine2(painter);
#elif 0
        // 平行线拉
        draw_centerLine3(painter);
#elif 0
        draw_centerLine4(painter);
#endif
    }
}

void Canvas::mousePressEvent(QMouseEvent *event){
    m_lastPos = event->pos();

    if (event->buttons() & Qt::LeftButton) {

        float dx = m_lastPos.x();
        float dy = m_lastPos.y();

        draw_lines_vector.push_back(dx);
        draw_lines_vector.push_back(dy);

        //        std::cout<<"dx: "<<dx<<" dy: "<<dy<<std::endl;

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

void Canvas::draw_centerLine(QPainter &painter)
{
    painter.setPen(Qt::green);

    QVector<QVector2D> head_circle, corner_line;
    for (int i = 0; i < draw_stack[0].size() - 1; i+=2){

        QVector2D point = QVector2D(draw_stack[0][i], draw_stack[0][i+1]);

        head_circle.push_back(point);
    }

    QVector<QLineF> temp_vector;
    QVector<QPointF> centerPoint_vector;
    QVector<QPointF> temp_centerPoint_vector;

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

    // 竖直垂线的长度
    float vertical_step = 8;

    // 平行垂线的长度
    float parallel_step = 1;

    QPointF p1,p2,p3,p4,p5,p1_p2,p3_p4;

    float p1_p2_length,p3_p4_length;

    for (int i = 1; i < corner_line.size(); i++) {

        if(i == 1)
        {
            p1 = QPointF( corner_line[0].x(), corner_line[0].y() );
            p2 = QPointF( corner_line[corner_line.size() - 1].x(), corner_line[corner_line.size() - 1].y());

            p1_p2_length = QVector2D(p2 - p1).length();

            // 向量 P1_p2
            p1_p2 = QPointF( p2 - p1);
            temp_vector.push_back(QLineF(p1,p2));

            continue;
        }

        // 直线 p1_p2 的直线方程:  0 = Ax + By + C
        float A = p2.y() - p1.y();
        float B = p1.x() - p2.x();
        float C = p2.x() * p1.y() - p1.x() * p2.y();

        p3 = QPointF(p1 + p2) / 2;

        //  把线段的中心点加入数组
        centerPoint_vector.push_back(p3);

        temp_centerPoint_vector.push_back(p3);

        if(0 == A * p3.x() + B * p3.y() + C)
        {
            std::cout<<"p3 success!"<<std::endl;
        }

        std::cout<<"p3: ("<<p3.x()<<","<<p3.y()<<")"<<std::endl;

        // 过点 p3 的另一直线为 y = (B/A)*x + p3.y() - (B/A)*p3.x() 即: B*x - A*y +A*p3.y() - B*p3.x() = 0

        QVector<QPointF> test_circle_vector;
        for (int i = 0; i <= 360 ; i++) {
            // i 是弧度，需要转成角度 t
            float t = i * 2 * M_PI / 360;

            // 改变圆心位置，只用给 x,y 加入固定的数值即可。
            test_circle_vector.push_back(QPointF(p3.x() + vertical_step * cos(t), p3.y() + vertical_step * sin(t)));
        }

        for(auto it: test_circle_vector)
        {
            QVector2D v1 = QVector2D(p1 - p3);
            QVector2D v2 = QVector2D(it - p3);
            float cos_a_b =(v1.x()*v2.x() + v1.y()*v2.y())/(v1.length()*v2.length());

            if(qAbs(cos_a_b) < 0.02)
            {
                p4 = QPointF(it.x(), it.y());
                break;
            }
        }

        std::cout<<"p4: ("<<p4.x()<<","<<p4.y()<<")"<<std::endl;

        // 连接两条线段的中点作延长线
        if(temp_centerPoint_vector.size() == 2)
        {
            p3 = temp_centerPoint_vector[0];
            p4 = temp_centerPoint_vector[1];

            std::cout<<"===================================="<<std::endl;
            std::cout<<"p3: ("<<p3.x()<<","<<p3.y()<<")"<<std::endl;
            std::cout<<"p4: ("<<p4.x()<<","<<p4.y()<<")"<<std::endl;

            // 已知两点 p3, p4坐标,求两点连成的直线中的某一点坐标
//            p3_p4 = QPointF(p4 - p3);
//            p5 = p4 + 5*(QVector2D(p3_p4)/QVector2D(p3_p4).length()).toPointF();

            int flag_last = 0;
            // 修复最后一段延长线过长的问题
            if(i == corner_line.size() - 1)
            {
                A = p4.y() - p3.y();
                B = p3.x() - p4.x();
                C = p4.x() * p3.y() - p3.x() * p4.y();

                QPointF last_point;

                if(A != 0 && B != 0 && C != 0)
                {
                    for (int j = 0; j < corner_line.size(); j++) {
                         if(A * corner_line[j].x() + B * corner_line[j].y() + C == 0)
                         {
                            flag_last += 1;
                            last_point = corner_line[j].toPoint();
                         }
                    }

                    centerPoint_vector.push_back(last_point);

                    p5 = 2 * p4 - p3;

                    std::cout<<"p5: ("<<p5.x()<<","<<p5.y()<<")"<<std::endl;

                    std::cout<<"A: "<<A<<std::endl;
                    std::cout<<"B: "<<B<<std::endl;
                    std::cout<<"C: "<<C<<std::endl;
                    std::cout<<"flag_last: "<< flag_last <<std::endl;
                }

                continue;
            }

            p5 = 2 * p4 - p3;

            std::cout<<"p5: ("<<p5.x()<<","<<p5.y()<<")"<<std::endl;

            p3 = p4;

            p4 = p5;

            // 新的向量 p3_p4
            p3_p4 =QPointF(p4 - p3);

            p3_p4_length = QVector2D(p4 - p3).length();

            std::cout<<"p4_p5_length: "<<p3_p4_length<<std::endl;

            temp_vector.push_back(QLineF(p3,p4));

            temp_centerPoint_vector.pop_front();
        }

        // 已知一个直线方程  p3_p4: Ax + By + C = 0 ，求垂直于它的另一个直线方程
        QVector<QPointF> line_vec;

        A = p4.y() - p3.y();
        B = p3.x() - p4.x();

        std::cout<<"A: "<<A<<std::endl;

        std::cout<<"B: "<<B<<std::endl;

        QVector<float> Var;

        QVector<QVector2D> Point;

        // 求向量 p3_p4 与琦角的交点
        for (int j = 0; j < corner_line.size(); j++) {

            float temp_var = B * corner_line[j].x() - A * corner_line[j].y() + A * p4.y() - B * p4.x();

            Var.push_back(temp_var);

            Point.push_back(corner_line[j]);
        }

        bool flag = false;

        for(int i = 1; i < Var.size(); i++)
        {

//            std::cout<<"Var["<<i-1<<"]: "<<Var[i-1]<<std::endl;

            float it_front = Var[i-1];
            float it_next = Var[i];

            if(it_front >= 0 && it_next < 0 )
            {
                line_vec.push_back(Point[i-1].toPointF());

                std::cout<<"====================="<<Var[i-1]<<std::endl;

                continue;
            }

            if(it_front < 0 && it_next >= 0)
            {
                line_vec.push_back(Point[i].toPointF());

                std::cout<<"====================="<<Var[i]<<std::endl;

                flag = true;

                continue;
            }
        }

        std::cout<<"line_vec.size: "<<line_vec.size()<<std::endl;

        if(line_vec.size() == 1)
        {
            if(flag)
                // 补尾巴
                line_vec.push_back(Point[0].toPointF());
            else
                // 补头
                line_vec.push_back(Point[corner_line.size() - 1].toPointF());
        }


        if(line_vec.size() == 2)
        {
            p1 = line_vec[0];
            p2 = line_vec[1];

            std::cout<<"p1: ("<<p1.x()<<","<<p1.y()<<")"<<std::endl;
            std::cout<<"p2: ("<<p2.x()<<","<<p2.y()<<")"<<std::endl;

            parallel_step = QVector2D(p2 - p1).length();

            // 向量 p3_p4 的垂直向量 除以 (p3_p4_length/parallel_step) 是为了保证水平垂线的长度一致
            p1_p2 = QPointF(p3_p4.y(), -p3_p4.x())/(p3_p4_length/parallel_step);

            p1_p2_length = parallel_step;

            temp_vector.push_back(QLineF(p1,p2));
        }
    }

    QVector<QLineF> centerLine_vector;

    for(int i = 0; i < centerPoint_vector.size() - 1; i++)
    {
        QLineF temp_line = QLineF(centerPoint_vector[i], centerPoint_vector[i+1]);
        centerLine_vector.push_back(temp_line);
    }

    painter.drawLines(temp_vector);

    painter.setPen(Qt::red);

    painter.drawLines(centerLine_vector);

    update();
}

void Canvas::draw_centerLine2(QPainter &painter)
{
    painter.setPen(Qt::green);

    QVector<QVector2D> corner_line;

    QVector<QLineF> temp_vector;
    QVector<QPointF> centerPoint_vector;

    if(draw_stack.size() == 2)
    {
        for (int i = 0; i < draw_stack[1].size() - 1; i+=2){

            QVector2D point = QVector2D(draw_stack[1][i], draw_stack[1][i+1]);

            corner_line.push_back(point);
        }
    }
    else
    {
        for (int i = 0; i < draw_stack[0].size() - 1; i+=2){

            QVector2D point = QVector2D(draw_stack[0][i], draw_stack[0][i+1]);

            corner_line.push_back(point);
        }
    }

    QPointF p1, p2;
    QVector<QPointF> line_vec;

//    std::cout<<"corner_line.size: "<<corner_line.size()<<std::endl;

    for (int i = 0; i < corner_line.size(); i++) {

        if(i == 0)
        {

            p1 = QPointF(corner_line[0].x(), corner_line[0].y());

            p2 = QPointF(corner_line[corner_line.size() - 1].x(), corner_line[corner_line.size() - 1].y());

            line_vec.push_back(p1);

            temp_vector.push_back(QLineF(p1,p2));

            centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

            continue;
        }

       if(i % 5 == 0)
           line_vec.push_back(QVector2D(corner_line[i]).toPointF());

    }

//    std::cout<<"line_vec.size: "<<line_vec.size()<<std::endl;

    QVector<QPointF>::iterator it_first = line_vec.begin();
    QVector<QPointF>::iterator it_end = line_vec.end();

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

        temp_vector.push_back(QLineF(p1,p2));

        centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

    }while(it_first != it_end);

    QVector<QLineF> centerLine_vector;

    for(int i = 0; i < centerPoint_vector.size() - 1; i++)
    {
        QLineF temp_line = QLineF(centerPoint_vector[i], centerPoint_vector[i+1]);
        centerLine_vector.push_back(temp_line);
    }

    painter.drawLines(temp_vector);

    painter.setPen(Qt::red);

    painter.drawLines(centerLine_vector);

    update();
}

void Canvas::draw_centerLine3(QPainter &painter)
{
    painter.setPen(Qt::green);

    QVector<QVector2D> head_circle, corner_line;
    for (int i = 0; i < draw_stack[0].size() - 1; i+=2){

        QVector2D point = QVector2D(draw_stack[0][i], draw_stack[0][i+1]);

        head_circle.push_back(point);
    }

    QVector<QLineF> temp_vector;
    QVector<QPointF> centerPoint_vector;
    QVector<QPointF> temp_centerPoint_vector;

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

    // 竖直垂线的长度
    float vertical_step = 8;

    // 平行垂线的长度
    float parallel_step = 1;

    QPointF p1,p2,p3,p4,p1_p2,p3_p4;

    float p1_p2_length,p3_p4_length;

    for (int i = 1; i < corner_line.size(); i++) {

        if(i == 1)
        {
            p1 = QPointF( corner_line[0].x(), corner_line[0].y() );
            p2 = QPointF( corner_line[corner_line.size() - 1].x(), corner_line[corner_line.size() - 1].y());

            p1_p2_length = QVector2D(p2 - p1).length();

            // 向量 P1_p2
            p1_p2 = QPointF( p2 - p1);
            temp_vector.push_back(QLineF(p1,p2));

            continue;
        }

        // 直线 p1_p2 的直线方程:  0 = Ax + By + C
        float A = p2.y() - p1.y();
        float B = p1.x() - p2.x();
        float C = p2.x() * p1.y() - p1.x() * p2.y();

        p3 = QPointF(p1 + p2) / 2;

        //  把线段的中心点加入数组
        centerPoint_vector.push_back(p3);

        temp_centerPoint_vector.push_back(p3);

        if(0 == A * p3.x() + B * p3.y() + C)
        {
            std::cout<<"p3 success!"<<std::endl;
        }

        std::cout<<"p3: ("<<p3.x()<<","<<p3.y()<<")"<<std::endl;

        // 过点 p3 的另一直线为 y = (B/A)*x + p3.y() - (B/A)*p3.x() 即: B*x - A*y +A*p3.y() - B*p3.x() = 0

        QVector<QPointF> test_circle_vector;
        for (int i = 0; i <= 360 ; i++) {
            // i 是弧度，需要转成角度 t
            float t = i * 2 * M_PI / 360;

            // 改变圆心位置，只用给 x,y 加入固定的数值即可。
            test_circle_vector.push_back(QPointF(p3.x() + vertical_step * cos(t), p3.y() + vertical_step * sin(t)));
        }

        for(auto it: test_circle_vector)
        {
            QVector2D v1 = QVector2D(p1 - p3);
            QVector2D v2 = QVector2D(it - p3);
            float cos_a_b =(v1.x()*v2.x() + v1.y()*v2.y())/(v1.length()*v2.length());

            if(qAbs(cos_a_b) < 0.02)
            {
                p4 = QPointF(it.x(), it.y());
                break;
            }
        }

        std::cout<<"p4: ("<<p4.x()<<","<<p4.y()<<")"<<std::endl;

        temp_vector.push_back(QLineF(p3,p4));

        //新的向量 p3_p4
        p3_p4 = QPoint( p4.x() - p3.x(), p4.y() - p3.y());

        p3_p4_length = QVector2D(p4 - p3).length();

        std::cout<<"p3_p4_length: "<<p3_p4_length<<std::endl;

        // 已知一个直线方程  p3_p4: Ax + By + C = 0 ，求垂直于它的另一个直线方程

        QVector<QPointF> line_vec;

        A = p4.y() - p3.y();
        B = p3.x() - p4.x();

        std::cout<<"A: "<<A<<std::endl;

        std::cout<<"B: "<<B<<std::endl;

        QVector<float> Var;

        QVector<QVector2D> Point;

        // 求向量 p3_p4 与琦角的交点
        for (int j = 0; j < corner_line.size(); j++) {

            float temp_var = B * corner_line[j].x() - A * corner_line[j].y() + A * p4.y() - B * p4.x();

            Var.push_back(temp_var);

            Point.push_back(corner_line[j]);
        }

        bool flag = false;

        for(int i = 1; i < Var.size(); i++)
        {

//            std::cout<<"Var["<<i-1<<"]: "<<Var[i-1]<<std::endl;

            float it_front = Var[i-1];
            float it_next = Var[i];

            if(it_front >= 0 && it_next < 0 )
            {
                line_vec.push_back(Point[i-1].toPointF());

                std::cout<<"====================="<<Var[i-1]<<std::endl;

                continue;
            }

            if(it_front < 0 && it_next >= 0)
            {
                line_vec.push_back(Point[i].toPointF());

                std::cout<<"====================="<<Var[i]<<std::endl;

                flag = true;

                continue;
            }
        }

        std::cout<<"line_vec.size: "<<line_vec.size()<<std::endl;

        if(line_vec.size() == 1)
        {
            if(flag)
                // 补尾巴
                line_vec.push_back(Point[0].toPointF());
            else
                // 补头
                line_vec.push_back(Point[corner_line.size() - 1].toPointF());
        }

        if(line_vec.size() == 2)
        {
            p1 = line_vec[0];
            p2 = line_vec[1];

            std::cout<<"p1: ("<<p1.x()<<","<<p1.y()<<")"<<std::endl;
            std::cout<<"p2: ("<<p2.x()<<","<<p2.y()<<")"<<std::endl;

            parallel_step = QVector2D(p2 - p1).length();

            // 向量 p3_p4 的垂直向量 除以 (p3_p4_length/parallel_step) 是为了保证水平垂线的长度一致
            p1_p2 = QPointF(p3_p4.y(), -p3_p4.x())/(p3_p4_length/parallel_step);

            p1_p2_length = parallel_step;

            temp_vector.push_back(QLineF(p1,p2));
        }
    }

    QVector<QLineF> centerLine_vector;

    for(int i = 0; i < centerPoint_vector.size() - 1; i++)
    {
        QLineF temp_line = QLineF(centerPoint_vector[i], centerPoint_vector[i+1]);
        centerLine_vector.push_back(temp_line);
    }

    painter.drawLines(temp_vector);

    painter.setPen(Qt::red);

    painter.drawLines(centerLine_vector);

    update();
}

void Canvas::draw_centerLine4(QPainter &painter)
{
    painter.setPen(Qt::green);

    QVector<QVector2D>  corner_line;

    QVector<QLineF> temp_vector;
    QVector<QPointF> centerPoint_vector;
    QVector<QPointF> p1_vec,p2_vec;

    // 琦角的线
    float tolal_length = 0;
    for (int i = 0; i < draw_stack[1].size() - 1; i+=2){

        QVector2D point = QVector2D(draw_stack[1][i], draw_stack[1][i+1]);

        corner_line.push_back(point);
    }

    for (int i = 0; i < corner_line.size(); i++) {
        int i_1 = (i+1)%corner_line.size();
        tolal_length += QVector2D(corner_line[i_1] - corner_line[i]).length();
    }

    // meth

    // 点
    QPointF p1, p2;

    std::cout<<"corner_line.size: "<<corner_line.size()<<std::endl;

    for (int i = 0; i < corner_line.size() / 2; i++) {

        if(i == 0)
        {

            p1 = QPointF(corner_line[0].x(), corner_line[0].y());

            p2 = QPointF(corner_line[corner_line.size() - 1].x(), corner_line[corner_line.size() - 1].y());

            p1_vec.push_back(p1);

            temp_vector.push_back(QLineF(p1,p2));

            centerPoint_vector.push_back(QPointF(p1 + p2) / 2);

            continue;
        }

       if(i % 5 == 0)
           p1_vec.push_back(QVector2D(corner_line[i]).toPointF());

    }

    std::cout<<"p1_vec.size: "<<p1_vec.size()<<std::endl;

    for (int i = corner_line.size()  - 1; i > corner_line.size() / 2; i-=5) {

        p2_vec.push_back(QVector2D(corner_line[i]).toPointF());

    }

    std::cout<<"p2_vec.size: "<<p2_vec.size()<<std::endl;

    // 线
    QVector<QVector<QPointF>> front_vec,rear_vec;

    for(int i = 0; i < corner_line.size() / 2; i+=5)
    {
        QVector<QPointF> temp_vec;
        for(int j = 0; j < 10; j++)
        {
            temp_vec.push_back(QVector2D(corner_line[i + j]).toPointF());
        }
        front_vec.push_back(temp_vec);
    }

    for(int j = corner_line.size() - 1; j > corner_line.size() / 2; j-=5)
    {
        QVector<QPointF> temp_vec;
        for(int i = 0; i < 10; i++)
        {
            temp_vec.push_back(QVector2D(corner_line[j - i]).toPointF());
        }
        rear_vec.push_back(temp_vec);
    }

//    std::cout<<"rear_vec.size: "<<rear_vec.size()<<std::endl;

    QVector<QPointF>::iterator it_p1 = p1_vec.begin();
    QVector<QPointF>::iterator it_p2 = p2_vec.begin();

    QVector<QVector<QPointF>>::iterator it_first = front_vec.begin();
    QVector<QVector<QPointF>>::iterator it_end = rear_vec.begin();

    // 求最短长度
    for(int i = 0; i < p1_vec.size(); i++)
    {
        QPointF p1 = QPointF(it_p1->x(), it_p1->y());
        QPointF p2 = QPointF(it_p2->x(), it_p2->y());

        QVector<QPointF> p1_vec;
        QVector<QPointF> p2_vec;

        if(i != 0)
        {
            it_p1 += 1;
            it_p2 += 1;
            it_first += 1;
            it_end += 1;
        }

        p1 = QPointF(it_p1->x(), it_p1->y());
        p2 = QPointF(it_p2->x(), it_p2->y());

        for(int j = 0; j < it_first->size(); j++)
        {
            p1_vec.push_back(it_first->data()[j]);
        }

        for(int j = 0; j < it_end->size(); j++)
        {
            p2_vec.push_back(it_end->data()[j]);
        }

        QMap<float, QPointF> map_1;
        QMap<float, QPointF> map_2;
        for(auto it: p2_vec)
        {
            map_2.insert(QVector2D(QPointF(it) - p1).length(), QPointF(it));
        }

//        p2 = map_2.first();

//        temp_vector.push_back(QLineF(p1,p2));

        for(auto it: p1_vec)
        {
            map_1.insert(QVector2D(QPointF(it) - p2).length(), QPointF(it));
        }

        p1 = map_1.first();

        p2 = map_2.first();

        temp_vector.push_back(QLineF(p1,p2));

        centerPoint_vector.push_back(QPointF(p1 + p2) / 2);
    }

    QVector<QLineF> centerLine_vector;

    for(int i = 0; i < centerPoint_vector.size() - 1; i++)
    {
        QLineF temp_line = QLineF(centerPoint_vector[i], centerPoint_vector[i+1]);
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
