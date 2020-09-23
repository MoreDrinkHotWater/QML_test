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

    void drawRandomPoints(QPainter &painter);

public:
    QVector<float> draw_lines_vector;

    QStack<QVector<float>> draw_stack;

    QVector<float> draw_vector;

    QVector<QPoint> randomPoint_vector;

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void paintEvent(QPaintEvent *event) override;

    void receive_randomPoint_vector(QVector<QPoint> randomPoint_vector);
};

#endif // CANVAS_H
