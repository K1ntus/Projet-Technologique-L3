#ifndef PT_ICALIBRATION_H
#define PT_ICALIBRATION_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "intrinsicparameters.h"

class PT_ICalibration
{
public:
    PT_ICalibration( int nLines = 6, int nCols = 9);
    virtual ~PT_ICalibration();

    virtual void newImageSet(std::vector<cv::Mat> const& images) = 0;
    virtual std::vector<cv::Mat>& getSet() const = 0;

    size_t getCurrentImgIndex() const;
    virtual void setNextImgIndex(size_t const& newIndex) = 0;
    virtual cv::Mat& get_image_origin() const = 0;
    virtual cv::Mat& get_compute_image() const = 0;

    virtual bool hasIntrinsicParameters() const = 0;

    virtual void clearCalib(bool clearSet = false);
    virtual void prepareCalibration() = 0;
    virtual bool find_corners() = 0;
    virtual void calibrate() = 0;
    virtual bool saveCalibration(std::string const &outFile) const = 0;
    virtual bool runCalibration(std::string const &inFile) = 0;
    virtual cv::Mat undistorted_image() const = 0;

protected:

    size_t currentImg;
    cv::Size board_size;

    cv::Mat* gray_image;

};

#endif // PT_ICALIBRATION_H
