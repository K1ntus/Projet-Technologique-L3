#ifndef IMAGECV_H
#define IMAGECV_H

#include <time.h>
#include <QImage>

#include <opencv2/core.hpp>
#include <QtGui>
#include <QPixmap>
#include <QSlider>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/ximgproc.hpp"
#include "opencv2/ximgproc/disparity_filter.hpp"

using namespace std;

namespace imagecv {

    bool load_file(const QString &fileName, cv::Mat *img_mat);

    typedef cv::Mat (*function_call)(cv::Mat args);
    QString speed_test(function_call func, cv::Mat args);

    typedef cv::Mat (*function_call_3_arg)(cv::Mat args, cv::Mat* arg2, cv::Mat* arg3);
    QString speed_test(function_call_3_arg func, cv::Mat args, cv::Mat* arg2, cv::Mat* arg3);

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

    cv::Mat contour_laplace(cv::Mat img);



    cv::Mat contour_sobel(cv::Mat img);

    /**
     * @brief 'Cut' an image in two new image of width/2
     * @param cv::Mat Image that will be splitted in two
     * @return nothing but store the result in two pointers
     */
    void split(cv::Mat img, cv::Mat *img_left, cv::Mat *img_right);


    cv::Mat orb_features(cv::Mat img, cv::Mat*, cv::Mat*);

    void disparityMapOrbs(cv::Mat img);
}

#endif // IMAGECV_H
