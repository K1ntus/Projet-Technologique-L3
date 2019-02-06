#include "imgcv.h"

using namespace cv;
using namespace cv::ximgproc;

ImgCv::ImgCv():
    Mat(),
    stereo(false)
{}

ImgCv::ImgCv(std::string filePath, bool is_stereo) :  Mat(), stereo(is_stereo)
{
    Mat img = imread(filePath);
    img.copyTo(*this);

}

ImgCv::ImgCv(const Mat &img, bool is_stereo) :Mat(img), stereo(is_stereo)
{}

ImgCv::ImgCv(const Mat &imgL, const Mat &imgR, bool is_stereo) : Mat(), stereo(is_stereo)
{


    // put the left and right image side by side
    int leftWidth = imgL.size().width;
    int rightWidth = imgR.size().width;
    cv::Mat cvImg(imgL.size().height, leftWidth + rightWidth, imgL.type());
    cvImg.adjustROI(0, 0, 0, -rightWidth);
    imgL.copyTo(cvImg);

    cvImg.adjustROI(0, 0, -leftWidth, rightWidth);
    imgR.copyTo(cvImg);

    cvImg.adjustROI(0, 0, leftWidth, 0);
    cvImg.copyTo(*this);
}

ImgCv::~ImgCv()
{}

/**
 * @brief ImgCv::contour_laplace :  Calculate the Laplacian filter to draw picture's countours
 * @param img :  the image loaded
 * @return the converted image
 */
Mat ImgCv::contour_laplace(Mat const&img)
{
    Mat gray_img, result, final;
    Mat img_read = img.clone();


    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT); // apply the gaussianBlur to smooth the img
    if(img_read.channels()!=1)
        cvtColor(img_read,gray_img,CV_BGR2GRAY);
    else
        gray_img=img_read.clone();

    Laplacian(gray_img,result,CV_16S,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(result,final,1,0);                          //convert to a CV_8U image
    return final;
}

/**
* @brief Convert an image following the laplace algorithm
* @return The parameters converted with laplace algorithm
*/

Mat ImgCv::contour_laplace() const
{
    Mat img_read;
    if(this->empty()){
        std::cout << "img is empty" << std::endl;
        return img_read;
    }else
        img_read = this->clone();
    Mat gray_img, result, final;

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT); // apply the gaussianBlur to smooth the img
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

    Laplacian(gray_img,result,CV_16S,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(result,final,1,0);                          //convert to a CV_8U image
    return final;
}

/**
 * @brief ImgCv::contour_sobel :  convert an image using the Sobel filter. Permits to detect the contours of an image.
 * @param img :  the loaded image
 * @return the converted image
 */
Mat ImgCv::contour_sobel(const Mat &img){
    Mat gray_img,final,gx,gy,gx_goodFormat, gy_goodFormat;
    Mat img_read=img.clone();

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT);
    if(img_read.channels()!=1)
        cvtColor(img_read,gray_img,CV_BGR2GRAY);
    else
        gray_img=img_read.clone();

    Sobel(gray_img,gx,CV_16S,1,0,3,1,0,BORDER_DEFAULT);  // derivative in x
    Sobel(gray_img,gy,CV_16S,0,1,3,1,0,BORDER_DEFAULT);// derivative in y

    convertScaleAbs(gy,gy_goodFormat,1,0);
    convertScaleAbs(gx,gx_goodFormat,1,0);

    addWeighted(gx_goodFormat,0.5,gy_goodFormat,0.5,0,final,-1); // final gradient is the addition of the two gradients
    return final;

}
/**
* @brief Convert an image following the sobel algorithm
* @param Image to convert
*  @return The parameters converted with sobel algorithm
*/
Mat ImgCv::contour_sobel() const
{
    Mat img_read;
    if(this->empty()){
        std::cout << "img is empty" << std::endl;
        return img_read;

    }else
        img_read = this->clone();

    Mat gray_img,final,gx,gy,gx_goodFormat, gy_goodFormat;

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT);
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

    Sobel(gray_img,gx,CV_16S,1,0,3,1,0,BORDER_DEFAULT);  // derivative in x
    Sobel(gray_img,gy,CV_16S,0,1,3,1,0,BORDER_DEFAULT);// derivative in y

    convertScaleAbs(gy,gy_goodFormat,1,0);
    convertScaleAbs(gx,gx_goodFormat,1,0);

    addWeighted(gx_goodFormat,0.5,gy_goodFormat,0.5,0,final,-1); // final gradient is the addition of the two gradients
    return final;
}

