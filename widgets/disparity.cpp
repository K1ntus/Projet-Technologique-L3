#include "disparity.h"



using namespace cv;
using namespace cv::ximgproc;
using namespace std;
using namespace imagecv;


Disparity::Disparity(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Disparity),
    img(nullptr)
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
    img = new ImgCv();
}

Disparity::~Disparity(){

    qDebug("delete disparity");
    delete ui;
    delete img;
}


/**
 * @brief Apply the disparity map parameters when the button got pressed and display it to the left
 */
void Disparity::on_apply_clicked(){
    QImage img1;
    if(img->getImg().empty() || img->getImgR().empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }

    if(ui->checkBox->isChecked()){
        if((this->IO_SADWindowSize = ui->slider_windowSize->value())%2==0)
            this->IO_SADWindowSize = ui->slider_windowSize->value()+1;
        else
            this->IO_SADWindowSize = ui->slider_windowSize->value();
        this->IO_numberOfDisparities = ui->slider_numberOfDisparities->value() * 16; //this parameters had to be a multiple of 16

        img1 = mat_to_qimage(img->sbm(IO_numberOfDisparities, IO_SADWindowSize));

    }else{

    this->IO_SADWindowSize = ui->slider_windowSize->value();    //Getting the value of the slider
    this->IO_numberOfDisparities = ui->slider_numberOfDisparities->value() * 16;    //this parameters had to be a multiple of 16
    this->IO_preFilterCap = ui->slider_preFilterCap->value();
    this->IO_minDisparity = ui->slider_minDisparity->value();
    this->IO_uniquenessRatio = ui->slider_uniquenessRatio->value();
    this->IO_speckleWindowSize = ui->slider_speckleWindowSize->value();
    this->IO_speckleRange = ui->slider_speckleRange->value();
    this->IO_disp12MaxDif = ui->slider_disp12MaxDiff->value();

    this->IO_P1 = 8* img->getImgR().channels() * IO_SADWindowSize * IO_SADWindowSize;
    this->IO_P2 = 32* img->getImgR().channels() * IO_SADWindowSize * IO_SADWindowSize;

    if(ui->checkbox_fullScale->isChecked()) //Consume a lot of process time
        this->IO_full_scale = StereoSGBM::MODE_HH;
    else
        this->IO_full_scale = false;

    //Generate the disparity map
    img1 = mat_to_qimage(img->disparity_map_SGBM(IO_minDisparity,
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
                                                 )
                         );

}
    QImage img2 = img1.scaled(width,height, Qt::KeepAspectRatio);   //Create a new image which will fit the window
    ui->image_loaded->setPixmap(QPixmap::fromImage(img2));  //Display the disparity map in the specific slot
    ui->image_loaded->adjustSize();
}

/**
 * @brief Open a gui to select an image when you push the load button
 */
void Disparity::on_loadImage_clicked(){
    if(load_file(*this, *img, true)){
        QMessageBox::information(this, "Open image", "Image loaded");
        displayImage(img->getImg());


        qDebug(" *** Image file correctly loaded *** ");
    }
    if(img->getImg().empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
}

/**
 * @brief Reset the sgbm parameters to default values
 */
void Disparity::on_reset_image_clicked() {

    displayImage(img->getImg());

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

    qDebug("Not yet implemented");
   displayImage(img->disparity_post_filtering());

}

/**
 * @brief Load an cv::mat img, display it in the right panel and finally split it then save them in two pointers
 * @param img
 */
void Disparity::set_img_mat(ImgCv &img){
    delete this->img;
    this->img = nullptr;
    this->img = &img;
    displayImage(this->img->getImg());
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

void Disparity::displayImage(Mat const& image){
    ui->image_loaded->setMaximumSize(width, height);
    ui->image_loaded->setPixmap(QPixmap::fromImage(mat_to_qimage(image)));
    ui->image_loaded->adjustSize();
}
