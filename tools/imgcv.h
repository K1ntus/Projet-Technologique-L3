#ifndef IMGCV_H
#define IMGCV_H

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/utility.hpp"

class ImgCv
{
public:
    ImgCv();
    ImgCv(std::string, bool);
    ImgCv(cv::Mat const&, bool);
    ImgCv(cv::Mat const&, cv::Mat const&, bool);
    ~ImgCv();

    /**
     * @brief 'Cut' an image in two new image of width/2
     * @param cv::Mat Image that will be splitted in two
     * @return nothing but store the result in two pointers
     */
    void static split(cv::Mat const&, cv::Mat*, cv::Mat*);

    /**
     * @brief Convert an image following the laplace algorithm
     * @param Image to convert
     * @return The parameters converted with laplace algorithm
     */
    cv::Mat static contour_laplace(cv::Mat const&);

    cv::Mat contour_laplace() const;

    /**
     * @brief Convert an image following the sobel algorithm
     * @param Image to convert
     * @return The parameters converted with sobel algorithm
     */
    cv::Mat static contour_sobel(cv::Mat const& img);

    cv::Mat contour_sobel() const;

    bool isStereo() const;

    cv::Mat getImg() const;

    void setImg(cv::Mat const&, bool);


private:

    cv::Mat *cvImg, *cvImgL, *cvImgR;
    bool stereo;

};

#endif // IMGCV_H
