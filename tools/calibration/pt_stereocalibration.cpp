#include "pt_stereocalibration.h"


PT_StereoCalibration::PT_StereoCalibration(std::vector<cv::Mat> &imgsL, std::vector<cv::Mat> &imgsR,
                                           int nLines, int nCols,
                                           CalibrationMode calibrationMode):
    PT_ICalibration(nLines, nCols),
    imgs(nullptr), calibLeft(nullptr), calibRight(nullptr),
    stereoImg(nullptr), R(nullptr), T(nullptr),
    essentialMatrix(nullptr), fundamentalMatrix(nullptr)
{

    switch (calibrationMode) {

    case CHARUCO:
                calibLeft = new CharucoCalibration(imgsL, nLines, nCols);
                calibRight = new CharucoCalibration(imgsR, nLines, nCols);
        break;

    default:
        calibLeft = new ChessboardCalibration(imgsL, nLines, nCols);
        calibRight = new ChessboardCalibration(imgsR, nLines, nCols);
        break;
    }
    imgs = new std::vector<cv::Mat>;
    stereoImg = new ImgCv(calibLeft->get_image_origin(), calibRight->get_image_origin(), true);

    R = new cv::Mat;
    T = new cv::Mat;
    essentialMatrix = new cv::Mat;
    fundamentalMatrix = new cv::Mat;

    for(size_t i(0); i < imgsL.size(); i++){
        stereoImg->setImg(imgsL.at(i), imgsR.at(i));

        imgs->push_back(stereoImg->getImg());
    }

}

PT_StereoCalibration::PT_StereoCalibration(std::vector<ImgCv> &stereoImgs, int nLines, int nCols, CalibrationMode calibrationMode):
    PT_ICalibration(nLines, nCols),
    imgs(nullptr), calibLeft(nullptr), calibRight(nullptr),
    stereoImg(nullptr), R(nullptr), T(nullptr),
    essentialMatrix(nullptr), fundamentalMatrix(nullptr)

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
    case CHARUCO:
                calibLeft = new CharucoCalibration(imgsL, nLines, nCols);
                calibRight = new CharucoCalibration(imgsR, nLines, nCols);
        break;
    default:
        calibLeft = new ChessboardCalibration(imgsL, nLines, nCols);
        calibRight = new ChessboardCalibration(imgsR, nLines, nCols);
        break;

    }
    stereoImg = new ImgCv(calibLeft->get_image_origin(), calibRight->get_image_origin(), true);
    R = new cv::Mat;
    T = new cv::Mat;
    essentialMatrix = new cv::Mat;
    fundamentalMatrix = new cv::Mat;

}

PT_StereoCalibration::PT_StereoCalibration(const std::string &calibFilename):
    PT_ICalibration(6, 9),
    imgs(nullptr), calibLeft(nullptr), calibRight(nullptr),
    stereoImg(nullptr), R(nullptr), T(nullptr),
    essentialMatrix(nullptr), fundamentalMatrix(nullptr)
{
    runCalibration(calibFilename);
}

PT_StereoCalibration::~PT_StereoCalibration()
{
    delete imgs;
    delete calibLeft;
    delete calibRight;
    delete stereoImg;
    delete R;
    delete T;
    delete essentialMatrix;
    delete fundamentalMatrix;
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

    double errR = cv::calibrateCamera(objectPoints, imagePointsR, imgSize, camera_matrixR, dist_coeffsR, rvecR, tvecR);
    cout << "done with right camera error=" << errR << endl;

    // set the new intrinsic parameters for the left and right calibration
    IntrinsicParameters intr(camera_matrixL, dist_coeffsL);
    calibLeft->setIntrinsincParameters(intr);

    intr.setCameraMatrix(camera_matrixR);
    intr.setDistCoeffsMatrix(dist_coeffsR);
    calibRight->setIntrinsincParameters(intr);

    currentImg = 0;
    cout << "calibrating stereo..." << endl;
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

    double errR = cv::calibrateCamera(objectPoints, imagePointsR, imgSize, camera_matrixR, dist_coeffsR, rvecR, tvecR);
    cout << "done with right camera error=" << errR << endl;

    currentImg = 0;
    cout << "calibrating stereo..." << endl;

    double err = cv::stereoCalibrate(objectPoints, imagePointsL, imagePointsR, camera_matrixL, dist_coeffsL,
                                     camera_matrixR, dist_coeffsR, imgSize,
                                     *R, *T, *essentialMatrix, *fundamentalMatrix,
                                     CV_CALIB_FIX_INTRINSIC);

    cout << "done with stereo calibration error :" << err << endl;

    // set the new intrinsic parameters for the left and right calibration
    IntrinsicParameters intr(camera_matrixL, dist_coeffsL);
    calibLeft->setIntrinsincParameters(intr);

    intr.setCameraMatrix(camera_matrixR);
    intr.setDistCoeffsMatrix(dist_coeffsR);
    calibRight->setIntrinsincParameters(intr);

    cv::Mat rotL, rotR, projL, projR, dispToDepthMat;

    cv::stereoRectify(camera_matrixL, dist_coeffsL, camera_matrixR, dist_coeffsR,
                      imgSize, *R, *T, rotL, rotR, projL, projR, dispToDepthMat);

    if(!saveCalibration("stereoCalibration_cached.yml"))
        cout << "couldn't print the extrinsic parameters" << endl;
}

