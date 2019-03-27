#ifndef IMAGECV_H
#define IMAGECV_H

// open cv
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>


// Qt
#include <QWidget>
#include <QImage>
#include <QtGui>
#include <QLabel>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>

#include <time.h>

#include "tools/imgcv.h"

namespace imagecv {

    typedef cv::Mat (*function_call)(cv::Mat const& args);
    typedef cv::Mat (*function_call_3_arg)(cv::Mat const& args, cv::Mat* arg2, cv::Mat* arg3);

    /**
    * @brief Load an image with a gui, then automatically split it
    * @param parent widget
    * @param ImgCv
    * @param boolean
    * @return true if an image has been loaded, else false
    */
    bool load_file(QWidget &, ImgCv &, bool= false);


    /**
     * @brief imagecv::speed_test Calculate the time consumption of a function and one cv::Mat parameter
     *
     * @param func the function to test
     * @param args argument of the function
     * @return string containing the result of speed test
     */
    QString speed_test(function_call func, cv::Mat const& args);

    /**
     * @brief imagecv::speed_test Calculate the time consumption of a function with the 3 cv::Mat parameters.
     *
     * @param func the function to test
     * @param args first argument of func
     * @param arg2 second argument of func
     * @param arg3 third argument of third
     * @return string containing the result of speed test
     */
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

    /**
     * @brief imagecv::displayImage display a mat image into a qt frame
     * @param frame The qt frame where you want to display the image
     * @param image the image to display
     */
    void displayImage(QLabel &frame, cv::Mat const& image);

    /**
     * @brief imagecv::displayImage display a mat image into a qt frame
     * @param frame The qt frame where you want to display the image
     * @param image the image to display
     */
    void displayVideo(QLabel &frame, cv::VideoCapture &capL,  cv::VideoCapture &capR, std::string const& calibFilePath = "");

    /**
     * @brief imagecv::displayImage display a mat image into a qt frame
     * @param frame The qt frame where you want to display the image
     * @param image the image to display
     */
    void displayVideo(QLabel &frame, cv::VideoCapture &capL);

}

#endif // IMAGECV_H
