
#include "disparity.h"

using namespace cv;
using namespace std;
using namespace imagecv;

/**
 * @brief Disparity::DisparityCreate the disparity gui, and define the "default" parameters for the disparity map generation.
 *
 * @param parent parent of this widget
 */
Disparity::Disparity(string const &calibFile, QWidget *parent) :
    QWidget(parent),
    displayMode(DispDisplayerMode::NORMAL),
    ui(new Ui::Disparity),
    dispMap(), depthMap(),
    img(nullptr),
    imgtoDisplay(),
    calibFilePath(calibFile),
    trackWindow(Rect(0, 0, 10, 10))
{
    ui->setupUi(this);

    IO_SADWindowSize = 5;
    IO_numberOfDisparities = 64;
    IO_preFilterCap = 62;
    IO_minDisparity = 19;
    IO_uniquenessRatio = 5;
    IO_speckleWindowSize = 0;
    IO_speckleRange = 8;
    IO_disp12MaxDif = -1;
    IO_textureTreshold = 5;
    IO_smallerBlockSize = 12;
    IO_tresholdFilter = 50;
    IO_sigma = 2.9;
    IO_lambda=10000;

    IO_P1 = 156;
    IO_P2 = 864;

    IO_full_scale = false;

    width = ui->image_loaded->width();
    height= ui->image_loaded->height();
    img = new ImgCv();
}

Disparity::~Disparity(){
    qDebug("delete disparity");
    delete ui;
    delete img;
}


/**
 * @brief Display the disparity map parameters and display it to the right
 * \n
 * The disparity map will be generated and displayed depending of the parameters\n
 * using the differents sbm and sliders.\n
 * \n
 * If no image has been loaded, the program just display an error.\n
 * \n
 * When the image has been correctly generated, it's displayed in the right slot.
 */
void Disparity::displayDisparityMap(){

    if(imgtoDisplay.empty() || imgtoDisplay.getImgR().empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }

    cv::Mat const &imgR(imgtoDisplay.getImgR());

    /*  APPLY SELECTED PARAMETERS DISPARITY MAP  */
    if(ui->filter->isChecked()){
        if(ui->sbm->isChecked()){
            dispMap = imgtoDisplay.disparity_post_filtering(IO_minDisparity,
                                                            IO_numberOfDisparities,
                                                            IO_SADWindowSize,
                                                            IO_disp12MaxDif,
                                                            IO_preFilterCap,
                                                            IO_uniquenessRatio,
                                                            IO_speckleWindowSize,
                                                            IO_speckleRange,
                                                            IO_textureTreshold,
                                                            IO_tresholdFilter,
                                                            IO_sigma,
                                                            IO_lambda);

        }else{
            dispMap = imgtoDisplay.disparity_post_filtering(IO_minDisparity,
                                                            IO_numberOfDisparities,
                                                            IO_SADWindowSize,
                                                            IO_P1,
                                                            IO_P2,
                                                            IO_disp12MaxDif,
                                                            IO_preFilterCap,
                                                            IO_uniquenessRatio,
                                                            IO_speckleWindowSize,
                                                            IO_speckleRange,
                                                            IO_full_scale,
                                                            IO_sigma,
                                                            IO_lambda);

        }
    }else{

        if(ui->sbm->isChecked()){
            dispMap = imgtoDisplay.sbm(IO_minDisparity,
                                       IO_numberOfDisparities,
                                       IO_SADWindowSize,
                                       IO_disp12MaxDif,
                                       IO_preFilterCap,
                                       IO_uniquenessRatio,
                                       IO_speckleWindowSize,
                                       IO_speckleRange,
                                       IO_textureTreshold,
                                       IO_tresholdFilter
                                       );

        }else{

            this->IO_P1 = 8* imgR.channels() * IO_SADWindowSize * IO_SADWindowSize;
            this->IO_P2 = 32* imgR.channels() * IO_SADWindowSize * IO_SADWindowSize;

            if(ui->checkbox_fullScale->isChecked()) //Consume a lot of process time
                this->IO_full_scale = StereoSGBM::MODE_HH;
            else
                this->IO_full_scale = false;
            /*  END PARAMETERS DISPARITY MAP    */

            //Generate the disparity map
            dispMap =imgtoDisplay.disparity_map_SGBM(IO_minDisparity,
                                                     IO_numberOfDisparities,
                                                     IO_SADWindowSize,
                                                     IO_P1,
                                                     IO_P2,
                                                     IO_disp12MaxDif,
                                                     IO_preFilterCap,
                                                     IO_uniquenessRatio,
                                                     IO_speckleWindowSize,
                                                     IO_speckleRange,
                                                     IO_full_scale
                                                     );


        }
    }
    cv::Mat dispNorm;
    normalize(dispMap, dispNorm, 0, 255, NORM_MINMAX, CV_8U);

    QImage img2 = mat_to_qimage(dispNorm).scaled(width,height, Qt::KeepAspectRatio);   //Create a new image which will fit the window

    //Colorize the disparity map if the dedicated sbm is checked
    if(ui->checkbox_colorize->isChecked()){
        // Apply the colormap:
        Mat cm_img0;
        applyColorMap(qimage_to_mat(img2), cm_img0, COLORMAP_JET);
        img2 = mat_to_qimage(cm_img0);
    }

    //Display the generated disparity map in the left image slot
    ui->image_loaded->setPixmap(QPixmap::fromImage(img2));
    ui->image_loaded->adjustSize();
    displayMode = DispDisplayerMode::DISPARITY;

}

