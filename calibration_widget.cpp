#include "calibration_widget.h"
#include "ui_calibration_widget.h"

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
    delete ui;
}

void Calibration_widget::on_undistortedButton_clicked()
{
    display_image(calib->undistorted_image());

}

void Calibration_widget::on_chesscorners_clicked()
{
        display_image(calib->get_gray_image());

}

void Calibration_widget::display_image(Mat *image){
    QImage qimg = mat_to_qimage(*image);
    int h = ui->Displayer->height();
    int w = ui->Displayer->width();
    ui->Displayer->setPixmap(QPixmap::fromImage(qimg.scaled(w, h, Qt::KeepAspectRatio)));
    ui->Displayer->adjustSize();
}
