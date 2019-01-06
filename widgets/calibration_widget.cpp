#include "calibration_widget.h"

using namespace std;
using namespace cv;
using namespace imagecv;

Calibration_widget::Calibration_widget(Calibration_intr* calib, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Calibration_widget)
{
    ui->setupUi(this);
    this->calib = calib;
    display_image(calib->get_gray_image());

}

Calibration_widget::~Calibration_widget()
{
    qDebug( "calibration deleted");
    delete ui;
    delete calib;
}

void Calibration_widget::on_undistortedButton_clicked()
{
    display_image(calib->undistorted_image());

}

void Calibration_widget::on_chesscorners_clicked()
{
        display_image(calib->get_gray_image());

}

void Calibration_widget::display_image(Mat const&image){
    QImage qimg = mat_to_qimage(image);
    int h = ui->Displayer->height();
    int w = ui->Displayer->width();
    ui->Displayer->setPixmap(QPixmap::fromImage(qimg.scaled(w, h, Qt::KeepAspectRatio)));
    ui->Displayer->adjustSize();
}

void Calibration_widget::on_newImageSet_clicked()
{
    QStringList filePath = QFileDialog::getOpenFileNames(
                this, tr("Select the files for calibration"),
                tr("./resources/"),
                tr("Image files (*.png *.jpg *.bmp)") );

    if(!filePath.isEmpty()){

        vector<Mat> images;
        size_t count = filePath.size();

        for(size_t i = 0; i < count; i++){
            images.push_back(imread(filePath[i].toStdString()));
        }

        calib->newImageSet(images);
    }

}

void Calibration_widget::on_nextImage_clicked()
{
    size_t const& incr = calib->getCurrentImgIndex();
    calib->setNextImgIndex(incr + 1);
    if(incr == calib->getCurrentImgIndex())
        QMessageBox::information(this, tr("End of set"), tr("reach the end of the set"));
    else
        display_image(calib->get_image_origin());
}

void Calibration_widget::on_prevImage_clicked()
{
    size_t const& decr = calib->getCurrentImgIndex();
    calib->setNextImgIndex(decr - 1);
    if(decr == calib->getCurrentImgIndex())
        QMessageBox::information(this, tr("start of set"), tr("reach the beginning of the set"));
    else
        display_image(calib->get_image_origin());
}