/**
 * @brief Open a gui to select an image when you push the load button
 */
void Disparity::on_loadImage_clicked(){
    if(load_file(*this, *img)){
        ui->sbm->setCheckState(Qt::CheckState::Unchecked);

        QMessageBox::information(this, "Open image", "Image loaded");
        img->setImg(*img, true);
        imgtoDisplay.setImg(img->getImg(), true);
        trackWindow = Rect(imgtoDisplay.rows/2, 0, imgtoDisplay.rows/2, imgtoDisplay.getImgL().cols/2);

        displayImage(imgtoDisplay);

        qDebug(" *** Image file correctly loaded *** ");
    }
    if(img->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
}

/**
 * @brief Reset the disparity map parameters to default values (arbitrary fixed, as the more "commons")
 */
void Disparity::on_reset_image_clicked() {
    displayMode = DispDisplayerMode::NORMAL;
    imgtoDisplay.setImg(img->getImg(), true);
    displayImage(imgtoDisplay);
}


/**
 * @brief Load an cv::mat img, display it in the right panel and finally split it then save them in two pointers
 * @param img the stereo image to manage, split and display
 */
void Disparity::set_img_mat(ImgCv &img){
    this->img->setImg(img, true);
    if(calibFilePath.empty())
        ui->rectified->setCheckState(Qt::CheckState::Unchecked);
    imgtoDisplay.setImg(this->img->getImg(), true);

    displayImage(imgtoDisplay);
}
/**
 * @brief Load the calibration file and associate each value of the "DisparityParameter" matrix to the
 * corresponding diparity map parameter according the disparity mode (SBM, SGBM, SBM+PS, SGBM+PS)
 * @param the calibration file to read
 */
void Disparity::setCalibrationFile(const string &calibFile)
{
    this->calibFilePath = calibFile;
    FileStorage fs(calibFilePath, FileStorage::READ);
    if(fs.isOpened()){
        cv::Mat param;
        fs["DisparityParameter"] >> param;
        switch(param.at<int>(0)){
        case 0:
            std::cout << "sbm entries" << std::endl;

            IO_minDisparity = param.at<int>(1);
            IO_numberOfDisparities = param.at<int>(2);
            IO_SADWindowSize = param.at<int>(3);
            IO_disp12MaxDif = param.at<int>(4);
            IO_preFilterCap = param.at<int>(5);
            IO_uniquenessRatio = param.at<int>(6);
            IO_speckleWindowSize = param.at<int>(7);
            IO_speckleRange = param.at<int>(8);
            IO_textureTreshold = param.at<int>(9);
            IO_tresholdFilter = param.at<int>(10);

            ui->sbm->setChecked(true);
            ui->filter->setChecked(false);
            std::cout << "sbm done" << std::endl;

            break;
        case 1:
            std::cout << "sbm + PS entries" << std::endl;

            IO_minDisparity = param.at<int>(1);
            IO_numberOfDisparities = param.at<int>(2);
            IO_SADWindowSize = param.at<int>(3);
            IO_disp12MaxDif = param.at<int>(4);
            IO_preFilterCap = param.at<int>(5);
            IO_uniquenessRatio = param.at<int>(6);
            IO_speckleWindowSize = param.at<int>(7);
            IO_speckleRange = param.at<int>(8);
            IO_textureTreshold = param.at<int>(9);
            IO_tresholdFilter = param.at<int>(10);
            IO_sigma = ((float)(param.at<int>(11))/10);
            IO_lambda = param.at<int>(12);

            ui->sbm->setChecked(true);
            ui->filter->setChecked(true);
            std::cout << "sbm  +PS done" << std::endl;
            break;
        case 2:
            std::cout << "SGBM entries" << std::endl;

            IO_minDisparity = param.at<int>(1);
            IO_numberOfDisparities = param.at<int>(2);
            IO_SADWindowSize = param.at<int>(3);
            IO_P1 = param.at<int>(4);
            IO_P2 = param.at<int>(5);
            IO_disp12MaxDif = param.at<int>(6);
            IO_preFilterCap = param.at<int>(7);
            IO_uniquenessRatio = param.at<int>(8);
            IO_speckleWindowSize =param.at<int>(9);
            IO_speckleRange = param.at<int>(10);
            IO_full_scale = param.at<int>(11);

            if(IO_full_scale != 0)
                ui->checkbox_fullScale->setChecked(true);
            else
                ui->checkbox_fullScale->setChecked(false);

            ui->sbm->setChecked(false);
            ui->filter->setChecked(false);
            std::cout << "SGBM done" << std::endl;
            break;
        case 3:
            std::cout << "SGBM + PS entries" << std::endl;

            IO_minDisparity = param.at<int>(1);
            IO_numberOfDisparities = param.at<int>(2);
            IO_SADWindowSize = param.at<int>(3);
            IO_P1 = param.at<int>(4);
            IO_P2 = param.at<int>(5);
            IO_disp12MaxDif = param.at<int>(6);
            IO_preFilterCap = param.at<int>(7);
            IO_uniquenessRatio = param.at<int>(8);
            IO_speckleWindowSize =param.at<int>(9);
            IO_speckleRange = param.at<int>(10);
            IO_full_scale = param.at<int>(11);
            IO_sigma = ((float)(param.at<int>(12))/10);
            IO_lambda = param.at<int>(13);

            if(IO_full_scale != 0)
                ui->checkbox_fullScale->setChecked(true);
            else
                ui->checkbox_fullScale->setChecked(false);
            ui->sbm->setChecked(false);
            ui->filter->setChecked(true);

            std::cout << "SGBM + PS done" << std::endl;

            break;
        default:
            std::cout << "[ERROR] can't match to any case" << std::endl;
            break;

        }
        ui->spinBox_minDisparity->setValue(IO_minDisparity);
        ui->spinBox_minDisparity->setValue(IO_numberOfDisparities);
        ui->spinBox_windowSize->setValue(IO_SADWindowSize);
        ui->spinBox_disp12MaxDiff->setValue(IO_disp12MaxDif);
        ui->spinBox_prefilterCap->setValue(IO_preFilterCap);
        ui->spinBox_uniquenessRatio->setValue(IO_uniquenessRatio);
        ui->spinBox_speckleWindowSize->setValue(IO_speckleWindowSize);
        ui->spinBox_speckleRange->setValue(IO_speckleRange);
        ui->spinBox_textureTreshlod->setValue(IO_textureTreshold);
        ui->spinBox_tresholdFilter->setValue(IO_tresholdFilter);
        ui->doubleSpinBox_sigma->setValue(IO_sigma);
        ui->spinBox_lambda_2->setValue(IO_lambda);
        fs.release();
    }
    else{
        qDebug("[ERROR] in: setCalibrationFile\nNo calibration file loaded");
    }
}

/**
 * @brief Return the image to display
 *
 */
ImgCv * Disparity::get_img_mat(){
    return &imgtoDisplay;
}
/**
 * @brief Using the tracker, find a ROI and then compute this ROI's depth map.
 * @param roiTracked : the rectangle corresponding to the region tracked
 * @return the depth map of the ROITracked
 */
Mat Disparity::get_depth_map(bool roiTracked)
{
    DispDisplayerMode old(displayMode);

    on_depthMap_clicked();
    if(roiTracked){
        on_track_clicked();
        displayMode = old;

        return depthMap(trackWindow);
    }
    displayMode = old;
    return depthMap;
}


/**
 * @brief Display a cv:mat image in the right image slot
 * @param image image to be displayed
 */
void Disparity::displayImage(Mat const& image){
    if (displayMode == DispDisplayerMode::NORMAL) {
        ui->image_loaded->setMaximumSize(width, height);
        ui->image_loaded->setPixmap(QPixmap::fromImage(mat_to_qimage(image)));
        ui->image_loaded->adjustSize();
    }else if(displayMode == DispDisplayerMode::DISPARITY)
        displayDisparityMap();
    else
        on_track_clicked();

}

/**
 * @brief Apply the Sobel filter on the disparity map, to display its contours
 */
void Disparity::on_Sobel_clicked()
{
    if(imgtoDisplay.empty() || imgtoDisplay.getImgR().empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }
    Mat sobel, dispNorm;
    displayDisparityMap();
    normalize(dispMap, dispNorm, 0, 255, NORM_MINMAX, CV_8U);
    sobel = ImgCv::contour_sobel(dispNorm);
    imagecv::displayImage(*ui->image_loaded, sobel);

}

/**
 * @brief Applies the Laplacian filter on the disparity map, to display its contours
 */

void Disparity::on_Laplace_clicked()
{
    if(imgtoDisplay.empty() || imgtoDisplay.getImgR().empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }

    Mat laplace, dispNorm;
    displayDisparityMap();
    normalize(dispMap, dispNorm, 0, 255, NORM_MINMAX, CV_8U);
    laplace = ImgCv::contour_laplace(dispNorm);
    imagecv::displayImage(*ui->image_loaded, laplace);

}

/**
 * @brief Load two image's sets, and display the stereo images as a video\n
 * Load the set corresponding to left images \n
 * Load the set corresponding to the right images\n
 * The stereo images are displayed, as a video
 */
void Disparity::on_video_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(
                this, tr("Select the files for calibration"),
                tr("./resources/") );

    if(!filePath.isEmpty()){

        string fileLeft(filePath.toStdString());
        std::cout << fileLeft+"/left_%03d.jpg" << std::endl;

        cv::VideoCapture capL(fileLeft+"/left_%03d.jpg", cv::CAP_IMAGES );
        if(capL.isOpened()){

            QString filePath = QFileDialog::getExistingDirectory(
                        this, tr("Select the files for calibration"),
                        tr("./resources/") );

            if(!filePath.isEmpty()){


                fileLeft = filePath.toStdString();
                cv::VideoCapture capR(fileLeft+"/right_%03d.jpg", cv::CAP_IMAGES );

                if(capR.isOpened()){
                    std::cout << "video opened" << std::endl;

                    cv::Mat displayedImg, imgVidL, imgVidR, Q, res;
                    int key = ' ', prevKey = key;
                    if(!capL.grab() || !capR.grab()) return;
                    capL.retrieve(imgVidL);
                    capR.retrieve(imgVidR);
                    img->setImg(imgVidL, imgVidR);
                    imgtoDisplay.setImg(*img, true);
                    capL.set(cv::CAP_PROP_POS_FRAMES, 0);
                    capL.grab();
                    capR.set(cv::CAP_PROP_POS_FRAMES, 0);
                    capR.grab();
                    if(calibFilePath.empty()){
                        qDebug("[INFO] Load a calibration file before");

                        QString inFile = QFileDialog::getOpenFileName(this, tr("open a calibration file"),"",tr("yaml files (*.yml)"));
                        if(!inFile.isEmpty()){
                            calibFilePath = inFile.toStdString();
                            Q = (ImgCv::getDispToDepthMat(calibFilePath));

                            on_depthMap_clicked();
                            qDebug("[INFO] depth loaded");

                        }
                        else{
                            qDebug("[ERROR] No calibration file loaded");
                        }
                    }
                    double framerate = capL.get(CV_CAP_PROP_FPS);
                    qDebug("[INFO] video runnig");

                    while(key != 'q'){

                        if(key == 'p'){

                            std::cout << "pause" << std::endl;
                            prevKey = key;
                        }else{
                            capL.retrieve(imgVidL);
                            capR.retrieve(imgVidR);
                            imgtoDisplay.setImg(imgVidL, imgVidR);
                            if(!calibFilePath.empty())
                                imgtoDisplay.setImg(ImgCv::rectifiedImage(imgtoDisplay, calibFilePath), true);

                            if(key == 's'){
                                displayDisparityMap();
                            }else{
                                displayMode = DispDisplayerMode::NORMAL;
                                if(key == 'l')
                                    displayedImg = imgtoDisplay.getImgL();
                                else if(key == 'r')
                                    displayedImg = imgtoDisplay.getImgR();
                                if(key == 'd' && !Q.empty()){
                                    displayDisparityMap();
                                    res = ImgCv::depthMap(dispMap, Q);
                                    normalize(res, res, 0, 255, NORM_MINMAX, CV_32F);
                                    displayedImg =res;
                                }else
                                    displayedImg = imgtoDisplay;


                                displayImage(displayedImg);
                            }
                            imshow("video test", displayedImg);

                            std::cout << "next frame" << "\tkey : " << (char)prevKey << std::endl;

                            prevKey = key;

                            if(!capL.grab() || !capR.grab()){
                                capL.set(cv::CAP_PROP_POS_FRAMES, 0);
                                capL.grab();
                                capR.set(cv::CAP_PROP_POS_FRAMES, 0);
                                capR.grab();
                            }

                        }

                        key = waitKey((int) framerate);
                        if(key == 255)
                            key = prevKey;
                    }

                }
                capR.release();
                destroyWindow("video test");

            }else
                std::cout << "file empty" << std::endl;
            capL.release();
        }
    }
}

