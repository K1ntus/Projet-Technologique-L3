#-------------------------------------------------
#
# Project created by QtCreator 2018-09-21T15:41:11
#
#-------------------------------------------------

QT      += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# add open CV
unix {
    message(" * Using Settings For Unix * ")
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

# uncomment when at home
#CONFIG += c++11

#INCLUDEPATH += `pkg-config --cflags opencv`
#LIBS += `pkg-config --libs opencv`

#INCLUDEPATH += C:/Users/meryl/source/opencv-3.2/build/install/include
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_core320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_highgui320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_imgcodecs320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_imgproc320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_features2d320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_calib3d320.dll
#LIBS += C:/Users/meryl/source/opencv-3.2/build/bin/libopencv_videoio320.dll

#INCLUDEPATH += /usr/local/lib64
#LIBS += `pkg-config --libs opencv`
#LIBS += -L/usr/local/lib64 -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_dnn_objdetect -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hdf -lopencv_hfs -lopencv_img_hash -lopencv_line_descriptor -lopencv_optflow -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_surface_matching -lopencv_tracking -lopencv_datasets -lopencv_text -lopencv_dnn -lopencv_plot -lopencv_xfeatures2d -lopencv_shape -lopencv_ml -lopencv_ximgproc -lopencv_xobjdetect -lopencv_objdetect -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_flann -lopencv_xphoto -lopencv_photo -lopencv_imgproc -lopencv_core

#LIBS += libopencv_core.so.3.4.5
#LIBS += libopencv_aruco.so
#LIBS += libopencv_highgui.so
#LIBS += libopencv_imgcodecs.so
#LIBS += libopencv_imgproc.so
#LIBS += libopencv_ximgproc.so
#LIBS += libopencv_features2d.so
#LIBS += libopencv_calib3d.so
#LIBS += libopencv_videoio.so

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
    tools/calibration/calibration_intr.cpp \
    tools/imgcv.cpp \
    tools/calibration/intrinsicparameters.cpp \
    tools/calibration/chessboardcalibration.cpp \
    tools/calibration/charucocalibration.cpp \
    tools/calibration/pt_stereocalibration.cpp \
    tools/calibration/pt_icalibration.cpp \
    network/server.cpp \
    network/tcp_thread.cpp

HEADERS  += widgets/mainwindow.h \
    widgets/disparity.h \
    tools/imagecv.h \
    widgets/calibration_widget.h \
    tools/calibration/calibration_intr.h \
    tools/imgcv.h \
    tools/calibration/intrinsicparameters.h \
    tools/calibration/chessboardcalibration.h \
    tools/calibration/charucocalibration.h \
    tools/calibration/pt_stereocalibration.h \
    tools/calibration/pt_icalibration.h \
    network/server.h \
    network/tcp_thread.h

FORMS    += widgets/mainwindow.ui \
    widgets/disparity.ui \
    widgets/calibration_widget.ui

RESOURCES += \
    image.qrc

