#include <unistd.h>

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
            loadFile(fileName);
}

void MainWindow::on_button_orbs_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if (!fileName.isEmpty())
                loadFile(fileName);
    }

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
    split(*img_mat);
    orbFeatures(*img_mat);
    disparityMapOrbs(*img_mat);

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
                loadFile(fileName);
    }

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
    cv::Mat img = contourSobel(*img_mat);
    QImage image = Mat2QImage(img);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));

}

//Display Laplace image
void MainWindow::on_button_laplace_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if (!fileName.isEmpty())
                loadFile(fileName);
    }

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
    cv::Mat img = contourLaplace(*img_mat);
    QImage image = Mat2QImage(img);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));

}


/**         Others Functions           **/

bool MainWindow::loadFile(const QString &fileName) {
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

    *img_mat = QImage2Mat(myImage);         //Convert QImage to cv::mat
    QImage img_qimg = Mat2QImage(*img_mat);         //Convert the new cv::mat to QImage

    statusBar()->showMessage(tr("file loaded"), 2500);

    return true;
}


/**
 * @brief Mat2QImage convert a cv::Mat image to a QImage using the RGB888 format
 * @param src the cv::Mat image to convert
 * @return QImage image
 */
QImage MainWindow::Mat2QImage(Mat const& src) {
    Mat temp;  // make the same cv::Mat than src
    if(src.channels()==1)
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
 * @brief QImage2Mat convert a QImage to a cv::Mat image
 * @param src the QImage to convert
 * @return cv::Mat image
 */
Mat MainWindow::QImage2Mat(const QImage& src) {
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

Mat MainWindow::contourLaplace(Mat img){
    Mat gray_img, result, final;
    Mat img_read = img.clone();

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT);
    cvtColor(img_read,gray_img,CV_BGR2GRAY);

    Laplacian(gray_img,result,CV_16S,3,1,0,BORDER_DEFAULT);
    convertScaleAbs(result,final,1,0);
    return final;
}



Mat MainWindow::contourSobel(Mat img){
    Mat gray_img,final,gx,gy,gx_goodFormat, gy_goodFormat;
    Mat img_read=img.clone();

    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT);
    cvtColor(img_read,gray_img,CV_BGR2GRAY); //CONVERT TO GRAY

    Sobel(gray_img,gx,CV_16S,1,0,3,1,0,BORDER_DEFAULT);  // DERIVATIVE IN X
    Sobel(gray_img,gy,CV_16S,0,1,3,1,0,BORDER_DEFAULT);// DERIVATIVE IN Y

    convertScaleAbs(gy,gy_goodFormat,1,0);
    convertScaleAbs(gx,gx_goodFormat,1,0);

    addWeighted(gx_goodFormat,0.5,gy_goodFormat,0.5,0,final,-1); // FINAL GRADIENT = SUM OF X AND Y
    return final;

}

void MainWindow::split(Mat img){
    int x = 0;
    int y = 0;
    int width=(int)img.cols/2 ;
    int height= (int) img.rows;
    int xR=width +img.cols%2;
    *img_left = Mat(img, Rect(x,y,width, height));
    *img_right = Mat(img,Rect(xR,y,width,height));
}


Mat MainWindow::orbFeatures(Mat img){
        Mat descriptorL, descriptorR, dst;
        Ptr<ORB> detector = ORB::create();
        vector<KeyPoint> keypointL, keypointR;
        vector<DMatch> matches, best_matches;
        Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
        detector->detect(*img_left,keypointL);
        detector->detect(*img_right, keypointR);
        detector->compute(*img_left,keypointL,descriptorL);
        detector->compute(*img_right, keypointR,descriptorR);
        matcher->match(descriptorL, descriptorR, matches);
        float d_max = 0, d_min = 50;
        for(int i = 0; i<(int)matches.size(); i++){  //Find d_max && d_min
            if(matches[i].distance<d_min)
                d_min = matches[i].distance;
            if(matches[i].distance > d_max)
                d_max= matches[i].distance;
        }
        for (int i = 0; i<(int) matches.size(); i++){       //select only keypoint with low distance
            if(matches[i].distance <= max(4.0*d_min,0.05))
                best_matches.push_back(matches[i]);

        }
        cv::drawMatches(*img_left, keypointL, *img_right, keypointR,best_matches,dst, DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        return dst;
}
void MainWindow::disparityMapOrbs(Mat img){
    Mat orbImage = orbFeatures(img);
    imshow("orb Features", orbImage);
}
