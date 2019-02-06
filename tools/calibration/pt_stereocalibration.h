#ifndef PT_STEREOCALIBRATION_H
#define PT_STEREOCALIBRATION_H

#include "pt_icalibration.h"
#include "calibration_intr.h"
#include "chessboardcalibration.h"
#include "charucocalibration.h"
#include "../imgcv.h"

enum CalibrationMode{
    CHARUCO,
    CHESSBOARD,
    STEREO
};

class PT_StereoCalibration : public PT_ICalibration
{
public:

    PT_StereoCalibration(std::vector<cv::Mat> &imgsL, std::vector<cv::Mat> &imgsR,
                         int nLines = 6, int nCols = 9,
                         CalibrationMode calibrationMode = CHESSBOARD);
    PT_StereoCalibration(std::vector<ImgCv> &stereoImgs, int nLines = 6, int nCols = 9,
                         CalibrationMode calibrationMode = CHESSBOARD);
    PT_StereoCalibration( std::string const& calibFilename);

    ~PT_StereoCalibration();

    void newImageSet(std::vector<cv::Mat> const& imagesSetLR);
    void newImageSet(std::vector<cv::Mat> const& imagesL, std::vector<cv::Mat> const& imagesR);

    std::vector<cv::Mat>& getSet() const;

    void setNextImgIndex(size_t const& newIndex);
    cv::Mat& get_image_origin() const;
    cv::Mat& get_compute_image() const;

    bool hasIntrinsicParameters() const;
    void clearCalib(bool clearSet = false);
    void prepareCalibration();
    bool find_corners();
    void calibrate();
    bool saveCalibration(std::string const &outFile) const;
    bool runCalibration(std::string const &inFile);

    void displayRectifiedImage();
    cv::Mat undistorted_image() const;
private:


    typedef PT_ICalibration super;

    std::vector<cv::Mat> *imgs;
    Calibration_intr *calibLeft;
    Calibration_intr *calibRight;
    ImgCv *stereoImg;
    cv::Mat *R, *T,
    *essentialMatrix, *fundamentalMatrix;
};

#endif // PT_STEREOCALIBRATION_H
