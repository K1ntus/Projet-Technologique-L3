#ifndef CALIBRATION_INTR_H
#define CALIBRATION_INTR_H

#include "pt_icalibration.h"


class Calibration_intr : public PT_ICalibration
{
public:
    /**
     * @brief Calibration_intr
     * @param imgs
     * @param nLines
     * @param nCols
     */
    Calibration_intr(std::vector<cv::Mat> &imgs, int nLines = 6, int nCols = 9);
    virtual ~Calibration_intr();

    virtual std::vector<std::vector<cv::Point2f>>& getImagePoints() const = 0;
    virtual void setImagePoints(std::vector<std::vector<cv::Point2f>>&) = 0;

    /**
     * @brief Calibration_intr::newImageSet create a new set from a vector of images
     * @param images the vector with the set image
     */
    void newImageSet(std::vector<cv::Mat> const& images);

    /**
     * @brief Calibration_intr::getSet get the current img set
     * @return  the image set
     */
    std::vector<cv::Mat>& getSet() const;

    /**
     * @brief Calibration_intr::setNextImgIndex update the index of the img vector
     * @param newIndex the new position in the vector
     */
    virtual void setNextImgIndex(size_t const& newIndex);

    cv::Mat& get_image_origin() const;
    cv::Mat& get_compute_image() const;

    /**
     * @brief Calibration_intr::hasIntrinsicParameters
     * @return true if the intrinsic parameters pointer is not null or empty
     */
    bool hasIntrinsicParameters() const;

    /**
     * @brief Calibration_intr::getIntrinsicParameters
     * @return the intrinsic parameters
     */
    IntrinsicParameters& getIntrinsicParameters() const;

    /**
     * @brief Calibration_intr::setIntrinsincParameters
     * @param intrinsicParam the intrincis parameters to set
     */
    void setIntrinsincParameters(IntrinsicParameters &intrinsicParam);

    /**
     * @brief Calibration_intr::get_rvecs
     * @return the rotation vector
     */
    std::vector<cv::Mat>& get_rvecs() const;

    /**
     * @brief Calibration_intr::get_tvecs
     * @return the translation vector
     */
    std::vector<cv::Mat>& get_tvecs() const;

    /**
     * @brief Calibration_intr::clearCalib
     * @param clearSet
     *
     * Clear the calibration set by deleting every image of the vector containing the image\n
     * and the translation (resp. rotation) vectors tvecs (resp. rvecs)
     */
    virtual void clearCalib(bool clearSet = false);
    virtual void prepareCalibration() = 0;
    virtual bool find_corners() = 0;
    virtual void calibrate() = 0;

    /**
     * @brief Calibration_intr::saveCalibration
     * @param outFile name of the calibration file
     * @return true if the file has been saved
     */
    bool saveCalibration(std::string const &outFile) const;

    /**
     * @brief Calibration_intr::runCalibration run the calibration using a file containing the params
     * @param inFile the string of the intrinsic parameters file
     * @return
     */
    bool runCalibration(std::string const &inFile);

    /**
     * @brief Calibration_intr::undistorted_image
     * @return undistorted image
     */
    cv::Mat undistorted_image() const;

protected:
    std::vector<cv::Mat> *imgs;

    // intrinseque parameters
    IntrinsicParameters *intrParam;
    // extrinseques
    std::vector<cv::Mat>* rvecs;
    std::vector<cv::Mat>* tvecs;

private:
    typedef PT_ICalibration super;

};

#endif // CALIBRATION_INTR_H