/**
 * @brief Compute and display the depth map when the depthMap's button is triggered
 */
void Disparity::on_depthMap_clicked()
{
    if(imgtoDisplay.empty()){
        qDebug("[INFO] Load a stereo file before");
        if(!load_file(*this, *img, true)){
            qDebug("[ERROR] No images loaded");
            return;
        }
        imgtoDisplay = img->getImg();
    }
    if(calibFilePath.empty()){
        qDebug("[INFO] Load a calibration file before");

        QString inFile = QFileDialog::getOpenFileName(this, tr("open a calibration file"),"",tr("yaml files (*.yml)"));
        if(!inFile.isEmpty()){
            calibFilePath = inFile.toStdString();
            on_depthMap_clicked();
        }
        else{
            qDebug("[ERROR] No calibration file loaded");
            return;
        }
    }
    else{
        qDebug("[INFO] in on_depthMap_clicked ");

        imgtoDisplay.setImg(ImgCv::rectifiedImage(*img, calibFilePath), true);
        displayDisparityMap();

        cv::Mat Q = ImgCv::getDispToDepthMat(calibFilePath);
        qDebug("[INFO] iQ ");

        depthMap = ImgCv::depthMap(dispMap,Q);
        qDebug("[INFO] in on_depthMap_clicked depth map generated ");
        ImgCv::trackCamShift(imgtoDisplay.getImgL(), trackWindow);
        displayMode = DispDisplayerMode::NORMAL;
        FileStorage fs("DepthMap.yml", FileStorage::WRITE);
        fs<<"Depth_map"<<depthMap;
        fs.release();
        Mat dst;
        normalize(depthMap, dst, 0, 255, CV_MINMAX, CV_32F );
        imshow("depth",dst);

    }
}


