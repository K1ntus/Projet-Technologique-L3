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

    if(!fileName.isEmpty()){
        Mat image = imread(fileName.toStdString());
        if(stereo){
            fileName = QFileDialog::getOpenFileName(&thisWidget, thisWidget.tr("Sélectionnez une image"), thisWidget.tr("resources/"), thisWidget.tr("Image Files (*.png *.jpg *.bmp)"));
            if(!fileName.isEmpty()){
                Mat imageRight = imread(fileName.toStdString());

                img.setImg(image, imageRight);
                return true;
            }else
                std::cout << "Error : couldn't open second file.\nin: imagecv::load_file" << std::endl;
        }
        img.setImg(image, stereo);
        return true;

    }else
        std::cout << "Error : couldn't open file.\nin: imagecv::load_file" << std::endl;

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
        cvtColor(src,temp,CV_GRAY2RGB);
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

/**
 * @brief imagecv::displayImage display a mat image into a qt frame
 * @param frame The qt frame where you want to display the image
 * @param image the image to display
 */
void imagecv::displayImage(QLabel &frame, const Mat &image)
{

    frame.setPixmap(QPixmap::fromImage(
                        mat_to_qimage(image).scaled(frame.width(),frame.height(), Qt::KeepAspectRatio))  //Convert the new cv::mat to QImage
                    ); //Display the original image

    frame.adjustSize();
}

