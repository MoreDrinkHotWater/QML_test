#ifndef CHOOSEDISTANCETIME_H
#define CHOOSEDISTANCETIME_H

#include <QWidget>

namespace Ui {
class chooseDistanceTime;
}

class chooseDistanceTime : public QWidget
{
    Q_OBJECT

public:
    explicit chooseDistanceTime(QWidget *parent = nullptr);
    ~chooseDistanceTime();

signals:
    void send_distanceTime(int distancetime);

private slots:
    void on_OK_clicked();

private:
    Ui::chooseDistanceTime *ui;
};

#endif // CHOOSEDISTANCETIME_H
