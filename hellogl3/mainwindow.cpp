#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"
#include "recognize_cup.h"
#include "recognize_desklamp.h"
#include "recognize_stool.h"

#include <QPainter>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <cassert>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      pmeunBar(menuBar()),

      saveCanvasAction(new QAction(this)),
      readCanvasAction(new QAction(this)),
      saveAction(new QAction(this)),
      exitAction(new QAction(this)),

      cupAction(new QAction(this)),
      deskLampAction(new QAction(this)),
      stoolAction(new QAction(this)),

      lineAction(new QAction(this)),
      triangleAction(new QAction(this)),
      rectAction(new QAction(this)),
      circleAction(new QAction(this))
{
    ui->setupUi(this);

    initActions();
    initMenu();

    recognizeCup = Recognize_cup::getInstance();

    recognizeDeskLamp = Recognize_deskLamp::getInstance();

    recognizeStool = Recognize_stool::getInstance();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initActions()
{
    saveCanvasAction->setObjectName(QString::fromUtf8("saveCanvasAction"));
    saveCanvasAction->setText("&SaveCanvas");
    connect(saveCanvasAction, &QAction::triggered, this, &MainWindow::saveCanvas);

    readCanvasAction->setObjectName(QString::fromUtf8("readCanvasAction"));
    readCanvasAction->setText("&ReadCanvas");
    connect(readCanvasAction, &QAction::triggered, this, &MainWindow::readCanvas);

    saveAction->setObjectName(QString::fromUtf8("saveAction"));
    saveAction->setText("&Save");
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    exitAction->setObjectName(QString::fromUtf8("exitAction"));
    exitAction->setText("&Exit");
    connect(exitAction, &QAction::triggered, this, &MainWindow::quitApp);

    cupAction->setObjectName(QString::fromUtf8("cupAction"));
    cupAction->setText("Cup");
    connect(cupAction, &QAction::triggered, this, &MainWindow::cup_clicked);

    deskLampAction->setObjectName(QString::fromUtf8("deskLampAction"));
    deskLampAction->setText("DeskLamp");
    connect(deskLampAction, &QAction::triggered, this, &MainWindow::deskLamp_clicked);

    stoolAction->setObjectName(QString::fromUtf8("stoolAction"));
    stoolAction->setText("Stool");
    connect(stoolAction, &QAction::triggered, this, &MainWindow::stool_clicked);

    lineAction->setObjectName(QString::fromUtf8("lineAction"));
    lineAction->setText("Line");
    connect(lineAction, &QAction::triggered, this, &MainWindow::line_clicked);

    triangleAction->setObjectName(QString::fromUtf8("triangleAction"));
    triangleAction->setText("Triangle");
    connect(triangleAction, &QAction::triggered, this, &MainWindow::triangle_clicked);

    rectAction->setObjectName(QString::fromUtf8("rectAction"));
    rectAction->setText("Rect");
    connect(rectAction, &QAction::triggered, this, &MainWindow::rect_clicked);

    circleAction->setObjectName(QString::fromUtf8("circleAction"));
    circleAction->setText("Circle");
    connect(circleAction, &QAction::triggered, this, &MainWindow::circle_clicked);
}

void MainWindow::initMenu()
{
    QMenu *menu;

    menu = new QMenu("&File", this);
    menu->addAction(saveCanvasAction);
    menu->addSeparator();
    menu->addAction(readCanvasAction);
    menu->addSeparator();
    menu->addAction(saveAction);
    menu->addSeparator();
    menu->addAction(exitAction);
    pmeunBar->addMenu(menu);

    menu = new QMenu("&Kind", this);
    QMenu *LifeGoodsMenu = new QMenu("&LifeGoods", this);
    LifeGoodsMenu->addAction(cupAction);
    LifeGoodsMenu->addSeparator();
    LifeGoodsMenu->addAction(deskLampAction);
    LifeGoodsMenu->addSeparator();
    LifeGoodsMenu->addAction(stoolAction);
    menu->addMenu(LifeGoodsMenu);
    pmeunBar->addMenu(menu);

    menu = new QMenu("&Polygon", this);
    menu->addAction(lineAction);
    menu->addSeparator();
    menu->addAction(triangleAction);
    menu->addSeparator();
    menu->addAction(rectAction);
    menu->addSeparator();
    menu->addAction(circleAction);
    pmeunBar->addMenu(menu);
}

// apply
void MainWindow::on_applyButton_clicked()
{
    ui->glwidget->glWidget->reviceStackDataSlot(ui->canvas->draw_stack);
}

void MainWindow::on_clearButton_clicked()
{
    ui->canvas->draw_stack.clear();

    ui->glwidget->glWidget->cylinder_vector.clear();

    ui->glwidget->glWidget->update();
}

// 保存画布
void MainWindow::saveCanvas()
{
    // 选择文件的保存路径
    QString filename;

    QFileDialog filedialog;
    filedialog.setAcceptMode(QFileDialog::AcceptOpen);
    filedialog.setViewMode(QFileDialog::List);
    // 选择文件夹
    filedialog.setFileMode(QFileDialog::DirectoryOnly);
    filedialog.setWindowTitle(tr("标题"));
    filedialog.setDefaultSuffix("txt");
    filedialog.setOption(QFileDialog::DontUseNativeDialog);
    if(filedialog.exec()==  QDialog::Accepted )
    {
        QStringList filePaths = filedialog.selectedFiles();
        filename =filePaths[0] + "/";
        qDebug() << filename;
    }

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy_MM_dd_hh.mm.ss");

    QString filePath = filename + current_date + ".txt";

    std::ofstream fout(filePath.toStdString());

    if ( fout ) { // 如果创建成功bai

        for(int i = 0; i < ui->canvas->draw_stack.size(); i++)
        {
            for(int j = 0; j < ui->canvas->draw_stack[i].size() - 1; j+=2)
                fout << ui->canvas->draw_stack[i][j] << " " << ui->canvas->draw_stack[i][j+1]<<std::endl;

            fout << (i+1) * 10000<<std::endl;
        }

        std::cout<<"成功保存Canvas"<<std::endl;

        fout.close();
    }

}

// 读取画布
void MainWindow::readCanvas()
{

    // 选择文件的保存路径
    QString filename;

    QFileDialog filedialog;
    filedialog.setAcceptMode(QFileDialog::AcceptOpen);
    filedialog.setViewMode(QFileDialog::List);
    // 选择文件夹
    filedialog.setFileMode(QFileDialog::AnyFile);
    filedialog.setWindowTitle(tr("标题"));
    filedialog.setDefaultSuffix("txt");
    filedialog.setOption(QFileDialog::DontUseNativeDialog);
    if(filedialog.exec()==  QDialog::Accepted )
    {
        QStringList filePaths = filedialog.selectedFiles();
        filename =filePaths[0];
        qDebug() << filename;
    }

    QVector<float> point;
    QStack<QVector<float>> draw_canvas_stack;
    std::ifstream infile;
    infile.open(filename.toStdString().data());   //将文件流对象与文件连接起来
    assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行

    float d;

    while (infile >> d)
    {
        if(d >= 10000 && !qIsNaN(d))
        {
            draw_canvas_stack.push_back(point);
            point.clear();
        }
        else
            point.push_back(d);//将数据压入堆栈。
    }

    infile.close();

    std::cout<<"draw_canvas_stack.size: "<<draw_canvas_stack.size()<<std::endl;

    ui->canvas->draw_canvas_stack = draw_canvas_stack;

}

// 保存为 stl 文件
void MainWindow::saveFile()
{

    std::cout<<"==================save=============="<<std::endl;

    // 选择文件的保存路径
    QString filename;

    QFileDialog filedialog;
    filedialog.setAcceptMode(QFileDialog::AcceptOpen);
    filedialog.setViewMode(QFileDialog::List);
    // 选择文件夹
    filedialog.setFileMode(QFileDialog::DirectoryOnly);
    filedialog.setWindowTitle(tr("标题"));
    filedialog.setDefaultSuffix("txt");
    filedialog.setOption(QFileDialog::DontUseNativeDialog);
    if(filedialog.exec()==  QDialog::Accepted )
    {
        QStringList filePaths = filedialog.selectedFiles();
        filename =filePaths[0] + "/";
        qDebug() << filename;
    }

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy_MM_dd_hh.mm.ss");

    QString filePath = filename + current_date + ".stl";

    std::ofstream fout(filePath.toStdString());
    if ( fout ) { // 如果创建成功bai
        fout << "solid " + current_date.toStdString()  + ".stl"<< std::endl; // 使用与cout同样的方式进行du写入

        QVector<float> coorXYZ;

        QVector<float> Normalvector;
        QVector<float> Vertex1;
        QVector<float> Vertex2;
        QVector<float> Vertex3;

        std::cout<<"cylinder_vector.size: "<<ui->glwidget->glWidget->cylinder_vector.size()<<std::endl;

        for(int i = 0; i < 18; i++)
            std::cout<<"ui->glwidget->glWidget->cylinder_vector: "<<ui->glwidget->glWidget->cylinder_vector[i]<<std::endl;

        // 获得用户画好的模型数据
        for(int i = 0; i < ui->glwidget->glWidget->cylinder_vector.size(); i++)
        {
            coorXYZ.push_back(ui->glwidget->glWidget->cylinder_vector[i]);

            if(coorXYZ.size() == 18)
            {

                for (int j = 3; j < 6; j++)
                    Normalvector.push_back(coorXYZ[j]);
                for (int j = 0; j < 3;j++)
                    Vertex1.push_back(coorXYZ[j]);
                for (int j = 6; j < 9;j++)
                    Vertex2.push_back(coorXYZ[j]);
                for (int j = 12; j < 15;j++)
                    Vertex3.push_back(coorXYZ[j]);

                auto Normalvector_string = QString("%1").arg((float)Normalvector[0]) + " " + QString("%1").arg((float)Normalvector[1]) + " " + QString("%1").arg((float)Normalvector[2]);

                auto Vertex1_string = QString("%1").arg(Vertex1[0]) + " " + QString("%1").arg(Vertex1[1]) + " " + QString("%1").arg(Vertex1[2]);

                auto Vertex2_string = QString("%1").arg(Vertex2[0]) + " " + QString("%1").arg(Vertex2[1]) + " " + QString("%1").arg(Vertex2[2]);

                auto Vertex3_string = QString("%1").arg(Vertex3[0]) + " " + QString("%1").arg(Vertex3[1]) + " " + QString("%1").arg(Vertex3[2]);

                fout << "facet normal " + Normalvector_string.toStdString() << std::endl;

                fout << "  outer loop"<< std::endl;

                // 3顶点沿指向实体外部的法矢量方向逆时针排列。

                fout << "    vertex " + Vertex1_string.toStdString() << std::endl;

                fout << "    vertex " + Vertex2_string.toStdString() << std::endl;

                fout << "    vertex " + Vertex3_string.toStdString() << std::endl;

                fout << "  endloop"<< std::endl;

                fout << "endfacet"<< std::endl;

                Normalvector.clear();
                Vertex1.clear();
                Vertex2.clear();
                Vertex3.clear();

                coorXYZ.clear();
            }
        }

        fout << "endsolid " + current_date.toStdString() + ".stl"<< std::endl; // 使用与cout同样的方式进行du写入

        std::cout<<"成功生成stl文件"<<std::endl;

        fout.close(); // 执行完操作zhi后关闭文件句柄
    }

    // 保存数据为STL文件类型

}

void MainWindow::quitApp()
{
    QApplication::quit();
}

// cup
void MainWindow::cup_clicked()
{
    if(recognizeCup->recognize_cup(ui->canvas->draw_stack))
        ui->glwidget->glWidget->draw_cup();
}

// deskLamp
void MainWindow::deskLamp_clicked()
{
    if(recognizeDeskLamp->recognize_deskLamp(ui->canvas->draw_stack))
        ui->glwidget->glWidget->draw_deskLamp();
}

// stool
void MainWindow::stool_clicked()
{
    if(recognizeStool->recognize_stool(ui->canvas->draw_stack))
        ui->glwidget->glWidget->draw_stool();
}

void MainWindow::line_clicked()
{
    ui->glwidget->glWidget->draw_line(ui->canvas->draw_stack[ui->canvas->draw_stack.size() - 1]);
}

void MainWindow::triangle_clicked()
{
    std::cout<<"===============triangle==========="<<std::endl;
}

void MainWindow::rect_clicked()
{
    std::cout<<"===============rect==========="<<std::endl;
}

void MainWindow::circle_clicked()
{
    ui->glwidget->glWidget->draw_circle(ui->canvas->draw_stack[ui->canvas->draw_stack.size() - 1]);
}
