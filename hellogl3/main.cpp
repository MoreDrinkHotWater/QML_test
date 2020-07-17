#include "mainwindow.h"

#include "glwidget.h"
#include <QApplication>

#include <QSurfaceFormat>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "Canvas.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QCommandLineParser parser;
//    parser.setApplicationDescription(QCoreApplication::applicationName());
//    parser.addHelpOption();
//    parser.addVersionOption();

//    QCommandLineOption multipleSampleOption("multisample", "Multisampling");
//    parser.addOption(multipleSampleOption);
//    QCommandLineOption coreProfileOption("coreprofile", "Use core profile");
//    parser.addOption(coreProfileOption);
//    QCommandLineOption transparentOption("transparent", "Transparent window");
//    parser.addOption(transparentOption);

//    parser.process(a);

//    QSurfaceFormat fmt;
//    fmt.setDepthBufferSize(24);
//    if (parser.isSet(multipleSampleOption))
//        fmt.setSamples(4);
//    if (parser.isSet(coreProfileOption)) {
//        fmt.setVersion(3, 2);
//        fmt.setProfile(QSurfaceFormat::CoreProfile);
//    }
//    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow w;
    w.show();
    return a.exec();
}
