#include "disparity.h"

using namespace cv;
using namespace std;
using namespace imagecv;

/**
 * @brief Disparity::DisparityCreate the disparity gui, and define the "default" parameters for the disparity map generation.
 *
 * @param parent parent of this widget
 */
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
 * @brief Apply the disparity map parameters when the button got pressed and display it to the right
 * \n
 * When the apply button is triggered, a disparity map will be generated and displayed depending of the parameters\n
 * using the differents checkbox and sliders.\n
 * \n
 * If no image has been loaded, the program just display an error.\n
 * \n
 * When the image has been correctly generated, it's displayed in the right slot.
 */
void Disparity::on_apply_clicked(){
    QImage img1;
    if(img->getImg().empty() || img->getImgR().empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }

    /*  APPLY SELECTED PARAMETERS DISPARITY MAP  */
    if(ui->filter->isChecked()){
        Mat filtered_disp;
        if(ui->checkBox->isChecked()){
            this->IO_numberOfDisparities = ui->slider_numberOfDisparities->value()*16;
            if((this->IO_SADWindowSize = ui->slider_windowSize->value())%2==0)
            this->IO_SADWindowSize = ui->slider_windowSize->value()+1;
            filtered_disp = img->disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize);
         }else{
             this->IO_numberOfDisparities = ui->slider_numberOfDisparities->value()*16;
            this->IO_SADWindowSize = ui->slider_windowSize->value();
            this->IO_preFilterCap =  ui->slider_preFilterCap->value();
            this->IO_P1= ui->slider_P1->value();
            this->IO_P2 = ui->slider_P2->value();
            filtered_disp = img->disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize, IO_preFilterCap, IO_P1, IO_P2);

         }
        img1 = mat_to_qimage(filtered_disp);
    }else{

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
        /*  END PARAMETERS DISPARITY MAP    */

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
  }
    QImage img2 = img1.scaled(width,height, Qt::KeepAspectRatio);   //Create a new image which will fit the window

    //Colorize the disparity map if the dedicated checkbox is checked
    if(ui->checkbox_colorize->isChecked()){
        // Apply the colormap:
        Mat cm_img0;
        applyColorMap(qimage_to_mat(img2), cm_img0, COLORMAP_JET);
        img2 = mat_to_qimage(cm_img0);
    }

    //Display the generated disparity map in the left image slot
    ui->image_loaded->setPixmap(QPixmap::fromImage(img2));
    ui->image_loaded->adjustSize();
}

/**
 * @brief Open a gui to select an image when you push the load button
 */
void Disparity::on_loadImage_clicked(){
    if(load_file(*this, *img)){
        QMessageBox::information(this, "Open image", "Image loaded");
        img->setImg(*img, true);
        displayImage(img->getImg());


        qDebug(" *** Image file correctly loaded *** ");
    }
    if(img->getImg().empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
}

/**
 * @brief Reset the sgbm parameters to default values (arbitrary fixed, as the more "commons")
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
 * @brief Load an cv::mat img, display it in the right panel and finally split it then save them in two pointers
 * @param img the stereo image to manage, split and display
 */
void Disparity::set_img_mat(ImgCv &img){
    delete this->img;
    this->img = nullptr;
    this->img = &img;
    displayImage(this->img->getImg());
}

/**
 * @brief Disparity::on_checkBox_clicked enable or disable the differents parameters, depending of the disparity generation mode (ie. sbm/sgbm)
 */
void Disparity::on_checkBox_clicked(){
    if(ui->checkBox->isChecked()){
        ui->slider_preFilterCap->setEnabled(false);
        ui->slider_minDisparity->setEnabled(false);
        ui->slider_uniquenessRatio->setEnabled(false);
        ui->slider_speckleWindowSize->setEnabled(false);
        ui->slider_speckleRange->setEnabled(false);
        ui->slider_disp12MaxDiff->setEnabled(false);
        ui->slider_P1->setEnabled(false);
        ui->slider_P2->setEnabled(false);
    }else{
        ui->slider_preFilterCap->setEnabled(true);
        ui->slider_minDisparity->setEnabled(true);
        ui->slider_uniquenessRatio->setEnabled(true);
        ui->slider_speckleWindowSize->setEnabled(true);
        ui->slider_speckleRange->setEnabled(true);
        ui->slider_disp12MaxDiff->setEnabled(true);
        if(ui->filter->isChecked()){
            ui->slider_P1->setEnabled(true);
            ui->slider_P2->setEnabled(true);
        }
    }
}

/**
 * @brief Disparity::displayImage display a cv:mat image in the right image slot
 * @param image image to be displayed
 */
void Disparity::displayImage(Mat const& image){
    ui->image_loaded->setMaximumSize(width, height);
    ui->image_loaded->setPixmap(QPixmap::fromImage(mat_to_qimage(image)));
    ui->image_loaded->adjustSize();
}

/**
 * @brief Disparity::on_Sobel_clicked applies the Sobel filter on the disparity map to display its contours
 */
void Disparity::on_Sobel_clicked()
{
    if(img->getImg().empty() || img->getImgR().empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }
    Mat sobel, img1;

    if(ui->checkBox->isChecked()&& ui->filter->isChecked())
        img1 = img->disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize);
    else if (ui->checkBox->isChecked())
             img1 = img->sbm(IO_numberOfDisparities, IO_SADWindowSize);

    else if (ui->filter->isChecked())
        img1= img->disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize,IO_preFilterCap, IO_P1, IO_P2);
    else {
        img1 = img->disparity_map_SGBM(IO_minDisparity,
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

    }
    sobel = ImgCv::contour_sobel(img1);
    imagecv::displayImage(*ui->image_loaded, sobel);

}
/**
 * @brief Disparity::on_Laplace_clicked applies the Laplacian filter on the disparity map to display its contours
 */

void Disparity::on_Laplace_clicked()
{
    if(img->getImg().empty() || img->getImgR().empty()){
        qDebug("[ERROR] Please, load a stereo image first");
        return;
    }

    Mat laplace, img1;
    if(ui->checkBox->isChecked()&& ui->filter->isChecked())
        img1 = img->disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize);
    else if(ui->checkBox->isChecked())
        img1 = img->sbm(IO_numberOfDisparities, IO_SADWindowSize);
    else if (ui->filter->isChecked())
        img1= img->disparity_post_filtering(IO_numberOfDisparities, IO_SADWindowSize,IO_preFilterCap, IO_P1, IO_P2);
    else {
        img1 = img->disparity_map_SGBM(IO_minDisparity,
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

    }

    laplace = ImgCv::contour_laplace(img1);
    imagecv::displayImage(*ui->image_loaded, laplace);

}
/**
 * @brief enable or disable parameters when the checkbox "post_filtered" is not checked.
 */
void Disparity::on_filter_clicked()
{
    if (ui->filter->isChecked()&& !(ui->checkBox->isChecked())){
        ui->slider_P1->setEnabled(true);
        ui->slider_P2->setEnabled(true);
    }else {
        ui->slider_P1->setEnabled(false);
        ui->slider_P2->setEnabled(false);
    }

}
