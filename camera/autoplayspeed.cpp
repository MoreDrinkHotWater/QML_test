#include "autoplayspeed.h"
#include "ui_autoplayspeed.h"

autoplaySpeed::autoplaySpeed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::autoplaySpeed)
{
    ui->setupUi(this);
}

autoplaySpeed::~autoplaySpeed()
{
    delete ui;
}

void autoplaySpeed::on_OK_clicked()
{
    int speed = ui->speedspinBox->text().toInt();

    close();

    emit seed_speed(speed);
}
