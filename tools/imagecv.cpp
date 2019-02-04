#include "imagecv.h"

using namespace std;
using namespace cv;

/**
 * @brief imagecv::speed_test Calculate the time consumption of a function and one cv::Mat parameter
 *
 * @param func the function to test
 * @param args argument of the function
 * @return string containing the result of speed test
 */
QString imagecv::speed_test(function_call func, cv::Mat const& args){
    clock_t start, end;
    start = clock();
    func(args);
    end = clock();
    double delta = (end-start);
    QString str = "result of speed test: " + QString::number((delta/CLOCKS_PER_SEC));
    return str;
}

/**
 * @brief imagecv::speed_test Calculate the time consumption of a function with the 3 cv::Mat parameters.
 *
 * @param func the function to test
 * @param args first argument of func
 * @param arg2 second argument of func
 * @param arg3 third argument of third
 * @return string containing the result of speed test
 */
QString imagecv::speed_test(function_call_3_arg func, cv::Mat const& args, cv::Mat* arg2, cv::Mat* arg3){
    clock_t start, end;
    start = clock();
    func(args, arg2, arg3);
    end = clock();
    double delta = (end-start);
    QString str = "result of speed test: " + QString::number((delta/CLOCKS_PER_SEC));
    return str;
}

/**
 * @brief imagecv::load_file
 *
 * @param thisWidget the widget using this method
 * @param img pointer that will contain the result image if the stereo parameter is set to false
 * @param stereo if true, the loaded image will be automatically split in two and store into pointers
 * @return true if a file has been loaded, else false
 */
bool imagecv::load_file(QWidget &thisWidget, ImgCv &img, bool stereo) {

    QString fileName = QFileDialog::getOpenFileName(&thisWidget, thisWidget.tr("Sélectionnez une image"), thisWidget.tr("resources/"), thisWidget.tr("Image Files (*.png *.jpg *.bmp)"));
    QFile file(fileName);

    qDebug(" *** Loading file *** ");

    if (!file.open(QFile::ReadOnly | QFile::Text)) {    //Check file validity/readable/...
        QMessageBox::warning(&thisWidget, thisWidget.tr("Application"),
                             thisWidget.tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }
    else if(!fileName.isEmpty()){
        Mat image = imread(fileName.toStdString());
        img.setImg(image, stereo);
        return true;

    }
    return false;
}

/**
 * @brief Convert a cv::Mat image to a QImage using the RGB888 format
 * @param src the cv::Mat image to convert
 * @return QImage image
 */
QImage imagecv::mat_to_qimage(Mat const& src) {
    Mat temp;  // make the same cv::Mat than src
    if(src.channels()==1)
        cvtColor(src,temp,CV_GRAY2BGR);
    else if(src.channels()==3)
        cvtColor(src,temp,CV_BGR2RGB);
    else
        cvtColor(src,temp,CV_BGRA2RGB);
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits();   // enforce deep copy, see documentation

    return dest.convertToFormat(QImage::Format_RGB888);
}


/**
 * @brief Convert a QImage to a cv::Mat image
 * @param src the QImage to convert
 * @return cv::Mat image
 */
Mat imagecv::qimage_to_mat(const QImage& src) {
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




void imagecv::displayImage(QLabel &frame, const Mat &image)
{

//    frame.setMaximumSize(width, height);
    frame.setPixmap(QPixmap::fromImage(
                        mat_to_qimage(image).scaled(frame.width(),frame.height(), Qt::KeepAspectRatio))  //Convert the new cv::mat to QImage
                    ); //Display the original image

    frame.adjustSize();
}
