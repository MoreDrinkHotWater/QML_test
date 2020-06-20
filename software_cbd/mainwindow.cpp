#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QCloseEvent>

#include <QMenu>
#include <QToolBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      pMenuBar(menuBar()),
      pToolBar(addToolBar("Tools")),
      pStatusBar(statusBar()),

      openFileAction(new QAction(this)),
      saveFileAction(new QAction(this)),
      closeCurrentFileAction(new QAction(this)),
      quitAppActionAction(new QAction(this))
{
    ui->setupUi(this);

    initAction();
    initMenu();
    initToolBar();
    initStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initAction() {
    //设置菜单栏图片的链接
//     openFileAction->setIcon(QIcon::fromTheme("document-open", QIcon(":/images/open.jpg")));

    openFileAction->setText("&Open..."); //？？？
    //设置快捷键
    openFileAction->setShortcut(QKeySequence::Open);
    //设置状态栏的提示信息
    openFileAction->setStatusTip("Open File");
    //信号槽：单击图片->启动open()函数
    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);

//     saveFileAction->setIcon(QIcon::fromTheme("document-save", QIcon(":/images/file_save.png")));
    saveFileAction->setText("&Save");
    saveFileAction->setShortcut(QKeySequence::Save);
    saveFileAction->setStatusTip("Save File");
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::saveFile);

//    closeCurrentFileAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/images/file_close.png")));
    closeCurrentFileAction->setText("&CloseCurrentFile");
    closeCurrentFileAction->setShortcut(QKeySequence::Close);
    closeCurrentFileAction->setStatusTip("Close Current File");
    connect(closeCurrentFileAction, &QAction::triggered, this, &MainWindow::closeCurrentFile);

//    quitAppActionAction->setIcon(QIcon::fromTheme("application-exit", QIcon(":/images/appExit.png")));
    quitAppActionAction->setText("&Quit");
    quitAppActionAction->setShortcut(QKeySequence::Quit);
    quitAppActionAction->setStatusTip("Quit");
    connect(quitAppActionAction, &QAction::triggered, this, &MainWindow::quitApp, Qt::QueuedConnection);
}

void MainWindow::initMenu() {

    QMenu *menu;
    menu = new QMenu("&File", this);

    menu->addAction(openFileAction);
    menu->addSeparator(); // 菜单中的横线分隔符号

    menu->addAction(closeCurrentFileAction);
    menu->addSeparator();

    menu->addAction(saveFileAction);
    menu->addSeparator();

    menu->addAction(quitAppActionAction);
    menu->addSeparator();

    pMenuBar->addMenu(menu);
}

void MainWindow::initToolBar() {
    pToolBar->addActions({openFileAction, saveFileAction, closeCurrentFileAction});
    pToolBar->addSeparator();

}

void MainWindow::initStatusBar() {

}

void MainWindow::closeEvent(QCloseEvent *e) {
    e->accept();
}

void MainWindow::openFile(){


}

void MainWindow::saveFile(){

}

void MainWindow::closeCurrentFile(){

}

void MainWindow::quitApp(){

}