/**
 * @brief Load two images and display them in the right slot
 */
void Disparity::on_load_two_images_clicked()
{
    if(load_file(*this, *img, true)){
        QMessageBox::information(this, "Open the first image", "Image loaded");
        ui->sbm->setCheckState(Qt::CheckState::Unchecked);
        imgtoDisplay.setImg(img->getImg(), true);
        trackWindow = Rect(imgtoDisplay.rows/2, 0, imgtoDisplay.rows/2, imgtoDisplay.getImgL().cols/2);

    }

    else{
        qDebug("Impossible to load files");
    }
    displayImage(imgtoDisplay);
}

/**
 * @brief when the checkBox SBM is clicked  : \n
 * if it's checked then compute, he disparity Map using the sbm parameters
 * else compute the disparity map using sgbm parameters
 */
void Disparity::on_sbm_clicked()
{
    if (ui->sbm->isChecked())
        displayDisparityMap();
    else
        displayDisparityMap();
}


/**
 * @brief Undistort the images when the checkBox "Rectified" is checked.
 */
void Disparity::on_rectified_clicked()
{
    if(img->empty() || img->getImgR().empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        ui->rectified->setCheckState(Qt::CheckState::Unchecked);
        return;
    }

    if(calibFilePath.empty()){
        qDebug("[ERROR] Please, load a calibration first");
        ui->rectified->setCheckState(Qt::CheckState::Unchecked);
        return;
    }

    if (ui->rectified->isChecked()){
        imgtoDisplay.setImg(ImgCv::rectifiedImage(*img, calibFilePath),true);
    }
    else
        imgtoDisplay.setImg(img->getImg(), true);

    displayImage(imgtoDisplay);
}
/**
 * @brief When the button is clicked, load the calibration file
 */
