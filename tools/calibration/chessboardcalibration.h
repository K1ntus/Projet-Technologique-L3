#ifndef CHESSBOARDCALIBRATION_H
#define CHESSBOARDCALIBRATION_H

#include "calibration_intr.h"

class ChessboardCalibration : public Calibration_intr
{

public:

    ChessboardCalibration(std::vector<cv::Mat> &imgs, int nLines = 6, int nCols = 9);
    ~ChessboardCalibration();

    void setNextImgIndex(size_t const& newIndex);

    std::vector<std::vector<cv::Point2f>>& getImagePoints() const;

    /**
     * @brief ChessboardCalibration::setImagePoints Clear the images point of the current class, and set the new one linked to the image
     * @param imagePoints the image points to set in the image
     */
    void setImagePoints(std::vector<std::vector<cv::Point2f>>& imagePoints);

    /**
     * @brief ChessboardCalibration::clearCalib clear the calibration, if set to true, the image vector will also be cleaned
     * @param clearSet set it to true to also clear the image vector. Default is set to false.
     */
    void clearCalib(bool clearSet = false);

    /**
     * @brief ChessboardCalibration::prepareCalibration get the chessboard corners and object point as a first step for the calibration
     */
    void prepareCalibration();
    bool find_corners();

    /**
     * @brief CharucoCalibration::find_charuco_corners Find the charuco corners of an image and save it into pointers
     * @param charucoCorners the pointer that will get the result of the list of charuco corners
     * @param charucoIds the pointer that will store the result of the different charuco id
     * @return true if the search has succeed, else false
     */
    bool find_chessboard_corners(std::vector<cv::Point2f>&corners);

    /**
     * @brief ChessboardCalibration::calibrate
     * Calibrate the image set using the chessboard pattern.\n
     * The clearCalib() call doesnt clear the image set because the boolean parameters is not set to true.\n
     * But its cleaning the corners and ids list got from before to prevent duplication, incorrect value from\n
     * before tests, ...
     */
    void calibrate();

private:
    typedef Calibration_intr super;
    std::vector<std::vector<cv::Point2f>>* image_points;
    std::vector<std::vector<cv::Point3f>>* object_points;
};

#endif // CHESSBOARDCALIBRATION_H
