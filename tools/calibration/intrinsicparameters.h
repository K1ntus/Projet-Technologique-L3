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
    void static readIntrCalibration(std::string& filename, IntrinsicParameters &outputIntrinsicParam);
    void static printIntrCalibration(std::string& filename, IntrinsicParameters &intrinsicParamToPrint);


private:
    cv::Mat* camera_matrix;
    cv::Mat* dist_coeffs;
};

#endif // INTRINSICPARAMETERS_H