void Disparity::on_calib_clicked()
{
    QString inFile = QFileDialog::getOpenFileName(this, tr("open a calibration file"),"",tr("yaml files (*.yml)"));
    if(!inFile.isEmpty())
        setCalibrationFile(inFile.toStdString());
    else
        qDebug("[ERROR] in: on_calib_clicked\nNo calibration parameters loaded");
}
/**
 * @brief Display the disparity map
 */
void Disparity::on_dispMap_clicked()
{

    displayDisparityMap();
}
/**
 * @brief Save the depthMap(trackWindow)'s values in a file
 * and print the average of it.
 */
void Disparity::on_save_depth_map_clicked(){

    on_depthMap_clicked();
    on_track_clicked();
    displayMode = DispDisplayerMode::DISPARITY;
    float average = ImgCv::calculateDistanceDepthMap(depthMap(trackWindow));
    std::cout<<"average : "<<average<<std::endl;
}

/**
 * @brief Save the current disparity map parameters in the calibration file.
 */
void Disparity::on_dispParam_clicked()
{
    if(dispMap.empty()){
        qDebug("[ERROR] in: on_dispParam_clicked\n No disparity map generated");
        return;
    }
    if(calibFilePath.empty()){
        QString filePath = QFileDialog::getOpenFileName(this, this->tr("calib file"), this->tr("./resources/"), this->tr("yaml Files (*.yml)"));
        if(filePath.isEmpty()){
            qDebug("[ERROR] in: on_dispParam_clicked\nNo file opened");

            return;
        }
        calibFilePath = filePath.toStdString();
    }
    FileStorage fs(calibFilePath, FileStorage::APPEND);
    if(fs.isOpened()){
        std::vector<int> param;

        if(ui->sbm->isChecked()){
            param.push_back(0);
            param.push_back(IO_minDisparity);
            param.push_back(IO_numberOfDisparities);
            param.push_back(IO_SADWindowSize);
            param.push_back(IO_disp12MaxDif);
            param.push_back(IO_preFilterCap);
            param.push_back(IO_uniquenessRatio);
            param.push_back(IO_speckleWindowSize);
            param.push_back(IO_speckleRange);
            param.push_back(IO_textureTreshold);
            param.push_back(IO_tresholdFilter);

        }
        else {
            param.push_back(2);
            param.push_back(IO_minDisparity);
            param.push_back(IO_numberOfDisparities);
            param.push_back(IO_SADWindowSize);
            param.push_back(IO_P1);
            param.push_back(IO_P2);
            param.push_back(IO_disp12MaxDif);
            param.push_back(IO_preFilterCap);
            param.push_back(IO_uniquenessRatio);
            param.push_back(IO_speckleWindowSize);
            param.push_back(IO_speckleRange);
            param.push_back(IO_full_scale);
        }
        if(ui->filter->isChecked()){
            param.at(0) += 1;
            param.push_back((int)(IO_sigma*10));
            param.push_back(IO_lambda);

        }

        cv::Mat paramMat(param, true);
        fs.writeComment("Disparity parameter\n0: sbm\n2: sgbm\n 1: sbm + post filtering\n 3: sgbm + post filtering", true);
        fs << "DisparityParameter" << paramMat;
        fs.release();
    }else
        qDebug("[ERROR] in: on_dispParam_clicked\n couldn't opend calib file.");




}
/**
 * @brief Track the images and display the tracker in a new window
 */
