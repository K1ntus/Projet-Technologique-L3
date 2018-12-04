#ifndef CALIBRATION_INTR_H
#define CALIBRATION_INTR_H

#include <QWidget>
#include <QtGui>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include "imagecv.h"


namespace Ui {
class Calibration_intr;
}

class Calibration_intr : public QWidget
{
    Q_OBJECT

public:
    explicit Calibration_intr(std::vector<cv::Mat>, QWidget *parent = 0);
    ~Calibration_intr();

    cv::Mat* get_camera_matrix();
    cv::Mat* get_dist_coeffs();
    std::vector<cv::Mat> get_rvecs();
    std::vector<cv::Mat> get_tvecs();

    void show_chessboard_corners(cv::Mat*);
    void show_undistorted_image(cv::Mat*);

private:
    Ui::Calibration_intr *ui;
    cv::Mat* camera_matrix;
    cv::Mat* dist_coeffs;
    std::vector<cv::Mat> rvecs;
    std::vector<cv::Mat> tvecs;
    cv::Mat* img;

private slots:

    void on_undistortedButton_clicked();
    void on_chesscorners_clicked();
};

#endif // CALIBRATION_INTR_H
