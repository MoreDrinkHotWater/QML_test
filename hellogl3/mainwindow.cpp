#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"

#include <QPainter>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <iostream>

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

    // 获得用户画好的模型数据
    ui->glwidget->glWidget->cylinder_vector;

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
        filename =filePaths[0];
        qDebug() << filename;
    }
}

void MainWindow::quitApp() {
    QApplication::quit();
}

// cup
void MainWindow::cup_clicked()
{
    ui->glwidget->glWidget->Recognize_cup(ui->canvas->draw_stack);
}
