#ifndef CANVAS_H
#define CANVAS_H

#include <QObject>
#include <QWidget>
#include <QPaintEvent>
#include <QStack>

#include <QPushButton>

class GLWidget;

class Canvas : public QWidget
{

    Q_OBJECT

public:
     Canvas(QWidget *parent);

     virtual ~Canvas() {};


private:
    QPoint m_lastPos;

    void drawPoint(QPainter &painter);

    void drawLines(QPainter &painter);

    GLWidget *glwidget;

public:
    QVector<float> draw_lines_vector;

    QStack<QVector<float>> draw_stack;

    QVector<float> draw_vector;

    QPushButton *button;

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void paintEvent(QPaintEvent *event) override;

    void send_dataSlot();

signals:
    void send_dataSignal(QVector<float> draw_vector);

};

#endif // CANVAS_H
