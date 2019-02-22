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

class Disparity : public QWidget
{
    Q_OBJECT

public:
    explicit Disparity(QWidget *parent = nullptr);
    ~Disparity();
    void set_img_mat(ImgCv &img);
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


    void on_checkBox_clicked();

    void on_slider_treshold_Filter_valueChanged(int value);

private:
    Ui::Disparity *ui;
    cv::Mat im1;
    ImgCv *img;
    int width ;
    int height;
};

#endif // DISPARITY_H
