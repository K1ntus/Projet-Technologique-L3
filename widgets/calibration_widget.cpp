#include "calibration_widget.h"

using namespace std;
using namespace cv;
using namespace imagecv;

/**
 * @brief Calibration_widget::Calibration_widget Create the calibration class and ui
 * @param calibration the calibration mode wanted (if null, a default one will be choose)
 * @param CalibMode The calibration mode wanted, that could be the stereo one, charuco or chessboard
 * @param parent the parent widget (most likely to be the main window, but could be any qt widget)
 */
Calibration_widget::Calibration_widget(PT_ICalibration *calibration, CalibrationMode CalibMode, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Calibration_widget),
    calib(calibration),
    currentCalibMode(CalibMode),
    currentMode(ORIGINAL){
    ui->setupUi(this);
    if(calib != nullptr){
        display_image(currentMode);
        int nbImg = calib->getSet().size();
        ui->nbImage->setNum(nbImg);
        enableFeatures(true);
    }else
        enableFeatures(false);
}

Calibration_widget::~Calibration_widget()
{
    qDebug( "calibration deleted");
    delete ui;
    delete calib;
}

/**
 * @brief Calibration_widget::on_undistortedButton_clicked is summoned when the linked button is pressed.
 * This function launch the calibration on distorted pictures from the fish eye effects and\n
 * undistorted the whole set using the intrinsic parameters of the camera
 */
void Calibration_widget::on_undistortedButton_clicked()
{
    if(!calib->hasIntrinsicParameters())
        return;

    currentMode = UNDISTORTED;
    display_image(UNDISTORTED);

}

/**
 * @brief Calibration_widget::on_chesscorners_clicked Detect the chessboard corners of an image set
 */
void Calibration_widget::on_chesscorners_clicked()
{
    currentMode = CHESSBOARD_CORNERS;
    display_image(CHESSBOARD_CORNERS);

}

/**
 * @brief Calibration_widget::display_image Display the current image of the set on screen
 * @param displayerMode
 */
void Calibration_widget::display_image(DisplayerMode const&displayerMode){
    Mat *imagePtr(nullptr);

    switch (displayerMode) {

    case ORIGINAL:
        imagePtr = &calib->get_image_origin();
        break;

    case CHESSBOARD_CORNERS:
        imagePtr = &calib->get_compute_image();
        break;

    case UNDISTORTED:
        break;
    }
    QImage qimg;
    if(imagePtr != nullptr)
        qimg = mat_to_qimage(*imagePtr);
    else
        qimg = mat_to_qimage(calib->undistorted_image());

    int h = ui->Displayer->height();
    int w = ui->Displayer->width();
    ui->Displayer->setPixmap(QPixmap::fromImage(qimg.scaled(w, h, Qt::KeepAspectRatio)));
    ui->Displayer->adjustSize();
    ui->currentImage->setNum((int)calib->getCurrentImgIndex()+1);
}

/**
 * @brief Calibration_widget::enableFeatures enable or disable some specific features.
 * @param isEnabled
 *
 * This function enable some features that could only be available\n
 * when an image set has been loaded, like:\n
 * + Display the original image\n
 * + Display the chesscorners that has been discovered\n
 * + Go to the previous image\n
 * + Go to the next image\n
 */
void Calibration_widget::enableFeatures(bool isEnabled)
{
    ui->originalImage->setEnabled(isEnabled);
    ui->chesscorners->setEnabled(isEnabled);
    ui->prevImage->setEnabled(isEnabled);
    ui->nextImage->setEnabled(isEnabled);
}

/**
 * @brief Calibration_widget::on_newImageSet_clicked load a new set of image to test
 */
void Calibration_widget::on_newImageSet_clicked()
{
    QStringList filePath = QFileDialog::getOpenFileNames(
                this, tr("Select the files for calibration"),
                tr("./resources/"),
                tr("Image files (*.png *.jpg *.bmp)") );

    if(!filePath.isEmpty()){

        vector<Mat> images;
        size_t count = filePath.size();

        for(size_t i = 0; i < count; i++){
            images.push_back(imread(filePath[i].toStdString()));
        }

        enableFeatures(true);
        on_CharucoCalib_clicked();
        calib->newImageSet(images);
        int nbImg = calib->getSet().size();
        ui->nbImage->setNum(nbImg);
        ui->undistortedButton->setEnabled(false);
        on_originalImage_clicked();

    }

}

/**
 * @brief Calibration_widget::on_nextImage_clicked Go to the next image
 */
