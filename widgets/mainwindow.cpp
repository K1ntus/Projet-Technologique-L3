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


        cv::Mat displayedImg;
        ImgCv imgVidL;

        int key = ' ', prevKey = key;
        if(!capL.grab()) return;

        double framerate = capL.get(CV_CAP_PROP_FPS);
        while(key != 'q'){

            if(key == 'p'){

                std::cout << "pause" << std::endl;
                prevKey = key;
            }else{
                capL.retrieve(imgVidL);

                if (key == 'k'){

                }if(key == 's')
                    displayedImg = imgVidL.getDisparityMap();
                else
                    displayedImg = imgVidL;

                imagecv::displayImage(*ui->backgroundLabel, displayedImg);

                imshow("video test", displayedImg);
                std::cout << "next frame" << "\tkey : " << (char)prevKey << std::endl;
                prevKey = key;

                if(!capL.grab()){
                    capL.set(cv::CAP_PROP_POS_FRAMES, 0);
                    capL.grab();
                }
            }

            key = waitKey((int) framerate);
            if(key == 255)
                key = prevKey;
        }

        destroyWindow("video test");
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

                int key = ' ', prevKey = key;
                if(!capL.grab() || !capR.grab()) return;
                if(!capL.isOpened()) return;
                cv::Mat displayedImg, imgVidL, imgVidR;
                ImgCv stereo;
                double framerate = capL.get(CV_CAP_PROP_FPS);
                while(key != 'q'){

                    if(key == 'p'){

                        std::cout << "pause" << std::endl;
                        prevKey = key;
                    }else{
                        capL.retrieve(imgVidL);
                        capR.retrieve(imgVidR);
                        stereo.setImg(imgVidL, imgVidR);
                        if (key == 'k'){

                        }if(key == 's')
                            displayedImg = stereo.getDisparityMap();
                        else
                            displayedImg = stereo;

                        imagecv::displayImage(*ui->backgroundLabel, displayedImg);

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

                destroyWindow("video test");
                capL.release();
                capR.release();
            }


        }else
            std::cout << "couldn't make video" << std::endl;


    }else{
        std::cout << "file empty" << std::endl;

    }
}

void MainWindow::on_actionrun_triggered()
{
    tcp_server.StartServer();
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

    QString filePath = QFileDialog::getOpenFileName(this, this->tr("Choose the calibration file"), this->tr("./resources/"), this->tr("yaml Files (*.yml)"));
    if(filePath.isEmpty()) return;

    cv::Mat param, dispPrev, dispNext, dispDiff;
    ImgCv nextFrame;
    FileStorage fs(filePath.toStdString(), FileStorage::READ);
    if(fs.isOpened()){


        fs["DisparityParameter"] >> param;
        fs.release();
    }else{
        qDebug("[ERROR] in: on_dispParam_clicked\n couldn't opend calib file.");
        return;
    }

    qDebug("[INFO] Load the next frame");

    if(!load_file(*this, nextFrame, true)){
        qDebug("[ERROR] No images loaded");
        return;
    }

    qDebug("next frame loaded");

    switch(param.at<int>(0)){
    case 0:
        qDebug("sbm entries");

        dispPrev = img->sbm(
                    param.at<int>(1), param.at<int>(2), param.at<int>(3),
                param.at<int>(4), param.at<int>(5), param.at<int>(6),
                param.at<int>(7), param.at<int>(8), param.at<int>(9), param.at<int>(10)
                );
        dispNext = nextFrame.sbm(
                    param.at<int>(1), param.at<int>(2), param.at<int>(3),
                param.at<int>(4), param.at<int>(5), param.at<int>(6),
                param.at<int>(7), param.at<int>(8), param.at<int>(9), param.at<int>(10)
                );
        qDebug("sbm done");

        break;
    case 1:
        break;
    case 2:
        dispPrev = img->disparity_map_SGBM(
                    param.at<int>(1), param.at<int>(2), param.at<int>(3),
                param.at<int>(4), param.at<int>(5), param.at<int>(6),
                param.at<int>(7), param.at<int>(8), param.at<int>(9),
                    param.at<int>(10),  param.at<int>(11)
                );
        dispNext = nextFrame.disparity_map_SGBM(
                    param.at<int>(1), param.at<int>(2), param.at<int>(3),
                param.at<int>(4), param.at<int>(5), param.at<int>(6),
                param.at<int>(7), param.at<int>(8), param.at<int>(9),
                param.at<int>(10),  param.at<int>(11)
                );
        break;
    case 4:
        break;
    default:
        qDebug("[ERROR] can't match to any case");
        return;

    }
    qDebug("dis loaded");
//    img->setImg(dispPrev, dispNext);

    dispDiff = dispNext - dispPrev;
    imagecv::displayImage(*ui->backgroundLabel, dispDiff);

}
