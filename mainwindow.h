#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "disparity.h"



namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    cv::Mat QImage2Mat(QImage const& src);
    QImage Mat2QImage(cv::Mat const& src);
    cv::Mat contourSobel(cv::Mat img);
    cv::Mat contourLaplace(cv::Mat img);

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
    void on_actionSobel_triggered();
    void on_actionLaplace_triggered();
    void on_actionSGBM_triggered();
    void on_actionOrbs_triggered();

    bool loadFile(const QString &fileName);
    void split(cv::Mat img);

    void orbFeatures(cv::Mat img);
    //cv::Mat disparityMapOrbs(cv::Mat img);
    void disparityMapOrbs(cv::Mat img);
    void on_actionParam_tres_triggered();
};

#endif // MAINWINDOW_H

