#ifndef CHARUCOCALIBRATION_H
#define CHARUCOCALIBRATION_H

#include "calibration_intr.h"
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/aruco/dictionary.hpp>

class CharucoCalibration : public Calibration_intr
{
public:
    CharucoCalibration(std::vector<cv::Mat> &imgs, int nLines = 7, int nCols = 10);
    ~CharucoCalibration();

    void setNextImgIndex(size_t const& newIndex);

    bool find_corners();
    bool find_charuco_corners(std::vector<std::vector<cv::Point2f>>&charucoCorners, std::vector<int>&charucoIds);
    void calibrate();
    void clearCalib(bool clearSet = false);

private:
    typedef Calibration_intr super;
    cv::Ptr<cv::aruco::CharucoBoard> board;
    std::vector< std::vector< std::vector< cv::Point2f > > > *charucoCornersTab;
    std::vector<std::vector<int>> *charucoIdsTab;
};

#endif // CHARUCOCALIBRATION_H
