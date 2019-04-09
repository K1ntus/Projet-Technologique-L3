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
    void set_img_mat(ImgCv &img);
    void setCalibrationFile(std::string const&calibFile);
    ImgCv * get_img_mat();
    cv::Mat get_depth_map(bool roiTracked = false);

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

    void on_loadImage_clicked();
    void on_reset_image_clicked();
    void displayImage(cv::Mat const&);
    void on_Sobel_clicked();
    void on_Laplace_clicked();
    void on_video_clicked();
    void on_depthMap_clicked();



    void on_slider_preFilterCap_valueChanged(int value);

    void on_slider_minDisparity_valueChanged(int value);

    void on_slider_uniquenessRatio_valueChanged(int value);

    void on_slider_speckleWindowSize_valueChanged(int value);

    void on_slider_speckleRange_valueChanged(int value);

    void on_slider_disp12MaxDiff_valueChanged(int value);

    void on_slider_textureTreshold_valueChanged(int value);

    void on_load_two_images_clicked();

    void on_slider_windowSize_valueChanged(int value);

    void on_slider_numberOfDisparities_valueChanged(int value);


    void on_sbm_clicked();

    void on_slider_treshold_Filter_valueChanged(int value);

    void on_rectified_clicked();

    void on_calib_clicked();

    void on_dispMap_clicked();

    void on_dispParam_clicked();

    void on_track_clicked();

    void on_doubleSpinBox_sigma_valueChanged(double arg1);

    void on_spinBox_lambda_2_valueChanged(int arg1);

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