/**
 * @brief ImgCv::disparity_map_SGBM : compute a disparity map, using SGBM (a block matching method)\n
 * @param IO_minDisparity : the minimal disparity\n
 * @param IO_numberOfDisparities
 * @param IO_SADWindowSize
 * @param IO_P1
 * @param IO_P2
 * @param IO_disp12MaxDif
 * @param IO_preFilterCap
 * @param IO_uniquenessRatio
 * @param IO_speckleWindowSize
 * @param IO_speckleRange
 * @param IO_full_scale\n
 * @return the disparity map computed.
 */
Mat ImgCv::disparity_map_SGBM(const size_t &IO_minDisparity, const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize,
                              const size_t &IO_P1, const size_t &IO_P2, const int &IO_disp12MaxDif,
                              const size_t &IO_preFilterCap, const size_t &IO_uniquenessRatio, const size_t &IO_speckleWindowSize,
                              const size_t &IO_speckleRange, const int &IO_full_scale) const
{
    Mat img_right_gray, img_left_gray;
    Mat disp;

    //Convert the two stereo image in gray
    cvtColor(getImgL(), img_left_gray, CV_BGR2GRAY);
    cvtColor(getImgR(),img_right_gray, CV_BGR2GRAY);

    Ptr<StereoSGBM> sgbm = StereoSGBM::create(
                IO_minDisparity,
                IO_numberOfDisparities,
                IO_SADWindowSize,
                IO_P1,
                IO_P2,
                IO_disp12MaxDif,
                IO_preFilterCap,
                IO_uniquenessRatio,
                IO_speckleWindowSize,
                IO_speckleRange,
                IO_full_scale
                );
    sgbm->compute(img_left_gray, img_right_gray, disp);    //Generate the disparity map
//    disp.convertTo(disp,CV_8U,1,0);     //Convert the disparity map to a good format and make him convertible to qimage
    disp.convertTo(disp, CV_8U, 255/(IO_numberOfDisparities*16.));
    bitwise_not(disp, disp);
    return disp;
}
/**
 * @brief ImgCv::sbm : Compute the disparity map using the SBM method (a block matching method)
 * @param IO_numberOfDisparities : the number of disparities
 * @param IO_SADWindowSize :  the size of the block
 * @return the disparity map
 */
Mat ImgCv::sbm(const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize) const
{
    Mat imgL, imgR;
    Mat dst = Mat(this->size(), CV_8U);
    cvtColor(getImgL(), imgL,CV_BGR2GRAY);
    cvtColor(getImgR(),imgR,CV_BGR2GRAY);

    Ptr<StereoBM> matcher= StereoBM::create(IO_numberOfDisparities,IO_SADWindowSize);
    matcher->compute(imgL,imgR,dst);
//    dst.convertTo(dst,CV_8U,1,0);
    dst.convertTo(dst, CV_8U, 255/(IO_numberOfDisparities*16.));
    bitwise_not(dst, dst);
    return dst;
}

/**
 * @brief ImgCv::disparity_post_filtering : Filter the disparity map
 * @return the filtered disparity map
 */
Mat ImgCv::disparity_post_filtering(const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize) {

    Mat left_disparity, right_disparity, filtered, left_for_matching, right_for_matching;
    Mat final_disparity_map;
    cv::resize(getImgL(),left_for_matching,Size(),0.5,0.5);//reduce the image's dimensions
    cv::resize(getImgR(),right_for_matching,Size(),0.5,0.5);

    Ptr<StereoBM> matcher_left = StereoBM::create(IO_numberOfDisparities, IO_SADWindowSize); // use StereoBM to create a matcher
    Ptr<DisparityWLSFilter> filter = cv::ximgproc::createDisparityWLSFilter(matcher_left); // creation of the filter
    Ptr<StereoMatcher> matcher_right= createRightMatcher(matcher_left);// creation of the right matcher

    cv::cvtColor(left_for_matching,left_for_matching,COLOR_BGR2GRAY);
    cv::cvtColor(right_for_matching, right_for_matching, COLOR_BGR2GRAY);

    matcher_left->compute(left_for_matching,right_for_matching,left_disparity);    // compute the left disparity map
    matcher_right->compute(right_for_matching,left_for_matching, right_disparity); // compute the right disparity map

    filter->setLambda(8000);// lambda defining regularization of the filter.  With a high value, the edge of the disparity map will "more" match with the source image
    filter->setSigmaColor(0.8); // sigma represents the sensitivity of the filter

    filter->filter(left_disparity,getImgL(),filtered,right_disparity); // apply the filter

    cv::ximgproc::getDisparityVis(filtered,final_disparity_map, 6.0);// permits to visualize the disparity map
    cv::bitwise_not(final_disparity_map, final_disparity_map);// reverse black & white colors
    return final_disparity_map;

}
/**
 * @brief ImgCv::disparity_post_filtering: applies a filter on a disparity map compute with SGBM
 * @return the disparity map post_filtered
 */
