#ifndef IMAGECV_H
#define IMAGECV_H

#include <time.h>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

using namespace std;

namespace imagecv {

    template<class T, class U>
    float speedTest(T (*f),U args);

    /**
     * @brief Mat2QImage convert a cv::Mat image to a QImage using the RGB888 format
     * @param src the cv::Mat image to convert
     * @return QImage image
     */
    QImage Mat2QImage(cv::Mat const& src);

    /**
     * @brief QImage2Mat convert a QImage to a cv::Mat image
     * @param src the QImage to convert
     * @return cv::Mat image
     */
    cv::Mat QImage2Mat(const QImage& src);

    cv::Mat contourLaplace(cv::Mat img);



    cv::Mat contourSobel(cv::Mat img);

    void split(cv::Mat img, cv::Mat *img_left, cv::Mat *img_right);


    void orbFeatures(cv::Mat img);

    void disparityMapOrbs(cv::Mat img);
}

#endif // IMAGECV_H
