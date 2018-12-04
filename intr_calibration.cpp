#include "intr_calibration.h"

using namespace std;
using namespace cv;

Intr_Calibration::Intr_Calibration(vector<Mat> imgs)
{
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

    dist_coeffs = new Mat;
    camera_matrix = new Mat(3, 3, CV_32FC1);
    camera_matrix->ptr<float>(0)[0] = 1;
    camera_matrix->ptr<float>(1)[1] = 1;

    vector<Mat> rvecs, tvecs;

    calibrateCamera(object_points, image_points, image.size(), *camera_matrix, *dist_coeffs, rvecs, tvecs);
}

Mat* Intr_Calibration::get_camera_matrix(){
    return camera_matrix;
}

Mat* Intr_Calibration::get_dist_coeffs(){
    return dist_coeffs;
}

vector<Mat> Intr_Calibration::get_rvecs(){
    return rvecs;
}

vector<Mat> Intr_Calibration::get_tvecs(){
    return tvecs;
}

void Intr_Calibration::show_chessboard_corners(Mat *image){
    Mat gray_image;
    int nb_lines = 6;
    int nb_columns = 9;
    int nb_squares = nb_lines * nb_columns;
    Size board_size = Size(nb_lines, nb_columns);
    vector<Point2f> corners;

    cvtColor(*image, gray_image, CV_BGR2GRAY);
    bool found = findChessboardCorners(*image, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

    if(found){
        cornerSubPix(gray_image, corners, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        drawChessboardCorners(gray_image, board_size, corners, found);
    }

    imshow("calibration points visible", gray_image);
}

void Intr_Calibration::show_undistorted_image(cv::Mat* image){

    Mat image_undistorted;
    undistort(*image, image_undistorted, *camera_matrix, *dist_coeffs);

    imshow("undirstorted", image_undistorted);
}
