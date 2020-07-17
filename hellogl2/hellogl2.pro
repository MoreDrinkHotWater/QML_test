HEADERS       = glwidget.h \
                recognizecube.h \
                window.h \
                mainwindow.h \
                logo.h
SOURCES       = glwidget.cpp \
                main.cpp \
                recognizecube.cpp \
                window.cpp \
                mainwindow.cpp \
                logo.cpp

QT           += widgets

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/hellogl2
INSTALLS += target

DISTFILES += \
    test_data/snake.stl

