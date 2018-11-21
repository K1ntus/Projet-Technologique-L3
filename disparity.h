#ifndef DISPARITY_H
#define DISPARITY_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

namespace Ui {
class Disparity;
}

class Disparity : public QWidget
{
    Q_OBJECT

public:
    explicit Disparity(QWidget *parent = 0);
    ~Disparity();



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


    void split(cv::Mat img);
    cv::Mat disparity_map_SGBM();

    bool load_stereo_image(const QString &fileName);

    cv::Mat qImage_to_mat(QImage const& src);
    QImage mat_to_qImage(cv::Mat const& src);


private:
    cv::Mat *img_mat;

    cv::Mat *img_left;
    cv::Mat *img_right;
    Ui::Disparity *ui;
    int width ;
    int height;
};

#endif // DISPARITY_H
