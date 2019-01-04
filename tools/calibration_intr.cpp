#include "calibration_intr.h"

using namespace std;
using namespace cv;
using namespace imagecv;

Calibration_intr::Calibration_intr(std::vector<cv::Mat> &imgs)
{
    nb_lines = 6;
    nb_columns = 9;
    board_size = Size(nb_lines, nb_columns);
    img = new Mat;
    gray_image = new Mat;
    object_points = new vector<vector<Point3f>>;
    image_points = new vector<vector<Point2f>>;
    dist_coeffs = new Mat;
    camera_matrix = new Mat(3, 3, CV_32FC1);
    camera_matrix->ptr<float>(0)[0] = 1;
    camera_matrix->ptr<float>(1)[1] = 1;
    rvecs = new std::vector<cv::Mat>;
    tvecs = new std::vector<cv::Mat>;

    calibrate(imgs);

}

Calibration_intr::~Calibration_intr()
{
    delete img;
}

Mat* Calibration_intr::get_image_origin(){
    return img;
}

Mat* Calibration_intr::get_gray_image(){
    return gray_image;
}

Mat* Calibration_intr::get_camera_matrix(){
    return camera_matrix;
}

Mat* Calibration_intr::get_dist_coeffs(){
    return dist_coeffs;
}

vector<Mat>* Calibration_intr::get_rvecs(){
    return rvecs;
}

vector<Mat>* Calibration_intr::get_tvecs(){
    return tvecs;
}



bool Calibration_intr::find_chessboard_corners(std::vector<cv::Point2f>& corners){

    cvtColor(*img, *gray_image, CV_BGR2GRAY);
    bool found = findChessboardCorners(*img, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

    if(found){
        cornerSubPix(*gray_image, corners, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        drawChessboardCorners(*gray_image, board_size, corners, found);
    }

    return found;
}

void Calibration_intr::calibrate(std::vector<cv::Mat> &imgs){
    int nb_squares = nb_lines * nb_columns;
    int squareSize = 2;

    // 3D coordinates of chessboard points

    vector<Point3f> obj;


    for(int i = 0; i < nb_squares; i++)
        obj.push_back(Point3f(i/nb_lines, i%nb_lines, 0.0f));

    size_t nb_image = imgs.size();
    vector<Point2f> corners;

    unsigned int im = 0;
    unsigned int nb_success = 0;
    while(nb_success<nb_image){
        *img = imgs[im%nb_image];

       if(find_chessboard_corners(corners)){
            image_points->push_back(corners);
            object_points->push_back(obj);

            nb_success++;

            if(nb_success >= nb_image)
                break;
        }

        im++;
    }

    calibrateCamera(*object_points, *image_points, img->size(), *camera_matrix, *dist_coeffs, *rvecs, *tvecs);// , CV_CALIB_USE_INTRINSIC_GUESS);

}

cv::Mat* Calibration_intr::undistorted_image(){
    Mat* image_undistorted = new cv::Mat;
    undistort(*img, *image_undistorted, *camera_matrix, *dist_coeffs);
    return image_undistorted;
}
