#ifndef IMAGECV_H
#define IMAGECV_H

// open cv
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/features2d.hpp>
//#include "opencv2/calib3d.hpp"
//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/core/utility.hpp"


// Qt

#include <QWidget>
#include <time.h>
#include <QImage>
#include <QtGui>
#include <QLabel>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>

#include "tools/imgcv.h"

using namespace std;

namespace imagecv {

    /**
    * @brief Load an image with a gui, then automatically split it
    * @param parent widget
    * @param ImgCv
    * @param boolean
    * @return true if an image has been loaded, else false
    */
    bool load_file(QWidget &, ImgCv &, bool= false);

    typedef cv::Mat (*function_call)(cv::Mat const& args);
    QString speed_test(function_call func, cv::Mat const& args);

    typedef cv::Mat (*function_call_3_arg)(cv::Mat const& args, cv::Mat* arg2, cv::Mat* arg3);
    QString speed_test(function_call_3_arg func, cv::Mat const& args, cv::Mat* arg2, cv::Mat* arg3);

    /**
     * @brief Convert a cv::Mat image to a QImage using the RGB888 format
     * @param src the cv::Mat image to convert
     * @return QImage image
     */
    QImage mat_to_qimage(cv::Mat const& src);

    /**
     * @brief Convert a QImage to a cv::Mat image
     * @param src the QImage to convert
     * @return cv::Mat image
     */
    cv::Mat qimage_to_mat(const QImage& src);

}

#endif // IMAGECV_H