void Calibration_widget::on_nextImage_clicked()
{
    size_t const& incr = calib->getCurrentImgIndex();
    calib->setNextImgIndex(incr + 1);
    if(incr == calib->getCurrentImgIndex()){
        QMessageBox::information(this, tr("End of set"), tr("reach the end of the set"));
    }
    else
        display_image(currentMode);
}

/**
 * @brief Calibration_widget::on_prevImage_clicked Go back to the previous image
 */
void Calibration_widget::on_prevImage_clicked()
{
    size_t const& decr = calib->getCurrentImgIndex();
    calib->setNextImgIndex(decr - 1);
    if(decr == calib->getCurrentImgIndex()){
        QMessageBox::information(this, tr("start of set"), tr("reach the beginning of the set"));
    } else
        display_image(currentMode);
}

/**
 * @brief Calibration_widget::on_save_clicked Save the intrinsic or extrinsic parameters depending of the detection.
 * If the detection mode is 'chessboard' or 'charuco', only the intrinsic parameters are saved.\n
 * If the detection mode is using a camera stream, the intrinsic AND extrinsic parameters are saved.\n
 */
void Calibration_widget::on_save_clicked()
{
    if( calib == nullptr || !calib->hasIntrinsicParameters()){
        QMessageBox::warning(this, tr("Error while saving file"), tr("no calibration to save."));
        return;
    }
    string outFile("calibration.yml");
    if(calib->saveCalibration(outFile))
        QMessageBox::information(this, tr("save calibration"), tr("saved!"));
    else
        QMessageBox::warning(this, tr("Error while saving file"), tr("something went wrong"));


}

/**
 * @brief Calibration_widget::on_loadCalib_clicked load a calibration file containing the intrinsic nor extrinsic parameters of the camera.
 */
void Calibration_widget::on_loadCalib_clicked()
{

    QString inFile = QFileDialog::getOpenFileName(this, tr("open a calibration file"),"",tr("yaml files (*.yml)"));

    if(inFile.isEmpty())
        return;

    if(calib->runCalibration(inFile.toStdString())){
        ui->undistortedButton->setEnabled(true);
        enableFeatures(true);
    }
    else
        QMessageBox::warning(this, tr("Parse error"), tr("Error while parsing file"));


}

/**
 * @brief Calibration_widget::on_originalImage_clicked go back to the 'original' (ie. without calibration) image of the set
 */
void Calibration_widget::on_originalImage_clicked()
{
    currentMode = ORIGINAL;
    display_image(ORIGINAL);
}

/**
 * @brief Calibration_widget::on_calibration_clicked Calibrate the image set, depending of the checkbox mode clicked
 */
void Calibration_widget::on_calibration_clicked()
{
    if(calib == nullptr){
        on_CharucoCalib_clicked();
        on_newImageSet_clicked();
    }
    calib->calibrate();
    QMessageBox::information(this, tr("Calibration"), tr("Calibrated!"));
    ui->undistortedButton->setEnabled(true);
    display_image(currentMode);

}

/**
 * @brief Calibration_widget::on_CharucoCalib_clicked if enabled, the detection mode will use the charuco method
 */
void Calibration_widget::on_CharucoCalib_clicked()
{
    vector<cv::Mat> imageSet;
    if(calib != nullptr)
        imageSet = calib->getSet();
    delete calib;
    if(ui->CharucoCalib->isChecked()){
//                calib = new CharucoCalibration(imageSet);
//                currentCalibMode = CalibrationMode::CHARUCO;
    }else{
        calib = new ChessboardCalibration(imageSet);
        currentCalibMode = CalibrationMode::CHESSBOARD;

    }
}

/**
 * @brief Calibration_widget::on_stereoCalibration_clicked this function will calibrate a set of pairs images
 */
void Calibration_widget::on_stereoCalibration_clicked()
{

    if( currentCalibMode == CalibrationMode::STEREO){
        calib->calibrate();

    }else{


        QStringList filePath = QFileDialog::getOpenFileNames(
                    this, tr("Select the files for calibration"),
                    tr("./resources/"),
                    tr("Image files (*.png *.jpg *.bmp)") );

        if(!filePath.isEmpty()){

            vector<Mat> imagesL;
            size_t count = filePath.size();

            for(size_t i = 0; i < count; i++){
                imagesL.push_back(imread(filePath[i].toStdString()));
            }

            QStringList filePath = QFileDialog::getOpenFileNames(
                        this, tr("Select the files for calibration"),
                        tr("./resources/"),
                        tr("Image files (*.png *.jpg *.bmp)") );

            if(!filePath.isEmpty()){

                vector<Mat> imagesR;
                size_t count = filePath.size();

                for(size_t i = 0; i < count; i++){
                    imagesR.push_back(imread(filePath[i].toStdString()));
                }

                delete calib;
                calib = new PT_StereoCalibration(imagesL, imagesR, 6, 9, currentCalibMode);
                ui->undistortedButton->setEnabled(true);
                enableFeatures(true);
                int nbImg = calib->getSet().size();
                ui->nbImage->setNum(nbImg);
                currentCalibMode = CalibrationMode::STEREO;
                on_originalImage_clicked();

            }
            else
                cout << "couldn't open right files" << endl;
        }else
            cout << "couldn't open left files" << endl;
    }

}