void imagecv::displayVideo(QLabel &frame, VideoCapture &capL, VideoCapture &capR, string const&calibFilePath)
{

    if(capL.isOpened() && capR.isOpened()){

        int key = ' ', prevKey = key;
        if(!capL.grab() || !capR.grab()){
            std::cout << "[ERROR] in:imagecv::displayVideo\n no frame to grab." << std::endl;
            return;
        }

        cv::Mat displayedImg, imgVidL, imgVidR, Q, depthMap;
        ImgCv stereo;
        capL.retrieve(imgVidL);
        capR.retrieve(imgVidR);
        Rect trackWind(Rect(imgVidL.rows/2, 0, imgVidL.rows/2, imgVidL.cols/(2)));
        if(!capL.grab() || !capR.grab()) return;

        cv::Mat param;
        bool hasFile(!calibFilePath.empty());
        if(hasFile){
            FileStorage fs(calibFilePath, FileStorage::READ);
            if(fs.isOpened()){

                fs["DisparityParameter"] >> param;
                fs["dispToDepthMatrix"] >> Q;
                fs.release();
            }else{
                std::cout << "[ERROR] in: on_dispParam_clicked\n couldn't opend calib file." << std::endl;
                return;
            }
        }
        double framerate = capL.get(CV_CAP_PROP_FPS);
        while(key != 'q'){

            if(key == 'p'){

                if(prevKey != 'p')
                    std::cout << "pause" << std::endl;
                prevKey = key;
            }else{
                capL.retrieve(imgVidL);
                capR.retrieve(imgVidR);
                stereo.setImg(imgVidL, imgVidR);
                if (key == 'k'){
                    ImgCv::trackCamShift(imgVidL,trackWind);

                    if(trackWind.width > trackWind.height){
                        std::cout << "[INFO] Tracked object has a larger width than height" << std::endl;
                        std::cout << "     * width = " << trackWind.width <<", height = " << trackWind.height << std::endl;

                        trackWind.height = imgVidL.rows*0.75;//Box have a height of 3/4 of the image
                        trackWind.width = imgVidL.cols*0.2;//Box have a width of 1/10 of the image

                        trackWind.x = imgVidL.cols/2;
                        trackWind.y = 0;
                    }

                    std::cout << "[INFO] Tracking position:" << std::endl;
                    std::cout << "     * TopCorner = (" << trackWind.x << "," << trackWind.y << ")" << std::endl;
                    std::cout << "     * Size      = (" << trackWind.width << "," << trackWind.height << ")" << std::endl;

                    if(trackWind.width < imgVidL.cols*0.2){
                        std::cout << "[INFO] INVALID OBJECT DETECTED." << std::endl;
                    }

                    rectangle(displayedImg, trackWind, Scalar(255), 2);
                }else if(key == 'l')
                    displayedImg = stereo.getImgL();
                else if(key == 'r')
                    displayedImg = stereo.getImgR();
                else if(key == 'u' && hasFile)
                    displayedImg = ImgCv::rectifiedImage(stereo, calibFilePath), true;
                else if(key == 's' && hasFile){
                    stereo.setImg(ImgCv::rectifiedImage(stereo, calibFilePath), true);
                    displayedImg = imgVidL;
                    std::cout << "param 0: " << param.at<int>(0) << std::endl;
                    imgVidL = stereo.getDisparityMap(calibFilePath, param);
                    ImgCv::trackCamShift(displayedImg, trackWind);
                    rectangle(imgVidL, trackWind, Scalar(255), 2);
                    normalize(imgVidL, displayedImg, 0, 255, NORM_MINMAX, CV_8U);

                }else if(key == 'j'){

                    stereo.setImg(ImgCv::rectifiedImage(stereo, calibFilePath), true);
                    displayedImg = imgVidL;
                    imgVidL = stereo.getDisparityMap(calibFilePath, param);
                    depthMap = stereo.depthMap(imgVidL, Q);
                    std::cout << "depthMap generated" << std::endl;

                    normalize(depthMap, depthMap, 0, 255, NORM_MINMAX, CV_32F);
                    std::cout << "depthMap normalized" << std::endl;

                    cvtColor(depthMap, depthMap, CV_GRAY2BGR);
                    ImgCv::trackCamShift(displayedImg,trackWind);
                    std::cout << "tracker tracked" << std::endl;

                    if(trackWind.width > trackWind.height){
                        std::cout << "[INFO] Tracked object has a larger width than height" << std::endl;
                        std::cout << "     * width = " << trackWind.width <<", height = " << trackWind.height << std::endl;

                        trackWind.height = imgVidL.rows*0.75;//Box have a height of 3/4 of the image
                        trackWind.width = imgVidL.cols*0.2;//Box have a width of 1/10 of the image

                        trackWind.x = imgVidL.cols/2;
                        trackWind.y = 0;
                    }

                    std::cout << "[INFO] Tracking position:" << std::endl;
                    std::cout << "     * TopCorner = (" << trackWind.x << "," << trackWind.y << ")" << std::endl;
                    std::cout << "     * Size      = (" << trackWind.width << "," << trackWind.height << ")" << std::endl;

                    if(trackWind.width < imgVidL.cols*0.2){
                        std::cout << "[INFO] INVALID OBJECT DETECTED." << std::endl;
                    }

                    Scalar avr = mean(depthMap(trackWind));
                    std::cout << "average value " << avr(1) << std::endl;
                    displayedImg = stereo;
                    rectangle(displayedImg, trackWind, Scalar(255), 2);
                    key = 'p';

                }
                else if(key == 'd' && hasFile){
                    stereo.setImg(ImgCv::rectifiedImage(stereo, calibFilePath), true);
                    displayedImg = imgVidL;
                    imgVidL = stereo.getDisparityMap(calibFilePath, param);
                    depthMap = stereo.depthMap(imgVidL, Q);
                    std::cout << "depthMap generated" << std::endl;

                    normalize(depthMap, depthMap, 0, 255, NORM_MINMAX, CV_32F);
                    std::cout << "depthMap normalized" << std::endl;

                    cvtColor(depthMap, depthMap, CV_GRAY2BGR);
                    ImgCv::trackCamShift(displayedImg,trackWind);
                    std::cout << "tracker tracked" << std::endl;

                    if(trackWind.width > trackWind.height){
                        std::cout << "[INFO] Tracked object has a larger width than height" << std::endl;
                        std::cout << "     * width = " << trackWind.width <<", height = " << trackWind.height << std::endl;

                        trackWind.height = imgVidL.rows*0.75;//Box have a height of 3/4 of the image
                        trackWind.width = imgVidL.cols*0.2;//Box have a width of 1/10 of the image

                        trackWind.x = imgVidL.cols/2;
                        trackWind.y = 0;
                    }

                    std::cout << "[INFO] Tracking position:" << std::endl;
                    std::cout << "     * TopCorner = (" << trackWind.x << "," << trackWind.y << ")" << std::endl;
                    std::cout << "     * Size      = (" << trackWind.width << "," << trackWind.height << ")" << std::endl;

                    if(trackWind.width < imgVidL.cols*0.2){
                        std::cout << "[INFO] INVALID OBJECT DETECTED." << std::endl;
                    }

                    Scalar avr = mean(depthMap(trackWind));
                    std::cout << "average value" << avr(1) << std::endl;
                    displayedImg = depthMap;
                    rectangle(displayedImg, trackWind, Scalar(255), 2);

                }else
                    displayedImg = stereo;

                imagecv::displayImage(frame, displayedImg);
                imshow("video test", displayedImg);
                prevKey = key;

                if(!capL.grab() || !capR.grab()){
                    capL.set(cv::CAP_PROP_POS_FRAMES, 0);
                    capL.grab();
                    capR.set(cv::CAP_PROP_POS_FRAMES, 0);
                    capR.grab();
                }
            }

            key = waitKey((int) framerate);
            if(key == 255)
                key = prevKey;
        }

        destroyWindow("video test");
    }else{
        std::cout << "Couldn't open video in imagecv::displayVideo" << std::endl;
    }
}

