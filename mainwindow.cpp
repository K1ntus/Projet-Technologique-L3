#include <unistd.h>
#include <opencv2/ximgproc.hpp>
#include <QtGui>
#include <QPixmap>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "disparity.h"

using namespace cv;
using namespace std;

/*  *********   */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    img_mat = new Mat;

    img_left = new Mat;
    img_right = new Mat;
    parametersWindow = new Disparity();

}

MainWindow::~MainWindow() {
    delete ui;


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
    exit(EXIT_SUCCESS);
}

void MainWindow::on_actionOuvrir_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
        if (!fileName.isEmpty())
            load_file(fileName);
}

void MainWindow::on_button_stereoBM_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if (!fileName.isEmpty())
                load_file(fileName);
    }

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
    split(*img_mat);
    Mat disparity = sbm();
    imshow("Disparity BM", disparity);



}

void MainWindow::on_button_disparity_clicked(){
    parametersWindow->show();
}

//Display Sobel Image
void MainWindow::on_button_sobel_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if (!fileName.isEmpty())
                load_file(fileName);
    }

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
    cv::Mat img = contour_sobel(*img_mat);
    QImage image = mat_2_qimage(img);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));

}

//Display Laplace image
void MainWindow::on_button_laplace_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if (!fileName.isEmpty())
                load_file(fileName);
    }

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
    cv::Mat img = contour_laplace(*img_mat);
    QImage image = mat_2_qimage(img);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));

}


/**         Others Functions           **/

bool MainWindow::load_file(const QString &fileName) {
    QFile file(fileName);

    qDebug(" *** Loading file *** ");

    if (!file.open(QFile::ReadOnly | QFile::Text)) {    //Check file validity/readable/...
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QImage myImage(fileName, "PNM");                                //load the file in  a QImage variable (pnm is a format like ttif, ...)
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(myImage));    //Display the original image

    qDebug(" *** Image file correctly loaded *** ");

    *img_mat = qimage_2_mat(myImage);         //Convert QImage to cv::mat
    QImage img_qimg = mat_2_qimage(*img_mat);         //Convert the new cv::mat to QImage

    statusBar()->showMessage(tr("file loaded"), 2500);

    return true;
}


/**
 * @brief mat_2_qimage convert a cv::Mat image to a QImage using the RGB888 format
 * @param src the cv::Mat image to convert
 * @return QImage image
 */
QImage MainWindow::mat_2_qimage(Mat const& src) {
    Mat temp;  // make the same cv::Mat than src
    if(src.channels()==1)           //convert the color according to the src's number of channels
        cvtColor(src,temp,CV_GRAY2BGR);
    else if(src.channels()==3)
        cvtColor(src,temp,CV_BGR2RGB);
    else
        cvtColor(src,temp,CV_BGRA2RGB);
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits();   // enforce deep copy, see documentation
    dest.convertToFormat(QImage::Format_RGB888);
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )

     return dest;
}

/**
 * @brief qimage_2_mat convert a QImage to a cv::Mat image
 * @param src the QImage to convert
 * @return cv::Mat image
 */
Mat MainWindow::qimage_2_mat(const QImage& src) {
    QImage copy;
    if(src.format() != QImage::Format_RGB888) {
        //qDebug("[INFO] Wrong qimage format. Conversion to RGB888...");
        copy = src.convertToFormat(QImage::Format_RGB888);
        //qDebug("[INFO] Conversion Done");
    } else {
        copy = src;
    }

    Mat mat(copy.height(),copy.width(),CV_8UC3,(uchar*)copy.bits(),copy.bytesPerLine());
    Mat result = Mat(mat.rows, mat.cols, CV_8UC3);
    cvtColor(mat, result, CV_RGB2BGR);  //Convert the mat file to get a layout that qt understand (bgr is default)

    return result;
}

Mat MainWindow::contour_laplace(Mat img){
    Mat gray_img, result, final;
    Mat img_read = img.clone();

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT); // apply the gaussianBlur to smooth the img
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

    Laplacian(gray_img,result,CV_16S,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(result,final,1,0);                          //convert to a CV_8U image
    return final;
}



Mat MainWindow::contour_sobel(Mat img){
    Mat gray_img,final,gx,gy,gx_goodFormat, gy_goodFormat;
    Mat img_read=img.clone();

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT);
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

    Sobel(gray_img,gx,CV_16S,1,0,3,1,0,BORDER_DEFAULT);  // derivative in x
    Sobel(gray_img,gy,CV_16S,0,1,3,1,0,BORDER_DEFAULT);// derivative in y

    convertScaleAbs(gy,gy_goodFormat,1,0);
    convertScaleAbs(gx,gx_goodFormat,1,0);

    addWeighted(gx_goodFormat,0.5,gy_goodFormat,0.5,0,final,-1); // final gradient is the addition of the two gradients
    return final;

}

void MainWindow::split(Mat img){
    int x = 0;
    int y = 0;
    int width=(int)img.cols/2 ;
    int height= (int) img.rows;
    int xR=width +img.cols%2;  // if img.cols%2 == 1 then img has an odd number of cols
    *img_left = Mat(img, Rect(x,y,width, height));
    *img_right = Mat(img,Rect(xR,y,width,height));
}


Mat MainWindow::sbm(){
    Mat dst, imgL, imgR;
    cvtColor(*img_left, imgL,CV_BGR2GRAY);
    cvtColor(*img_right,imgR,CV_BGR2GRAY);
    int window_size = 21;
    int numDisparity = 32; // must be a 16's multiple
    Ptr<StereoBM> matcher= StereoBM::create(numDisparity,window_size);
    matcher->compute(imgL,imgR,dst);
     dst.convertTo(dst,CV_8U,1,0);
     return dst;
}

