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

void MainWindow::on_actionA_propos_triggered() {
    QMessageBox::about(this, tr("About"),
                       tr("Project members:\n\n"
                          "Jordane Masson\n"
                          "Virginie Montalibet\n"
                          "Meryl Eugenie\n"));
}

void MainWindow::on_actionQuitter_triggered() {
    this->~MainWindow();
    exit(EXIT_SUCCESS);
}

void MainWindow::on_actionOuvrir_triggered() {
//    load_image();
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

void MainWindow::on_button_disparity_clicked(){

    if(parametersWindow == nullptr)
        parametersWindow = new Disparity();
    if(img != nullptr && img->isStereo()){
        parametersWindow->set_img_mat(*img);
    }
    parametersWindow->show();

}

//Display Sobel Image
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

//Display Laplace image
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

void MainWindow::on_calibrate_clicked()
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

        if(calib_widget != nullptr)
            delete calib_widget;
        calib_widget = new Calibration_widget(new Calibration_intr(images));
        calib_widget->show();
    }
}