void imagecv::displayVideo(QLabel &frame, VideoCapture &capL)
{
    if(capL.isOpened()){

        int key = ' ', prevKey = key;
        if(!capL.grab()){
            std::cout << "[ERROR] in:imagecv::displayVideo\n no frame to grab." << std::endl;
            return;
        }

        cv::Mat displayedImg, imgVidL;
        capL.retrieve(imgVidL);
        Rect trackWind(Rect(imgVidL.rows/2, 0, imgVidL.rows/2, imgVidL.cols/(2)));
        if(!capL.grab()) return;

        double framerate = capL.get(CV_CAP_PROP_FPS);
        while(key != 'q'){

            if(key == 'p'){
                if(prevKey != 'p')
                    std::cout << "pause" << std::endl;
                prevKey = key;
            }else{
                capL.retrieve(imgVidL);
                if (key == 'k'){
                    ImgCv::trackCamShift(imgVidL,trackWind);

                    if(trackWind.width > trackWind.height){
                        std::cout << "[INFO] Tracked object has a larger width than height" << std::endl;
                        std::cout << "     * width = " << trackWind.width <<", height = " << trackWind.height << std::endl;

                        trackWind.height = imgVidL.rows*0.75;//Box have a height of 3/4 of the image
                        trackWind.width = imgVidL.cols*0.2;//Box have a width of 1/10 of the image

                        trackWind.x = imgVidL.cols/2;
                        trackWind.y = 0;
                    }

                    std::cout << "[INFO] Tracking position:" << std::endl;
                    std::cout << "     * TopCorner = (" << trackWind.x << "," << trackWind.y << ")" << std::endl;
                    std::cout << "     * Size      = (" << trackWind.width << "," << trackWind.height << ")" << std::endl;

                    if(trackWind.width < imgVidL.cols*0.2){
                        std::cout << "[INFO] INVALID OBJECT DETECTED." << std::endl;
                    }

                    rectangle(displayedImg, trackWind, Scalar(255), 2);

                }else
                    displayedImg = imgVidL;

                imagecv::displayImage(frame, displayedImg);
                imshow("video test", displayedImg);
                std::cout << "next frame" << "\tkey : " << (char)prevKey << std::endl;
                prevKey = key;

                if(!capL.grab()){
                    capL.set(cv::CAP_PROP_POS_FRAMES, 0);
                    capL.grab();
                }
            }

            key = waitKey((int) framerate);
            if(key == 255)
                key = prevKey;
        }

        destroyWindow("video test");
    }else{
        std::cout << "Couldn't open video in imagecv::displayVideo" << std::endl;
    }
}