Mat ImgCv::disparity_post_filtering(const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize, const size_t &IO_preFilterCap, const size_t &IO_P1, const size_t &IO_P2){

    Mat left_disparity, right_disparity, filtered, left_for_matching, right_for_matching;
    Mat final_disparity_map;

    cv::resize(getImgL(),left_for_matching,Size(),0.5,0.5); //reduce the image's dimensions
    cv::resize(getImgR(),right_for_matching,Size(),0.5,0.5);

    Ptr <StereoSGBM> matcher_left = StereoSGBM::create(0,IO_numberOfDisparities,IO_SADWindowSize); // use SBM to create a matcher

    matcher_left->setPreFilterCap(IO_preFilterCap);
    matcher_left->setP1(IO_P1); // smoothness of the disparity map
    matcher_left->setP2(IO_P2);

    Ptr<DisparityWLSFilter> filter = cv::ximgproc::createDisparityWLSFilter(matcher_left); // creates the WLSfilter
    Ptr<StereoMatcher> matcher_right= createRightMatcher(matcher_left);


    matcher_left->compute(left_for_matching,right_for_matching,left_disparity); // compute the left disparity map
    matcher_right->compute(right_for_matching,left_for_matching, right_disparity); // compute the right disparity map

    filter->setLambda(8000);
    filter->setSigmaColor(1.0);

    filter->filter(left_disparity,getImgL(),filtered,right_disparity);
    cv::ximgproc::getDisparityVis(filtered,final_disparity_map, 5.0);
    cv::bitwise_not(final_disparity_map, final_disparity_map); // reverse black and white
    return final_disparity_map;

}

ImgCv ImgCv::rectifiedImage(ImgCv &distortedImage, const IntrinsicParameters &paramL, const IntrinsicParameters &paramR, const Mat &R, const Mat &T) const
{
    return rectifiedImage(distortedImage, paramL.getDistCoeffs(), paramL.getCameraMatrix(),
                          paramR.getDistCoeffs(), paramR.getCameraMatrix(), R, T);
}

ImgCv ImgCv::rectifiedImage(ImgCv &distortedImage, cv::Mat const&dist_coeffsL, cv::Mat const&camera_matrixL,
                            cv::Mat const&dist_coeffsR, cv::Mat const&camera_matrixR,
                            cv::Mat const&R, cv::Mat const&T) const
{
    if(!distortedImage.isStereo())
        std::cout << "Error: the image is not a stereo file. \nin: Imgcv::rectifiedImage" << std::endl;

    ImgCv rectifiedImg;

    cv::Mat imgLrectified, imgRrectified,
            rotL, rotR, projL, projR, dispToDepthMat;
    cv::Mat const &imgL = distortedImage.getImgL();
    cv::Mat const &imgR = distortedImage.getImgR();

    cv::Rect roi1, roi2;
    cv::Size imgSize(imgL.size());
    cv::stereoRectify(camera_matrixL, dist_coeffsL, camera_matrixR, dist_coeffsR,
                      imgSize, R, T, rotL, rotR, projL, projR, dispToDepthMat,
                      cv::CALIB_ZERO_DISPARITY, -1, imgSize, &roi1, &roi2);


    cv::Mat map11, map12, map21, map22;
    cv::initUndistortRectifyMap(camera_matrixL, dist_coeffsL, rotL, projL, imgSize, CV_16SC2, map11, map12);
    cv::initUndistortRectifyMap(camera_matrixR, dist_coeffsR, rotR, projR, imgSize, CV_16SC2, map21, map22);


    cv::remap(imgL, imgLrectified, map11, map12, cv::INTER_LINEAR);
    cv::remap(imgR, imgRrectified, map21, map22, cv::INTER_LINEAR);


    cv::Rect vroi1(cvRound(roi1.x), cvRound(roi1.y),
                   cvRound(roi1.width), cvRound(roi1.height));



    cv::Rect vroi2(cvRound(roi2.x), cvRound(roi2.y),
                   cvRound(roi2.width), cvRound(roi2.height));

    rectifiedImg.setImg(imgLrectified(vroi1), imgRrectified(vroi2));

    return rectifiedImg;
}

