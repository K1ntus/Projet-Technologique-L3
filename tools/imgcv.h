#ifndef IMGCV_H
#define IMGCV_H

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/ximgproc.hpp"
#include <iostream>

class ImgCv : public cv::Mat
{
public:

    ImgCv();
    ImgCv(std::string, bool = false);
    ImgCv(cv::Mat const&, bool = false);
    ImgCv(cv::Mat const&, cv::Mat const&, bool = true);
    ~ImgCv();

    bool isStereo() const;

    cv::Mat getImg() const;
    cv::Mat getImgL() const;
    cv::Mat getImgR() const;

    void setImg(cv::Mat const&, bool);

    void setImg(cv::Mat const&imgL, cv::Mat const&imgR);
    /**
     * @brief 'Cut' an image in two new image of width/2
     * @param cv::Mat Image that will be splitted in two
     * @return nothing but store the result in two pointers
     */
    void static split(cv::Mat &, cv::Mat&, cv::Mat&);

    /** static version of the method
     * @brief Convert an image following the laplace algorithm
     * @param Image to convert
     * @return The parameters converted with laplace algorithm
     */
    cv::Mat static contour_laplace(cv::Mat const&);

    /**
    * @brief Convert an image following the laplace algorithm
    * @return The parameters converted with laplace algorithm
    */
    cv::Mat contour_laplace() const;

    /** static version of the method
     * @brief Convert an image following the sobel algorithm
     * @param Image to convert
     * @return The parameters converted with sobel algorithm
     */
    cv::Mat static contour_sobel(cv::Mat const& img);

    /**
     * @brief convert an image following the sobel algorithm
     * @return The parameters converted with sobel algorithm
     */
    cv::Mat contour_sobel() const;

    /**
     * @brief Display a disparity map using sbm parameters
     * @return The disparity map using sbm and the left and right image og the object
     */
    cv::Mat sbm(size_t const& IO_numberOfDisparities= 144, size_t const& IO_SADWindowSize= 9) const;

    /**
     * @brief Generate the disparity map from two cv::mat pointer of a stereo image
     * &param size_t
     * @return disparity map
     */
    cv::Mat disparity_map_SGBM(size_t const&, size_t const&, size_t const&,
                               size_t const&, size_t const&, int const&,
                               size_t const&, size_t const&, size_t const&,
                               size_t const&, int const&) const;

    /**
     * @brief disparity_post_filtering
     * @return the post_filtered disparity map using SBM
     */
    cv::Mat disparity_post_filtering(const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize);

    /**
     * @brief disparity_post_filtering
     * @return the post_filtered disparity map using SGBM
     */
    cv::Mat disparity_post_filtering(const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize, const size_t &IO_preFilterCap, const size_t &IO_P1, const size_t &IO_P2);
    /**
     * @brief depthMap :  computes the depth map
     * @param disparityMap
     * @param dispToDepthMatrix
     * @return the depth map
     */
    cv::Mat depthMap(cv::Mat &disparityMap, cv::Mat &dispToDepthMatrix);


private:

    bool stereo;

};

#endif // IMGCV_H
