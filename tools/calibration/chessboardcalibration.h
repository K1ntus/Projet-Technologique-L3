#ifndef CHESSBOARDCALIBRATION_H
#define CHESSBOARDCALIBRATION_H

#include "calibration_intr.h"

class ChessboardCalibration : public Calibration_intr
{

public:
    ChessboardCalibration(std::vector<cv::Mat> &imgs, int nLines = 6, int nCols = 9);
    ~ChessboardCalibration();
    bool find_chessboard_corners(std::vector<cv::Point2f>&corners);
    void calibrate();

};

#endif // CHESSBOARDCALIBRATION_H
