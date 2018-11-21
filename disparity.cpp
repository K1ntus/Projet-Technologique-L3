#include <QtGui>
#include <QPixmap>

#include <opencv2/ximgproc/disparity_filter.hpp>
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

    IO_full_scale = false;

    width = ui->image_loaded->width();
    height= ui->image_loaded->height();
    img_mat = new Mat;

    img_left = new Mat;
    img_right = new Mat;
}

Disparity::~Disparity(){
    delete ui;
}

/**
 * @brief Apply the disparity map parameters when the button got pressed and display it to the left
 */
void Disparity::on_apply_clicked(){

    this->IO_SADWindowSize = ui->slider_windowSize->value();    //Getting the value of the slider
    this->IO_numberOfDisparities = ui->slider_numberOfDisparities->value() * 16;    //this parameters had to be a multiple of 16
    this->IO_preFilterCap = ui->slider_preFilterCap->value();
    this->IO_minDisparity = ui->slider_minDisparity->value();
    this->IO_uniquenessRatio = ui->slider_uniquenessRatio->value();
    this->IO_speckleWindowSize = ui->slider_speckleWindowSize->value();
    this->IO_speckleRange = ui->slider_speckleRange->value();
    this->IO_disp12MaxDif = ui->slider_disp12MaxDiff->value();

    this->IO_P1 = 8* img_right->channels() * IO_SADWindowSize * IO_SADWindowSize;
    this->IO_P2 = 32* img_right->channels() * IO_SADWindowSize * IO_SADWindowSize;

    if(ui->checkbox_fullScale->isChecked())//Consume a lot
        this->IO_full_scale = StereoSGBM::MODE_HH;
    else
        this->IO_full_scale = false;

    if(img_mat->empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }

    QImage img1 = mat_to_qImage(disparity_map_SGBM());  //Generate the disparity map
    QImage img2 = img1.scaled(width,height, Qt::KeepAspectRatio);   //Create a new image which will fit the window
    ui->image_loaded->setPixmap(QPixmap::fromImage(img2));  //Display the disparity map in the specific slot
    ui->image_loaded->adjustSize();

}

/**
 * @brief Open a gui to select an image when you push the load button
 */
void Disparity::on_loadImage_clicked(){
    QString file_name = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (!file_name.isEmpty())
                load_stereo_image(file_name);

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
}

void Disparity::on_reset_image_clicked() {
    ui->image_loaded->setMaximumSize(width, height);
    ui->image_loaded->setPixmap(QPixmap::fromImage(mat_to_qImage(*img_mat)));
    ui->image_loaded->adjustSize();
}

/**
 * @brief Generate the disparity map from two cv::mat pointer of a stereo image
 * @return disparity map
 */
Mat Disparity::disparity_map_SGBM() {


    Mat img_right_gray, img_left_gray;
    Mat disp, disp8;

    //Convert the two stereo image in gray
    cvtColor(*img_left, img_left_gray, CV_BGR2GRAY);
    cvtColor(*img_right, img_right_gray, CV_BGR2GRAY);

    Ptr<StereoSGBM> sgbm = StereoSGBM::create(
                    IO_minDisparity,
                    IO_numberOfDisparities,
                    IO_SADWindowSize,
                    IO_P1,
                    IO_P2,
                    IO_disp12MaxDif,
                    IO_preFilterCap,
                    IO_uniquenessRatio,
                    IO_speckleWindowSize,
                    IO_speckleRange,
                    IO_full_scale
                );
    sgbm->compute(img_left_gray, img_right_gray, disp);    //Generate the disparity map
    disp.convertTo(disp,CV_8U,1,0);     //Convert the disparity map to a good format and make him convertible to qimage
    disp8= cv::Scalar::all(255)-disp;
    return disp8;
}

/**
 * @brief Load an image with a gui, then automatically split it
 * @param file_name
 * @return true if an image has been loaded, else false
 */
bool Disparity::load_stereo_image(const QString &file_name) {

    QFile file(file_name);

    qDebug(" *** Loading file *** ");

    if (!file.open(QFile::ReadOnly | QFile::Text)) {    //Check file validity/readable/...
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(file_name), file.errorString()));
        return false;
    }
    ui->image_loaded->setMaximumSize(width*2, height*2);
    QImage img1(file_name, "PNM"); //load the file in  a QImage variable (pnm is a format like ttif, ...)
    std::cout<< ui->image_loaded->width()<<std::endl;
    std::cout<< width<<std::endl;
    QImage img_fit = img1.scaled(width,height, Qt::KeepAspectRatio);   //resize the qimage
    ui->image_loaded->setPixmap(QPixmap::fromImage(img_fit));  //Display the original image
    ui->image_loaded->adjustSize();


    qDebug(" *** Image file correctly loaded *** ");

    *img_mat = qImage_to_mat(img1);

    split(*img_mat);    //cut the img_mat in two and save the result in two public variables (resp. img_left & img_right)
    return true;
}

/**
 * @brief 'Cut' an image in two new image of width/2
 * @param cv::Mat Image that will be splitted in two
 * @return nothing but store the result in two pointers
 */
void Disparity::split(Mat img){
    int x = 0;
    int y = 0;
    int width=(int)img.cols/2 ;
    int height= (int) img.rows;
    int x_right=width +img.cols%2; //First width position for the right image

    //Store the result in two pointer of this class
    *img_left = Mat(img, Rect(x,y,width, height));
    *img_right = Mat(img,Rect(x_right,y,width,height));
}


/**
 * @brief Convert a cv::Mat image to a QImage using the RGB888 format
 * @param src the cv::Mat image to convert
 * @return QImage image
 */
QImage Disparity::mat_to_qImage(Mat const& src) {
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

    return dest;
}

/**
 * @brief Convert a QImage to a cv::Mat image
 * @param src the QImage to convert
 * @return cv::Mat image
 */
Mat Disparity::qImage_to_mat(const QImage& src) {
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

