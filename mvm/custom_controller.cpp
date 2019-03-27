#include "custom_controller.h"

namespace mvm
{

using namespace cv;
	CustomController::CustomController():
	camera_matrixL(), dist_coeffsL(),
  camera_matrixR(), dist_coeffsR(), disparityParam(), Q(),
  distMin(), distMax(){

  }
  CustomController::~CustomController() {
    // Do not forget to move any memory owned by your controller
  }

  void CustomController::process(const cv::Mat & left_img,
    const cv::Mat & right_img,
    float * vx, float * vy, float * omega) {
    	std::string folderPath("./images/"),
    	extension(".png");
      system("mkdir -p \"folderPath\"");

      // Just keep static

      static int image_number = 0;
      static int nb_frame_forward = 3*FPS;
      static int nb_frame_rotation_left = 3*FPS;
      static   cv::Rect trackWindow(cv::Rect(left_img.rows/2, 0, left_img.rows/2, left_img.cols/2)) ;

      if(nb_frame_forward%5 == 0){
      // enregistrement image
        std::cout << "Image number: " << image_number << "\n";

        std::string path_img_right = folderPath + "right_" + std::to_string(image_number) + extension;
        std::string path_img_left = folderPath + "left_" + std::to_string(image_number) + extension;

        if(!right_img.empty() && !left_img.empty()){

					std::cout << "[FORWARD] disparity map generation !\n";
          cv::Mat disparityMap;
          sbm(left_img, right_img, disparityMap,
						 disparityParam.at<int>(0), disparityParam.at<int>(1), disparityParam.at<int>(2),
						 disparityParam.at<int>(3), disparityParam.at<int>(4), disparityParam.at<int>(5),
						 disparityParam.at<int>(6), disparityParam.at<int>(7), disparityParam.at<int>(8),
						 disparityParam.at<int>(9)
					 );

					std::cout << "[FORWARD] depth map generation !\n";
					cv::Mat depthMapIm(depthMap(disparityMap, Q));

          trackCamShift(left_img, trackWindow);
          Scalar avr= mean(depthMapIm(trackWindow));

          if(avr(0) > distMax){
            *vx = 0.3;
          }else if(avr(0) < distMin){
            *vx = -0.3;
          }else{
            *vx = 0;

          }
					// enregistrement image

          image_number +=1;
          std::cout << "[FORWARD] Image recorded: " << path_img_left << " !\n";
          cv::imwrite(path_img_right, right_img);
          cv::imwrite(path_img_left, left_img);
        } else {
          std::cout << "[FORWARD] Image are null!\n";
        }
      }
      if(FPS%5 == 0){
        std::cout << "Image number: " << image_number << "\n";

        std::string path_img_right = folderPath + "right_" + std::to_string(image_number) + extension;
        std::string path_img_left = folderPath + "left_" + std::to_string(image_number) + extension;

        if(!right_img.empty() && !left_img.empty()){
          image_number +=1;
          std::cout << "[WAIT] Image recorded: " << std::to_string(image_number) << " !\n";
          cv::imwrite(path_img_right, right_img);
          cv::imwrite(path_img_left, left_img);
        } else {
          std::cout << "[WAIT] Image are null!\n";
        }
      }




    /*
      *vx = 0;
      *vy = 0;
      *omega = 0;
    */
    }



