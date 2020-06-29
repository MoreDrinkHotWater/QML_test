#include "ReadFile.h"
#include <QFileDialog>
#include <QDebug>

#include <iostream>
#include <QThread>

void ReadFile::startReadSlot(QMainWindow *mainWindow)
{

    std::cout << "ReadFile thread: " << QThread::currentThreadId() << std::endl;

    QString fileName =
            QFileDialog::getOpenFileName(mainWindow, "Select File", "",
                                         "stl(*.stl)");

    qDebug() << "==============";

    qDebug() << fileName;

//    if (fileName.isEmpty()) {
//        return;
//    }

}
