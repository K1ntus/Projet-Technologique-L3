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

    img_mat = new Mat;

    img_left = new Mat;
    img_right = new Mat;
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

void MainWindow::on_actionSobel_triggered() {
    //Display Sobel Image
    cv::Mat img = contourSobel(*img_mat);
    QImage image = Mat2QImage(img);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));
}


void MainWindow::on_actionLaplace_triggered() {
    //Display Laplace image
    cv::Mat img = contourLaplace(*img_mat);
    QImage image = Mat2QImage(img);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));
}


void MainWindow::on_actionSGBM_triggered(){
    if(img_mat->empty()){
        qDebug("[ERROR] Load stereo file before");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if (!fileName.isEmpty())
                loadFile(fileName);
    }
    if(img_mat->empty()){
        qDebug("[ERROR] No image has been loaded");
        return;
    }

    split(*img_mat);
    cv::Mat img_disp = disparityMapSGBM();
}

void MainWindow::on_actionOrbs_triggered(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if (!fileName.isEmpty())
                loadFile(fileName);
    }
    if(img_mat->empty()){
        qDebug("[ERROR] No image has been loaded");
        return;
    }

    split(*img_mat);
    orbFeatures(*img_mat);

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

    qDebug(" *** Image has been converted *** ");

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

    // APPLY THE GAUSSIAN BLUR TO AVOID BLUR
    GaussianBlur(img_read,img_read,Size(3,3),0,0,BORDER_DEFAULT);
    cvtColor(img_read,gray_img,CV_BGR2GRAY); //CONVERT TO GRAY

    Sobel(gray_img,gx,CV_16S,1,0,3,1,0,BORDER_DEFAULT);  // DERIVATIVE IN X
    Sobel(gray_img,gy,CV_16S,0,1,3,1,0,BORDER_DEFAULT);// DERIVATIVE IN Y

    convertScaleAbs(gy,gy_goodFormat,1,0);
    convertScaleAbs(gx,gx_goodFormat,1,0);

    addWeighted(gx_goodFormat,0.5,gy_goodFormat,0.5,0,final,-1); // FINAL GRADIENT = SUM OF X AND Y
    return final;

}

Mat MainWindow::disparityMapSGBM() {
    Mat imgR, imgL;
    Mat disp, disp8;

    cvtColor(*img_left, imgL, CV_BGR2GRAY);
    cvtColor(*img_right, imgR, CV_BGR2GRAY);

    //parameters: http://answers.opencv.org/question/182049/pythonstereo-disparity-quality-problems/
    //or better : https://docs.opencv.org/3.4/d2/d85/classcv_1_1StereoSGBM.html
    StereoSGBM sbm;
    sbm.SADWindowSize = 9;          //Matched block size (>= 1)
    sbm.numberOfDisparities = 144;  //multiple de 16
    sbm.preFilterCap = 50;          //Truncation value for prefiltered pixels
    sbm.minDisparity = 0;           //minimum disparity value
    sbm.uniquenessRatio = 10;       //usually between 5 & 15
    sbm.speckleWindowSize = 0;      //0-> disable | usually between 50 & 200
    sbm.speckleRange = 8;           //Maximum disparity variation
    sbm.disp12MaxDiff = -1;         //Maximum allowed difference
    sbm.fullDP = false;             //full-scale two-pass-dynamic (use a lot of bytes)
    sbm.P1 = 156;                   //Disparity smoothness
    sbm.P2 = 864;                   //same, larger => bigger smoothness
    sbm(imgL, imgR, disp);

    normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);

    imshow("disparity map", disp8);
    return disp8;
}

Mat MainWindow::disparityMap_postFiltering(Mat disparityMap){
    //tmp for warn
    Mat filtered_disp_vis = disparityMap.clone();

    return filtered_disp_vis;
}

void MainWindow::orbFeatures(Mat img){
    int nfeatures = 500;
    float scaleFactor =1.2f;
    int nlevels = 8;
    int edgeTreshold=31;
    int firstLevel =0;
    int WTA_K =2;
    int scoreType = ORB::HARRIS_SCORE;
    int patchSize = 31;
    Mat grayImage;
    cvtColor(img, grayImage,CV_BGR2GRAY);
    ORB detector = ORB(nfeatures,scaleFactor,nlevels,edgeTreshold,firstLevel,WTA_K,scoreType,patchSize);
    vector<KeyPoint> keypoint;
    detector.detect(grayImage, keypoint);
    Mat dst;
    cv::drawKeypoints(img,keypoint,dst,-1,DrawMatchesFlags::DEFAULT);
    imshow("OrbDetector",dst);
}

void MainWindow::split(Mat img){
    int x =0;
    int y = 0;
    int height =(int)img.cols/2;
    int width = (int) img.rows;
    int xR = height;
    *img_left = Mat(img, Rect(x,y,height, width));
    *img_right = Mat(img,Rect(xR,y,height,width));
}
