#include "charucocalibration.h"

using namespace std;
using namespace cv;
using namespace cv::aruco;

CharucoCalibration::CharucoCalibration(std::vector<cv::Mat> &imgs, int nLines, int nCols): Calibration_intr(imgs, nLines, nCols)
{
    charucoIdsTab = new vector<vector<int>>;

    // creation of the charuco board
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_ARUCO_ORIGINAL);
    int const& squareX = board_size.width;
    int const& squareY = board_size.height;
    board = aruco::CharucoBoard::create(squareX, squareY, 0.02, 0.01, dictionary);
}

CharucoCalibration::~CharucoCalibration()
{}

bool CharucoCalibration::find_charuco_corners(std::vector<Point2f> &charucoCorners, std::vector<int> &charucoIds)
{
    Mat &img = imgs->at(currentImg);
    cvtColor(img, *gray_image, CV_BGR2GRAY);
    vector<int> markerIds;
    Ptr<aruco::DetectorParameters> params = new DetectorParameters;
    params->doCornerRefinement = false;

    // detection of the corners
    aruco::detectMarkers(img, board->dictionary, *image_points, markerIds, params);

    if(markerIds.size() > 0) {

        aruco::interpolateCornersCharuco(*image_points, markerIds, img, board, charucoCorners, charucoIds);

        if(charucoIds.size() > 0){
            aruco::drawDetectedCornersCharuco(*gray_image, charucoCorners, charucoIds, cv::Scalar(255, 0, 0));
            return true;
        }
    }

    return false;
}

bool CharucoCalibration::find_chessboard_corners(vector<Point2f> &charucoCorners)
{
    vector<int> dmp;
    return find_charuco_corners(charucoCorners, dmp);
}

void CharucoCalibration::calibrate()
{

    Size imgSize = imgs->at(currentImg).size();
//    Mat boardImage;
//    board->draw(imgSize, boardImage, 10, 1);

    // detection of the corners
    size_t nb_image = imgs->size();
    vector<Point2f> charucoCorners;
    vector<int> charucoIds;

    size_t im = 0;
    size_t nb_success = 0;
    while(nb_success<nb_image){
        currentImg =  im%nb_image;

        //TODO debugging
        if(find_charuco_corners(charucoCorners, charucoIds)){
            image_points->push_back(charucoCorners);
            charucoIdsTab->push_back(charucoIds);

            nb_success++;

            if(nb_success >= nb_image)
                break;
        }
        else if(im > 3*nb_image)
            return;

        im++;

    }


    Mat *dist_coeffs = new Mat;
    Mat *camera_matrix = new Mat(3, 3, CV_32FC1);
    camera_matrix->ptr<float>(0)[0] = 1;
    camera_matrix->ptr<float>(1)[1] = 1;

    double repError = calibrateCameraCharuco(*image_points, *charucoIdsTab, board, imgSize, *camera_matrix, *dist_coeffs, *rvecs, *tvecs);

    intrParam->setCameraMatrix(*camera_matrix);
    intrParam->setDistCoeffsMatrix(*dist_coeffs);


}
