#include "calibration_intr.h"

using namespace std;
using namespace cv;

Calibration_intr::Calibration_intr(std::vector<cv::Mat> &imgs, int nLines, int nCols):
    PT_ICalibration (nLines, nCols)
{
    this->imgs = new std::vector<cv::Mat>(imgs);
    intrParam = new IntrinsicParameters();

}

Calibration_intr::~Calibration_intr(){
    delete imgs;
    delete intrParam;

}

void Calibration_intr::newImageSet(const std::vector<Mat> &images)
{
    clearCalib(true);
    imgs->clear();
    for (size_t i = 0; i < images.size(); i++) {
        imgs->push_back(images[i]);
    }
}

std::vector<Mat> &Calibration_intr::getSet() const
{
    return *imgs;
}

void Calibration_intr::setNextImgIndex(size_t const& newIndex)
{
    if(newIndex < imgs->size())
        currentImg = newIndex;

}

Mat& Calibration_intr::get_image_origin() const{
    return imgs->at(currentImg);
}

Mat& Calibration_intr::get_compute_image() const{
    return *gray_image;
}

bool Calibration_intr::hasIntrinsicParameters() const
{
    return !(intrParam == nullptr || intrParam->empty());
}

IntrinsicParameters& Calibration_intr::getIntrinsicParameters() const{
    return *this->intrParam;
}

void Calibration_intr::setIntrinsincParameters(IntrinsicParameters &intrinsicParam)
{
    this->intrParam->setCameraMatrix(intrinsicParam.getCameraMatrix());
    this->intrParam->setDistCoeffsMatrix(intrinsicParam.getDistCoeffs());

}

void Calibration_intr::clearCalib(bool clearSet)
{
    super::clearCalib(clearSet);
    if(clearSet)
        imgs->clear();
}

bool Calibration_intr::saveCalibration(string const &outFile) const
{
    return IntrinsicParameters::printIntrCalibration(outFile, *intrParam);

}

bool Calibration_intr::runCalibration(string const &inFile)
{
    return IntrinsicParameters::readIntrCalibration(inFile, *intrParam);

}

cv::Mat Calibration_intr::undistorted_image() const{
    Mat image_undistorted;
    if(imgs->empty() || !hasIntrinsicParameters()){}
    else {
        Mat &img = imgs->at(currentImg);
        Mat &dist_coeffs = intrParam->getDistCoeffs();
        Mat &camera_matrix = intrParam->getCameraMatrix();

        undistort(img, image_undistorted, camera_matrix, dist_coeffs);
    }
    return image_undistorted;
}
