#include <QtGui>
#include <QPixmap>

#include "disparity.h"
#include "ui_disparity.h"

using namespace cv;
using namespace std;

Disparity::Disparity(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Disparity)
{
    ui->setupUi(this);

    IO_SADWindowSize = 9;
    IO_numberOfDisparities = 144;
    IO_preFilterCap = 50;
    IO_minDisparity = 0;
    IO_uniquenessRatio = 10;
    IO_speckleWindowSize = 0;
    IO_speckleRange = 8;
    IO_disp12MaxDif = -1;

    IO_P1 = 156;
    IO_P2 = 864;

    IO_fullDP = false;

    img_mat = new Mat;

    img_left = new Mat;
    img_right = new Mat;
}

Disparity::~Disparity(){
    delete ui;
}


void Disparity::on_apply_clicked(){
    this->IO_SADWindowSize = ui->slider_windowSize->value();
    this->IO_numberOfDisparities = ui->slider_numberOfDisparities->value() * 16;
    this->IO_preFilterCap = ui->slider_preFilterCap->value();
    this->IO_minDisparity = ui->slider_minDisparity->value();
    this->IO_uniquenessRatio = ui->slider_uniquenessRatio->value();
    this->IO_speckleWindowSize = ui->slider_speckleWindowSize->value();
    this->IO_speckleRange = ui->slider_speckleRange->value();
    this->IO_disp12MaxDif = ui->slider_disp12MaxDiff->value();

    this->IO_P1 = ui->slider_P1->value();
    this->IO_P2 = ui->slider_P2->value();

    if(ui->checkbox_fullScale->isChecked())
        this->IO_fullDP = true;
    else
        this->IO_fullDP = false;

    QImage img1 = Mat2QImage(disparityMapSGBM());
    QImage img2 = img1.scaled(ui->image_loaded->width(),ui->image_loaded->height(), Qt::KeepAspectRatio);
    ui->image_loaded->setPixmap(QPixmap::fromImage(img2));


}

void Disparity::on_loadImage_clicked(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty())
                loadFile(fileName);

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
}



Mat Disparity::disparityMapSGBM() {
    Mat imgR, imgL, imgR_0 = *img_right, imgL_0 = *img_left;
    Mat disp, disp8;

    cvtColor(imgL_0, imgL, CV_BGR2GRAY);
    cvtColor(imgR_0, imgR, CV_BGR2GRAY);

    //parameters: http://answers.opencv.org/question/182049/pythonstereo-disparity-quality-problems/
    //or better : https://docs.opencv.org/3.4/d2/d85/classcv_1_1StereoSGBM.html
    StereoSGBM sbm;
    sbm.SADWindowSize = IO_SADWindowSize;          //Matched block size (>= 1)
    sbm.numberOfDisparities = IO_numberOfDisparities;  //multiple de 16
    sbm.preFilterCap = IO_preFilterCap;          //Truncation value for prefiltered pixels
    sbm.minDisparity = IO_minDisparity;           //minimum disparity value
    sbm.uniquenessRatio = IO_uniquenessRatio;       //usually between 5 & 15
    sbm.speckleWindowSize = IO_speckleWindowSize;      //0-> disable | usually between 50 & 200
    sbm.speckleRange = IO_speckleRange;           //Maximum disparity variation
    sbm.disp12MaxDiff = IO_disp12MaxDif;         //Maximum allowed difference
    sbm.fullDP = IO_fullDP;             //full-scale two-pass-dynamic (use a lot of bytes)
    sbm.P1 = IO_P1;                   //Disparity smoothness
    sbm.P2 = IO_P2;                   //same, larger => bigger smoothness
    sbm(imgL, imgR, disp);

    normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);

    return disp8;
}

bool Disparity::loadFile(const QString &fileName) {
    QFile file(fileName);

    qDebug(" *** Loading file *** ");

    if (!file.open(QFile::ReadOnly | QFile::Text)) {    //Check file validity/readable/...
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QImage img1(fileName, "PNM");//load the file in  a QImage variable (pnm is a format like ttif, ...)
    QImage img2 = img1.scaled(ui->image_loaded->width(),ui->image_loaded->height(), Qt::KeepAspectRatio);//resize the qimage
    ui->image_loaded->setPixmap(QPixmap::fromImage(img2));//Display the original image


    qDebug(" *** Image file correctly loaded *** ");

    *img_mat = QImage2Mat(img1);         //Convert QImage to cv::mat

    split(*img_mat);
    return true;
}

void Disparity::split(Mat img){
    int x =0;
    int y = 0;
    int height =(int)img.cols/2;
    int width = (int) img.rows;
    int xR = height;
    * img_left =  Mat(img, Rect(x, y, height, width));
    * img_right = Mat(img, Rect(xR, y, height, width));
}


/**
 * @brief Mat2QImage convert a cv::Mat image to a QImage using the RGB888 format
 * @param src the cv::Mat image to convert
 * @return QImage image
 */
QImage Disparity::Mat2QImage(Mat const& src) {
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
Mat Disparity::QImage2Mat(const QImage& src) {
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
