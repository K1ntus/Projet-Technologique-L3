#ifndef INTRINSICPARAMETERS_H
#define INTRINSICPARAMETERS_H

#include <opencv2/opencv.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <iostream>

class IntrinsicParameters
{
public:
    IntrinsicParameters();
    IntrinsicParameters(cv::Mat& camera_matrix, cv::Mat& dist_coeffs_mat);

    ~IntrinsicParameters();
    cv::Mat& getCameraMatrix() const;
    cv::Mat& getDistCoeffs() const;

    void setCameraMatrix(cv::Mat& camera_matrix);
    void setDistCoeffsMatrix(cv::Mat& dist_coeffs_mat);

    bool empty();
    bool static readIntrCalibration(std::string const &filename, IntrinsicParameters &outputIntrinsicParam);
    bool static readIntrStereoCalibration(std::string const &filename, IntrinsicParameters &outputIntrinsicParamLeft, IntrinsicParameters &outputIntrinsicParamRight);
    bool static readIntrCalibration(std::string const &filename, cv::Mat &cameraMat, cv::Mat &distCoeffs);
    bool static readIntrStereoCalibration(std::string const &filename, cv::Mat &cameraMatL, cv::Mat &distCoeffsL,
                                          cv::Mat &cameraMatR, cv::Mat &distCoeffsR);

    bool static printIntrCalibration(std::string const &filename, IntrinsicParameters const &intrinsicParamToPrint);
    bool static printIntrStereoCalibration(std::string const &filename, IntrinsicParameters const &intrinsicParamLeftToPrint, IntrinsicParameters const &intrinsicParamRightToPrint);
    bool static printIntrCalibration(std::string const &filename, cv::Mat &cameraMat, cv::Mat &distCoeffs);
    bool static printIntrStereoCalibration(std::string const &filename, cv::Mat &cameraMatL, cv::Mat &distCoeffsL,
                                          cv::Mat &cameraMatR, cv::Mat &distCoeffsR);

private:
    cv::Mat* camera_matrix;
    cv::Mat* dist_coeffs;
};

#endif // INTRINSICPARAMETERS_H