ImgCv ImgCv::rectifiedImage(ImgCv &distortedImage, const std::string &outFile) const
{
    ImgCv rectifiedImg;

    if(!distortedImage.isStereo())
        std::cout << "Error: the image is not a stereo file. \nin: Imgcv::rectifiedImage" << std::endl;
    FileStorage fs(outFile, FileStorage::READ);
    if(fs.isOpened()){
        cv::Mat imgLrectified, imgRrectified,
                dist_coeffsL, camera_matrixL,
                dist_coeffsR, camera_matrixR,
                R, T, rotL, rotR, projL, projR, dispToDepthMat;

        fs["cameraMatrixLeft"] >> camera_matrixL;
        fs["distCoefficientsMatrixLeft"] >> dist_coeffsL;
        fs["cameraMatrixRight"] >> camera_matrixR;
        fs["distCoefficientsMatrixRight"] >> dist_coeffsR;
        fs["rotationMatrix"] >> R;
        fs["translationMatrix"] >> T;

        fs.release();

        rectifiedImg = rectifiedImage(distortedImage, dist_coeffsL, camera_matrixL,
                                      dist_coeffsR, camera_matrixR,
                                      R, T);
    }else
        std::cout << "Error: Couldn't open file. \nin: Imgcv::rectifiedImage" << std::endl;

    return rectifiedImg;

}

Mat ImgCv::getDispToDepthMat(const std::string &outFile)
{
    cv::Mat dispToDepthMat;
    FileStorage fs(outFile, FileStorage::READ);
    if(fs.isOpened()){

        fs["dispToDepthMatrix"] >> dispToDepthMat;

        fs.release();

    }else
        std::cout << "Error: Couldn't open file. \nin: Imgcv::rectifiedImage" << std::endl;
    return dispToDepthMat;
}

/**
 * @brief ImgCv::depthMap :  Compute the depth map using the disparity and the camera parameters\n
 * @param disparityMap
 * @param dispToDepthMatrix
 * @return the depth map
 */
Mat ImgCv::depthMap(const Mat &disparityMap, Mat &dispToDepthMatrix)
{
    Mat depthMapImage;
    reprojectImageTo3D(disparityMap, depthMapImage, dispToDepthMatrix);
    return depthMapImage;

}
/**
 * @brief ImgCv::isStereo : Check if
 * @return
 */
bool ImgCv::isStereo() const
{
    return stereo;
}
/**
 * @brief ImgCv::getImg : Clone an image and recover it.
 * @return a clone of the image loaded
 */
Mat ImgCv::getImg() const
{
    return this->clone();
}
/**
 * @brief ImgCv::getImgL : Get the left side of a stereo image
 * @return  the left image
 */
Mat ImgCv::getImgL() const
{
    Range rows(0, this->rows), columns(0, this->cols >> 1);
    return this->operator()(rows, columns);
}
/**
 * @brief ImgCv::getImgR : Get the right side of an image
 * @return the right image
 */
Mat ImgCv::getImgR() const
{
    Range rows(0, this->rows), columns((this->cols & 1) ? (this->cols >> 1) + this->cols % 2 : this->cols >> 1, this->cols);
    return this->operator()(rows, columns);
}

Mat ImgCv::getDisparityMap()
{
    return sbm(((this->size().width >> 3) + 15) & -16 , 15);
}

Mat ImgCv::getDepthMap(Mat &TProjectionMat)
{

    return depthMap(getDisparityMap(), TProjectionMat);
}

/**
 * @brief ImgCv::setImg set the image, is the stereo parameters is enable, automatically split them
 *
 * @param newImg image to set
 * @param isStereo check if the image is stereo
 */
void ImgCv::setImg(const Mat &img, bool isStereo){
    img.copyTo(*this);
    stereo = isStereo;
}

void ImgCv::setImg(const Mat &imgL, const Mat &imgR)
{

    const int& leftWidth = imgL.cols;
    const int& rightWidth = imgR.cols;
    Mat res(imgL.rows, leftWidth + rightWidth,imgL.type());

    res.adjustROI(0, 0, 0, -rightWidth);
    imgL.copyTo(res);

    res.adjustROI(0, 0, -leftWidth, rightWidth);
    imgR.copyTo(res);

    res.adjustROI(0, 0, leftWidth, 0);
    res.copyTo(*this);
    stereo = true;
}

/**
 * @brief ImgCv::split Split the first parameters into two image of the same width and put them into the two others parameters
 *
 * @param img image to split
 * @param img_left left splitted image
 * @param img_right right splitted image
 */
void ImgCv::split(Mat &img, Mat &img_left, Mat &img_right){
    int width= img.cols >> 1 ;
    int x_right= width +img.cols%2; //First width position for the right image

    // check if the ptr is already in use
    img.adjustROI(0,0, 0, -x_right);
    img.copyTo(img_left);

    img.adjustROI(0,0, -x_right, x_right);
    img.copyTo(img_right);

    img.adjustROI(0,0, x_right, 0);
}


