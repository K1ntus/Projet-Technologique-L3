#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// opencv uitlities
// uncomment for common project
#include <unistd.h>

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
    void on_actionOuvrir_triggered();
    void on_button_disparity_clicked();
    void on_button_sobel_clicked();
    void on_button_laplace_clicked();
    void on_calibrate_clicked();
};

#endif // MAINWINDOW_H

