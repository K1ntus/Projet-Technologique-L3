#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// opencv uitlities
// uncomment for common project
#include <unistd.h>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
// Qt
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QtGui>
#include <QPixmap>

// own
#include "tools/imagecv.h"
#include "tools/imgcv.h"
#include "disparity.h"
#include "calibration_widget.h"
#include "ui_mainwindow.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ImgCv *img;
    Disparity * parametersWindow;
    Calibration_widget *calib_widget;


private slots:
    void on_actionA_propos_triggered();
    void on_actionQuitter_triggered();
    void on_button_disparity_clicked();
    void on_button_sobel_clicked();
    void on_button_laplace_clicked();
    void on_calibrate_clicked();
    void on_videoTest_clicked();
    void on_actionCv_Mat_triggered();
    void on_actionOuvrir_image_triggered();
    void on_action1_image_triggered();
    void on_action2_image_triggered();
    void on_actionQImage_triggered();
};

#endif // MAINWINDOW_H

