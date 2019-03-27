#ifndef CALIBRATION_WIDGET_H
#define CALIBRATION_WIDGET_H

#include <QWidget>
#include <QtGui>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_calibration_widget.h"

#include "tools/calibration/pt_stereocalibration.h"
#include "tools/calibration/chessboardcalibration.h"
//#include "tools/calibration/charucocalibration.h"
#include "tools/imagecv.h"


namespace Ui {
class Calibration_widget;
}

enum DisplayerMode{
    ORIGINAL,
    CHESSBOARD_CORNERS,
    UNDISTORTED
};

class Calibration_widget : public QWidget
{
    Q_OBJECT

public:
    explicit Calibration_widget(PT_ICalibration *calibration = nullptr, CalibrationMode CalibMode = CalibrationMode::CHESSBOARD, QWidget *parent = nullptr);
    ~Calibration_widget();

    /**
     * @brief Calibration_widget::display_image Display the current image of the set on screen
     * @param displayerMode
     */
    void display_image(DisplayerMode const&displayerMode);

private:
    Ui::Calibration_widget *ui;
    PT_ICalibration* calib;
    CalibrationMode currentCalibMode;
    DisplayerMode currentMode;

    /**
     * @brief Calibration_widget::enableFeatures enable or disable some specific features.
     * @param isEnabled
     *
     * This function enable some features that could only be available\n
     * when an image set has been loaded, like:\n
     * + Display the original image\n
     * + Display the chesscorners that has been discovered\n
     * + Go to the previous image\n
     * + Go to the next image\n
     */
    void enableFeatures(bool isEnabled =false);

private slots:

    /**
     * @brief Calibration_widget::on_undistortedButton_clicked is summoned when the linked button is pressed.
     * This function launch the calibration on distorted pictures from the fish eye effects and\n
     * undistorted the whole set using the intrinsic parameters of the camera
     */
    void on_undistortedButton_clicked();

    /**
     * @brief Calibration_widget::on_chesscorners_clicked Detect the chessboard corners of an image set
     */
    void on_chesscorners_clicked();

    /**
     * @brief Calibration_widget::on_newImageSet_clicked load a new set of image to test
     */
    void on_newImageSet_clicked();

    /**
     * @brief Calibration_widget::on_nextImage_clicked Go to the next image
     */
    void on_nextImage_clicked();

    /**
     * @brief Calibration_widget::on_prevImage_clicked Go back to the previous image
     */
    void on_prevImage_clicked();

    /**
     * @brief Calibration_widget::on_save_clicked Save the intrinsic or extrinsic parameters depending of the detection.
     * If the detection mode is 'chessboard' or 'charuco', only the intrinsic parameters are saved.\n
     * If the detection mode is using a camera stream, the intrinsic AND extrinsic parameters are saved.\n
     */
    void on_save_clicked();

    /**
     * @brief Calibration_widget::on_loadCalib_clicked load a calibration file containing the intrinsic nor extrinsic parameters of the camera.
     */
    void on_loadCalib_clicked();

    /**
     * @brief Calibration_widget::on_originalImage_clicked go back to the 'original' (ie. without calibration) image of the set
     */
    void on_originalImage_clicked();

    /**
     * @brief Calibration_widget::on_calibration_clicked Calibrate the image set, depending of the checkbox mode clicked
     */
    void on_calibration_clicked();

    /**
     * @brief Calibration_widget::on_CharucoCalib_clicked if enabled, the detection mode will use the charuco method
     */
    void on_CharucoCalib_clicked();

    /**
     * @brief Calibration_widget::on_stereoCalibration_clicked this function will calibrate a set of pairs images
     */
    void on_stereoCalibration_clicked();

    /**
     * @brief Calibration_widget::on_Quit_clicked delete the calibration ui
     */
    void on_Quit_clicked();

    /**
     * @brief Calibration_widget::on_ImageSetFromVideo_clicked Load a pair of stereo image from directory and launch it.
     * When used, a popup video will open, displaying the video while running.\n
     * You have access to multiple commands:\n
     * + 'p' : pause the video\n
     * + 'q' : close the video\n
     * + 'v' : save an image of the video to test for the calibration\n
     *
     * To have enough image for calibration, you need at least 10 images. If you don't have enough of them,\n
     * The calibration will be impossible
     */
    void on_ImageSetFromVideo_clicked();

    /**
     * @brief Calibration_widget::on_saveSet_clicked Save the image (more likely to be distorted) set into directories.
     * Depending of the mode (ie. stereo or single),\n
     * This function will save the stereo image set into two directories.\n
     * The first (resp. the second) directory selection popup is for the left (resp. right) image set.\n
     * \n
     * If that is just a single image, you will have to only chose one directory to save this set.
     * \n
     * The image are saved with the format jpg because of his smaller size, but we are losing few informations\n
     * because of this compression. Depending of the robot settings, png could be a better choice\n
     * \n
     *
     */
    void on_saveSet_clicked();

    /**
     * @brief Calibration_widget::on_saveUndistorted_clicked Save the undistorted image set into directories.
     * Depending of the mode (ie. stereo or single),\n
     * This function will save the stereo image set into two directories.\n
     * The first (resp. the second) directory selection popup is for the left (resp. right) image set.\n
     * \n
     * If that is just a single image, you will have to only chose one directory to save this set.
     * \n
     * The image are saved with the format jpg because of his smaller size, but we are losing few informations\n
     * because of this compression. Depending of the robot settings, png could be a better choice\n
     * \n
     *
     */
    void on_saveUndistorted_clicked();

};

#endif // CALIBRATION_WIDGET_H
