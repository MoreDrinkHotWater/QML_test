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

    void cup_clicked();

    void saveFile();

    void quitApp();

private:
    Ui::MainWindow *ui;

    QMenuBar *pmeunBar;

    QAction *saveAction;
    QAction *exitAction;

    QAction *cupAction;

    void initActions();
    void initMenu();
};
#endif // MAINWINDOW_H