/**
 * @brief Calibration_widget::on_Quit_clicked delete the calibration ui
 */
void Calibration_widget::on_Quit_clicked()
{
    delete this;
}

/**
 * @brief Calibration_widget::on_ImageSetFromVideo_clicked Load a pair of stereo image from directory and launch it.
 * When used, a popup video will open, displaying the video while running.\n
 * You have access to multiple commands:\n
 * + 'p' : pause the video\n
 * + 'q' : close the video\n
 * + 'v' : save an image of the video to test for the calibration\n
 *
 * To have enough image for calibration, you need at least 10 images. If you don't have enough of them,\n
 * The calibration will be impossible
 */
void Calibration_widget::on_ImageSetFromVideo_clicked()
{
    QString fileName2 = QFileDialog::getOpenFileName(this, this->tr("Select a video"), "resources/", this->tr("Video Files (*.mp4)"));
    if(fileName2.isEmpty()){
        cout << "couldn't open left video" << endl;
        return;
    }
    std::cout << fileName2.toStdString() << std::endl;
    cv::VideoCapture cap(fileName2.toStdString());
    if(cap.isOpened()){
        vector<ImgCv> stereoImgTab;

        std::cout << "video opened" << std::endl;
        fileName2 = QFileDialog::getOpenFileName(this, this->tr("Select a video"), "resources/", this->tr("Video Files (*.mp4)"));
        if(fileName2.isEmpty()){
            cout << "couldn't open right video" << endl;
            return;
        }
        std::cout << fileName2.toStdString() << std::endl;
        cv::VideoCapture capR(fileName2.toStdString());
        if(capR.isOpened()){
            cv::Mat imgVidL, imgVidR;
            ImgCv img;
            int key = ' ';
            if(!cap.grab() || !capR.grab()) return;

            double framerate = cap.get(CV_CAP_PROP_FPS);
            while(key != 'q'){

                if(key == 'p'){

                    std::cout << "pause" << std::endl;

                    key = waitKey((int) framerate);
                    if(key == 'p')
                        key = ' ';
                    else if(key == 'v'){
                        std::cout << "saved" << std::endl;

                        stereoImgTab.push_back(img.getImg());
                        if(stereoImgTab.size() >= 20)
                            key = 'q';
                    }else
                        key = 'p';

                }else{
                    cap.retrieve(imgVidL);
                    capR.retrieve(imgVidR);
                    img.setImg(imgVidL, imgVidR);

                    imagecv::displayImage(*ui->Displayer, img);

                    imshow("video test", img);
                    std::cout << "next frame" << std::endl;

                    if(key == 'v'){
                        std::cout << "saved" << std::endl;

                        stereoImgTab.push_back(img.getImg());
                        if(stereoImgTab.size() >= 20)
                            key = 'q';
                    }
                    key = waitKey((int) framerate);
                    if(!cap.grab() || !capR.grab())
                        key = 'q';

                }
            }

            capR.release();
        }
        destroyWindow("video test");
        cap.release();
        if(!stereoImgTab.empty() && stereoImgTab.size() >= 10){
            delete calib;
            calib = new PT_StereoCalibration(stereoImgTab);
            ui->undistortedButton->setEnabled(true);
            enableFeatures(true);
            int nbImg = calib->getSet().size();
            ui->saveSet->setEnabled(true);
            currentCalibMode = CalibrationMode::STEREO;

            ui->nbImage->setNum(nbImg);
            on_originalImage_clicked();
        }else
            std::cout << "not enough material to calibrate." << std::endl;

    }else
        std::cout << "file empty" << std::endl;

}

/**
 * @brief Calibration_widget::on_saveSet_clicked Save the image (more likely to be distorted) set into directories.
 * Depending of the mode (ie. stereo or single),\n
 * This function will save the stereo image set into two directories.\n
 * The first (resp. the second) directory selection popup is for the left (resp. right) image set.\n
 * \n
 * If that is just a single image, you will have to only chose one directory to save this set.
 * \n
 * The image are saved with the format jpg because of his smaller size, but we are losing few informations\n
 * because of this compression. Depending of the robot settings, png could be a better choice\n
 * \n
 *
 */
