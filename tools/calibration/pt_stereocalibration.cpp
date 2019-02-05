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
        //        calibLeft = new CharucoCalibration(imgsL, nLines, nCols);
        //        calibRight = new CharucoCalibration(imgsR, nLines, nCols);
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
        //        calibLeft = new CharucoCalibration(imgsL, nLines, nCols);
        //        calibRight = new CharucoCalibration(imgsR, nLines, nCols);
        break;
    }
    stereoImg = new ImgCv(calibLeft->get_image_origin(), calibRight->get_image_origin(), true);

}

PT_StereoCalibration::~PT_StereoCalibration()
{
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

    vector<vector<cv::Point3f>> objectPoints;
    vector<vector<cv::Point2f>> imagePointsL;
    vector<vector<cv::Point2f>> imagePointsR;
    cv::Size imgSize(calibLeft->get_image_origin().size());
    int &nb_lines(board_size.height), &nb_columns(board_size.width),
            squareSize = 2;

    cv::Mat grayL, grayR, imgL, imgR;
    for (size_t i(0); i < imgs->size(); i++) {
        using namespace cv;
        imgs->at(i).copyTo(*stereoImg);
        imgL = stereoImg->getImgL();
        imgR = stereoImg->getImgR();

        cvtColor(imgL, grayL, CV_BGR2GRAY);
        cvtColor(imgR, grayR, CV_BGR2GRAY);


        bool foundL =false, foundR = false;
        vector<Point2f> cornersL, cornersR;
        foundL = findChessboardCorners(imgL, board_size, cornersL);
        foundR = findChessboardCorners(imgR, board_size, cornersR);

        if(foundL)
            cornerSubPix(grayL, cornersL, Size(5,5), Size(-1, -1),
                         TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));

        if(foundR)
            cornerSubPix(grayR, cornersR, Size(5,5), Size(-1, -1),
                         TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));


        vector<cv::Point3f> obj;
        for (int i = 0; i < nb_lines; i++)
            for (int j = 0; j < nb_columns; j++)
                obj.push_back(cv::Point3f((float)j * squareSize, (float)i * squareSize, 0));
        if(foundL && foundR){
            cout << "stereo corners found" << endl;
            imagePointsL.push_back(cornersL);
            imagePointsR.push_back(cornersR);
            objectPoints.push_back(obj);
        }
    }
    calibLeft->setImagePoints(imagePointsL);
    calibRight->setImagePoints(imagePointsR);

    cv::Mat dist_coeffsL, camera_matrixL(3, 3, CV_32FC1), rvecL, tvecL,
            dist_coeffsR, camera_matrixR(3, 3, CV_32FC1), rvecR, tvecR ;
    camera_matrixL.ptr<float>(0)[0] = 1;
    camera_matrixL.ptr<float>(1)[1] = 1;
    camera_matrixR.ptr<float>(0)[0] = 1;
    camera_matrixR.ptr<float>(1)[1] = 1;

    double errL = cv::calibrateCamera(objectPoints, imagePointsL, imgSize, camera_matrixL, dist_coeffsL, rvecL, tvecL);
    cout << "done with left camera error=" << errL << endl;

    IntrinsicParameters intr(camera_matrixL, dist_coeffsL);
    calibLeft->setIntrinsincParameters(intr);

    double errR = cv::calibrateCamera(objectPoints, imagePointsR, imgSize, camera_matrixR, dist_coeffsR, rvecR, tvecR);
    cout << "done with right camera error=" << errR << endl;

    intr.setCameraMatrix(camera_matrixR);
    intr.setDistCoeffsMatrix(dist_coeffsR);
    calibRight->setIntrinsincParameters(intr);

    cv::Mat RMat, E, F;
    cv::Vec3d TMat;
    currentImg = 0;
    cout << "calibrating stereo..." << endl;

    double err = cv::stereoCalibrate(objectPoints, imagePointsL, imagePointsR, camera_matrixL, dist_coeffsL,
                                     camera_matrixR, dist_coeffsR, imgSize,
                                     RMat, TMat, E, F,
                                     CV_CALIB_FIX_INTRINSIC);

    cout << "done with stereo calibration error :" << err << endl;

    cv::Mat rotL, rotR, projL, projR, dispToDepthMat;

    cv::stereoRectify(camera_matrixL, dist_coeffsL, camera_matrixR, dist_coeffsR, imgSize, RMat, TMat, rotL, rotR, projL, projR, dispToDepthMat);

    cv::FileStorage fs("extrinics.yml", cv::FileStorage::WRITE);
    if(!saveCalibration("intrinsic.yml"))
        cout << "couldn't print the intrinsic parameters" << endl;

    if(fs.isOpened()){
        fs << "rotationMatrix" << RMat;
        fs << "translationMatrix" << TMat;
        fs << "essentialMatrix" << E;
        fs << "fundamentalMatrix" << E;
        fs << "rotationMatrixLeft" << rotL;
        fs << "rotationMatrixRight" << rotR;
        fs << "projectionMatrixLeft" << projL;
        fs << "projectionMatrixRight" << projR;
        fs << "dispToDepthMatrix" << dispToDepthMat;
        fs.release();
    }else
        cout << "couldn't print the extrinsic parameters" << endl;
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



