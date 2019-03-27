#pragma once

#include "controller.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>


namespace mvm
{
#define CONFIG_FILE_PATH "calibration.yml"
#define FPS 30

class CustomController : public Controller
{
public:
  CustomController();

  virtual ~CustomController();

  virtual void process(const cv::Mat & left_img,
                       const cv::Mat & right_img,
                       float * vx, float * vy, float * omega) override;

  virtual void load() override;

 	void sbm(cv::Mat const&imageL, cv::Mat const&imageR, cv::Mat &dst, size_t const &IO_SADWindowSize, size_t const &IO_numberOfDisparities,  size_t const &IO_preFilterCap, const size_t &IO_minDisparity,
                   const size_t &IO_uniquenessRatio, const size_t &IO_speckleWindowSize,
                   const size_t &IO_speckleRange, const int &IO_disp12MaxDif, const size_t &IO_textureTreshold, const size_t &IO_tresholdFilter);

    /**
     * @brief ImgCv::depthMap :  Compute the depth map using the disparity and the camera parameters\n
     * @param disparityMap Tje dosparity map
     * @param dispToDepthMatrix The matrix containing the intrinsec and extrinsec parameters of the camera
     * @return the depth map
     */
    cv::Mat depthMap(cv::Mat const& disparityMap, cv::Mat &dispToDepthMatrix);


    /**
     * @brief ImgCv::rectifiedImage
     * @param distortedImage the image to undistort
     * @param dist_coeffsL left parameters of the camera to undistort the pict
     * @param camera_matrixL left parameters of the camera to undistort the pict
     * @param dist_coeffsR right parameters of the camera to undistort the pict
     * @param camera_matrixR right parameters of the camera to undistort the pict
     * @param R rotation parameters of the camera to undistort the pict
     * @param T translation parameters of the camera to undistort the pict
     * @return the rectified image
     */
    void rectifiedImage(cv::Mat &imageL, cv::Mat &imageR, cv::Mat const&dist_coeffsL, cv::Mat const&camera_matrixL,
                         cv::Mat const&dist_coeffsR, cv::Mat const&camera_matrixR);

    void trackCamShift(cv::Mat const& image, cv::Rect &trackWindow);


private:

  cv::Mat camera_matrixL, dist_coeffsL,
  camera_matrixR, dist_coeffsR, disparityParam, Q;
  int distMin, distMax;
};


}
