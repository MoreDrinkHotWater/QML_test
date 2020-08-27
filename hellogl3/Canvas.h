#ifndef CANVAS_H
#define CANVAS_H

#include <QObject>
#include <QWidget>
#include <QPaintEvent>
#include <QStack>

#include <QPainter>

#include <QPushButton>



class Canvas : public QWidget
{

    Q_OBJECT

public:
     Canvas(QWidget *parent);

     virtual ~Canvas() {};


private:
    QPointF m_lastPos;

    void drawPoint(QPainter &painter);

    void drawLines(QPainter &painter);

    void draw_centerLine(QPainter &painter);

    void draw_centerLine2(QPainter &painter);

    void draw_centerLine3(QPainter &painter);

    void draw_centerLine4(QPainter &painter);

public:
    QVector<float> draw_lines_vector;

    QStack<QVector<float>> draw_stack;

    QVector<float> draw_vector;

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void paintEvent(QPaintEvent *event) override;




};

#endif // CANVAS_H
