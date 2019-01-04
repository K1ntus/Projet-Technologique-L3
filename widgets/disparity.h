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


namespace Ui {
class Disparity;
}

class Disparity : public QWidget
{
    Q_OBJECT

public:
    explicit Disparity(QWidget *parent = nullptr);
    ~Disparity();
    void set_img_mat(cv::Mat *img);


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

    cv::Mat disparity_map_SGBM();
    cv::Mat disparity_post_filtering();
    cv::Mat sbm(cv::Mat img, cv::Mat *img_left, cv::Mat *img_right);

    bool load_stereo_image(const QString &fileName);

    void on_post_filtering_clicked();
    void on_checkBox_clicked();

private:
    cv::Mat *img_mat;

    cv::Mat *img_left;
    cv::Mat *img_right;
    Ui::Disparity *ui;
    int width ;
    int height;
};

#endif // DISPARITY_H
