#include "choosedistancetime.h"
#include "ui_choosedistancetime.h"

chooseDistanceTime::chooseDistanceTime(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chooseDistanceTime)
{
    ui->setupUi(this);
}

chooseDistanceTime::~chooseDistanceTime()
{
    delete ui;
}

void chooseDistanceTime::on_OK_clicked()
{
    int distanceTime = ui->distanceTime->text().toInt();
    emit send_distanceTime(distanceTime);
    close();
}
