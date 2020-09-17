#include "setdiff.h"
#include "ui_setdiff.h"

setDiff::setDiff(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setDiff)
{
    ui->setupUi(this);
}

setDiff::~setDiff()
{
    delete ui;
}

void setDiff::on_OK_clicked()
{
    float diff = ui->setDiffEdit->text().toFloat();

    emit send_Diff(diff);

    close();
}
