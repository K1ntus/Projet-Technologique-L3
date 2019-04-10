#ifndef DISPARITY_H
#define DISPARITY_H

#include <QWidget>
#include <QtGui>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_disparity.h"

#include "tools/imagecv.h"
#include "tools/imgcv.h"


namespace Ui {
class Disparity;
}
enum DispDisplayerMode{
    NORMAL,
    DISPARITY,
    TRACKER
};

class Disparity : public QWidget
{
    Q_OBJECT

public:
    explicit Disparity(std::string const&calibFile = "", QWidget *parent = nullptr);
    ~Disparity();

    /**
     * @brief Load an cv::mat img, display it in the right panel and finally split it then save them in two pointers
     * @param img the stereo image to manage, split and display
     */
    void set_img_mat(ImgCv &img);

    /**
     * @brief Load the calibration file and associate each value of the "DisparityParameter" matrix to the
     * corresponding diparity map parameter according the disparity mode (SBM, SGBM, SBM+PS, SGBM+PS)
     * @param the calibration file to read
     */
    void setCalibrationFile(std::string const&calibFile);

    /**
     * @brief Return the image to display
     */
    ImgCv * get_img_mat();

    /**
     * @brief Using the tracker, find a ROI and then compute this ROI's depth map.
     * @param roiTracked : the rectangle corresponding to the region tracked
     * @return the depth map of the ROITracked
     */
    cv::Mat get_depth_map(bool roiTracked = false);

    /**
     * @brief Display the disparity map parameters and display it to the right
     * \n
     * The disparity map will be generated and displayed depending of the parameters\n
     * using the differents sbm and sliders.\n
     * \n
     * If no image has been loaded, the program just display an error.\n
     * \n
     * When the image has been correctly generated, it's displayed in the right slot.
     */
    void displayDisparityMap();


    unsigned int IO_SADWindowSize;
    unsigned int IO_numberOfDisparities;
    unsigned int IO_preFilterCap;
    unsigned int IO_minDisparity;
    unsigned int IO_uniquenessRatio;
    unsigned int IO_speckleWindowSize;
    unsigned int IO_speckleRange;
    unsigned int IO_smallerBlockSize;
    int IO_disp12MaxDif;
    unsigned int IO_P1;
    unsigned int IO_P2;
    int IO_full_scale;
    unsigned int IO_textureTreshold;
    unsigned int IO_tresholdFilter;
    float IO_sigma;
    unsigned int IO_lambda;


private slots:
    /**
     * @brief Open a gui to select an image when you push the load button
     */
    void on_loadImage_clicked();

    /**
     * @brief Reset the disparity map parameters to default values (arbitrary fixed, as the more "commons")
     */
    void on_reset_image_clicked();

    /**
     * @brief Display a cv:mat image in the right image slot
     * @param image image to be displayed
     */
    void displayImage(cv::Mat const&);

    /**
     * @brief Apply the Sobel filter on the disparity map, to display its contours
     */
    void on_Sobel_clicked();

    /**
     * @brief Applies the Laplacian filter on the disparity map, to display its contours
     */
    void on_Laplace_clicked();

    /**
     * @brief Load two image's sets, and display the stereo images as a video
     */
    void on_video_clicked();

    /**
     * @brief Compute and display the depth map when the depthMap's button is triggered
     */
    void on_depthMap_clicked();

    /**
     * @brief Load two images and display them in the right slot
     */
    void on_load_two_images_clicked();


    /**
     * @brief Modify the value of the disparity parameter IO_WindowSize according to the slider's one and compute the new disparity map\n
     *  Display also the new value in the corresponding spinBox
     * @param the new IO_WindowSize's value
     */
    void on_slider_windowSize_valueChanged(int value);
    /**
     * @brief  Modify the value of the disparity parameter IO_numberOfDisparities according to the slider's one and compute the new disparity map\n
     *  Display also the new value in the corresponding spinBox
     * @param the new IO_numberOfDisparities' value
     */
    void on_slider_numberOfDisparities_valueChanged(int value);

