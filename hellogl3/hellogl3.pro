QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

LIBS += -lGL -lglut

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Canvas.cpp \
    ExtrudeProperty_dialog.cpp \
    Lineproperty_dialog.cpp \
    common.cpp \
    gen_model.cpp \
    glwidget.cpp \
    identification_relation.cpp \
    identification_type.cpp \
    logo.cpp \
    main.cpp \
    mainwindow.cpp \
    recognize_cup.cpp \
    recognize_desklamp.cpp \
    recognize_stool.cpp \
    recognizecorner.cpp \
    recognizecube.cpp \
    recognizecylinder.cpp \
    window.cpp

HEADERS += \
    Canvas.h \
    ExtrudeProperty_dialog.h \
    Lineproperty_dialog.h \
    common.h \
    gen_model.h \
    glwidget.h \
    identification_relation.h \
    identification_type.h \
    logo.h \
    mainwindow.h \
    recognize_cup.h \
    recognize_desklamp.h \
    recognize_stool.h \
    recognizecorner.h \
    recognizecube.h \
    recognizecylinder.h \
    window.h

FORMS += \
    ExtrudeProperty_dialog.ui \
    Lineproperty_dialog.ui \
    mainwindow.ui

# link opengl libray
LIBS += -lglut -lGL -lGLU -lGLEW

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
