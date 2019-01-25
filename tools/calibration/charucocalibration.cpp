#include "charucocalibration.h"
#include <QWidget>
#include <QtGui>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
using namespace std;
using namespace cv;
using namespace cv::aruco;

CharucoCalibration::CharucoCalibration(std::vector<cv::Mat> &imgs, int nLines, int nCols): Calibration_intr(imgs, nLines, nCols)
{
    charucoIds = new vector<int>;
}

CharucoCalibration::~CharucoCalibration()
{}

bool CharucoCalibration::find_chessboard_corners(std::vector<cv::Point2f> &corners)
{

}

void CharucoCalibration::calibrate()
{
    // creation of the charuco board
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(PREDEFINED_DICTIONARY_NAME::DICT_6X6_250);

    Ptr<aruco::CharucoBoard> board = aruco::CharucoBoard::create(board_size.width, board_size.height, 0.02, 0.01, dictionary);
    Mat boardImage;
    Mat &img = imgs->at(currentImg);
    Size imgSize = img.size();
    board->draw(imgSize, boardImage, 10, 1);

    // detection of the corners;
    vector<int> markerIds;
    Ptr<aruco::DetectorParameters> params;
    params->doCornerRefinement = false;
    aruco::detectMarkers(img, board->dictionary, *image_points, markerIds, params);

    if(markerIds.size() > 0) {
        vector<Point2f> charucoCorners;
        aruco::interpolateCornersCharuco(*image_points, markerIds, img, board, charucoCorners, *charucoIds);
        Mat image;
        img.copyTo(image);
        if(charucoIds->size() > 0){
            aruco::drawDetectedCornersCharuco(image, charucoCorners, *charucoIds, Scalar(255, 0, 0));
            imshow("test charuco", image);
        }
    }


    Mat *dist_coeffs = new Mat;
    Mat *camera_matrix = new Mat(3, 3, CV_32FC1);
    camera_matrix->ptr<float>(0)[0] = 1;
    camera_matrix->ptr<float>(1)[1] = 1;

    double repError = calibrateCameraCharuco(*image_points, *charucoIds, board, imgSize, *camera_matrix, *dist_coeffs, *rvecs, *tvecs);

    intrParam->setCameraMatrix(*camera_matrix);
    intrParam->setDistCoeffsMatrix(*dist_coeffs);

}