void Disparity::on_track_clicked()
{
    if(calibFilePath.empty()){
        QString filePath = QFileDialog::getOpenFileName(this, this->tr("calib file"), this->tr("./resources/"), this->tr("yaml Files (*.yml)"));
        if(filePath.isEmpty()){
            qDebug("[ERROR] in: on_dispParam_clicked\nNo file opened");

            return;
        }
        calibFilePath = filePath.toStdString();
    }

    Mat dst(imgtoDisplay.getImgL());
    rectangle(dst, trackWindow,cv::Scalar(255), 2);
    displayMode = DispDisplayerMode::TRACKER;
    imshow("tracker", dst);
}



                                                                                        /*   SLIDERS     */
/**
 * @brief Modify the value of IO_WindowSize according to the slider's one and compute the new disparity map\n
 *  Display also the new value in the corresponding spinBox
 * @param the new IO_WindowSize's value
 */
void Disparity::on_slider_windowSize_valueChanged(int value)
{
    if(value%2==0){
        value +=1;
        ui->slider_windowSize->setValue(value);
        ui->spinBox_windowSize->setValue(value);
        this->IO_SADWindowSize = value;
    }else{
        this->IO_SADWindowSize = value;
        ui->spinBox_windowSize->setValue(value);
    }
    displayDisparityMap();
}
/**
 * @brief  Modify the value of IO_numberOfDisparities according to the slider's one and compute the new disparity map\n
 *  Display also the new value in the corresponding spinBox
 * @param the new IO_numberOfDisparities' value
 */