    /**
     * @brief when the checkBox SBM is clicked  : \n
     * if it's checked then compute, he disparity Map using the sbm parameters
     * else compute the disparity map using sgbm parameters
     */
    void on_sbm_clicked();

    /**
     * @brief Undistort the images when the checkBox "Rectified" is checked.
     */
    void on_rectified_clicked();

    /**
     * @brief When the button is clicked, load the calibration file
     */
    void on_calib_clicked();

    /**
     * @brief Display the disparity map
     */
    void on_dispMap_clicked();

    /**
     * @brief Save the current disparity map parameters in the calibration file.
     */
    void on_dispParam_clicked();

    /**
     * @brief Track the images and display the tracker in a new window
     */
    void on_track_clicked();


                                                /*            SLIDERS               */


    /**
     * @brief Modify the value of the disparity parameter IO_preFilterCap according to the slider's one and compute the new disparity map\n
     *  Display also the new value in the corresponding spinBox
     * @param the new IO_preFilterCap's value
     */
    void on_slider_preFilterCap_valueChanged(int value);

    /**
     * @brief Modify the value of the disparity parameter IO_minDisparity according to the slider's one and compute the new disparity map\n
     *  Display also the new value in the corresponding spinBox
     * @param the new IO_minDisparity's value
     */
    void on_slider_minDisparity_valueChanged(int value);

    /**
     * @brief Modify the value of  the disparity parameter IO_uniquenessRatio according to the slider's one and compute the new disparity map\n
     *  Display also the new value in the corresponding spinBox
     * @param the new IO_uniquenessRatio's value
     */
    void on_slider_uniquenessRatio_valueChanged(int value);

    /**
     * @brief Modify the value of IO_speckleWindowSize according to the slider's one and compute the new disparity map\n
     *  Display also the new value in the corresponding spinBox
     * @param the new IO_speckleWindowSize's value
     */
    void on_slider_speckleWindowSize_valueChanged(int value);

    /**
     * @brief Modify the value of IO_speckleRange according to the slider's one and compute the new disparity map\n
     *  Display also the new value in the corresponding spinBox
     * @param the new IO_speckleRange's value
     */
    void on_slider_speckleRange_valueChanged(int value);

    /**
     * @brief Modify the value of IO_disp12MaxDiff according to the slider's one and compute the new disparity map\n
     *  Display also the new value in the corresponding spinBox
     * @param the new IO_disp12MaxDiff's value
     */
    void on_slider_disp12MaxDiff_valueChanged(int value);

    /**
     * @brief Modify the value of IO_textureTreshold according to the slider's one and compute the new disparity map\n
     *  Display also the new value in the corresponding spinBox
     * @param the new value of textureTreshold
     */
    void on_slider_textureTreshold_valueChanged(int value);

    /**
     * @brief Modify the value of IO_tresholdFilter according to the slider's one and compute the new disparity map\n
     *  Display also the new value in the corresponding spinBox
     * @param the new value of IO_tresholdFilter
     */
    void on_slider_treshold_Filter_valueChanged(int value);

    /**
     * @brief Modify the value of IO_sigma(useful only for post_filtering disparity maps) according to the spinbox's one and compute the new disparity map\n
     * @param the new value of IO_sigma
     */
    void on_doubleSpinBox_sigma_valueChanged(double arg1);

    /**
     * @brief Modify the value of IO_lambda(useful only for post_filtering disparity maps) according to the spinbox' one and compute the new disparity map\n
     *
     * @param the new value of the parameter IO_lambda
     */
    void on_spinBox_lambda_2_valueChanged(int arg1);

    /**
     * @brief Save the depthMap(trackWindow)'s values in a file
     * and print the average of it.
     */
    void on_save_depth_map_clicked();

private:
    DispDisplayerMode displayMode;
    Ui::Disparity *ui;
    cv::Mat dispMap, depthMap;
    ImgCv *img, imgtoDisplay;
    std::string calibFilePath;
    int width ;
    int height;
    cv::Rect trackWindow;

};

#endif // DISPARITY_H