bool PT_StereoCalibration::saveCalibration(const std::string &outFile) const
{
    cv::FileStorage fs(outFile, cv::FileStorage::WRITE);

    if(fs.isOpened()){
        std::cout << "loaded" << std::endl;

        cv::Mat &dist_coeffsL(calibLeft->getIntrinsicParameters().getDistCoeffs()),
                &camera_matrixL(calibLeft->getIntrinsicParameters().getCameraMatrix()),
                &dist_coeffsR(calibRight->getIntrinsicParameters().getDistCoeffs()),
                &camera_matrixR(calibRight->getIntrinsicParameters().getCameraMatrix());

        cv::Mat rotL, rotR, projL, projR, dispToDepthMat;
        cv::stereoRectify(camera_matrixL, dist_coeffsL, camera_matrixR, dist_coeffsR,
                          calibLeft->get_image_origin().size(), *R, *T, rotL, rotR, projL, projR, dispToDepthMat);

        fs.writeComment("Intrinsic parameters");
        fs << "cameraMatrixLeft" << camera_matrixL;
        fs << "distCoefficientsMatrixLeft" << dist_coeffsL;
        fs << "cameraMatrixRight" << camera_matrixR;
        fs << "distCoefficientsMatrixRight" << dist_coeffsR;

        fs.writeComment("Extrinsic parameters");
        fs << "rotationMatrix" << *R;
        fs << "translationMatrix" << *T;
        fs << "essentialMatrix" << *essentialMatrix;
        fs << "fundamentalMatrix" << *fundamentalMatrix;
        fs << "rotationMatrixLeft" << rotL;
        fs << "rotationMatrixRight" << rotR;
        fs << "projectionMatrixLeft" << projL;
        fs << "projectionMatrixRight" << projR;
        fs << "dispToDepthMatrix" << dispToDepthMat;

        fs.release();

        return true;

    }
    std::cout << "couldn't open file" << std::endl;

    return false;
}

bool PT_StereoCalibration::runCalibration(std::string const &inFile)
{
    cv::FileStorage fs(inFile, cv::FileStorage::READ);

    if(fs.isOpened()){
        std::cout << "loaded" << std::endl;

        fs["rotationMatrix"] >> *R;
        fs["translationMatrix"] >> *T;
        fs["essentialMatrix"] >> *essentialMatrix;
        fs["fundamentalMatrix"] >> *fundamentalMatrix;
        fs.release();
        std::cout << "extrinsic loaded" << std::endl;
        return IntrinsicParameters::readIntrStereoCalibration(inFile, calibLeft->getIntrinsicParameters(), calibRight->getIntrinsicParameters());
    }else
        return false;

}

