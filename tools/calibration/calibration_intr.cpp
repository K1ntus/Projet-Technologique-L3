#include "calibration_intr.h"

using namespace std;
using namespace cv;

Calibration_intr::Calibration_intr(std::vector<cv::Mat> &imgs, int nLines, int nCols):
    PT_ICalibration (nLines, nCols),
    imgs(nullptr), intrParam(nullptr),
    rvecs(nullptr), tvecs(nullptr)
{
    this->imgs = new std::vector<cv::Mat>(imgs);
    intrParam = new IntrinsicParameters();
    rvecs = new std::vector<cv::Mat>;
    tvecs = new std::vector<cv::Mat>;

}

Calibration_intr::~Calibration_intr(){
    delete imgs;
    delete intrParam;
    delete rvecs;
    delete tvecs;

}

/**
 * @brief Calibration_intr::newImageSet create a new set from a vector of images
 * @param images the vector with the set image
 */
void Calibration_intr::newImageSet(const std::vector<Mat> &images)
{
    clearCalib(true);
    imgs->clear();
    for (size_t i = 0; i < images.size(); i++) {
        imgs->push_back(images[i]);
    }
}

/**
 * @brief Calibration_intr::getSet get the current img set
 * @return  the image set
 */
std::vector<Mat> &Calibration_intr::getSet() const
{
    return *imgs;
}

/**
 * @brief Calibration_intr::setNextImgIndex update the indec of the img vector
 * @param newIndex the new position in the vector
 */
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

/**
 * @brief Calibration_intr::hasIntrinsicParameters
 * @return true if the intrinsic parameters pointer is not null or empty
 */
bool Calibration_intr::hasIntrinsicParameters() const
{
    return !(intrParam == nullptr || intrParam->empty());
}

/**
 * @brief Calibration_intr::getIntrinsicParameters
 * @return the intrinsic parameters
 */
IntrinsicParameters& Calibration_intr::getIntrinsicParameters() const{
    return *this->intrParam;
}

/**
 * @brief Calibration_intr::setIntrinsincParameters
 * @param intrinsicParam the intrincis parameters to set
 */
void Calibration_intr::setIntrinsincParameters(IntrinsicParameters &intrinsicParam)
{
    this->intrParam->setCameraMatrix(intrinsicParam.getCameraMatrix());
    this->intrParam->setDistCoeffsMatrix(intrinsicParam.getDistCoeffs());

}

std::vector<cv::Mat>& Calibration_intr::get_rvecs() const{
    return *rvecs;
}

std::vector<cv::Mat>& Calibration_intr::get_tvecs() const{
    return *tvecs;
}

/**
 * @brief Calibration_intr::clearCalib
 * @param clearSet
 *
 * Clear the calibration set by deleting every image of the vector containing the image\n
 * and the translation (resp. rotation) vectors tvecs (resp. rvecs)
 */
void Calibration_intr::clearCalib(bool clearSet)
{
    super::clearCalib(clearSet);
    rvecs->clear();
    tvecs->clear();
    if(clearSet)
        imgs->clear();
}

/**
 * @brief Calibration_intr::saveCalibration
 * @param outFile name of the calibration file
 * @return true if the file has been saved
 */
bool Calibration_intr::saveCalibration(string const &outFile) const
{
    return IntrinsicParameters::printIntrCalibration(outFile, *intrParam);

}

/**
 * @brief Calibration_intr::runCalibration run the calibration using a file containing the params
 * @param inFile the string of the intrinsic parameters file
 * @return
 */
bool Calibration_intr::runCalibration(string const &inFile)
{
    return IntrinsicParameters::readIntrCalibration(inFile, *intrParam);

}

/**
 * @brief Calibration_intr::undistorted_image
 * @return undistorted image
 */
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
