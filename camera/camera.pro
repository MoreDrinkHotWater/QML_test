TEMPLATE = app
TARGET = camera

QT += multimedia multimediawidgets

HEADERS = \
    camera.h \
    choosedistancetime.h \
    imagesettings.h \
    setdiff.h \
    videosettings.h

SOURCES = \
    choosedistancetime.cpp \
    main.cpp \
    camera.cpp \
    imagesettings.cpp \
    setdiff.cpp \
    videosettings.cpp

FORMS += \
    camera.ui \
    choosedistancetime.ui \
    setdiff.ui \
    videosettings.ui \
    imagesettings.ui

RESOURCES += camera.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/multimediawidgets/camera
INSTALLS += target

QT+=widgets
include(../../multimedia/shared/shared.pri)
