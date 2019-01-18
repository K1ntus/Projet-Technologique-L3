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

    bool find_chessboard_corners(std::vector<cv::Point2f>&corners);
    void calibrate();

private:
    std::vector<int> *charucoIds;
};

#endif // CHARUCOCALIBRATION_H
