#include "calibration_intr.h"

using namespace std;
using namespace cv;

Calibration_intr::Calibration_intr(std::vector<cv::Mat> &imgs, int nLines, int nCols) :
    currentImg(0)
{
    this->imgs = new vector<cv::Mat>(imgs);
    board_size = Size(nCols, nLines);
    gray_image = new Mat;

    intrParam = new IntrinsicParameters();
    rvecs = new std::vector<cv::Mat>;
    tvecs = new std::vector<cv::Mat>;
}

Calibration_intr::~Calibration_intr()
{
    delete gray_image;
    delete intrParam;
    delete rvecs;
    delete tvecs;
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

size_t Calibration_intr::getCurrentImgIndex() const
{
    return currentImg;
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

IntrinsicParameters& Calibration_intr::getIntrinsicParameters() const{
    return *this->intrParam;
}

void Calibration_intr::setIntrinsincParameters(IntrinsicParameters &intrinsicParam)
{
    this->intrParam->setCameraMatrix(intrinsicParam.getCameraMatrix());
    this->intrParam->setDistCoeffsMatrix(intrinsicParam.getDistCoeffs());

}

vector<Mat>& Calibration_intr::get_rvecs() const{
    return *rvecs;
}

vector<Mat>& Calibration_intr::get_tvecs() const{
    return *tvecs;
}

void Calibration_intr::clearCalib(bool clearSet)
{
    rvecs->clear();
    tvecs->clear();
    currentImg = 0;
//    if(clearSet)
//        imgs->clear();
}

cv::Mat Calibration_intr::undistorted_image() const{
    Mat image_undistorted;
    if(imgs->empty() || intrParam == nullptr || intrParam->empty()){}
    else {
        Mat &img = imgs->at(currentImg);
        Mat &dist_coeffs = intrParam->getDistCoeffs();
        Mat &camera_matrix = intrParam->getCameraMatrix();

        undistort(img, image_undistorted, camera_matrix, dist_coeffs);
    }
    return image_undistorted;
}
