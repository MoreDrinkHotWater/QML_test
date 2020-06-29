/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "window.h"
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>

#include <QDebug>
#include <iostream>

#include <fstream>
#include <string>

#include <QVector>

#include "ReadFile.h"
#include "glwidget.h"

MainWindow::MainWindow():
    readFile(new ReadFile()),
    glwidget(new GLWidget())
{
    QMenuBar *menuBar = new QMenuBar;
    QMenu *menuWindow = menuBar->addMenu(tr("&Window"));


    QAction *addNew = new QAction(menuWindow);
    addNew->setText(tr("Add new"));
    menuWindow->addAction(addNew);
    connect(addNew, &QAction::triggered, this, &MainWindow::onAddNew);

    QAction *readFileAction = new QAction(menuWindow);
    readFileAction->setText(tr("Read file"));
    menuWindow->addAction(readFileAction);

    connect(readFileAction, &QAction::triggered, this, &MainWindow::tempSlot);

    setMenuBar(menuBar);

    onAddNew();
}

void MainWindow::onAddNew()
{
    if (!centralWidget())
        setCentralWidget(new Window(this));
    else
        QMessageBox::information(0, tr("Cannot add new window"), tr("Already occupied. Undock first."));
}

void MainWindow::tempSlot()
{
    QString filePath =
            QFileDialog::getOpenFileName(this, "Select File", "",
                                         "stl(*.stl)");

    qDebug() << filePath;

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "warning", "Can't open file!");
        return ;
    }

    QString lineString;
    QTextStream in(&file);

    QString headStr = in.readLine();

    if (headStr == "")
        return;

    // ReadASCII
    if (headStr[0] == 's')
    {
        qDebug() <<"ReadASCII";
    }

    // ReadBinary
    else
    {
        qDebug() <<"ReadBinary";

//        QVector<GLfloat> normal;
//        QVector<GLfloat> coorX;
//        QVector<GLfloat> coorY;
//        QVector<GLfloat> coorZ;

        std::ifstream in(filePath.toStdString(), std::ifstream::in | std::ifstream::binary);

        char str[80];

        in.read(str, 80);

        //number of triangles
        int triangles;
        in.read((char*)&triangles, sizeof(int));

        if (triangles == 0)
            return;

        for (int i = 0; i < triangles; i++)
        {
            float coorXYZ[12];
            in.read((char*)coorXYZ, 12 * sizeof(float));

            for (int j = 1; j < 4; j++)
            {
                // 顶点
                temp.push_back(coorXYZ[j * 3]);
                temp.push_back(coorXYZ[j * 3 + 1]);
                temp.push_back(coorXYZ[j * 3 + 2]);

                // 法向量
                for(int j = 0; j < 3; j++)
                {
                    temp.push_back(coorXYZ[j]);
                }
            }

//            if(i==0)
//            {
//                std::cout<< "temp size: "<< temp.size() << std::endl;
//            }

            in.read((char*)coorXYZ, 2);

        }

        std::cout << "the triangles size: "<< triangles <<std::endl;

        connect(this, &MainWindow::emitVectorDataSignal, glwidget, &GLWidget::reviceVectorDataSlot);


//        for(int i = 0; i < normal.size(); i++)
//        {
//            temp_vector.append(normal[i]);
//            temp_vector.append(coorX[i]);
//            temp_vector.append(coorY[i]);
//            temp_vector.append(coorZ[i]);
//        }

//        for (int i = 0; i<12; i++) {
//            std::cout<< temp[i]<<std::endl;
//        }

        std::cout << temp.size() <<std::endl;

        emit emitVectorDataSignal(temp);

        in.close();

    }

    file.close();
}

