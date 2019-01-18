#ifndef CALIBRATION_INTR_H
#define CALIBRATION_INTR_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "intrinsicparameters.h"


class Calibration_intr
{
public:

    Calibration_intr(std::vector<cv::Mat> &imgs, int nLines = 6, int nCols = 9);
    virtual ~Calibration_intr() = 0;

    void newImageSet(std::vector<cv::Mat> const& images);
    std::vector<cv::Mat>& getSet() const;
    size_t getCurrentImgIndex() const;
    void setNextImgIndex(size_t const& newIndex);

    cv::Mat& get_image_origin() const;
    cv::Mat& get_gray_image() const;

    IntrinsicParameters& getIntrinsicParameters() const;
    void setIntrinsincParameters(IntrinsicParameters &intrinsicParam);
    std::vector<cv::Mat>& get_rvecs() const;
    std::vector<cv::Mat>& get_tvecs() const;

    bool find_chessboard_corners(std::vector<cv::Point2f>&corners);
    virtual void calibrate() = 0;
    cv::Mat undistorted_image() const;

protected:

    std::vector<cv::Mat> *imgs;
    size_t currentImg;
    cv::Size board_size;

    cv::Mat* gray_image;

    std::vector<std::vector<cv::Point2f>>* image_points;

    // intrinseque parameters
    IntrinsicParameters *intrParam;

    // extrinseques
    std::vector<cv::Mat>* rvecs;
    std::vector<cv::Mat>* tvecs;

};

#endif // CALIBRATION_INTR_H
