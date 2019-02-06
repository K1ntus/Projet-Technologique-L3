#include "pt_icalibration.h"

PT_ICalibration::PT_ICalibration(int nLines, int nCols):
    currentImg(0),
    board_size(nCols, nLines)
{
    gray_image = new cv::Mat;
}

PT_ICalibration::~PT_ICalibration()
{
    delete gray_image;
}


size_t PT_ICalibration::getCurrentImgIndex() const
{
    return currentImg;
}

void PT_ICalibration::clearCalib(bool clearSet)
{

    currentImg = 0;
}
