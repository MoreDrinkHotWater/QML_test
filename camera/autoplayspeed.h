#ifndef AUTOPLAYSPEED_H
#define AUTOPLAYSPEED_H

#include <QWidget>

namespace Ui {
class autoplaySpeed;
}

class autoplaySpeed : public QWidget
{
    Q_OBJECT

public:
    explicit autoplaySpeed(QWidget *parent = nullptr);
    ~autoplaySpeed();

signals:
    void seed_speed(int speed);

private slots:
    void on_OK_clicked();

private:
    Ui::autoplaySpeed *ui;
};

#endif // AUTOPLAYSPEED_H
