#ifndef DISPARITY_H
#define DISPARITY_H

#include <QWidget>
#include <QPlainTextEdit>

#include "imagecv.h"

namespace Ui {
class Disparity;
}

class Disparity : public QWidget
{
    Q_OBJECT

public:
    explicit Disparity(QWidget *parent = 0);
    ~Disparity();
    void setImg_mat(cv::Mat *img);


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

    cv::Mat disparityMapSGBM();

    bool loadFile(const QString &fileName);

    void on_loadImage_clicked();
    void on_apply_clicked();

private:
    cv::Mat *img_mat;

    cv::Mat *img_left;
    cv::Mat *img_right;
    Ui::Disparity *ui;
};

#endif // DISPARITY_H
