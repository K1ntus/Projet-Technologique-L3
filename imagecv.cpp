#include <time.h>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include "imagecv.h"
using namespace std;


template<class T, class U>
float imagecv::speedTest(T (*f),U args){
    clock_t start, end;
    start = clock();
    ((*f)(args));
    end = clock();

    return ((float)(end-start)/CLOCKS_PER_SEC);
}

/**
 * @brief Mat2QImage convert a cv::Mat image to a QImage using the RGB888 format
 * @param src the cv::Mat image to convert
 * @return QImage image
 */
QImage imagecv::Mat2QImage(cv::Mat const& src) {
    cv::Mat temp;  // make the same cv::Mat than src
    if(src.channels()==1)
        cv::cvtColor(src,temp,CV_GRAY2BGR);
    else if(src.channels()==3)
        cv::cvtColor(src,temp,CV_BGR2RGB);
    else
        cv::cvtColor(src,temp,CV_BGRA2RGB);
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits();   // enforce deep copy, see documentation
    dest.convertToFormat(QImage::Format_RGB888);
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )

     return dest;
}

/**
 * @brief QImage2Mat convert a QImage to a cv::Mat image
 * @param src the QImage to convert
 * @return cv::Mat image
 */
cv::Mat imagecv::QImage2Mat(const QImage& src) {
    QImage copy;
    if(src.format() != QImage::Format_RGB888) {
        //qDebug("[INFO] Wrong qimage format. Conversion to RGB888...");
        copy = src.convertToFormat(QImage::Format_RGB888);
        //qDebug("[INFO] Conversion Done");
    } else {
        copy = src;
    }

    cv::Mat mat(copy.height(),copy.width(),CV_8UC3,(uchar*)copy.bits(),copy.bytesPerLine());
    cv::Mat result = cv::Mat(mat.rows, mat.cols, CV_8UC3);
    cv::cvtColor(mat, result, CV_RGB2BGR);  //Convert the mat file to get a layout that qt understand (bgr is default)

    return result;
}

cv::Mat imagecv::contourLaplace(cv::Mat img){
    cv::Mat gray_img, result, final;
    cv::Mat img_read = img.clone();

    cv::GaussianBlur(img_read,img_read,cv::Size(3,3),0,0,cv::BORDER_DEFAULT);
    cv::cvtColor(img_read,gray_img,CV_BGR2GRAY);

    cv::Laplacian(gray_img,result,CV_16S,3,1,0,cv::BORDER_DEFAULT);
    cv::convertScaleAbs(result,final,1,0);
    return final;
}



cv::Mat imagecv::contourSobel(cv::Mat img){
    cv::Mat gray_img,final,gx,gy,gx_goodFormat, gy_goodFormat;
    cv::Mat img_read=img.clone();

    // APPLY THE GAUSSIAN BLUR TO AVOID BLUR
    cv::GaussianBlur(img_read,img_read,cv::Size(3,3),0,0,cv::BORDER_DEFAULT);
    cv::cvtColor(img_read,gray_img,CV_BGR2GRAY); //CONVERT TO GRAY

    cv::Sobel(gray_img,gx,CV_16S,1,0,3,1,0,cv::BORDER_DEFAULT);  // DERIVATIVE IN X
    cv::Sobel(gray_img,gy,CV_16S,0,1,3,1,0,cv::BORDER_DEFAULT);// DERIVATIVE IN Y

    cv::convertScaleAbs(gy,gy_goodFormat,1,0);
    cv::convertScaleAbs(gx,gx_goodFormat,1,0);

    cv::addWeighted(gx_goodFormat,0.5,gy_goodFormat,0.5,0,final,-1); // FINAL GRADIENT = SUM OF X AND Y
    return final;

}

void imagecv::split(cv::Mat img, cv::Mat *img_left, cv::Mat *img_right){
    int x =0;
    int y = 0;
    int height =(int)img.cols/2;
    int width = (int) img.rows;
    int xR = height;
    *img_left = cv::Mat(img, cv::Rect(x,y,height, width));
    *img_right = cv::Mat(img,cv::Rect(xR,y,height,width));
}


void imagecv::orbFeatures(cv::Mat img){
    int nfeatures = 500;
    float scaleFactor =1.2f;
    int nlevels = 8;
    int edgeTreshold=31;
    int firstLevel =0;
    int WTA_K =2;
    int scoreType = cv::ORB::HARRIS_SCORE;
    int patchSize = 31;
    cv::Mat grayImage;
    cv::cvtColor(img, grayImage,CV_BGR2GRAY);
   // ORB detector = ORB(nfeatures,scaleFactor,nlevels,edgeTreshold,firstLevel,WTA_K,scoreType,patchSize);
    vector<cv::KeyPoint> keypoint;
    //detector.detect(grayImage, keypoint);
    cv::Mat dst;
    cv::drawKeypoints(img,keypoint,dst,-1,cv::DrawMatchesFlags::DEFAULT);
    imshow("OrbDetector",dst);
}

void imagecv::disparityMapOrbs(cv::Mat img){
    //TODO
    //cv::Mat res = new cv::Mat;
    //return res;
    return;
}

