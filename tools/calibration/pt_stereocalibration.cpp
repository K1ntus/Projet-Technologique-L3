#include "pt_stereocalibration.h"


PT_StereoCalibration::PT_StereoCalibration(std::vector<cv::Mat> &imgsL, std::vector<cv::Mat> &imgsR, int nLines, int nCols, PT_StereoCalibration::CalibrationMode calibrationMode):
Calibration_intr(imgsR, nLines, nCols)
{

    switch (calibrationMode) {
    case CHESSBOARD:
        calibLeft = new ChessboardCalibration(imgsL, nLines, nCols);
        calibRight = new ChessboardCalibration(imgsR, nLines, nCols);
        break;
    case CHARUCO:
        calibLeft = new CharucoCalibration(imgsL, nLines, nCols);
        calibRight = new CharucoCalibration(imgsR, nLines, nCols);
        break;
    }

    stereoImg = new ImgCv(calibLeft->get_image_origin(), calibRight->get_image_origin(), true);

}

PT_StereoCalibration::~PT_StereoCalibration()
{
    delete calibLeft;
    delete calibRight;
    delete stereoImg;
}

void PT_StereoCalibration::setNextImgIndex(const size_t &newIndex)
{
    calibLeft->setNextImgIndex(newIndex);
    calibRight->setNextImgIndex(newIndex);
    stereoImg->setImg(calibLeft->get_image_origin(), calibRight->get_image_origin());
}

void PT_StereoCalibration::clearCalib()
{
    super::clearCalib();
    calibLeft->clearCalib();
    calibRight->clearCalib();
}

void PT_StereoCalibration::prepareCalibration()
{
    calibLeft->prepareCalibration();
    calibRight->prepareCalibration();
    stereoImg->setImg(calibLeft->get_image_origin(), calibRight->get_image_origin());

}

bool PT_StereoCalibration::find_corners()
{
    return true;
}

void PT_StereoCalibration::calibrate()
{
    using namespace std;

    vector<vector<cv::Point3f>> objectPoints;
    vector<vector<cv::Point2f>> imagePointsL;
    vector<vector<cv::Point2f>> imagePointsR;

    cv::Mat &dist_coeffsL = calibLeft->getIntrinsicParameters().getDistCoeffs();
    cv::Mat &camera_matrixL = calibRight->getIntrinsicParameters().getCameraMatrix();
    cv::Mat &dist_coeffsR = calibLeft->getIntrinsicParameters().getDistCoeffs();
    cv::Mat &camera_matrixR = calibRight->getIntrinsicParameters().getCameraMatrix();

    cv::OutputArray RMat, TMat, E, F;
    cv::stereoCalibrate(objectPoints, imagePointsL, imagePointsR, camera_matrixL, dist_coeffsL,
                        camera_matrixR, dist_coeffsR, calibLeft->get_image_origin().size(), RMat, TMat, E, F);
}



