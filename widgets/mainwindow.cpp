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
    QString fileName2 = QFileDialog::getOpenFileName(this, this->tr("Choose a video for left camera"), this->tr("./resources/"), this->tr("Video Files (*.mp4)"));
    std::cout << fileName2.toStdString() << std::endl;
    const string& fileLeft(fileName2.toStdString());
    cv::VideoCapture capL(fileLeft);
    if(capL.isOpened()){

        fileName2 = QFileDialog::getOpenFileName(this, this->tr("Choose a video for right camera"), this->tr("./resources/"), this->tr("Video Files (*.mp4)"));
        std::cout << fileName2.toStdString() << std::endl;
        const string& fileRight(fileName2.toStdString());
        cv::VideoCapture capR(fileRight);
        if(capR.isOpened()){
            std::cout << "video opened" << std::endl;

            fileName2 = QFileDialog::getOpenFileName(this, this->tr("Choose the calibration file"), this->tr("./resources/"), this->tr("yaml Files (*.yml)"));
            FileStorage fs(fileName2.toStdString(), FileStorage::READ);

            if(fs.isOpened()){
                cv::Mat displayedImg, imgVidL, imgVidR, Q;
                fs["dispToDepthMatrix"] >> Q;
                int key = ' ', prevKey = key;
                if(!capL.grab() || !capR.grab()) return;

                double framerate = capL.get(CV_CAP_PROP_FPS);
                while(key != 'q'){

                    if(key == 'p'){

                        std::cout << "pause" << std::endl;
                        prevKey = key;
                    }else{
                        capL.retrieve(imgVidL);
                        capR.retrieve(imgVidR);
                        img->setImg(imgVidL, imgVidR);

                        if(key == 'l')
                            displayedImg = img->getImgL();
                        else if(key == 'r')
                            displayedImg = img->getImgR();
                        if(key == 'd')
                            displayedImg = img->getDepthMap(Q);
                        else if(key == 's')
                            displayedImg = img->getDisparityMap();
                        else
                            displayedImg = *img;

                        imagecv::displayImage(*ui->backgroundLabel, displayedImg);

                        imshow("video test", displayedImg);
                        std::cout << "next frame" << "\tkey : " << (char)prevKey << std::endl;
                        prevKey = key;

                        if(!capL.grab() || !capR.grab())
                            prevKey = 'q';
                    }

                    key = waitKey((int) framerate);
                    if(key == 255)
                        key = prevKey;
                }

            }
        }
        capR.release();
        destroyWindow("video test");

    }else
        std::cout << "file empty" << std::endl;
    capL.release();

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
