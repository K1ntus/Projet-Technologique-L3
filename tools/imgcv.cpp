#include "imgcv.h"

using namespace cv;

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

// static version
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

// static version
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
    disp.convertTo(disp,CV_8U,1,0);     //Convert the disparity map to a good format and make him convertible to qimage
    disp= cv::Scalar::all(255)-disp;
    return disp;
}

Mat ImgCv::sbm(const Mat &img, const Mat &img_left, const Mat &img_right, const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize)
{
    Mat dst, imgL, imgR;
    dst = Mat(img.size(), CV_8U);
    cvtColor(img_left, imgL,CV_BGR2GRAY);
    cvtColor(img_right,imgR,CV_BGR2GRAY);
    Ptr<StereoBM> matcher= StereoBM::create(IO_numberOfDisparities,IO_SADWindowSize);
    matcher->compute(imgL,imgR,dst);
    dst.convertTo(dst,CV_8U,1,0);
    return dst;
}

Mat ImgCv::sbm(const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize) const
{
    Mat imgL, imgR;
    Mat dst = Mat(this->size(), CV_8U);
    cvtColor(getImgL(), imgL,CV_BGR2GRAY);
    cvtColor(getImgR(),imgR,CV_BGR2GRAY);

    Ptr<StereoBM> matcher= StereoBM::create(IO_numberOfDisparities,IO_SADWindowSize);
    matcher->compute(imgL,imgR,dst);
    dst.convertTo(dst,CV_8U,1,0);
    bitwise_not(dst, dst);
    return dst;
}

//TODO
Mat& ImgCv::disparity_post_filtering() {
    std::cout << "not yet implemented" << std::endl;
    return *this;
}

Mat ImgCv::depthMap(cv::Mat &disparityMap, Mat &dispToDepthMatrix)
{
    Mat depthMapImage;
    reprojectImageTo3D(disparityMap, depthMapImage, dispToDepthMatrix);
    return depthMapImage;

}

bool ImgCv::isStereo() const
{
    return stereo;
}

Mat ImgCv::getImg() const
{
    return this->clone();
}

Mat ImgCv::getImgL() const
{
    Range rows(0, this->rows), columns(0, this->cols >> 1);
    return this->operator()(rows, columns);
}

Mat ImgCv::getImgR() const
{
    Range rows(0, this->rows), columns((this->cols & 1) ? (this->cols >> 1) + this->cols % 2 : this->cols >> 1, this->cols);
    return this->operator()(rows, columns);
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


