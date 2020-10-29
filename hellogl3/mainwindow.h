#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "glwidget.h"

#include <QMainWindow>
#include <QThread>
#include <QVector3D>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Recognize_cup;

class Recognize_deskLamp;

class Recognize_stool;

class Common;

class Draw_bezier;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_applyButton_clicked();
    void on_clearButton_clicked();

    void saveCanvas();
    void readCanvas();
    void saveFile();
    void quitApp();

    void cup_clicked();
    void deskLamp_clicked();
    void stool_clicked();

    void line_clicked();
    void extrude_clicked();
    void circle_clicked();

private:
    Ui::MainWindow *ui;

    QMenuBar *pmeunBar;

    QAction *saveCanvasAction;
    QAction *readCanvasAction;
    QAction *saveAction;
    QAction *exitAction;

    QAction *cupAction;
    QAction *deskLampAction;
    QAction *stoolAction;

    // polygon
    QAction *lineAction;
    QAction *extrudeAction;
    QAction *circleAction;

    void initActions();
    void initMenu();

private:
    Recognize_cup *recognizeCup;

    Recognize_deskLamp *recognizeDeskLamp;

    Recognize_stool *recognizeStool;

    Common *common;

    Draw_bezier *draw_bezier;

    QThread draw_bezierThread;

signals:

    void send_bezierSignal(QStack<QVector<QVector3D>> draw_stack);

private slots:
    void receive_ExtrudeProperty(float width_var, float up_var, float down_var);

    void receive_LineProperty(float width_var);
    void on_BezierButton_clicked();
};
#endif // MAINWINDOW_H
