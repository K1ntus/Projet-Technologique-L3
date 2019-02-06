#ifndef INTRINSICPARAMETERS_H
#define INTRINSICPARAMETERS_H

#include <opencv2/opencv.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <iostream>

class IntrinsicParameters
{
public:

    /**
     * @brief IntrinsicParameters::IntrinsicParameters Create an instance of this class with default camera matrix value
     */
    IntrinsicParameters();

    /**
     * @brief IntrinsicParameters::IntrinsicParameters Create an instance of this class with specific camera matrix value
     * @param new_camera_matrix The camera matrix value to use
     * @param dist_coeffs_mat The distorsion matrix value to use
     */
    IntrinsicParameters(cv::Mat& camera_matrix, cv::Mat& dist_coeffs_mat);

    /**
     * @brief IntrinsicParameters::~IntrinsicParameters delete the instance of this class, and the pointers linked to it
     */
    ~IntrinsicParameters();

    /**
     * @brief IntrinsicParameters::getCameraMatrix
     * @return the camera matrix
     */
    cv::Mat& getCameraMatrix() const;

    /**
     * @brief IntrinsicParameters::getDistCoeffs
     * @return the distorsion coeff matrix
     */
    cv::Mat& getDistCoeffs() const;

    /**
     * @brief IntrinsicParameters::setCameraMatrix set the camera matrix
     * @param camera_matrix the new camera matrix
     */
    void setCameraMatrix(cv::Mat& camera_matrix);

    /**
     * @brief IntrinsicParameters::setDistCoeffsMatrix set the distorsion coeffs matrix
     * @param dist_coeffs_mat the distorsion coeffs matrix to use
     */
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
