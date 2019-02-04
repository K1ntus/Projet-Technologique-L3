#include "pt_icalibration.h"

PT_ICalibration::PT_ICalibration(int nLines, int nCols):
    currentImg(0),
    board_size(nCols, nLines)
{
    gray_image = new cv::Mat;

    rvecs = new std::vector<cv::Mat>;
    tvecs = new std::vector<cv::Mat>;
}

PT_ICalibration::~PT_ICalibration()
{
    delete gray_image;
    delete rvecs;
    delete tvecs;
}


size_t PT_ICalibration::getCurrentImgIndex() const
{
    return currentImg;
}

std::vector<cv::Mat>& PT_ICalibration::get_rvecs() const{
    return *rvecs;
}

std::vector<cv::Mat>& PT_ICalibration::get_tvecs() const{
    return *tvecs;
}

void PT_ICalibration::clearCalib(bool clearSet)
{
    rvecs->clear();
    tvecs->clear();
    currentImg = 0;
}
