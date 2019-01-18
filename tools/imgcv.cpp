#include "imgcv.h"

using namespace cv;

ImgCv::ImgCv():
    cvImg(nullptr),
    cvImgL(nullptr),
    cvImgR(nullptr),
    stereo(false)
{
    cvImg = new Mat();
    cvImgL = new Mat();
    cvImgR = new Mat();
}

ImgCv::ImgCv(std::string filePath, bool is_stereo) : cvImg(nullptr), cvImgL(nullptr), cvImgR(nullptr), stereo(is_stereo)
{
    Mat img = imread(filePath);
    cvImg = new Mat(img);
    cvImgL = new Mat();
    cvImgR = new Mat();
    if(stereo)
        split(*cvImg, *cvImgL, *cvImgR);

}

ImgCv::ImgCv(const Mat &img, bool is_stereo) :cvImg(nullptr), cvImgL(nullptr), cvImgR(nullptr), stereo(is_stereo)
{
    cvImg = new Mat(img);
    cvImgL = new Mat();
    cvImgR = new Mat();
    if(stereo)
        split(*cvImg, *cvImgL, *cvImgR);

}

ImgCv::ImgCv(const Mat &imgL, const Mat &imgR, bool is_stereo) : cvImg(nullptr), cvImgL(nullptr), cvImgR(nullptr), stereo(is_stereo)
{

    cvImgL = new Mat(imgL);
    cvImgR = new Mat(imgR);
    if(stereo){
        // put the left and right image side by side
        int leftWidth = cvImgL->size().width;
        int rightWidth = cvImgR->size().width;
        cvImg = new Mat(cvImgL->size().height, leftWidth + rightWidth, cvImgL->type());
        cvImg->adjustROI(0, 0, 0, -rightWidth);
        cvImgL->copyTo(*cvImg);

        cvImg->adjustROI(0, 0, -leftWidth, rightWidth);
        cvImgR->copyTo(*cvImg);

        cvImg->adjustROI(0, 0, leftWidth, 0);
    }
}

ImgCv::~ImgCv()
{
    delete cvImg;
    cvImg = nullptr;

    delete cvImgL;
    cvImgL = nullptr;

    delete cvImgR;
    cvImgR = nullptr;

}

// static version
Mat ImgCv::contour_laplace(Mat const&img)
{
    Mat gray_img, result, final;
    Mat img_read = img.clone();

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT); // apply the gaussianBlur to smooth the img
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

    Laplacian(gray_img,result,CV_16S,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(result,final,1,0);                          //convert to a CV_8U image
    return final;
}

Mat ImgCv::contour_laplace() const
{
    Mat img_read;
    if(cvImg == nullptr){
        if(cvImgL == nullptr)
            exit(EXIT_FAILURE);
        else
            img_read = cvImgL->clone();
    }else
        img_read = cvImg->clone();
    Mat gray_img, result, final;

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT); // apply the gaussianBlur to smooth the img
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

    Laplacian(gray_img,result,CV_16S,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(result,final,1,0);                          //convert to a CV_8U image
    return final;
}

// static version
cv::Mat ImgCv::contour_sobel(Mat const& img){
    Mat gray_img,final,gx,gy,gx_goodFormat, gy_goodFormat;
    Mat img_read=img.clone();

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT);
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

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
    if(cvImg == nullptr){
        if(cvImgL == nullptr)
            exit(EXIT_FAILURE);
        else
            img_read = cvImgL->clone();
    }else
        img_read = cvImg->clone();

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
    cvtColor(*cvImgL, img_left_gray, CV_BGR2GRAY);
    cvtColor(*cvImgR, img_right_gray, CV_BGR2GRAY);

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
    Mat dst = Mat(cvImg->size(), CV_8U);
    cvtColor(*this->cvImgL, imgL,CV_BGR2GRAY);
    cvtColor(*this->cvImgR,imgR,CV_BGR2GRAY);
    Ptr<StereoBM> matcher= StereoBM::create(IO_numberOfDisparities,IO_SADWindowSize);
    matcher->compute(imgL,imgR,dst);
    dst.convertTo(dst,CV_8U,1,0);
    return dst;
}

//TODO
Mat ImgCv::disparity_post_filtering() {
    int leftWidth = cvImgL->size().width;
    int rightWidth = cvImgR->size().width;
    Mat res(cvImgL->size().height, leftWidth + rightWidth, cvImgL->type());

    res.adjustROI(0, 0, 0, -rightWidth);
    cvImgL->copyTo(res);

    res.adjustROI(0, 0, -leftWidth, rightWidth);
    cvImgR->copyTo(res);

    res.adjustROI(0, 0, leftWidth, 0);
    return res;
}

bool ImgCv::isStereo() const
{
    return stereo;
}

Mat &ImgCv::getImg() const
{
    return *this->cvImg;
}

Mat &ImgCv::getImgR() const
{
    return *this->cvImgR;
}

/**
 * @brief ImgCv::setImg set the image, is the stereo parameters is enable, automatically split them
 *
 * @param newImg image to set
 * @param isStereo check if the image is stereo
 */
void ImgCv::setImg(const Mat &newImg, bool isStereo){
    newImg.copyTo(*cvImg);
    if(isStereo)
        split(*cvImg, *cvImgL, *cvImgR);
    else{
        cvImgL->deallocate();
        cvImgR->deallocate();
    }
}

/**
 * @brief ImgCv::split Split the first parameters into two image of the same width and put them into the two others parameters
 *
 * @param img image to split
 * @param img_left left splitted image
 * @param img_right right splitted image
 */
void ImgCv::split(Mat &img, Mat &img_left, Mat &img_right){
    int width= img.cols/2 ;
    int x_right= width +img.cols%2; //First width position for the right image

    // check if the ptr is already in use
    img.adjustROI(0,0, 0, -x_right);
    img.copyTo(img_left);

    img.adjustROI(0,0, -x_right, x_right);
    img.copyTo(img_right);

    img.adjustROI(0,0, x_right, 0);
}


