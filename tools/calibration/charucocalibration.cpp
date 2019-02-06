#include "charucocalibration.h"

using namespace std;
using namespace cv;
using namespace cv::aruco;

/**
 * @brief CharucoCalibration::CharucoCalibration Class managing the charuco calibration
 * @param imgs vector of image to calibrate
 * @param nLines number of lines from the charuco chessboard to find
 * @param nCols number of columns from the charuco chessboard to find
 */
CharucoCalibration::CharucoCalibration(std::vector<cv::Mat> &imgs, int nLines, int nCols):
    Calibration_intr(imgs, nLines, nCols),
    charucoCornersTab(nullptr),
    charucoIdsTab(nullptr)
{

    charucoCornersTab = new vector< vector<Point2f> >;
    charucoIdsTab = new vector<vector<int>>;

    // creation of the charuco board
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_ARUCO_ORIGINAL);
    int const& squareX = board_size.width;
    int const& squareY = board_size.height;
    board = aruco::CharucoBoard::create(squareX, squareY, 0.02, 0.01, dictionary);
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

std::vector<std::vector<Point2f> > &CharucoCalibration::getImagePoints() const
{
    return *charucoCornersTab;
}

void CharucoCalibration::setImagePoints(std::vector<std::vector<Point2f> > &charuCorners )
{
    this->charucoCornersTab->clear();
    *this->charucoCornersTab = charuCorners;
}

/**
 * @brief CharucoCalibration::prepareCalibration get the charuco corners and id as a first step for the calibration
 */
void CharucoCalibration::prepareCalibration()
{
    size_t nb_image = imgs->size();
    vector<Point2f> charucoCorners;
    vector<int> charucoIds;

    size_t im = 0;
    size_t nb_success = 0;
    while(im < nb_image){
        currentImg =  im%nb_image;

        if(find_charuco_corners(charucoCorners, charucoIds)){
            charucoCornersTab->push_back(charucoCorners);
            charucoIdsTab->push_back(charucoIds);

            nb_success++;

            if(nb_success >= nb_image)
                break;
            } else if(im > (nb_image << 2))
                return;
            im++;
    }

    std::cout << "charuco corners: " << charucoCornersTab->size() << std::endl;
    std::cout << "charuco ids: " << charucoIdsTab->size() << std::endl;
}

/**
 * @brief CharucoCalibration::find_charuco_corners Find the charuco corners of an image and save it into pointers
 * @param charucoCorners the pointer that will get the result of the list of charuco corners
 * @param charucoIds the pointer that will store the result of the different charuco id
 * @return true if the search has succeed, else false
 */
bool CharucoCalibration::find_charuco_corners(std::vector<Point2f> &charucoCorners, std::vector<int> &charucoIds)
{
    Mat &img = imgs->at(currentImg);
    vector<int> markerIds;
    vector<vector<Point2f>> markerCorners;
    Ptr<aruco::DetectorParameters> params = new DetectorParameters;
    params->doCornerRefinement = false;

    // detection of the corners
    aruco::detectMarkers(img, board->dictionary, markerCorners, markerIds, params);

    if(markerIds.size() > 0) {
        cvtColor(img, *gray_image, CV_BGR2GRAY);

        aruco::drawDetectedMarkers(*gray_image, markerCorners, markerIds, cv::Scalar(255,0 ,0));

        aruco::interpolateCornersCharuco(markerCorners, markerIds, img, board, charucoCorners, charucoIds);

        if(charucoIds.size() > 4){
            std::cout << "charuco corners: " << markerCorners.size() << std::endl;
            std::cout << "charuco ids: " << markerIds.size() << std::endl;
            aruco::drawDetectedCornersCharuco(*gray_image, charucoCorners, charucoIds, cv::Scalar(255, 0, 0));
            return true;
        }
    }
    cout << "couldn't find markers" << endl;
    return false;
}

bool CharucoCalibration::find_corners()
{
    vector<Point2f> charucoCorners;
    vector<int> charucoIds;
    return find_charuco_corners(charucoCorners, charucoIds);
}

/**
 * @brief CharucoCalibration::calibrate
 * Calibrate the image set using charuco pattern.\n
 * The clearCalib() call doesnt clear the image set because the boolean parameters is not set to true.\n
 * But its cleaning the corners and ids list got from before to prevent duplication, incorrect value from\n
 * before tests, ...
 */
void CharucoCalibration::calibrate()
{
    clearCalib();

    // detection of the corners
    prepareCalibration();
    currentImg = 0;

    Size imgSize = imgs->at(currentImg).size();

    Mat dist_coeffs;
    Mat camera_matrix(3, 3, CV_32FC1);
    camera_matrix.ptr<float>(0)[0] = 1;
    camera_matrix.ptr<float>(1)[1] = 1;

    try{
        double repError = calibrateCameraCharuco(*charucoCornersTab,
                                                 *charucoIdsTab,
                                                 board,
                                                 imgSize,
                                                 camera_matrix,
                                                 dist_coeffs,
                                                 *rvecs,
                                                 *tvecs,
                                                 0,
                                                 TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, DBL_EPSILON)
        );

        std::cout << "The error percent of this charuco calibration is of:" << repError << "\%" << std::endl;
    } catch(cv::Exception){
        std::cout << "Charuco Calibration error" << std::endl;
    }

    intrParam->setCameraMatrix(camera_matrix);
    intrParam->setDistCoeffsMatrix(dist_coeffs);
}

/**
 * @brief CharucoCalibration::clearCalib clear the calibration, if set to true, the image vector will also be cleaned
 * @param clearSet set it to true to also clear the image vector. Default is set to false.
 */
void CharucoCalibration::clearCalib(bool clearSet)
{
    super::clearCalib(clearSet);
    charucoCornersTab->clear();
    charucoIdsTab->clear();
}
