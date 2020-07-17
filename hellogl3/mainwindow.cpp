#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    connect(ui->canvas, &Canvas::send_dataSignal, ui->glwidget, &GLWidget::reviceVectorDataSlot);

//    connect(ui->canvas, SIGNAL(send_dataSignal), ui->glwidget, SLOT(reviceVectorDataSlot));

}

MainWindow::~MainWindow()
{
    delete ui;
}



