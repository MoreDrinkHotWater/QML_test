#ifndef WIDGET_H
#define WIDGET_H

#include "Canvas.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:

    Widget(QWidget *parent = nullptr);
    ~Widget();

    QVector<QPoint> randomPoint_vector;

signals:
    void send_randomPoint_vector(QVector<QPoint> randomPoint_vector);

private slots:
    void on_generateButton_clicked();

    void on_cleanButton_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
