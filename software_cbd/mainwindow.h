#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // 菜单、工具和状态栏#
    QMenuBar *pMenuBar;
    QToolBar *pToolBar;
    QStatusBar *pStatusBar;

    // 对于文件的处理动作
    QAction *openFileAction, *saveFileAction, *closeCurrentFileAction, *quitAppActionAction;

    // 新建文件的动作
    QAction *newfileAction;

private:

    void initAction(); // 初始化动作的函数

    void initMenu(); // 初始化菜单栏的函数

    void initToolBar(); // 初始化工具栏的函数

    void initStatusBar(); // 初始化状态栏的函数

protected:
    void closeEvent(QCloseEvent *e) override; // 关闭事件的函数

private Q_SLOTS:
    void openFile();

    void saveFile();

    void closeCurrentFile();

    void quitApp();

};
#endif // MAINWINDOW_H