void PT_StereoCalibration::displayRectifiedImage()
{
    if(imgs->empty() || !hasIntrinsicParameters()){}
    else {
        ImgCv undistortedImg;

        cv::Mat &dist_coeffsL(calibLeft->getIntrinsicParameters().getDistCoeffs()),
                &camera_matrixL(calibLeft->getIntrinsicParameters().getCameraMatrix()),
                &dist_coeffsR(calibRight->getIntrinsicParameters().getDistCoeffs()),
                &camera_matrixR(calibRight->getIntrinsicParameters().getCameraMatrix());

        cv::Mat &imgL = calibLeft->get_image_origin(),
                imgR = calibRight->get_image_origin(),
                imgLrectified, imgRrectified;
        cv::Mat rotL, rotR, projL, projR, dispToDepthMat;
        cv::Rect roi1, roi2;
        cv::Size imgSize(imgL.size());
        cv::stereoRectify(camera_matrixL, dist_coeffsL, camera_matrixR, dist_coeffsR,
                          imgSize, *R, *T, rotL, rotR, projL, projR, dispToDepthMat,
                          cv::CALIB_ZERO_DISPARITY, -1, imgSize, &roi1, &roi2);


        cv::Mat map11, map12, map21, map22;
        cv::initUndistortRectifyMap(camera_matrixL, dist_coeffsL, rotL, projL, imgSize, CV_16SC2, map11, map12);
        cv::initUndistortRectifyMap(camera_matrixR, dist_coeffsR, rotR, projR, imgSize, CV_16SC2, map21, map22);


        cv::remap(imgL, imgLrectified, map11, map12, cv::INTER_LINEAR);
        cv::remap(imgR, imgRrectified, map21, map22, cv::INTER_LINEAR);

        undistortedImg.setImg(imgLrectified, imgRrectified);

        cv::Mat canvasL(imgLrectified);

        cv::Rect vroi1(cvRound(roi1.x), cvRound(roi1.y),
                      cvRound(roi1.width), cvRound(roi1.height));

        cv::rectangle(canvasL, vroi1, cv::Scalar(0, 0, 255), 3, 8);

        cv::Mat canvasR(imgRrectified);

        cv::Rect vroi2(cvRound(roi2.x), cvRound(roi2.y),
                      cvRound(roi2.width), cvRound(roi2.height));

        cv::rectangle(canvasR, vroi2, cv::Scalar(0, 0, 255), 3, 8);

        ImgCv canvas(canvasL, canvasR);
        for(int j = 0; j < canvas.rows; j += 16 )
                        line(canvas, cv::Point(0, j), cv::Point(canvas.cols, j), cv::Scalar(0, 255, 0), 1, 8);

        ImgCv rectified(undistortedImg.getImgL()(vroi1), undistortedImg.getImgR()(vroi2));
        cv::imshow("rectified", canvas);
        cv::imshow("rectified full", rectified);

    }
}

cv::Mat PT_StereoCalibration::undistorted_image() const
{
    ImgCv undistortedImg;
    if(imgs->empty() || !hasIntrinsicParameters()){}
    else {


        cv::Mat &dist_coeffsL(calibLeft->getIntrinsicParameters().getDistCoeffs()),
                &camera_matrixL(calibLeft->getIntrinsicParameters().getCameraMatrix()),
                &dist_coeffsR(calibRight->getIntrinsicParameters().getDistCoeffs()),
                &camera_matrixR(calibRight->getIntrinsicParameters().getCameraMatrix());

        cv::Mat &imgL = calibLeft->get_image_origin(),
                imgR = calibRight->get_image_origin(),
                imgLrectified, imgRrectified;
        cv::Mat rotL, rotR, projL, projR, dispToDepthMat;
        cv::Rect roi1, roi2;
        cv::Size imgSize(imgL.size());
        cv::stereoRectify(camera_matrixL, dist_coeffsL, camera_matrixR, dist_coeffsR,
                          imgSize, *R, *T, rotL, rotR, projL, projR, dispToDepthMat,
                          cv::CALIB_ZERO_DISPARITY, -1, imgSize, &roi1, &roi2);


        cv::Mat map11, map12, map21, map22;
        cv::initUndistortRectifyMap(camera_matrixL, dist_coeffsL, rotL, projL, imgSize, CV_16SC2, map11, map12);
        cv::initUndistortRectifyMap(camera_matrixR, dist_coeffsR, rotR, projR, imgSize, CV_16SC2, map21, map22);


        cv::remap(imgL, imgLrectified, map11, map12, cv::INTER_LINEAR);
        cv::remap(imgR, imgRrectified, map21, map22, cv::INTER_LINEAR);


        cv::Rect vroi1(cvRound(roi1.x), cvRound(roi1.y),
                      cvRound(roi1.width), cvRound(roi1.height));



        cv::Rect vroi2(cvRound(roi2.x), cvRound(roi2.y),
                      cvRound(roi2.width), cvRound(roi2.height));

        undistortedImg.setImg(imgLrectified(vroi1), imgRrectified(vroi2));

    }

    return undistortedImg.getImg();
}