void Disparity::on_slider_numberOfDisparities_valueChanged(int value)
{
    if(value%16!=0){
        int a = value%16;
        value -= a;
        this->IO_numberOfDisparities = value;

    }else{
        this->IO_numberOfDisparities=ui->slider_numberOfDisparities->value();

    }
    ui->spinBox_numberOfDisparities->setValue(value);
    displayDisparityMap();
}
/**
 * @brief Modify the value of IO_preFilterCap according to the slider's one and compute the new disparity map\n
 *  Display also the new value in the corresponding spinBox
 * @param the new IO_preFilterCap's value
 */
void Disparity::on_slider_preFilterCap_valueChanged(int value)
{
    if(ui->sbm->isChecked()&& value>=63){
        qDebug("prefilterCap must be within 1...63");
        value -= (value-63);
        this->IO_preFilterCap= value;
    }else if(ui->sbm->isChecked() && value<=1){
        value +=1;
        this->IO_preFilterCap=value;

    }else{
        this->IO_preFilterCap =  ui->slider_preFilterCap->value();
    }
    ui->spinBox_prefilterCap->setValue(value);
    displayDisparityMap();
}

/**
 * @brief Modify the value of IO_minDisparity according to the slider's one and compute the new disparity map\n
 *  Display also the new value in the corresponding spinBox
 * @param the new IO_minDisparity's value
 */
