#include "calibration_widget.h"

using namespace std;
using namespace cv;
using namespace imagecv;

Calibration_widget::Calibration_widget(Calibration_intr* calib, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Calibration_widget),
    currentMode(ORIGINAL)
{
    ui->setupUi(this);
    this->calib = calib;
    display_image(currentMode);

}

Calibration_widget::~Calibration_widget()
{
    qDebug( "calibration deleted");
    delete ui;
    delete calib;
}

void Calibration_widget::on_undistortedButton_clicked()
{
    if(calib->getIntrinsicParameters().empty())
        return;

    currentMode = UNDISTORTED;
    display_image(UNDISTORTED);

}

void Calibration_widget::on_chesscorners_clicked()
{
    currentMode = CHESSBOARD_CORNERS;
    display_image(CHESSBOARD_CORNERS);

}

void Calibration_widget::display_image(DisplayerMode const&displayerMode){
    Mat *imagePtr(nullptr);

    switch (displayerMode) {

    case ORIGINAL:
        imagePtr = &calib->get_image_origin();
        break;

    case CHESSBOARD_CORNERS:
        imagePtr = &calib->get_compute_image();
        break;

    case UNDISTORTED:
        break;
    }
    QImage qimg;
    if(imagePtr != nullptr)
        qimg = mat_to_qimage(*imagePtr);
    else
        qimg = mat_to_qimage(calib->undistorted_image());

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
        ui->undistortedButton->setEnabled(false);
        on_originalImage_clicked();

    }

}

void Calibration_widget::on_nextImage_clicked()
{
    size_t const& incr = calib->getCurrentImgIndex();
    calib->setNextImgIndex(incr + 1);
    if(incr == calib->getCurrentImgIndex()){
        QMessageBox::information(this, tr("End of set"), tr("reach the end of the set"));
    }
    else
        display_image(currentMode);
}

void Calibration_widget::on_prevImage_clicked()
{
    size_t const& decr = calib->getCurrentImgIndex();
    calib->setNextImgIndex(decr - 1);
    if(decr == calib->getCurrentImgIndex()){
        QMessageBox::information(this, tr("start of set"), tr("reach the beginning of the set"));
    } else
        display_image(currentMode);
}

void Calibration_widget::on_save_clicked()
{

    if(calib->getIntrinsicParameters().empty()){
        QMessageBox::warning(this, tr("Error while saving file"), tr("no calibration to save."));
        return;
    }
    string outFile("test.yml");
    IntrinsicParameters::printIntrCalibration(outFile, calib->getIntrinsicParameters());
    //    delete this;
}

void Calibration_widget::on_loadCalib_clicked()
{
    string inFile("test.yml");
    IntrinsicParameters intrParam;
    IntrinsicParameters::readIntrCalibration(inFile, intrParam);
    if(!intrParam.empty()){
        calib->setIntrinsincParameters(intrParam);
        ui->undistortedButton->setEnabled(true);
    }
    else
        QMessageBox::warning(this, tr("Parse error"), tr("Error while parsing file"));

}

void Calibration_widget::on_originalImage_clicked()
{
    currentMode = ORIGINAL;
    display_image(ORIGINAL);
}

void Calibration_widget::on_calibration_clicked()
{
    calib->calibrate();
    QMessageBox::information(this, tr("Calibration"), tr("Calibrated!"));
    ui->undistortedButton->setEnabled(true);
    display_image(currentMode);

}

void Calibration_widget::on_Calibration_widget_destroyed()
{
    delete this;
}

void Calibration_widget::on_CharucoCalib_clicked()
{
    vector<cv::Mat> &imageSet = calib->getSet();
    delete calib;
    if(ui->CharucoCalib->isChecked()){
        calib = new CharucoCalibration(imageSet);
    }else{
        calib = new ChessboardCalibration(imageSet);
    }
}
