#include <QtGui>
#include <QPixmap>

#include "disparity.h"
#include "ui_disparity.h"

using namespace cv;
using namespace cv::ximgproc;
using namespace std;
using namespace imagecv;


Disparity::Disparity(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Disparity)
{
    ui->setupUi(this);

    IO_SADWindowSize = 9;
    IO_numberOfDisparities = 144;
    IO_preFilterCap = 50;
    IO_minDisparity = 5;
    IO_uniquenessRatio = 5;
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
    QImage img1;
    if(img_mat->empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }

    if(ui->checkBox->isChecked()){
        if((this->IO_SADWindowSize = ui->slider_windowSize->value())%2==0)
            this->IO_SADWindowSize = ui->slider_windowSize->value()+1;
        else
            this->IO_SADWindowSize = ui->slider_windowSize->value();
        this->IO_numberOfDisparities = ui->slider_numberOfDisparities->value() * 16; //this parameters had to be a multiple of 16

        img1 = mat_to_qimage(sbm(*img_mat,img_left, img_right));

    }else{

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

    if(ui->checkbox_fullScale->isChecked()) //Consume a lot of process time
        this->IO_full_scale = StereoSGBM::MODE_HH;
    else
        this->IO_full_scale = false;

    img1 = mat_to_qimage(disparity_map_SGBM());  //Generate the disparity map

}
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

/**
 * @brief Reset the sgbm parameters to default values
 */
void Disparity::on_reset_image_clicked() {

    ui->image_loaded->setMaximumSize(width, height);
    ui->image_loaded->setPixmap(QPixmap::fromImage(mat_to_qimage(*img_mat)));
    ui->image_loaded->adjustSize();

    ui->slider_windowSize->setSliderPosition(9);
    ui->slider_numberOfDisparities->setSliderPosition(9);
    ui->slider_preFilterCap->setSliderPosition(50);
    ui->slider_minDisparity->setSliderPosition(5);
    ui->slider_uniquenessRatio->setSliderPosition(5);
    ui->slider_speckleWindowSize->setSliderPosition(0);
    ui->slider_speckleRange->setSliderPosition(8);
    ui->slider_disp12MaxDiff->setSliderPosition(-1);
}

/**
 * @brief filter the disparity map
 */
void Disparity::on_post_filtering_clicked(){
    //disparity_post_filtering();
    qDebug("Not yet implemented");
}

/**
 * @brief Generate the disparity map from two cv::mat pointer of a stereo image
 * @return disparity map
 */
Mat Disparity::disparity_map_SGBM() {

    Mat img_right_gray, img_left_gray;
    Mat disp;

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
    disp= cv::Scalar::all(255)-disp;
    return disp;
}

//TODO
Mat Disparity::disparity_post_filtering() {
    Mat * res = new Mat;
    return *res;
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
    ui->image_loaded->setMaximumSize(width, height);
    QImage img1(file_name, "PNM"); //load the file in  a QImage variable (pnm is a format like ttif, ...)
    QImage img_fit = img1.scaled(width,height, Qt::KeepAspectRatio);   //resize the qimage
    ui->image_loaded->setPixmap(QPixmap::fromImage(img_fit));  //Display the original image
    ui->image_loaded->adjustSize();


    qDebug(" *** Image file correctly loaded *** ");

    *img_mat = qimage_to_mat(img1);

    split(*img_mat, img_left, img_right);    //cut the img_mat in two and save the result in two public variables (resp. img_left & img_right)
    return true;
}


/**
 * @brief Load an cv::mat img, display it in the right panel and finally split it then save them in two pointers
 * @param img
 */
void Disparity::set_img_mat(cv::Mat *img){
    img_mat = img;
    QImage img1 = imagecv::mat_to_qimage(*img_mat);
    QImage img2 =img1.scaled(ui->image_loaded->width(),ui->image_loaded->height(), Qt::KeepAspectRatio);//resize the qimage
    ui->image_loaded->setPixmap(QPixmap::fromImage(img2));//Display the original image
    imagecv::split(*img_mat, img_left, img_right);
}
/**
 * @brief Display a disparity map using sbm parameters
 * @param img_left the left point of view of a scene
 * @param img_right the right point of view of a scene
 * @return The disparity map using sbm
 */

Mat Disparity::sbm(Mat img, Mat *img_left, Mat *img_right){
    Mat dst, imgL, imgR;
    dst = Mat(img.size(), CV_8U);
    cvtColor(*img_left, imgL,CV_BGR2GRAY);
    cvtColor(*img_right,imgR,CV_BGR2GRAY);
    Ptr<StereoBM> matcher= StereoBM::create(IO_numberOfDisparities,IO_SADWindowSize);
    matcher->compute(imgL,imgR,dst);
     dst.convertTo(dst,CV_8U,1,0);
     return dst;
}


void Disparity::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked()){
        ui->slider_preFilterCap->setEnabled(false);
        ui->slider_minDisparity->setEnabled(false);
        ui->slider_uniquenessRatio->setEnabled(false);
        ui->slider_speckleWindowSize->setEnabled(false);
        ui->slider_speckleRange->setEnabled(false);
        ui->slider_disp12MaxDiff->setEnabled(false);
    }else{
        ui->slider_preFilterCap->setEnabled(true);
        ui->slider_minDisparity->setEnabled(true);
        ui->slider_uniquenessRatio->setEnabled(true);
        ui->slider_speckleWindowSize->setEnabled(true);
        ui->slider_speckleRange->setEnabled(true);
        ui->slider_disp12MaxDiff->setEnabled(true);
    }
}
