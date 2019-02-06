#include "chessboardcalibration.h"

using namespace std;
using namespace cv;

/**
 * @brief ChessboardCalibration::ChessboardCalibration Class that calibrate a set of images using the chessboard pattern
 * @param imgs vector with the set of images to calibrate
 * @param nLines number of lines to find in the chessboard
 * @param nCols number of columns to find in the chessboard
 */
ChessboardCalibration::ChessboardCalibration(std::vector<cv::Mat> &imgs, int nLines, int nCols):
    Calibration_intr(imgs, nLines, nCols),
     image_points(nullptr),
     object_points(nullptr)
{
    image_points = new vector<vector<Point2f>>;
    object_points = new vector<vector<Point3f>>;
}

ChessboardCalibration::~ChessboardCalibration()
{
    delete image_points;
    delete object_points;

}

void ChessboardCalibration::setNextImgIndex(const size_t &newIndex)
{
    super::setNextImgIndex(newIndex);
    if(image_points->size() != 0 && currentImg < image_points->size())
        find_chessboard_corners(image_points->at(currentImg));
    else
        find_corners();

}

std::vector<std::vector<Point2f> > &ChessboardCalibration::getImagePoints() const
{
    return *image_points;
}

/**
 * @brief ChessboardCalibration::setImagePoints Clear the images point of the current class, and set the new one linked to the image
 * @param imagePoints the image points to set in the image
 */
void ChessboardCalibration::setImagePoints(std::vector<std::vector<Point2f> > &imagePoints)
{
    this->image_points->clear();
    *this->image_points = imagePoints;
}

/**
 * @brief ChessboardCalibration::prepareCalibration get the chessboard corners and object point as a first step for the calibration
 */
void ChessboardCalibration::prepareCalibration()
{
    int &nb_lines(board_size.height), &nb_columns(board_size.width),
            squareSize = 2;

    // 3D coordinates of chessboard points

    vector<Point3f> obj;

    for (int i = 0; i < nb_lines; i++)
        for (int j = 0; j < nb_columns; j++)
            obj.push_back(Point3f((float)j * squareSize, (float)i * squareSize, 0));

    size_t nb_image = imgs->size();
    vector<Point2f> corners;

    size_t im = 0;
    size_t nb_success = 0;
    while(nb_success<nb_image){
        currentImg =  im%nb_image;

        if(find_chessboard_corners(corners)){
            image_points->push_back(corners);
            object_points->push_back(obj);

            nb_success++;

            if(nb_success >= nb_image)
                break;
        }
        else if(im > 3*nb_image)
            return;

        im++;

    }
}

bool ChessboardCalibration::find_corners()
{
    vector<Point2f> corners;
    return find_chessboard_corners(corners);
}

bool ChessboardCalibration::find_chessboard_corners(std::vector<cv::Point2f>& corners){
    Mat &img = imgs->at(currentImg);
    cvtColor(img, *gray_image, CV_BGR2GRAY);
    bool found = findChessboardCorners(img, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

    if(found){
        cornerSubPix(*gray_image, corners, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        drawChessboardCorners(*gray_image, board_size, corners, found);
    }

    return found;
}

/**
 * @brief ChessboardCalibration::calibrate
 * Calibrate the image set using the chessboard pattern.\n
 * The clearCalib() call doesnt clear the image set because the boolean parameters is not set to true.\n
 * But its cleaning the corners and ids list got from before to prevent duplication, incorrect value from\n
 * before tests, ...
 */
void ChessboardCalibration::calibrate(){

    clearCalib();
    prepareCalibration();
    currentImg = 0;
    Mat &img = imgs->at(currentImg);
    Mat dist_coeffs;
    Mat camera_matrix(3, 3, CV_32FC1);
    camera_matrix.ptr<float>(0)[0] = 1;
    camera_matrix.ptr<float>(1)[1] = 1;

    double err = calibrateCamera(*object_points, *image_points, img.size(), camera_matrix, dist_coeffs, *rvecs, *tvecs);// , CV_CALIB_USE_INTRINSIC_GUESS);

    std::cout <<  "error percentage : "  << err << endl;
    intrParam->setCameraMatrix(camera_matrix);
    intrParam->setDistCoeffsMatrix(dist_coeffs);
}

/**
 * @brief ChessboardCalibration::clearCalib clear the calibration, if set to true, the image vector will also be cleaned
 * @param clearSet set it to true to also clear the image vector. Default is set to false.
 */
void ChessboardCalibration::clearCalib(bool clearSet)
{
    super::clearCalib(clearSet);
    image_points->clear();
    object_points->clear();
}
