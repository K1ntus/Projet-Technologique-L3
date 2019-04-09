#include "mainwindow.h"

using namespace cv;
using namespace std;
using namespace imagecv;

/*  *********   */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    img(nullptr),
    parametersWindow(nullptr),
    calib_widget(nullptr)
{

    ui->setupUi(this);
    img = new ImgCv();
}

MainWindow::~MainWindow() {

    qDebug("delete mainWindow");
    delete ui;
    delete img;
    delete parametersWindow;
    delete calib_widget;

}

/*         Interface Interaction           */

/**
 * @brief MainWindow::on_actionA_propos_triggered open a little popup window displaying few informations about this project
 */
void MainWindow::on_actionA_propos_triggered() {
    QMessageBox::about(this, tr("About"),
                       tr("Project members:\n\n"
                          "Jordane Masson\n"
                          "Virginie Montalibet\n"
                          "Meryl Eugenie\n"));
}

/**
 * @brief MainWindow::on_actionQuitter_triggered exit the application
 */
void MainWindow::on_actionQuitter_triggered() {
    this->~MainWindow();
    exit(EXIT_SUCCESS);
}


/**
 * @brief MainWindow::on_button_disparity_clicked triggered when the disparity button is pressed. \n
 * \n
 * It'll display a new window to manage the disparity generation parameters.\n
 * \n
 * @sa Disparity
 */
void MainWindow::on_button_disparity_clicked(){

    if(parametersWindow == nullptr)
        parametersWindow = new Disparity();
    if(img != nullptr && img->isStereo()){
        parametersWindow->set_img_mat(*img);
    }
    parametersWindow->show();
}

/**
 * @brief MainWindow::on_button_sobel_clicked triggered when the sobel button is pressed.\n
 * \n
 * This method apply the sobelian filter on the loaded image if not null, and display it in the application main menu.\n
 * The sobelian filter detect edges of each elements of an image like the laplacian filter, \n
 * but using the gradient method
 */
void MainWindow::on_button_sobel_clicked(){
    if(img->getImg().empty()){
        qDebug("[INFO] Load a stereo file before");
        if(!load_file(*this, *img)){
            qDebug("[ERROR] No images loaded");
            return;
        }
    }
    imagecv::displayImage(*ui->backgroundLabel, ImgCv::contour_sobel(*img));
    QString str = speed_test((function_call) ImgCv::contour_sobel, img->getImg());
    statusBar()->showMessage(str);


}

/**
 * @brief MainWindow::on_button_laplace_clicked triggered when the laplace button is pressed.\n
 * \n
 * This method apply the laplacian filter on the loaded image if not null, then display it in the application main menu \n
 * The laplacian filter is useful to detect edges of elements of the image.
 */
void MainWindow::on_button_laplace_clicked(){
    if(img->getImg().empty()){
        qDebug("[INFO] Load a stereo file before");
        if(!load_file(*this, *img)){
            qDebug("[ERROR] No images loaded");
            return;
        }
    }

    imagecv::displayImage(*ui->backgroundLabel, ImgCv::contour_laplace(*img));

    QString str = speed_test((function_call) ImgCv::contour_laplace, img->getImg());
    statusBar()->showMessage(str);

}

/**
 * @brief MainWindow::on_calibrate_clicked is triggered when the calibration button is pressed
 */
void MainWindow::on_calibrate_clicked() {

    if(calib_widget != nullptr)
        delete calib_widget;
    calib_widget = new Calibration_widget();
    calib_widget->show();

}

void MainWindow::on_videoTest_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, this->tr("Choose a video"), this->tr("./resources/"), this->tr("Video Files (*.mp4)"));
    if(filePath.isEmpty()) return;
    std::cout << filePath.toStdString() << std::endl;
    const string& fileLeft(filePath.toStdString());
    cv::VideoCapture capL(fileLeft);
    if(capL.isOpened()){
        imagecv::displayVideo(*ui->backgroundLabel, capL);
        capL.release();

    }else
        std::cout << "file empty" << std::endl;

}

