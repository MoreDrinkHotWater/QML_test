#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"

#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    ui->glwidget->glWidget->reviceStackDataSlot(ui->canvas->draw_stack);

    // 用于 test
//    ui->canvas->draw_stack.clear();

}


// cup
void MainWindow::cup_clicked()
{
    ui->glwidget->glWidget->Recognize_cup(ui->canvas->draw_stack);
}
