#ifndef CHESSBOARDCALIBRATION_H
#define CHESSBOARDCALIBRATION_H

#include "calibration_intr.h"

class ChessboardCalibration : public Calibration_intr
{

public:

    ChessboardCalibration(std::vector<cv::Mat> &imgs, int nLines = 6, int nCols = 9);
    ~ChessboardCalibration();

    void setNextImgIndex(size_t const& newIndex);

    std::vector<std::vector<cv::Point2f>>& getImagePoints() const;
    void setImagePoints(std::vector<std::vector<cv::Point2f>>& imagePoints);
    void clearCalib(bool clearSet = false);
    void prepareCalibration();
    bool find_corners();
    bool find_chessboard_corners(std::vector<cv::Point2f>&corners);
    void calibrate();

private:
    typedef Calibration_intr super;
    std::vector<std::vector<cv::Point2f>>* image_points;
    std::vector<std::vector<cv::Point3f>>* object_points;
};

#endif // CHESSBOARDCALIBRATION_H
