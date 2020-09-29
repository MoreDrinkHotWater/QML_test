#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void saveCanvas();

    void readCanvas();

    void saveFile();

    void quitApp();

    void cup_clicked();

    void deskLamp_clicked();

private:
    Ui::MainWindow *ui;

    QMenuBar *pmeunBar;

    // 保存画布信息
    QAction *saveCanvasAction;
    // 读取画布信息
    QAction *readCanvasAction;

    QAction *saveAction;
    QAction *exitAction;

    QAction *cupAction;
    QAction *deskLampAction;

    void initActions();
    void initMenu();
};
#endif // MAINWINDOW_H
