#include <unistd.h>

#include <QtGui>
#include <QPixmap>

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

/*  *********   */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    img_mat = new cv::Mat;
}

MainWindow::~MainWindow() {
    delete ui;
}

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
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
}

void MainWindow::loadFile(const QString &fileName) {
    QFile file(fileName);

    qDebug(" *** Loading file *** ");

    if (!file.open(QFile::ReadOnly | QFile::Text)) {    //Check file validity/readable/...
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QImage myImage(fileName, "PNM");                                //load the file in  a QImage variable (pnm is a format like ttif, ...)
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(myImage));    //Display the original image

    qDebug(" *** Image file correctly loaded *** ");

    *img_mat = QImage2Mat(myImage);         //Convert QImage to cv::mat
    QImage img_qimg = Mat2QImage(*img_mat);         //Convert the new cv::mat to QImage

    qDebug(" *** Image has been converted *** ");

    //Display QImage in a new window
    /*QLabel * label_qimg1 = new QLabel(this);
    label_qimg1 -> setWindowFlags(Qt::Window);
    label_qimg1 ->setWindowTitle("cv::Mat -> QImage");
    label_qimg1->setPixmap(QPixmap::fromImage(img_qimg));
    label_qimg1->show();

    //Display cv::Mat image in a new window
    namedWindow( "QImage -> cv::Mat", WINDOW_AUTOSIZE );
    imshow( "QImage -> cv::Mat", *img_mat );
    */
    qDebug(" *** Images has been displayed *** ");

    statusBar()->showMessage(tr("file loaded"), 2500);
}


//https://stackoverflow.com/questions/17127762/cvmat-to-qimage-and-back
/**
 * @brief Mat2QImage convert a cv::Mat image to a QImage using the RGB888 format
 * @param src the cv::Mat image to convert
 * @return QImage image
 */
QImage MainWindow::Mat2QImage(cv::Mat const& src) {
     cv::Mat temp;  // make the same cv::Mat than src
     cvtColor(src, temp,CV_BGR2RGB); //Convert the mat file to get a layout that qt understand (bgr is default)

     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits();   // enforce deep copy, see documentation
     dest.convertToFormat(QImage::Format_RGB888);
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )

     return dest;
}

/**
 * @brief QImage2Mat convert a QImage to a cv::Mat image
 * @param src the QImage to convert
 * @return cv::Mat image
 */
cv::Mat MainWindow::QImage2Mat(const QImage& src) {
    QImage copy;
    if(src.format() != QImage::Format_RGB888){
        qDebug("[INFO] Wrong qimage format. Conversion to RGB888...");
        copy = src.convertToFormat(QImage::Format_RGB888);
        qDebug("[INFO] Conversion Done");
    }else{
        copy = src;
    }

    cv::Mat mat(copy.height(),copy.width(),CV_8UC3,(uchar*)copy.bits(),copy.bytesPerLine());
    cv::Mat result = cv::Mat(mat.rows, mat.cols, CV_8UC3);
    cvtColor(mat, result, CV_BGR2RGB);  //Convert the mat file to get a layout that qt understand (bgr is default)

    return result;
}

cv::Mat MainWindow::contourLaplace(cv::Mat img){
    cv::Mat gray_img, result, final;
    cv::Mat img_read = img.clone();

    cv::GaussianBlur(img_read,img_read,cv::Size(3,3),0,0,cv::BORDER_DEFAULT);
    cv::cvtColor(img_read,gray_img,CV_BGR2GRAY);

    cv::Laplacian(gray_img,result,CV_16S,3,1,0,cv::BORDER_DEFAULT);
    cv::convertScaleAbs(result,final,1,0);
    return final;
}



cv::Mat MainWindow::contourSobel(cv::Mat img){
    cv::Mat gray_img,final,gx,gy,gx_goodFormat, gy_goodFormat;

 // ENCODE & DECODE THE MAT INTO/FROM THE BUFFER (=IMWRITE)
    cv::Mat img_read=img.clone();

// APPLY THE GAUSSIAN BLUR TO AVOID BLUR
    cv::GaussianBlur(img_read,img_read,cv::Size(3,3),0,0,cv::BORDER_DEFAULT);
    cv::cvtColor(img_read,gray_img,CV_BGR2GRAY); //CONVERT TO GRAY

    cv::Sobel(gray_img,gx,CV_16S,1,0,3,1,0,cv::BORDER_DEFAULT);  // DERIVATIVE IN X
    cv::Sobel(gray_img,gy,CV_16S,0,1,3,1,0,cv::BORDER_DEFAULT);// DERIVATIVE IN Y

    cv::convertScaleAbs(gy,gy_goodFormat,1,0);
    cv::convertScaleAbs(gx,gx_goodFormat,1,0);

    cv::addWeighted(gx_goodFormat,0.5,gy_goodFormat,0.5,0,final,-1); // FINAL GRADIENT = SUM OF X AND Y
    return final;

}


void MainWindow::on_actionSobel_triggered()
{

    //Display Sobel image in a new window
    namedWindow( "Sobel", WINDOW_AUTOSIZE );
    imshow( "Sobel", contourSobel(*img_mat) );
}


void MainWindow::on_actionLaplace_triggered()
{
    //Display Laplace image in a new window
    namedWindow( "Laplace", WINDOW_AUTOSIZE );
    *img_mat = contourLaplace(*img_mat);
    imshow( "Laplace", *img_mat );
}
