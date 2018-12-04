#ifndef INTR_CALIBRATION_H
#define INTR_CALIBRATION_H
#include <opencv2/opencv.hpp>

class Intr_Calibration
{
    public:
        Intr_Calibration(std::vector<cv::Mat>);

        cv::Mat* get_camera_matrix();
        cv::Mat* get_dist_coeffs();
        std::vector<cv::Mat> get_rvecs();
        std::vector<cv::Mat> get_tvecs();

        void show_chessboard_corners(cv::Mat*);
        void show_undistorted_image(cv::Mat*);

    private:

        cv::Mat* camera_matrix;
        cv::Mat* dist_coeffs;
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;
};

#endif // INTR_CALIBRATION_H
