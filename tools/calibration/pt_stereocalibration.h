#ifndef PT_STEREOCALIBRATION_H
#define PT_STEREOCALIBRATION_H

#include "calibration_intr.h"
#include "chessboardcalibration.h"
#include "charucocalibration.h"
#include "../imgcv.h"

class PT_StereoCalibration : public Calibration_intr
{
public:

    PT_StereoCalibration(std::vector<cv::Mat> &imgsL, std::vector<cv::Mat> &imgsR,
                         int nLines = 6, int nCols = 9,
                         CalibrationMode calibrationMode = CHESSBOARD);
    ~PT_StereoCalibration();

    void setNextImgIndex(size_t const& newIndex);
    void clearCalib();
    void prepareCalibration();
    bool find_corners();
    void calibrate();

private:
    enum CalibrationMode{
        CHARUCO,
        CHESSBOARD
    };

    typedef Calibration_intr super;
    Calibration_intr *calibLeft;
    Calibration_intr *calibRight;
    ImgCv *stereoImg;
};

#endif // PT_STEREOCALIBRATION_H
