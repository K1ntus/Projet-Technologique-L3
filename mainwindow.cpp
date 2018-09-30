#include <unistd.h>

#include <QtGui>
#include <QPixmap>
#include <QImage>
#include <opencv2/opencv.hpp>

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

/*  Prototype   */

cv::Mat QImage2Mat(QImage const& src);
QImage Mat2QImage(cv::Mat const& src);

/*  *********   */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

void MainWindow::on_actionQuitter_triggered()   {
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

    cv::Mat img_mat = QImage2Mat(myImage);          //Convert QImage to cv::mat
    QImage img_qimg = Mat2QImage(img_mat);         //Convert the new cv::mat to QImage

    qDebug(" *** Image has been converted *** ");

    //Display QImage in a new window
    QLabel * label_qimg1 = new QLabel(this);
    label_qimg1 -> setWindowFlags(Qt::Window);
    label_qimg1 ->setWindowTitle("cv::Mat -> QImage");
    label_qimg1->setPixmap(QPixmap::fromImage(img_qimg));
    label_qimg1->show();

    //Display cv::Mat image in a new window
    namedWindow( "QImage -> cv::Mat", WINDOW_AUTOSIZE );
    imshow( "QImage -> cv::Mat", img_mat );

    qDebug(" *** Images has been displayed *** ");

    statusBar()->showMessage(tr("file loaded"), 2500);
}


//https://stackoverflow.com/questions/17127762/cvmat-to-qimage-and-back
QImage Mat2QImage(cv::Mat const& src){
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); //Convert the mat file to get a layout that qt understand (bgr is default)
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}

cv::Mat QImage2Mat(const QImage& src){
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
