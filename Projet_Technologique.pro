#-------------------------------------------------
#
# Project created by QtCreator 2018-09-21T15:41:11
#
#-------------------------------------------------

QT       += core gui

# add open CV
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}
TARGET = Projet_Technologique
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += opencv

SOURCES += main.cpp\
        mainwindow.cpp \
    disparity.cpp \
    imagecv.cpp \
    calibration_widget.cpp \
    calibration_intr.cpp

HEADERS  += mainwindow.h \
    disparity.h \
    imagecv.h \
    calibration_widget.h \
    calibration_intr.h

FORMS    += mainwindow.ui \
    disparity.ui \
    calibration_widget.ui

RESOURCES += \
    image.qrc

