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

    bool IO_fullDP;


private slots:
    void split(cv::Mat img);
    cv::Mat disparityMapSGBM();

    bool loadFile(const QString &fileName);

    cv::Mat QImage2Mat(QImage const& src);
    QImage Mat2QImage(cv::Mat const& src);

    void on_loadImage_clicked();
    void on_apply_clicked();

private:
    cv::Mat *img_mat;

    cv::Mat *img_left;
    cv::Mat *img_right;
    Ui::Disparity *ui;
};

#endif // DISPARITY_H
