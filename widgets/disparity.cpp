
#include "disparity.h"

using namespace cv;
using namespace std;
using namespace imagecv;

/**
 * @brief Disparity::DisparityCreate the disparity gui, and define the "default" parameters for the disparity map generation.
 *
 * @param parent parent of this widget
 */
Disparity::Disparity(QWidget *parent) :
    QWidget(parent),
    displayMode(DispDisplayerMode::NORMAL),
    ui(new Ui::Disparity),
    dispMap(),
    img(nullptr),
    imgtoDisplay(),
    calibFilePath()

{
    ui->setupUi(this);

    IO_SADWindowSize = 9;
    IO_numberOfDisparities = 32;
    IO_preFilterCap = 50;
    IO_minDisparity = 5;
    IO_uniquenessRatio = 5;
    IO_speckleWindowSize = 0;
    IO_speckleRange = 8;
    IO_disp12MaxDif = -1;
    IO_textureTreshold = 5;
    IO_smallerBlockSize = 12;
    IO_tresholdFilter = 50;

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
 * @brief Apply the disparity map parameters when the button got pressed and display it to the right
 * \n
 * When the apply button is triggered, a disparity map will be generated and displayed depending of the parameters\n
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

    displayMode = DispDisplayerMode::DISPARITY;
    cv::Mat const &imgR(imgtoDisplay.getImgR());
    /*  APPLY SELECTED PARAMETERS DISPARITY MAP  */
    if(ui->filter->isChecked()){
        if(ui->sbm->isChecked()){
            dispMap = imgtoDisplay.disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize);

        }else{
            dispMap = imgtoDisplay.disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize, IO_preFilterCap, IO_P1, IO_P2);

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

    QImage img2 = mat_to_qimage(dispMap).scaled(width,height, Qt::KeepAspectRatio);   //Create a new image which will fit the window

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
        displayImage(imgtoDisplay);

        qDebug(" *** Image file correctly loaded *** ");
    }
    if(img->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
}

/**
 * @brief Reset the sgbm parameters to default values (arbitrary fixed, as the more "commons")
 */