void Calibration_widget::on_saveSet_clicked()
{
    if(currentCalibMode == CalibrationMode::STEREO){
        QString dirPathL = QFileDialog::getExistingDirectory(this, tr("choose the directory that will contain the left images set"));
        QString dirPathR = QFileDialog::getExistingDirectory(this, tr("choose the directory that will contain the right images set"));

        if(!dirPathL.isEmpty() && !dirPathR.isEmpty()){
            vector<Mat> &calibSet = calib->getSet();
            QString dpathL(dirPathL), dpathR(dirPathR);
            for (size_t i(0); i < calibSet.size(); i++) {
                dirPathL = dpathL;
                dirPathR = dpathR;
                string filenameL("/calib_reftImage_");
                string filenameR("/calib_rightImage_");

                filenameL += std::to_string(i+1);
                filenameL += ".jpg";
                filenameR += std::to_string(i+1);
                filenameR += ".jpg";
                ImgCv Matimg = calibSet.at(i);
                QImage const &imgL = mat_to_qimage(Matimg.getImgL());
                QImage const &imgR = mat_to_qimage(Matimg.getImgR());

                imgL.save(dirPathL.append(tr(filenameL.c_str())));
                imgR.save(dirPathR.append(tr(filenameR.c_str())));

            }
        }
    }
    else{
        QString dirPath = QFileDialog::getExistingDirectory(this, tr("choose a dir to save the set"));
        if(!dirPath.isEmpty()){
            vector<Mat> &calibSet = calib->getSet();
            QString dpath(dirPath);
            for (size_t i(0); i < calibSet.size(); i++) {
                dirPath = dpath;
                string filename("/calib_stereoImage_");
                filename += std::to_string(i+1);
                filename += ".jpg";
                Mat &Matimg = calibSet.at(i);
                QImage const &img = mat_to_qimage(Matimg);
                img.save(dirPath.append(tr(filename.c_str())));
            }
        }
    }
}

/**
 * @brief Calibration_widget::on_saveUndistorted_clicked Save the undistorted image set into directories.
 * Depending of the mode (ie. stereo or single),\n
 * This function will save the stereo image set into two directories.\n
 * The first (resp. the second) directory selection popup is for the left (resp. right) image set.\n
 * \n
 * If that is just a single image, you will have to only chose one directory to save this set.
 * \n
 * The image are saved with the format jpg because of his smaller size, but we are losing few informations\n
 * because of this compression. Depending of the robot settings, png could be a better choice\n
 * \n
 *
 */
void Calibration_widget::on_saveUndistorted_clicked()
{
    if(currentCalibMode == CalibrationMode::STEREO){
        QString dirPathL = QFileDialog::getExistingDirectory(this, tr("choose the directory that will contain the left images set"));
        QString dirPathR = QFileDialog::getExistingDirectory(this, tr("choose the directory that will contain the right images set"));

        if(!dirPathL.isEmpty() && !dirPathR.isEmpty()){
            vector<Mat> &calibSet = calib->getSet();
            QString dpathL(dirPathL), dpathR(dirPathR);
            for (size_t i(0); i < calibSet.size(); i++) {
                calib->setNextImgIndex(i);
                dirPathL = dpathL;
                dirPathR = dpathR;
                string filenameL("/ND_reftImage_");
                string filenameR("/ND_rightImage_");

                filenameL += std::to_string(i+1);
                filenameL += ".jpg";
                filenameR += std::to_string(i+1);
                filenameR += ".jpg";
                ImgCv Matimg = calib->undistorted_image();
                QImage const &imgL = mat_to_qimage(Matimg.getImgL());
                QImage const &imgR = mat_to_qimage(Matimg.getImgR());

                imgL.save(dirPathL.append(tr(filenameL.c_str())));
                imgR.save(dirPathR.append(tr(filenameR.c_str())));

            }
        }
    }
    else{
        QString dirPath = QFileDialog::getExistingDirectory(this, tr("choose a dir to save the set"));
        if(!dirPath.isEmpty()){
            vector<Mat> &calibSet = calib->getSet();
            QString dpath(dirPath);
            for (size_t i(0); i < calibSet.size(); i++) {
                dirPath = dpath;
                string filename("/ND_stereoImage_");
                filename += std::to_string(i+1);
                filename += ".jpg";
                Mat &Matimg = calibSet.at(i);
                QImage const &img = mat_to_qimage(Matimg);
                img.save(dirPath.append(tr(filename.c_str())));
            }
        }
    }
}