void Disparity::on_slider_minDisparity_valueChanged(int value)
{
    this->IO_minDisparity = value;
    ui->spinBox_minDisparity->setValue(value);
    displayDisparityMap();
}

/**
 * @brief Modify the value of IO_uniquenessRatio according to the slider's one and compute the new disparity map\n
 *  Display also the new value in the corresponding spinBox
 * @param the new IO_uniquenessRatio's value
 */
void Disparity::on_slider_uniquenessRatio_valueChanged(int value)
{
    this->IO_uniquenessRatio = value;
    ui->spinBox_uniquenessRatio->setValue(value);
    displayDisparityMap();
}


/**
 * @brief Modify the value of IO_speckleWindowSize according to the slider's one and compute the new disparity map\n
 *  Display also the new value in the corresponding spinBox
 * @param the new IO_speckleWindowSize's value
 */
void Disparity::on_slider_speckleWindowSize_valueChanged(int value)
{
    this->IO_speckleWindowSize = value;
    ui->spinBox_speckleWindowSize->setValue(value);
    displayDisparityMap();
}

/**
 * @brief Modify the value of IO_speckleRange according to the slider's one and compute the new disparity map\n
 *  Display also the new value in the corresponding spinBox
 * @param the new IO_speckleRange's value
 */
void Disparity::on_slider_speckleRange_valueChanged(int value)
{
    this->IO_speckleRange = value;
    ui->spinBox_speckleRange->setValue(value);
    displayDisparityMap();
}

/**
 * @brief Modify the value of IO_disp12MaxDiff according to the slider's one and compute the new disparity map\n
 *  Display also the new value in the corresponding spinBox
 * @param the new IO_disp12MaxDiff's value
 */
void Disparity::on_slider_disp12MaxDiff_valueChanged(int value)
{
    this->IO_disp12MaxDif = value;
    ui->spinBox_disp12MaxDiff->setValue(value);
    displayDisparityMap();
}
/**
 * @brief Modify the value of IO_textureTreshold according to the slider's one and compute the new disparity map\n
 *  Display also the new value in the corresponding spinBox
 * @param the new value of textureTreshold
 */
void Disparity::on_slider_textureTreshold_valueChanged(int value)
{
    this->IO_textureTreshold = value;
    ui->spinBox_textureTreshlod->setValue(value);
    displayDisparityMap();
}
/**
 * @brief Modify the value of IO_tresholdFilter according to the slider's one and compute the new disparity map\n
 *  Display also the new value in the corresponding spinBox
 * @param the new value of IO_tresholdFilter
 */
void Disparity::on_slider_treshold_Filter_valueChanged(int value)
{
    this->IO_tresholdFilter = value;
    ui->spinBox_tresholdFilter->setValue(value);
    displayDisparityMap();
}

/**
 * @brief Modify the value of IO_sigma(useful only for post_filtering disparity maps) according to the spinbox's one and compute the new disparity map\n
 * @param the new value of IO_sigma
 */
void Disparity::on_doubleSpinBox_sigma_valueChanged(double arg1)
{
    this->IO_sigma = float(arg1);
    displayDisparityMap();
}
/**
 * @brief Modify the value of IO_lambda(useful only for post_filtering disparity maps) according to the spinbox' one and compute the new disparity map\n
 *
 * @param the new value of the parameter IO_lambda
 */
void Disparity::on_spinBox_lambda_2_valueChanged(int arg1)
{
    this->IO_lambda = arg1;
    displayDisparityMap();
}
