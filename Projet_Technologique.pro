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

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

DISTFILES += \
    ../espaces/travail/Année/L3/Projet Technologique/Projet-Technologique-L3/Semestre 1/STEREO/STEREO/____stereoscopic_3d_by_osipenkov-d40zsws.jpg

RESOURCES += image1.qrc

LIBS += -lopencv_core -lopencv_imgproc