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


    unsigned int IO_SADWindowSize;
    unsigned int IO_numberOfDisparities;
    unsigned int IO_preFilterCap;
    unsigned int IO_minDisparity;
    unsigned int IO_uniquenessRatio;
    unsigned int IO_speckleWindowSize;
    unsigned int IO_speckleRange;
    int IO_disp12MaxDif;
    unsigned int IO_P1;
    unsigned int IO_P2;

    int IO_full_scale;


private slots:

    void on_loadImage_clicked();
    void on_apply_clicked();
    void on_reset_image_clicked();
    void on_post_filtering_clicked();
    void on_checkBox_clicked();
    void displayImage(cv::Mat const&);

private:
    Ui::Disparity *ui;

    ImgCv *img;
    int width ;
    int height;
};

#endif // DISPARITY_H
