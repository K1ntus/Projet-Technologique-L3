#include "imgcv.h"

using namespace cv;

ImgCv::ImgCv(std::string filePath, bool is_stereo) : cvImg(nullptr), cvImgL(nullptr), cvImgR(nullptr), stereo(is_stereo)
{
    Mat img = imread(filePath);
    cvImg = new Mat(img);
    if(stereo){
        cvImgL = new Mat();
        cvImgR = new Mat();
    }

}

ImgCv::ImgCv(const Mat &img, bool is_stereo) :cvImg(nullptr), cvImgL(nullptr), cvImgR(nullptr), stereo(is_stereo)
{
    cvImg = new Mat(img);
    if(stereo){
        split(*cvImg, cvImgL, cvImgR);
    }

}

ImgCv::ImgCv(const Mat &imgL, const Mat &imgR, bool is_stereo) : cvImg(nullptr), cvImgL(nullptr), cvImgR(nullptr), stereo(is_stereo)
{
    if(stereo){
        cvImgL = new Mat(imgL);
        cvImgR = new Mat(imgR);
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

void ImgCv::split(Mat const& img, Mat *img_left, Mat *img_right)
{
    int x = 0;
    int y = 0;
    int width= img.cols/2 ;
    int height= img.rows;
    int x_right= width +img.cols%2; //First width position for the right image

    // check if the ptr is already in use
    if(img_left != nullptr)
        delete img_left;
    if(img_right != nullptr)
        delete  img_right;
    //Store the result in two pointer of this class
    *img_left = Mat(img, Rect(x,y,width, height));
    *img_right = Mat(img,Rect(x_right,y,width,height));
}


