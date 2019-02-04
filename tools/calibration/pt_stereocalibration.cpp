#include "pt_stereocalibration.h"


PT_StereoCalibration::PT_StereoCalibration(std::vector<cv::Mat> &imgsL, std::vector<cv::Mat> &imgsR,
                                           int nLines, int nCols,
                                           CalibrationMode calibrationMode):
    PT_ICalibration(nLines, nCols)
{

    switch (calibrationMode) {
    case CHESSBOARD:
        calibLeft = new ChessboardCalibration(imgsL, nLines, nCols);
        calibRight = new ChessboardCalibration(imgsR, nLines, nCols);
        break;
    case CHARUCO:
        calibLeft = new CharucoCalibration(imgsL, nLines, nCols);
        calibRight = new CharucoCalibration(imgsR, nLines, nCols);
        break;
    }
    imgs = new std::vector<cv::Mat>;
    stereoImg = new ImgCv(calibLeft->get_image_origin(), calibRight->get_image_origin(), true);

    for(size_t i(0); i < imgsL.size(); i++){
        stereoImg->setImg(imgsL.at(i), imgsR.at(i));

        imgs->push_back(stereoImg->getImg());
    }

}

PT_StereoCalibration::PT_StereoCalibration(std::vector<ImgCv> &stereoImgs, int nLines, int nCols, CalibrationMode calibrationMode):
    PT_ICalibration(nLines, nCols)

{

    imgs = new std::vector<cv::Mat>;
    std::vector<cv::Mat> imgsL, imgsR;
    ImgCv img;
    for(size_t i(0); i < stereoImgs.size(); i++){
        img = stereoImgs.at(i);
        imgsL.push_back(img.getImgL());
        imgsR.push_back(img.getImgR());
        imgs->push_back(img);
    }

    switch (calibrationMode) {
    case CHESSBOARD:
        calibLeft = new ChessboardCalibration(imgsL, nLines, nCols);
        calibRight = new ChessboardCalibration(imgsR, nLines, nCols);
        break;
    case CHARUCO:
        calibLeft = new CharucoCalibration(imgsL, nLines, nCols);
        calibRight = new CharucoCalibration(imgsR, nLines, nCols);
        break;
    }
    stereoImg = new ImgCv(calibLeft->get_image_origin(), calibRight->get_image_origin(), true);

}

PT_StereoCalibration::~PT_StereoCalibration()
{
    imgs->~vector()->clear();
    delete imgs;
    delete calibLeft;
    delete calibRight;
    delete stereoImg;
}

void PT_StereoCalibration::newImageSet(const std::vector<cv::Mat> &imagesSetLR)
{
    if(imagesSetLR.size() & 1){
        std::cout << "the image set does not contain equal number of image" << std::endl;
        return;
    }
    int setSize = imagesSetLR.size() >> 1;
    std::vector<cv::Mat>::const_iterator first =  imagesSetLR.begin() + setSize;
    std::vector<cv::Mat>::const_iterator last =  imagesSetLR.end();
    std::vector<cv::Mat> imgL(imagesSetLR.begin(), first - 1 );
    std::vector<cv::Mat> imgR(first, last);
    newImageSet(imgL, imgR);
}

void PT_StereoCalibration::newImageSet(const std::vector<cv::Mat> &imagesL, const std::vector<cv::Mat> &imagesR)
{
    if(imagesL.size() != imagesR.size()){
        std::cout << "the image set does not contain equal number of image" << std::endl;
        return;
    }
    clearCalib(true);
    calibLeft->newImageSet(imagesL);
    calibRight->newImageSet(imagesR);
    for(size_t i(0); i < imagesL.size(); i++){
        stereoImg->setImg(imagesL.at(i), imagesR.at(i));
        cv::Mat img;
        stereoImg->getImg().copyTo(img);
        imgs->push_back(img);
    }
}

std::vector<cv::Mat> &PT_StereoCalibration::getSet() const
{

    return *imgs;
}

cv::Mat &PT_StereoCalibration::get_image_origin() const
{
    return imgs->at(currentImg);
}

cv::Mat &PT_StereoCalibration::get_compute_image() const
{
    return *gray_image;
}

