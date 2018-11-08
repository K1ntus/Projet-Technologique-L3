#include "disparity.h"
#include "ui_disparity.h"

Disparity::Disparity(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Disparity)
{
    ui->setupUi(this);
}

Disparity::~Disparity()
{
    delete ui;
}

bool Disparity::on_pushButton_clicked()
{
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

    this->hide();
}
