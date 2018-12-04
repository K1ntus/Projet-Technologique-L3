#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include "imagecv.h"
#include "disparity.h"
#include "intr_calibration.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPlainTextEdit *textEdit;
    cv::Mat *img_mat;

    cv::Mat *img_left;
    cv::Mat *img_right;
    Disparity * parametersWindow;



private slots:
    void on_actionA_propos_triggered();
    void on_actionQuitter_triggered();
    void on_actionOuvrir_triggered();

    bool load_image();

    void on_button_disparity_clicked();
    void on_button_sobel_clicked();
    void on_button_laplace_clicked();
    void on_button_stereoBM_clicked();
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H

