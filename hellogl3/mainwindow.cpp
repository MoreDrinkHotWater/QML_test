#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glwidget.h"

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

//    QVector<float> draw_vector;

//    for(auto it = ui->canvas->draw_stack.begin(); it != ui->canvas->draw_stack.end(); ++it)
//    {
//        for (int i = 0; i < it->size(); i++) {

//            draw_vector.push_back(it->data()[i]);
//        }
//    }

//    ui->glwidget->glWidget->reviceVectorDataSlot(draw_vector);

    ui->glwidget->glWidget->reviceStackDataSlot(ui->canvas->draw_stack);

    // 用于 test
    ui->canvas->draw_stack.clear();

}