bool PT_StereoCalibration::hasIntrinsicParameters() const
{
    return calibLeft->hasIntrinsicParameters() && calibRight->hasIntrinsicParameters();
}

void PT_StereoCalibration::setNextImgIndex(const size_t &newIndex)
{
    if(newIndex < imgs->size()){
        currentImg = newIndex;
        calibLeft->setNextImgIndex(newIndex);
        calibRight->setNextImgIndex(newIndex);
        stereoImg->setImg(calibLeft->get_compute_image(), calibRight->get_compute_image());
        stereoImg->getImg().copyTo(*gray_image);
    }
}

void PT_StereoCalibration::clearCalib(bool clearSet)
{
    super::clearCalib(clearSet);
    calibLeft->clearCalib(clearSet);
    calibRight->clearCalib(clearSet);
    if(clearSet)
        imgs->clear();
}

void PT_StereoCalibration::prepareCalibration()
{
    calibLeft->prepareCalibration();
    calibRight->prepareCalibration();
    stereoImg->setImg(calibLeft->get_image_origin(), calibRight->get_image_origin());

}

bool PT_StereoCalibration::find_corners()
{
    bool result = false;
    result |= calibLeft->find_corners();
    result |= calibRight->find_corners();
    stereoImg->setImg(calibLeft->get_compute_image(), calibRight->get_compute_image());
    *gray_image = stereoImg->getImg();
    return result;
}

void PT_StereoCalibration::calibrate()
{
    using namespace std;

    cout << "calibrating left camera" << endl;
    calibLeft->calibrate();

    cout << "calibrating right camera" << endl;
    calibRight->calibrate();

    vector<vector<cv::Point3f>> objectPoints;
    vector<vector<cv::Point2f>> &imagePointsL = calibLeft->getImagePoints();
    vector<vector<cv::Point2f>> &imagePointsR = calibRight->getImagePoints();

    int &nb_lines(board_size.height), &nb_columns(board_size.width),
            squareSize = 2;

    vector<cv::Point3f> obj;

    if(imagePointsL.size() != imagePointsR.size())
        return;

    const size_t &imgPointsSize = imagePointsL.size();

    cout << "making of object points..." << endl;


    for (int i = 0; i < nb_lines; i++)
        for (int j = 0; j < nb_columns; j++)
            obj.push_back(cv::Point3f((float)j * squareSize, (float)i * squareSize, 0));
    for(size_t k(0); k < imgPointsSize; k++){
        objectPoints.push_back(obj);
    }

    cv::Mat &dist_coeffsL = calibLeft->getIntrinsicParameters().getDistCoeffs();
    cv::Mat &camera_matrixL = calibLeft->getIntrinsicParameters().getCameraMatrix();
    cv::Mat &dist_coeffsR = calibRight->getIntrinsicParameters().getDistCoeffs();
    cv::Mat &camera_matrixR = calibRight->getIntrinsicParameters().getCameraMatrix();

    cv::Mat RMat, TMat, E, F;
    currentImg = 0;
    cout << "calibrating stereo..." << endl;

    cv::stereoCalibrate(objectPoints, imagePointsL, imagePointsR, camera_matrixL, dist_coeffsL,
                        camera_matrixR, dist_coeffsR, calibLeft->get_image_origin().size(),
                        RMat, TMat, E, F,
                        CV_CALIB_FIX_INTRINSIC);
}

bool PT_StereoCalibration::saveCalibration(const std::string &outFile) const
{
    return IntrinsicParameters::printIntrStereoCalibration(outFile, calibLeft->getIntrinsicParameters(), calibRight->getIntrinsicParameters());
}

bool PT_StereoCalibration::runCalibration(std::string const &inFile)
{
    return IntrinsicParameters::readIntrStereoCalibration(inFile, calibLeft->getIntrinsicParameters(), calibRight->getIntrinsicParameters());

}

cv::Mat PT_StereoCalibration::undistorted_image() const
{
    ImgCv undistortedImg;
    if(imgs->empty() || !hasIntrinsicParameters()){}
    else {
        const cv::Mat &image_undistortedL = calibLeft->undistorted_image();
        const cv::Mat &image_undistortedR = calibRight->undistorted_image();
        undistortedImg.setImg(image_undistortedL, image_undistortedR);

    }
    return undistortedImg.getImg();
}



