#include "calibration_widget.h"

using namespace std;
using namespace cv;
using namespace imagecv;

Calibration_widget::Calibration_widget(PT_ICalibration *calibration, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Calibration_widget),
    calib(calibration),
    currentMode(ORIGINAL)
{
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

void Calibration_widget::on_undistortedButton_clicked()
{
    if(!calib->hasIntrinsicParameters())
        return;

    currentMode = UNDISTORTED;
    display_image(UNDISTORTED);

}

void Calibration_widget::on_chesscorners_clicked()
{
    currentMode = CHESSBOARD_CORNERS;
    display_image(CHESSBOARD_CORNERS);

}

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

void Calibration_widget::enableFeatures(bool isEnabled)
{
    ui->originalImage->setEnabled(isEnabled);
    ui->chesscorners->setEnabled(isEnabled);
    ui->prevImage->setEnabled(isEnabled);
    ui->nextImage->setEnabled(isEnabled);
}

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

void Calibration_widget::on_prevImage_clicked()
{
    size_t const& decr = calib->getCurrentImgIndex();
    calib->setNextImgIndex(decr - 1);
    if(decr == calib->getCurrentImgIndex()){
        QMessageBox::information(this, tr("start of set"), tr("reach the beginning of the set"));
    } else
        display_image(currentMode);
}

void Calibration_widget::on_save_clicked()
{

    if( calib == nullptr || !calib->hasIntrinsicParameters()){
        QMessageBox::warning(this, tr("Error while saving file"), tr("no calibration to save."));
        return;
    }
    string outFile("test.yml");
    calib->saveCalibration(outFile);
}

void Calibration_widget::on_loadCalib_clicked()
{
    string inFile("test.yml");

    if(calib->runCalibration(inFile)){
        ui->undistortedButton->setEnabled(true);
        enableFeatures(true);
    }
    else
        QMessageBox::warning(this, tr("Parse error"), tr("Error while parsing file"));

}

void Calibration_widget::on_originalImage_clicked()
{
    currentMode = ORIGINAL;
    display_image(ORIGINAL);
}

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

void Calibration_widget::on_CharucoCalib_clicked()
{
    vector<cv::Mat> imageSet;
    if(calib != nullptr)
        imageSet = calib->getSet();
    delete calib;
    if(ui->CharucoCalib->isChecked()){
//        calib = new CharucoCalibration(imageSet);
    }else{
        calib = new ChessboardCalibration(imageSet);
    }
}

void Calibration_widget::on_stereoCalibration_clicked()
{

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
            calib = new PT_StereoCalibration(imagesL, imagesR);
            calib->prepareCalibration();
            ui->undistortedButton->setEnabled(true);
            enableFeatures(true);
            int nbImg = calib->getSet().size();
            ui->nbImage->setNum(nbImg);
            on_originalImage_clicked();
        }
        else
            cout << "couldn't open files" << endl;
    }else
        cout << "couldn't open files" << endl;

}

void Calibration_widget::on_Quit_clicked()
{
    delete this;
}

void Calibration_widget::on_ImageSetFromVideo_clicked()
{
    QString fileName2 = QFileDialog::getOpenFileName(this, this->tr("Sélectionnez une image"), "resources/", this->tr("Video Files (*.mp4)"));
    std::cout << fileName2.toStdString() << std::endl;
    cv::VideoCapture cap(fileName2.toStdString());
    if(cap.isOpened()){
        vector<ImgCv> stereoImgTab;

        std::cout << "video opend" << std::endl;
        fileName2 = QFileDialog::getOpenFileName(this, this->tr("Sélectionnez une image"), "resources/", this->tr("Video Files (*.mp4)"));
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
            ui->nbImage->setNum(nbImg);
            on_originalImage_clicked();
        }else
            std::cout << "not enough material to calibrate." << std::endl;

    }else
        std::cout << "file empty" << std::endl;

}
