#include <time.h>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include "imagecv.h"
using namespace std;
using namespace cv;

QString imagecv::speed_test(function_call func, cv::Mat args){
    clock_t start, end;
    start = clock();
    func(args);
    end = clock();
    QString str = "result of speed test: " + QString::number(((float)(end-start)/CLOCKS_PER_SEC));
    return str;
}

QString imagecv::speed_test(function_call_3_arg func, cv::Mat args, cv::Mat* arg2, cv::Mat* arg3){
    clock_t start, end;
    start = clock();
    func(args, arg2, arg3);
    end = clock();
    QString str = "result of speed test: " + QString::number(((float)(end-start)/CLOCKS_PER_SEC));
    return str;
}

bool imagecv::load_file(const QString &fileName , Mat *img_mat) {
    QImage my_image(fileName, "PNM");                                //load the file in  a QImage variable (pnm is a format like ttif, ...)
    qDebug(" *** Image file correctly loaded *** ");

    *img_mat = qimage_to_mat(my_image);         //Convert QImage to cv::mat

    return true;
}

/**
 * @brief Convert a cv::Mat image to a QImage using the RGB888 format
 * @param src the cv::Mat image to convert
 * @return QImage image
 */
QImage imagecv::mat_to_qimage(Mat const& src) {
    Mat temp;  // make the same cv::Mat than src
    if(src.channels()==1)
        cvtColor(src,temp,CV_GRAY2BGR);
    else if(src.channels()==3)
        cvtColor(src,temp,CV_BGR2RGB);
    else
        cvtColor(src,temp,CV_BGRA2RGB);
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits();   // enforce deep copy, see documentation
    dest.convertToFormat(QImage::Format_RGB888);

    return dest;
}


/**
 * @brief Convert a QImage to a cv::Mat image
 * @param src the QImage to convert
 * @return cv::Mat image
 */
Mat imagecv::qimage_to_mat(const QImage& src) {
    QImage copy;
    if(src.format() != QImage::Format_RGB888) {
        //qDebug("[INFO] Wrong qimage format. Conversion to RGB888...");
        copy = src.convertToFormat(QImage::Format_RGB888);
        //qDebug("[INFO] Conversion Done");
    } else {
        copy = src;
    }

    Mat mat(copy.height(),copy.width(),CV_8UC3,(uchar*)copy.bits(),copy.bytesPerLine());
    Mat result = Mat(mat.rows, mat.cols, CV_8UC3);
    cvtColor(mat, result, CV_RGB2BGR);  //Convert the mat file to get a layout that qt understand (bgr is default)

    return result;
}
/**
 * @brief Convert an image following the laplace algorithm
 * @param Image to convert
 * @return The parameters converted with laplace algorithm
 */
cv::Mat imagecv::contour_laplace(Mat img){
    Mat gray_img, result, final;
    Mat img_read = img.clone();

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT); // apply the gaussianBlur to smooth the img
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

    Laplacian(gray_img,result,CV_16S,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(result,final,1,0);                          //convert to a CV_8U image
    return final;
}

/**
 * @brief Convert an image following the sobel algorithm
 * @param Image to convert
 * @return The parameters converted with sobel algorithm
 */
cv::Mat imagecv::contour_sobel(Mat img){
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
/**
 * @brief 'Cut' an image in two new image of width/2
 * @param cv::Mat Image that will be splitted in two
 * @return nothing but store the result in two pointers
 */
void imagecv::split(Mat img, Mat *img_left, Mat *img_right){
    int x = 0;
    int y = 0;
    int width=(int)img.cols/2 ;
    int height= (int) img.rows;
    int x_right=width +img.cols%2; //First width position for the right image


    //Store the result in two pointer of this class
    *img_left = Mat(img, Rect(x,y,width, height));
    *img_right = Mat(img,Rect(x_right,y,width,height));
}

/**
 * @brief Display a disparity map using sbm parameters
 * @param img_left the left point of view of a scene
 * @param img_right the right point of view of a scene
 * @return The image with the similarities displayed
 */
Mat imagecv::sbm(Mat img, Mat *img_left, Mat *img_right){
    Mat dst, imgL, imgR;
    cvtColor(*img_left, imgL,CV_BGR2GRAY);
    cvtColor(*img_right,imgR,CV_BGR2GRAY);
    int window_size = 21;
    int numDisparity = 32; // must be a 16's multiple
    Ptr<StereoBM> matcher= StereoBM::create(numDisparity,window_size);
    matcher->compute(imgL,imgR,dst);
     dst.convertTo(dst,CV_8U,1,0);
     return dst;
}

Mat* imagecv::calibration(vector<Mat> imgs){
    size_t nb_image = imgs.size();
    int nb_lines = 6;
    int nb_columns = 9;
    int nb_squares = nb_lines * nb_columns;
    Size board_size = Size(nb_lines, nb_columns);
    int squareSize = 2;

    // 3D coordinates of chessboard points
    vector<vector<Point3f>> object_points;
    vector<vector<Point2f>> image_points;
    vector<Point2f> corners;
    vector<Point3f> obj;
    int nb_success = 0;

    Mat image;
    Mat gray_image;

    for(int i = 0; i < nb_squares; i++)
        obj.push_back(Point3f(i/nb_columns, i%nb_lines, 0.0f));

    int im = 0;
    while(nb_success<nb_image){
        image = imgs[im%nb_image];
        cvtColor(image, gray_image, CV_BGR2GRAY);
        bool found = findChessboardCorners(image, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

        if(found){
            cornerSubPix(gray_image, corners, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            drawChessboardCorners(gray_image, board_size, corners, found);
        }

        if(found!=0){
            image_points.push_back(corners);
            object_points.push_back(obj);

            nb_success++;
            if(nb_success >= nb_image)
                break;
        }

        im++;
    }

    Mat distCoeffs;
    Mat cameraMatrix = Mat(3, 3, CV_32FC1);
    cameraMatrix.ptr<float>(0)[0] = 1;
    cameraMatrix.ptr<float>(1)[1] = 1;

    vector<Mat> rvecs, tvecs;

    calibrateCamera(object_points, image_points, image.size(), cameraMatrix, distCoeffs, rvecs, tvecs);


    Mat image_undistorted;
    undistort(image, image_undistorted, cameraMatrix, distCoeffs);

    imshow("calibration points visible", image);
    imshow("undirstorted", image_undistorted);

    Mat res[2] = {distCoeffs, cameraMatrix};

    return res;
}