void Disparity::on_reset_image_clicked() {
    displayMode = DispDisplayerMode::NORMAL;
    ui->sbm->setCheckState(Qt::CheckState::Unchecked);
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

ImgCv * Disparity::get_img_mat(){
    return &imgtoDisplay;
}


/**
 * @brief Disparity::displayImage display a cv:mat image in the right image slot
 * @param image image to be displayed
 */
void Disparity::displayImage(Mat const& image){
    if (displayMode == DispDisplayerMode::NORMAL) {
        ui->image_loaded->setMaximumSize(width, height);
        ui->image_loaded->setPixmap(QPixmap::fromImage(mat_to_qimage(image)));
        ui->image_loaded->adjustSize();
    }else
        displayDisparityMap();

}

/**
 * @brief Disparity::on_Sobel_clicked applies the Sobel filter on the disparity map to display its contours
 */
void Disparity::on_Sobel_clicked()
{
    if(imgtoDisplay.empty() || imgtoDisplay.getImgR().empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }
    Mat sobel;
    if(ui->sbm->isChecked()&& ui->filter->isChecked())
        dispMap = imgtoDisplay.disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize);
    else if (ui->sbm->isChecked()){
        dispMap = imgtoDisplay.sbm(IO_minDisparity,
                                   IO_numberOfDisparities,
                                   IO_SADWindowSize,
                                   IO_disp12MaxDif,
                                   IO_preFilterCap,
                                   IO_uniquenessRatio,
                                   IO_speckleWindowSize,
                                   IO_speckleRange,
                                   IO_textureTreshold,
                                   IO_tresholdFilter);
    }else if (ui->filter->isChecked())
        dispMap= imgtoDisplay.disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize,IO_preFilterCap, IO_P1, IO_P2);
    else {
        dispMap = imgtoDisplay.disparity_map_SGBM(IO_minDisparity,
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
    sobel = ImgCv::contour_sobel(dispMap);
    imagecv::displayImage(*ui->image_loaded, sobel);

}

/**
 * @brief Disparity::on_Laplace_clicked applies the Laplacian filter on the disparity map to display its contours
 */

void Disparity::on_Laplace_clicked()
{
    if(imgtoDisplay.empty() || imgtoDisplay.getImgR().empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }

    Mat laplace;
    if(ui->sbm->isChecked()&& ui->filter->isChecked())
        dispMap = imgtoDisplay.disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize);
    else if(ui->sbm->isChecked())
        dispMap = imgtoDisplay.sbm(IO_minDisparity,
                                   IO_numberOfDisparities,
                                   IO_SADWindowSize,
                                   IO_disp12MaxDif,
                                   IO_preFilterCap,
                                   IO_uniquenessRatio,
                                   IO_speckleWindowSize,
                                   IO_speckleRange,
                                   IO_textureTreshold,
                                   IO_tresholdFilter);
    else if (ui->filter->isChecked())
        dispMap= imgtoDisplay.disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize,IO_preFilterCap, IO_P1, IO_P2);
    else {
        dispMap = imgtoDisplay.disparity_map_SGBM(IO_minDisparity,
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

    laplace = ImgCv::contour_laplace(dispMap);
    imagecv::displayImage(*ui->image_loaded, laplace);

}


void Disparity::on_video_clicked()
{
    /* QString filePath = QFileDialog::getOpenFileName(this, this->tr("Choose a video for left camera"), this->tr("./resources/"), this->tr("Video Files (*.mp4)"));
    if(filePath.isEmpty()) return;
    std::cout << filePath.toStdString() << std::endl;

    const string& fileLeft(filePath.toStdString());
    cv::VideoCapture capL(fileLeft);
    if(capL.isOpened()){

        filePath = QFileDialog::getOpenFileName(this, this->tr("Choose a video for right camera"), this->tr("./resources/"), this->tr("Video Files (*.mp4)"));
        if(filePath.isEmpty()) return;

        std::cout << filePath.toStdString() << std::endl;
        const string& fileRight(filePath.toStdString());
        cv::VideoCapture capR(fileRight);
        */
    QString filePath = QFileDialog::getExistingDirectory(
                this, tr("Select the files for calibration"),
                tr("./resources/") );

    if(!filePath.isEmpty()){

        string fileLeft(filePath.toStdString());
        std::cout << fileLeft+"/left_%02d.jpg" << std::endl;

        cv::VideoCapture capL(fileLeft+"/left_%02d.jpg", cv::CAP_IMAGES );
        if(capL.isOpened()){

            QString filePath = QFileDialog::getExistingDirectory(
                        this, tr("Select the files for calibration"),
                        tr("./resources/") );

            if(!filePath.isEmpty()){


                fileLeft = filePath.toStdString();
                cv::VideoCapture capR(fileLeft+"/right_%02d.jpg", cv::CAP_IMAGES );

                if(capR.isOpened()){
                    std::cout << "video opened" << std::endl;

                    cv::Mat displayedImg, imgVidL, imgVidR, Q;
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
                        }
                        else{
                            qDebug("[ERROR] No calibration file loaded");
                        }
                    }
                    double framerate = capL.get(CV_CAP_PROP_FPS);
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
                                    displayedImg = ImgCv::depthMap(dispMap, Q);
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
 * @brief Disparity::on_depthMap_clicked
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
        imgtoDisplay.setImg(ImgCv::rectifiedImage(*img, calibFilePath), true);
        displayDisparityMap();

        cv::Mat Q = ImgCv::getDispToDepthMat(calibFilePath);
        Mat depth_map = ImgCv::depthMap(dispMap,Q), dst;
        imshow("depth_map", depth_map);
        displayMode = DispDisplayerMode::NORMAL;
        displayImage(depth_map);

    }
}
/**
 * @brief Disparity::on_slider_windowSize_valueChanged
 * @param value
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
 * @brief Disparity::on_slider_numberOfDisparities_valueChanged
 * @param value
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
 * @brief Disparity::on_slider_preFilterCap_valueChanged
 * @param value
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
 * @brief Disparity::on_slider_minDisparity_valueChanged
 * @param value
 */
void Disparity::on_slider_minDisparity_valueChanged(int value)
{
    this->IO_minDisparity = value;
    ui->spinBox_minDisparity->setValue(value);
    displayDisparityMap();
}

/**
 * @brief Disparity::on_slider_uniquenessRatio_valueChanged
 * @param value
 */
void Disparity::on_slider_uniquenessRatio_valueChanged(int value)
{
    this->IO_uniquenessRatio = value;
    ui->spinBox_uniquenessRatio->setValue(value);
    displayDisparityMap();
}


/**
 * @brief Disparity::on_slider_speckleWindowSize_valueChanged
 * @param value
 */
void Disparity::on_slider_speckleWindowSize_valueChanged(int value)
{
    this->IO_speckleWindowSize = value;
    ui->spinBox_speckleWindowSize->setValue(value);
    displayDisparityMap();
}

/**
 * @brief Disparity::on_slider_speckleRange_valueChanged
 * @param value
 */
void Disparity::on_slider_speckleRange_valueChanged(int value)
{
    this->IO_speckleRange = value;
    ui->spinBox_speckleRange->setValue(value);
    displayDisparityMap();
}

/**
 * @brief Disparity::on_slider_disp12MaxDiff_valueChanged
 * @param value
 */
void Disparity::on_slider_disp12MaxDiff_valueChanged(int value)
{
    this->IO_disp12MaxDif = value;
    ui->spinBox_disp12MaxDiff->setValue(value);
    displayDisparityMap();
}
/**
 * @brief Disparity::on_slider_textureTreshold_valueChanged
 * @param value
 */
void Disparity::on_slider_textureTreshold_valueChanged(int value)
{
    this->IO_textureTreshold = value;
    ui->spinBox_textureTreshlod->setValue(value);
    displayDisparityMap();
}
/**
 * @brief Disparity::on_slider_treshold_Filter_valueChanged
 * @param value
 */
void Disparity::on_slider_treshold_Filter_valueChanged(int value)
{
    this->IO_tresholdFilter = value;
    ui->spinBox_tresholdFilter->setValue(value);
    displayDisparityMap();
}

/**
 * @brief Disparity::on_load_two_images_clicked
 */
void Disparity::on_load_two_images_clicked()
{
    if(load_file(*this, *img, true)){
        QMessageBox::information(this, "Open the first image", "Image loaded");
        ui->sbm->setCheckState(Qt::CheckState::Unchecked);
        imgtoDisplay.setImg(img->getImg(), true);
    }

    else{
        qDebug("Impossible to load files");
    }
    displayImage(imgtoDisplay);
}

/**
 * @brief Disparity::on_sbm_clicked
 */
void Disparity::on_sbm_clicked()
{
    if (ui->sbm->isChecked())
        displayDisparityMap();
    else
        displayDisparityMap();
}



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

void Disparity::on_calib_clicked()
{

    QString inFile = QFileDialog::getOpenFileName(this, tr("open a calibration file"),"",tr("yaml files (*.yml)"));
    if(!inFile.isEmpty()){
        calibFilePath = inFile.toStdString();
    }
    else{
        qDebug("[ERROR] in: on_calib_clicked\nNo calibration file loaded");
    }
}

void Disparity::on_dispMap_clicked()
{
    displayDisparityMap();
}

void Disparity::on_dispParam_clicked()
{
    if(dispMap.empty()){
        qDebug("[ERROR] in: on_dispParam_clicked\n No disparity map generated");
        return;
    }
    QString filePath = QFileDialog::getOpenFileName(this, this->tr("calib file"), this->tr("./resources/"), this->tr("yaml Files (*.yml)"));
    if(filePath.isEmpty()){
        qDebug("[ERROR] in: on_dispParam_clicked\nNo file opened");

        return;
    }

    FileStorage fs(filePath.toStdString(), FileStorage::APPEND);
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
        if(ui->filter->isChecked())
            param.at(0) += 1;

        cv::Mat paramMat(param, true);
        fs.writeComment("Disparity parameter\n0: sbm\n2: sgbm\n 1: sbm + post filtering\n 3: sgbm + post filtering", true);
        fs << "DisparityParameter" << paramMat;
        fs.release();
    }else
              qDebug("[ERROR] in: on_dispParam_clicked\n couldn't opend calib file.");




    }
