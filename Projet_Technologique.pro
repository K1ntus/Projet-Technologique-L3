#-------------------------------------------------
#
# Project created by QtCreator 2018-09-21T15:41:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# add open CV
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

# uncomment when at home
#CONFIG += c++11
#INCLUDEPATH += C:/Users/meryl/source/opencv-3.2/build/install/include

#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_core320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_highgui320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_imgcodecs320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_imgproc320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_features2d320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_calib3d320.dll


# uncomment for commoj project
INCLUDEPATH += C:/opencv/build/include/

LIBS += -LC:/opencv/build/bin -libopencv_core -libopencv_imgproc -libopencv_highgui

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
        widgets/mainwindow.cpp \
    widgets/disparity.cpp \
    tools/imagecv.cpp \
    widgets/calibration_widget.cpp \
    tools/calibration_intr.cpp \
    tools/imgcv.cpp

HEADERS  += widgets/mainwindow.h \
    widgets/disparity.h \
    tools/imagecv.h \
    widgets/calibration_widget.h \
    tools/calibration_intr.h \
    tools/imgcv.h

FORMS    += widgets/mainwindow.ui \
    widgets/disparity.ui \
    widgets/calibration_widget.ui

RESOURCES += \
    image.qrc

