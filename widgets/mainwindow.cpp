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


/**         Interface Interaction           **/

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
 * @brief MainWindow::on_actionOuvrir_triggered triggered when the open button is pressed.
 * \n
 * Open a file dialog to load an image that will be bufferized for later modification,\n
 * filters applies, ...\n
 * \n
 * The loaded image is displayed to the left of the application main menu.
 */
void MainWindow::on_actionOuvrir_triggered() {
    if(load_file(*this, *img)){
        QMessageBox::information(this, "Open image", "Image loaded");
        QImage img_qimg = mat_to_qimage(img->getImg());         //Convert the new cv::mat to QImage
        ui->backgroundLabel->setPixmap(QPixmap::fromImage(img_qimg));    //Display the original image
        statusBar()->showMessage(tr("file loaded"), 2500);
    }
    else{
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file\n please select a new file "));
    }
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

    QImage image = mat_to_qimage(img->contour_sobel());

    QString str = speed_test((function_call) ImgCv::contour_sobel, img->getImg());
    statusBar()->showMessage(str);

    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));

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

    QImage image = mat_to_qimage(img->contour_laplace());

    QString str = speed_test((function_call) ImgCv::contour_laplace, img->getImg());
    statusBar()->showMessage(str);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));

}

/**
 * @brief MainWindow::on_calibrate_clicked is triggered when the calibration button is pressed
 */
void MainWindow::on_calibrate_clicked() {
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

        if(calib_widget != nullptr)
            delete calib_widget;
        calib_widget = new Calibration_widget(new ChessboardCalibration(images));
        calib_widget->show();
    }
}
