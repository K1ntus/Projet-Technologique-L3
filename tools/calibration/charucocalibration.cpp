#include "charucocalibration.h"

using namespace std;
using namespace cv;
using namespace cv::aruco;

CharucoCalibration::CharucoCalibration(std::vector<cv::Mat> &imgs, int nLines, int nCols):
    Calibration_intr(imgs, nLines, nCols),
    charucoCornersTab(nullptr),
    charucoIdsTab(nullptr)
{

    charucoCornersTab = new vector< vector< vector<Point2f> > >;
    charucoIdsTab = new vector<vector<int>>;

    // creation of the charuco board
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_ARUCO_ORIGINAL);
    int const& squareX = board_size.width;
    int const& squareY = board_size.height;
    board = aruco::CharucoBoard::create(squareX, squareY, 0.02, 0.01, dictionary);

    Mat boardImage;
//    board->draw(imgs.at(currentImg).size(), boardImage, 10, 1);
//    imshow("test board", boardImage);
}

CharucoCalibration::~CharucoCalibration()
{
    delete charucoIdsTab;
    delete charucoCornersTab;
}

void CharucoCalibration::setNextImgIndex(const size_t &newIndex)
{
    super::setNextImgIndex(newIndex);
    if(currentImg < charucoIdsTab->size() && currentImg < charucoCornersTab->size())
        find_charuco_corners(charucoCornersTab->at(currentImg), charucoIdsTab->at(currentImg));
    else
        find_corners();
}

void CharucoCalibration::prepareCalibration()
{
    size_t nb_image = imgs->size();
    vector<vector<Point2f>> charucoCorners;
    vector<int> charucoIds;

    size_t im = 0;
    size_t nb_success = 0;
    while(im < nb_image){
        currentImg =  im%nb_image;

        //TODO debugging
        if(find_charuco_corners(charucoCorners, charucoIds)){
            charucoCornersTab->push_back(charucoCorners);
            charucoIdsTab->push_back(charucoIds);

            nb_success++;

            if(nb_success >= nb_image)
                break;
        }
        else if(im > (nb_image << 2))
            return;

        im++;

    }



    std::cout << "charuco corners: " << charucoCornersTab->size() << std::endl;
    std::cout << "charuco ids: " << charucoIdsTab->size() << std::endl;

}

bool CharucoCalibration::find_charuco_corners(std::vector<std::vector<Point2f> > &charucoCorners, std::vector<int> &charucoIds)
{
    Mat &img = imgs->at(currentImg);
    Ptr<aruco::DetectorParameters> params = new DetectorParameters;
    params->doCornerRefinement = false;

    // detection of the corners
    aruco::detectMarkers(img, board->dictionary, charucoCorners, charucoIds, params);

    if(charucoIds.size() > 0) {
        cvtColor(img, *gray_image, CV_BGR2GRAY);

        aruco::drawDetectedMarkers(*gray_image, charucoCorners, charucoIds, cv::Scalar(255,0 ,0));

        vector<int> markerIds;
        vector<Point2f> markerCorners;
        aruco::interpolateCornersCharuco(charucoCorners, charucoIds, img, board, markerCorners, markerIds);

        if(markerIds.size() > 0){
            std::cout << "charuco corners: " << markerCorners.size() << std::endl;
            std::cout << "charuco ids: " << markerIds.size() << std::endl;
            aruco::drawDetectedCornersCharuco(*gray_image, markerCorners, markerIds, cv::Scalar(255, 0, 0));
            return true;
        }
    }
    cout << "couldn't find markers" << endl;
    return false;
}

bool CharucoCalibration::find_corners()
{
    vector<vector<Point2f>> charucoCorners;
    vector<int> charucoIds;
    return find_charuco_corners(charucoCorners, charucoIds);
}

void CharucoCalibration::calibrate()
{
    clearCalib();

    // detection of the corners
    prepareCalibration();

    Size imgSize = imgs->at(currentImg).size();

    Mat dist_coeffs;
    Mat camera_matrix(3, 3, CV_32FC1);
    camera_matrix.ptr<float>(0)[0] = 1;
    camera_matrix.ptr<float>(1)[1] = 1;

            double repError = calibrateCameraCharuco(*charucoCornersTab, *charucoIdsTab, board, imgSize, camera_matrix, dist_coeffs, *rvecs, *tvecs,0,  TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, DBL_EPSILON));

    intrParam->setCameraMatrix(camera_matrix);
    intrParam->setDistCoeffsMatrix(dist_coeffs);


}

void CharucoCalibration::clearCalib(bool clearSet)
{
    super::clearCalib(clearSet);
    charucoCornersTab->clear();
    charucoIdsTab->clear();
}