void MainWindow::on_actionCv_Mat_triggered()
{

    if(img->getImg().empty()){
        qDebug("[INFO] Load a stereo file before");
        if(!load_file(*this, *img)){
            qDebug("[ERROR] No images loaded");
            return;
        }
    }

    QImage image = mat_to_qimage(*img);
    clock_t start, end;
    start = clock();
    qimage_to_mat(image);

    end = clock();
    double delta = (end-start);
    QString str = "result of speed test: " + QString::number((delta/CLOCKS_PER_SEC));
    statusBar()->showMessage(str);

    imshow("cv image", *img);
}

/**
 * @brief MainWindow::on_actionOuvrir_triggered triggered when the open button is pressed.
 * \n
 * Open a file dialog to load an image that will be bufferized for later modification,\n
 * filters applies, ...\n
 * \n
 * The loaded image is displayed to the left of the application main menu.
 */
void MainWindow::on_actionOuvrir_image_triggered()
{
    if(load_file(*this, *img)){
        QMessageBox::information(this, "Open image", "Image loaded");
        imagecv::displayImage(*ui->backgroundLabel, *img);  //Display the original image

        statusBar()->showMessage(tr("file loaded"), 2500);
    }
    else{
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file\n please select a new file "));
    }
}

/**
 * @brief MainWindow::on_actionOuvrir_triggered triggered when the open button is pressed.
 * \n
 * Open a file dialog to load an image that will be bufferized for later modification,\n
 * filters applies, ...\n
 * \n
 * The loaded image is displayed to the left of the application main menu.
 */
void MainWindow::on_action1_image_triggered()
{
    if(load_file(*this, *img)){
        QMessageBox::information(this, "Open image", "Image loaded");
        img->setImg(*img, true);
        imagecv::displayImage(*ui->backgroundLabel, *img);  //Display the original image

        statusBar()->showMessage(tr("file loaded"), 2500);
    }
    else{
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file\n please select a new file "));
    }
}

/**
 * @brief MainWindow::on_actionOuvrir_triggered triggered when the open button is pressed.
 * \n
 * Open a file dialog to load an image that will be bufferized for later modification,\n
 * filters applies, ...\n
 * \n
 * The loaded image is displayed to the left of the application main menu.
 */
void MainWindow::on_action2_image_triggered()
{
    if(load_file(*this, *img, true)){
        QMessageBox::information(this, "Open images", "Image loaded");
        imagecv::displayImage(*ui->backgroundLabel, *img);  //Display the original image

        statusBar()->showMessage(tr("file loaded"), 2500);
    }
    else{
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file\n please select a new file "));
    }
}


void MainWindow::on_actionQImage_triggered()
{
    if(img->getImg().empty()){
        qDebug("[INFO] Load a stereo file before");
        if(!load_file(*this, *img, true)){
            qDebug("[ERROR] No images loaded");
            return;
        }
    }

    clock_t start, end;
    start = clock();
    QImage image = mat_to_qimage(*img);

    end = clock();
    double delta = (end-start);
    QString str = "result of speed test: " + QString::number((delta/CLOCKS_PER_SEC));
    statusBar()->showMessage(str);

    imagecv::displayImage(*ui->backgroundLabel, *img);

}

void MainWindow::on_actionrecitify_image_triggered()
{
    if(img->getImg().empty()){
        qDebug("[INFO] Load a stereo file before");
        if(!load_file(*this, *img, true)){
            qDebug("[ERROR] No images loaded");
            return;
        }
    }
    QString filePath = QFileDialog::getOpenFileName(this, this->tr("Choose the calibration file"), this->tr("./resources/"), this->tr("yaml Files (*.yml)"));
    if(filePath.isEmpty()) return;

    string const &outFile(filePath.toStdString());
    cv::Mat const &imgRectified = ImgCv::rectifiedImage(*img, outFile);

    img->setImg(imgRectified, true);
    imagecv::displayImage(*ui->backgroundLabel, *img);

}

void MainWindow::on_actionEnregistrer_triggered()
{
    if(img->getImg().empty()){
        qDebug("[INFO] Load a stereo file before");

    }
    QString filePath = QFileDialog::getSaveFileName(this, this->tr("Save file"), this->tr("./resources/"), this->tr("Images Files (*.jpg *.jpeg *.png)"));
    if(filePath.isEmpty()) return;

    imwrite(filePath.toStdString(), *img);

}

