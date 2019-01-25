#include "calibration_intr.h"

using namespace std;
using namespace cv;

Calibration_intr::Calibration_intr(std::vector<cv::Mat> &imgs, int nLines, int nCols) :
    currentImg(0)
{
    this->imgs = new vector<cv::Mat>(imgs);
    board_size = Size(nCols, nLines);
    gray_image = new Mat;

    image_points = new vector<vector<Point2f>>;

    intrParam = new IntrinsicParameters();
    rvecs = new std::vector<cv::Mat>;
    tvecs = new std::vector<cv::Mat>;
}

Calibration_intr::~Calibration_intr()
{
    delete gray_image;

    delete image_points;

    delete intrParam;

    delete rvecs;
    delete tvecs;
}

void Calibration_intr::newImageSet(const std::vector<Mat> &images)
{
    image_points->clear();
    rvecs->clear();
    tvecs->clear();
    currentImg = 0;
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
    if(newIndex < imgs->size()){
        currentImg = newIndex;
        if(image_points->size() != 0)
            find_chessboard_corners(image_points->at(currentImg));
        else{
            vector<Point2f> corners;
            find_chessboard_corners(corners);
        }

    }
}

Mat& Calibration_intr::get_image_origin() const{
    return imgs->at(currentImg);
}

Mat& Calibration_intr::get_gray_image() const{
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



bool Calibration_intr::find_chessboard_corners(std::vector<cv::Point2f>& corners){
    Mat &img = imgs->at(currentImg);
    cvtColor(img, *gray_image, CV_BGR2GRAY);
    bool found = findChessboardCorners(img, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

    if(found){
        cornerSubPix(*gray_image, corners, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        drawChessboardCorners(*gray_image, board_size, corners, found);
    }

    return found;
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
