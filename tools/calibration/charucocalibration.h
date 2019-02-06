#ifndef CHARUCOCALIBRATION_H
#define CHARUCOCALIBRATION_H

#include "calibration_intr.h"
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/aruco/dictionary.hpp>

class CharucoCalibration : public Calibration_intr
{
public:
    CharucoCalibration(std::vector<cv::Mat> &imgs, int nLines = 7, int nCols = 10);
    ~CharucoCalibration();

    void setNextImgIndex(size_t const& newIndex);

    std::vector<std::vector<cv::Point2f>>& getImagePoints() const;
    void setImagePoints(std::vector<std::vector<cv::Point2f>>& imagePoints);

    /**
     * @brief CharucoCalibration::clearCalib clear the calibration, if set to true, the image vector will also be cleaned
     * @param clearSet set it to true to also clear the image vector. Default is set to false.
     */
    void clearCalib(bool clearSet = false);

    /**
     * @brief CharucoCalibration::prepareCalibration get the charuco corners and id as a first step for the calibration
     */
    void prepareCalibration();
    bool find_corners();

    /**
     * @brief CharucoCalibration::find_charuco_corners Find the charuco corners of an image and save it into pointers
     * @param charucoCorners the pointer that will get the result of the list of charuco corners
     * @param charucoIds the pointer that will store the result of the different charuco id
     * @return true if the search has succeed, else false
     */
    bool find_charuco_corners(std::vector<cv::Point2f>&charucoCorners, std::vector<int>&charucoIds);

    /**
     * @brief CharucoCalibration::calibrate
     * Calibrate the image set using charuco calibration.\n
     * The clearCalib() call doesnt clear the image set because the boolean parameters is not set to true.\n
     * But its cleaning the corners and ids list got from before to prevent duplication, incorrect value from\n
     * before tests, ...
     */
    void calibrate();

private:
    typedef Calibration_intr super;
    cv::Ptr<cv::aruco::CharucoBoard> board;
    std::vector< std::vector< cv::Point2f > > *charucoCornersTab;
    std::vector<std::vector<int>> *charucoIdsTab;
};

#endif // CHARUCOCALIBRATION_H
