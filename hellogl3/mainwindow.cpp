#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"

#include <QPainter>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      pmeunBar(menuBar()),

      saveAction(new QAction(this)),
      exitAction(new QAction(this)),
      cupAction(new QAction(this))
{
    ui->setupUi(this);

    initActions();
    initMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initActions()
{
    saveAction->setObjectName(QString::fromUtf8("saveAction"));
    saveAction->setText("&Save");
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    exitAction->setObjectName(QString::fromUtf8("exitAction"));
    exitAction->setText("&Exit");
    connect(exitAction, &QAction::triggered, this, &MainWindow::quitApp);

    cupAction->setObjectName(QString::fromUtf8("cupAction"));
    cupAction->setText("Cup");
    connect(cupAction, &QAction::triggered, this, &MainWindow::cup_clicked);
}

void MainWindow::initMenu()
{
    QMenu *menu;

    menu = new QMenu("&File", this);
    menu->addAction(saveAction);
    menu->addAction(exitAction);
    menu->addSeparator();

    pmeunBar->addMenu(menu);

    menu = new QMenu("&Kind", this);
    QMenu *LifeGoodsMenu = new QMenu("&LifeGoods", this);
    LifeGoodsMenu->addAction(cupAction);
    menu->addMenu(LifeGoodsMenu);

    pmeunBar->addMenu(menu);
}

void MainWindow::on_pushButton_clicked()
{

    ui->glwidget->glWidget->reviceStackDataSlot(ui->canvas->draw_stack);

    // 用于 test
    //    ui->canvas->draw_stack.clear();

}

void MainWindow::saveFile() {

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
        fout << "solid " + current_date.toStdString() + " stl" << std::endl; // 使用与cout同样的方式进行du写入

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

                auto Normalvector_string = QString("%1").arg(Normalvector[0]) + " " + QString("%1").arg(Normalvector[1]) + " " + QString("%1").arg(Normalvector[2]);

                auto Vertex1_string = QString("%1").arg(Vertex1[0]) + " " + QString("%1").arg(Vertex1[1]) + " " + QString("%1").arg(Vertex1[2]);

                auto Vertex2_string = QString("%1").arg(Vertex2[0]) + " " + QString("%1").arg(Vertex2[1]) + " " + QString("%1").arg(Vertex2[2]);

                auto Vertex3_string = QString("%1").arg(Vertex3[0]) + " " + QString("%1").arg(Vertex3[1]) + " " + QString("%1").arg(Vertex3[2]);

                fout << "facet normal " + Normalvector_string.toStdString() << std::endl;

                fout << "outor loop"<< std::endl;

                fout << "Vertex " + Vertex1_string.toStdString() << std::endl;

                fout << "Vertex " + Vertex2_string.toStdString() << std::endl;

                fout << "Vertex " + Vertex3_string.toStdString() << std::endl;

                fout << "endloop"<< std::endl;

                fout << "endfacet"<< std::endl;

                coorXYZ.clear();
            }
        }
        fout.close(); // 执行完操作zhi后关闭文件句柄
    }

    // 保存数据为STL文件类型

}

void MainWindow::quitApp() {
    QApplication::quit();
}

// cup
void MainWindow::cup_clicked()
{
    ui->glwidget->glWidget->Recognize_cup(ui->canvas->draw_stack);
}