    void CustomController::load() {
      // This function can be used to load internal parameters of the controller
      // without requiring compilation
      cv::FileStorage fs(CONFIG_FILE_PATH, cv::FileStorage::READ);
      if(fs.isOpened()){
        cv::Mat distanceParam;
        fs["DistParam"] >> distanceParam;
          std::cout << "distParamLoaded" << std::endl;
        fs["DisparityParameter"] >> disparityParam;
          std::cout << "Error passed" << std::endl;
        fs["cameraMatrixLeft"] >> camera_matrixL;
        fs["distCoefficientsMatrixRight"] >> dist_coeffsL;
        fs["cameraMatrixRight"] >> camera_matrixR;
        fs["distCoefficientsMatrixRight"] >> dist_coeffsR;
        fs["dispToDepthMatrix"] >> Q;

        distMin = distanceParam.at<int>(0);
        distMax = distanceParam.at<int>(0);
        fs.release();

          std::cout << "intrinsic parameters successfully loaded" << std::endl;

      }else
        std::cout << "couldn't open " << CONFIG_FILE_PATH << " file."<< std::endl;

    }

void CustomController::sbm(cv::Mat const&imageL, cv::Mat const&imageR, cv::Mat &dst, size_t const &IO_SADWindowSize, size_t const &IO_numberOfDisparities,  size_t const &IO_preFilterCap, const size_t &IO_minDisparity,
                   const size_t &IO_uniquenessRatio, const size_t &IO_speckleWindowSize,
                   const size_t &IO_speckleRange, const int &IO_disp12MaxDif, const size_t &IO_textureTreshold, const size_t &IO_tresholdFilter)
{
    cv::Mat imgL, imgR;
    cvtColor(imageL, imgL,CV_BGR2GRAY);
    cvtColor(imageR,imgR,CV_BGR2GRAY);

    Ptr<StereoBM> matcher= StereoBM::create(IO_numberOfDisparities,IO_SADWindowSize);

    matcher->setPreFilterCap(IO_preFilterCap);
    matcher->setMinDisparity(IO_minDisparity);
    matcher->setDisp12MaxDiff(IO_disp12MaxDif);
    matcher->setUniquenessRatio(IO_uniquenessRatio);
    matcher->setSpeckleWindowSize(IO_speckleWindowSize);
    matcher->setSpeckleRange(IO_speckleRange);
    matcher->setTextureThreshold(IO_textureTreshold);
    matcher-> setNumDisparities(IO_numberOfDisparities);

    matcher->compute(imgL,imgR,dst);
    normalize(dst, dst, 0, 255, NORM_MINMAX, CV_8U);
    threshold(dst,dst,IO_tresholdFilter,255,THRESH_TOZERO);

}


cv::Mat CustomController::depthMap(const cv::Mat &disparityMap, cv::Mat &dispToDepthMatrix)
{
    cv::Mat depthMapImage;
    cv::Vec3f floatPoint;
    cv::reprojectImageTo3D(disparityMap, depthMapImage, dispToDepthMatrix, true);


    cv::Mat depthMapVal(depthMapImage.rows, depthMapImage.cols, CV_32F);
    for(size_t i(0); i < depthMapImage.rows; i++){
        for(size_t j(0); j < depthMapImage.cols; j++){
            floatPoint = depthMapImage.at<Vec3f>(i,j);
            depthMapVal.at<float>(i, j) = floatPoint[2];
        }
    }
    normalize(depthMapVal, depthMapVal, 0, 255, NORM_MINMAX, CV_32F);

    return depthMapVal;

}

void CustomController::rectifiedImage(cv::Mat &imageL, cv::Mat &imageR, cv::Mat const&dist_coeffsL, cv::Mat const&camera_matrixL,
                         cv::Mat const&dist_coeffsR, cv::Mat const&camera_matrixR)
{
  cv::Mat imgDistortedL(imageL);
  cv::Mat imgDistortedR(imageR);

  undistort(imgDistortedL, imageL, camera_matrixL, dist_coeffsL);
  undistort(imgDistortedR, imageR, camera_matrixR, dist_coeffsR);
}

void CustomController::trackCamShift(cv::Mat const& image, cv::Rect &trackWindow){
  int hsize(16), vmin(0), vmax(180), smin(180), channels[] = {0, 0};
    float hranges[] = {0, 180};
    const float* phranges = hranges;
    Mat hsv, hue, mask, hist, backproj;
    cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    inRange(hsv, Scalar(0, smin, vmin), Scalar(180, 256, vmax), mask);
    hue.create(hsv.size(), hsv.depth());
    mixChannels(&hsv, 1, &hue, 1, channels, 1);


    Mat roi(hue, trackWindow), maskroi( mask, trackWindow);
    calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
    normalize(hist, hist, 0, 255, NORM_MINMAX);
    

    calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
    backproj &= mask;
    CamShift(backproj, trackWindow, TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1));

}
}
