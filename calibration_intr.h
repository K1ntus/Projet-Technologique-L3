#ifndef CALIBRATION_INTR_H
#define CALIBRATION_INTR_H

#include <opencv2/opencv.hpp>
#include "imagecv.h"

class Calibration_intr
{
public:

    Calibration_intr(std::vector<cv::Mat> imgs);
    ~Calibration_intr();
    cv::Mat* get_image_origin();
    cv::Mat* get_gray_image();
    cv::Mat* get_camera_matrix();
    cv::Mat* get_dist_coeffs();
    std::vector<cv::Mat>* get_rvecs();
    std::vector<cv::Mat>* get_tvecs();

    bool find_chessboard_corners(std::vector<cv::Point2f>&);
    void calibrate(std::vector<cv::Mat>);
    cv::Mat* undistorted_image();

private:

    int nb_lines;
    int nb_columns;
    cv::Size board_size;
    cv::Mat* img;
    cv::Mat* gray_image;

    vector<vector<cv::Point3f>>* object_points;
    vector<vector<cv::Point2f>>* image_points;
    cv::Mat* camera_matrix;
    cv::Mat* dist_coeffs;
    std::vector<cv::Mat>* rvecs;
    std::vector<cv::Mat>* tvecs;

};

#endif // CALIBRATION_INTR_H
