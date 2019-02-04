#ifndef CALIBRATION_INTR_H
#define CALIBRATION_INTR_H

#include "pt_icalibration.h"


class Calibration_intr : public PT_ICalibration
{
public:

    Calibration_intr(std::vector<cv::Mat> &imgs, int nLines = 6, int nCols = 9);
    virtual ~Calibration_intr();

    virtual std::vector<std::vector<cv::Point2f>>& getImagePoints() const = 0;
    virtual std::vector<std::vector<cv::Point3f>>& getObjectPoints() const = 0;

    void newImageSet(std::vector<cv::Mat> const& images);
    std::vector<cv::Mat>& getSet() const;
    virtual void setNextImgIndex(size_t const& newIndex);

    cv::Mat& get_image_origin() const;
    cv::Mat& get_compute_image() const;

    bool hasIntrinsicParameters() const;
    IntrinsicParameters& getIntrinsicParameters() const;
    void setIntrinsincParameters(IntrinsicParameters &intrinsicParam);
    virtual void clearCalib(bool clearSet = false);
    virtual void prepareCalibration() = 0;
    virtual bool find_corners() = 0;
    virtual void calibrate() = 0;
    bool saveCalibration(std::string const &outFile) const;
    bool runCalibration(std::string const &inFile);
    cv::Mat undistorted_image() const;

protected:
    std::vector<cv::Mat> *imgs;
    // intrinseque parameters
    IntrinsicParameters *intrParam;

private:
    typedef PT_ICalibration super;

};

#endif // CALIBRATION_INTR_H