void MainWindow::on_videoFromSet_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(
                this, tr("Select the left video set"),
                tr("./resources/") );

    if(!filePath.isEmpty()){

        string fileLeft(filePath.toStdString());
        std::string name("left_");
        std::cout << fileLeft+"/"+ name + "%03d.jpg" << std::endl;

        cv::VideoCapture capL(fileLeft+"/"+ name + "%03d.jpg", cv::CAP_IMAGES );
        if(capL.isOpened()){

            QString filePath = QFileDialog::getExistingDirectory(
                        this, tr("Select the right video set"),
                        tr("./resources/") );

            if(!filePath.isEmpty()){


                fileLeft = filePath.toStdString();
                name = "right_";
                cv::VideoCapture capR(fileLeft+"/"+ name + "%03d.jpg", cv::CAP_IMAGES );
                if(capR.isOpened()){
                    QString filePath = QFileDialog::getOpenFileName(
                                this, tr("Select a calibration file with a DispParameter member"),
                                tr("./resources/") );
                    std::string const&calibFile(filePath.toStdString());
                    imagecv::displayVideo(*ui->backgroundLabel, capL, capR, calibFile);
                    capR.release();

                }
                capL.release();
            }


        }else
            std::cout << "couldn't make video" << std::endl;


    }else{
        std::cout << "file empty" << std::endl;

    }
}

void MainWindow::on_actionrun_triggered()
{
//    QString filePath = QFileDialog::getOpenFileName(this, this->tr("Choose the calibration file"), this->tr("./resources/"), this->tr("yaml Files (*.yml)"));
//    if(filePath.isEmpty()) return;
    tcp_server.StartServer("");
}


void MainWindow::on_actionclose_triggered()
{
    tcp_server.close();
}

void MainWindow::on_tracking_clicked()
{
    if(img->getImg().empty()){
        qDebug("[INFO] Load a stereo file before");
        if(!load_file(*this, *img, true)){
            qDebug("[ERROR] No images loaded");
            return;
        }

    }

    Mat copyImg;
    if(img->channels()==1)
        cvtColor((img->isStereo())? img->getImgR() : *img,copyImg,CV_GRAY2BGR);
    else
        copyImg = (img->isStereo())? img->getImgR() : *img;
    Rect trackWind(Rect(copyImg.rows/2, 0, copyImg.rows/2, copyImg.cols/(2)));


    ImgCv::trackCamShift(copyImg, trackWind);
    rectangle( copyImg, trackWind, cv::Scalar(255), 2);
    imagecv::displayImage(*ui->backgroundLabel, copyImg);

}

void MainWindow::on_actiondisparity_map_triggered()
{
        QString filePath = QFileDialog::getOpenFileName(this, this->tr("Choose the disparity map file"), this->tr("./resources/"), this->tr("yaml Files (*.yml)"));
        if(filePath.isEmpty()) return;

        cv::Mat dispMap;
        FileStorage fs(filePath.toStdString(), FileStorage::READ);
        if(fs.isOpened()){
            fs["DisparityMap"] >> dispMap;
            displayImage(*ui->backgroundLabel, dispMap);
            img->setImg(dispMap, false);
            fs.release();
        }
}

void MainWindow::on_actiondepth_map_triggered()
{
    if(img->empty()){
        qDebug("[INFO] Load a disparity map before");
        on_actiondisparity_map_triggered();

    }
    Mat copyImg;
    if(img->channels()==1)
        cvtColor(*img,copyImg,CV_GRAY2BGR);
    else{
        qDebug("[ERROR] not a disparity map ");

        return;
    }
    QString filePath = QFileDialog::getOpenFileName(this, this->tr("Choose the calibration file"), this->tr("./resources/"), this->tr("yaml Files (*.yml)"));
    if(filePath.isEmpty()) return;

    cv::Mat Q(ImgCv::getDispToDepthMat(filePath.toStdString()));
    cv::Mat depthMap(ImgCv::depthMap(*img, Q));

    Rect trackWind(Rect(copyImg.rows/2, 0, copyImg.rows/2, copyImg.cols/(2)));
    ImgCv::trackCamShift(copyImg, trackWind);
    rectangle( depthMap, trackWind, cv::Scalar(255), 2);
    Scalar avr = mean(depthMap(trackWind));
    std::cout << "average value 0: " << avr(0)
              << "\naverage value 1" << avr(1)<< std::endl;
    normalize(depthMap, depthMap, 0, 255, NORM_MINMAX, CV_32F);

    imshow("depth map", depthMap);


}
