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
			*vx = 0;
			*vy = 0;
			*omega = 0;

			std::string folderPath("./images/"),
			extension(".png");
			system("mkdir -p \"folderPath\"");

			// Just keep static

			static int image_number = 0;
			static   cv::Rect trackWindow(cv::Rect(left_img.rows/2, 0, left_img.rows/2, left_img.cols/2)) ;
			// enregistrement image
			std::cout << "[INFO] Image number: " << image_number << "\n";


			if(!right_img.empty() && !left_img.empty()){

				std::cout << "[FORWARD] disparity map generation !\n";
				cv::Mat disparityMap;
				getDisparityMap(left_img, right_img, disparityMap,disparityParam);

				std::cout << "[INFO] depth map generation !\n";
				cv::Mat depthMapIm(depthMap(disparityMap, Q));

				trackCamShift(left_img, trackWindow);
				Scalar avr= mean(depthMapIm(trackWindow));

				if(trackWindow.width > trackWindow.height){
					std::cout << "[WARNING] Tracking object has a larger width than height" << std::endl;
					std::cout << "     * width = " << trackWindow.width <<", height = " << trackWindow.height << std::endl;

					trackWindow.height = left_img.rows*0.75;//Box have a height of 3/4 of the image
					trackWindow.width = left_img.cols*0.2;//Box have a width of 1/10 of the image

					trackWindow.x = left_img.cols/2;
					trackWindow.y = 0;
				}
				std::cout << "[INFO] Tracking lambda position:" << std::endl;
				std::cout << "     * TopCorner = (" << trackWindow.x << "," << trackWindow.y << ")" << std::endl;
				std::cout << "     * Size      = (" << trackWindow.width << "," << trackWindow.height << ")" << std::endl;
				avr = mean(depthMapIm(trackWindow));
				std::cout << "(" << avr(0) << "," << avr(1) << "," << avr(2) << ")" << std::endl;

				if(trackWindow.width < left_img.cols*0.1 || trackWindow.y > 0){
					std::cout << "[INFO] No need to move on *vx. Distance: " << avr(0) << std::endl;
					*vx = 0;
				}else if(avr(0) > distMax){
					std::cout << "[FORWARD] Tracking object at a distance of " << avr(0) << std::endl;
					*vx = VMAX * (avr(0)/distMax);
				}else{// if(avr(0) < distMin){
					std::cout << "[BACKWARD] Tracking object at a distance of " << avr(0) << std::endl;
					*vx = -VMAX * (distMax/avr(0));
				}

				//Rotation handler
				//rad/s, + is left
				if(trackWindow.x > (left_img.cols/2 + left_img.cols/10)){
					std::cout << "[ROTATION RIGHT] Tracking object at a distance of " << distMax << std::endl;
					*omega = -0.3;
				}else if (trackWindow.x < left_img.cols/2){
					std::cout << "[ROTATION LEFT] Tracking object at a distance of " << distMax << std::endl;
					*omega = +0.3;
				}

			} else {
				std::cout << "[WARN] Image are null!\n";
			}

			std::cout << "[INFO] Saving logs: " << image_number << "\n";

			std::string path_img_right = folderPath + "right_" + std::to_string(image_number) + extension;
			std::cout << "     * [SAVING] Img right saved:" << path_img_right << std::endl;
			std::string path_img_left = folderPath + "left_" + std::to_string(image_number) + extension;
			std::cout << "     * [SAVING] Img left saved:" << path_img_left << std::endl;

			if(!right_img.empty() && !left_img.empty()){
			image_number +=1;
			std::cout << "[WAIT] Image recorded: " << std::to_string(image_number) << " !\n";
			cv::imwrite(path_img_right, right_img);
			cv::imwrite(path_img_left, left_img);
		} else {
		std::cout << "[WAIT] Image are null!\n";
	}

}



void CustomController::load() {
	// This function can be used to load internal parameters of the controller
	// without requiring compilation
	cv::FileStorage fs(CONFIG_FILE_PATH, cv::FileStorage::READ);
	if(fs.isOpened()){
		cv::Mat distanceParam;
		fs["DisparityParameter"] >> disparityParam;
		fs["cameraMatrixLeft"] >> camera_matrixL;
		fs["distCoefficientsMatrixRight"] >> dist_coeffsL;
		fs["cameraMatrixRight"] >> camera_matrixR;
		fs["distCoefficientsMatrixRight"] >> dist_coeffsR;
		fs["dispToDepthMatrix"] >> Q;
		fs["DistParam"] >> distanceParam;
		distMin = distanceParam.at<int>(0);
		distMax = distanceParam.at<int>(0);
		fs.release();

		std::cout << "intrinsic parameters successfully loaded" << std::endl;

	}else
	std::cout << "couldn't open " << CONFIG_FILE_PATH << " file."<< std::endl;

}

void CustomController::sbm(cv::Mat const&imageL, cv::Mat const&imageR, cv::Mat &dst, size_t const &IO_minDisparity, size_t const &IO_numberOfDisparities,  size_t const &IO_SADWindowSize, const int &IO_disp12MaxDif,
	const size_t &IO_preFilterCap, const size_t &IO_uniquenessRatio, const size_t &IO_speckleWindowSize,
	const size_t &IO_speckleRange, const size_t &IO_textureTreshold, const size_t &IO_tresholdFilter)
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
		matcher->setNumDisparities(IO_numberOfDisparities);

		matcher->compute(imgL,imgR,dst);
		normalize(dst, dst, 0, 255, NORM_MINMAX, CV_8U);
		threshold(dst,dst,IO_tresholdFilter,255,THRESH_TOZERO);

	}

	void CustomController::disparity_map_SGBM(cv::Mat const&imageL, cv::Mat const&imageR, cv::Mat &dst, const size_t &IO_minDisparity, const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize,
		const size_t &IO_P1, const size_t &IO_P2, const int &IO_disp12MaxDif,
		const size_t &IO_preFilterCap, const size_t &IO_uniquenessRatio, const size_t &IO_speckleWindowSize,
		const size_t &IO_speckleRange, const int &IO_full_scale) const
		{
			Mat img_right_gray, img_left_gray;
			//Convert the two stereo image in gray
			cvtColor(imageL, img_left_gray, CV_BGR2GRAY);
			cvtColor(imageR,img_right_gray, CV_BGR2GRAY);

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
			sgbm->compute(img_left_gray, img_right_gray, dst);    //Generate the disparity map
			normalize(dst, dst, 0, 255, NORM_MINMAX, CV_8U);
		}

		void CustomController::disparity_post_filtering(cv::Mat const&imageL, cv::Mat const&imageR, cv::Mat &dst,
			size_t const &IO_minDisparity, size_t const &IO_numberOfDisparities,
			size_t const &IO_SADWindowSize, const int &IO_disp12MaxDif,
			const size_t &IO_preFilterCap, const size_t &IO_uniquenessRatio,
			const size_t &IO_speckleWindowSize,	const size_t &IO_speckleRange,
			const size_t &IO_textureTreshold, const size_t &IO_tresholdFilter,
			const size_t &IO_sigma, const size_t &IO_lambda)
			{
				using namespace cv::ximgproc;

				Mat left_disparity, right_disparity, filtered, left_for_matching, right_for_matching;
				left_for_matching= imageL.clone();
				right_for_matching = imageR.clone();

				Ptr<StereoBM> matcher_left = StereoBM::create(IO_numberOfDisparities, IO_SADWindowSize); // use StereoBM to create a matcher
				matcher_left->setPreFilterCap(IO_preFilterCap);
				matcher_left->setMinDisparity(IO_minDisparity);
				matcher_left->setDisp12MaxDiff(IO_disp12MaxDif);
				matcher_left->setUniquenessRatio(IO_uniquenessRatio);
				matcher_left->setSpeckleWindowSize(IO_speckleWindowSize);
				matcher_left->setSpeckleRange(IO_speckleRange);
				matcher_left->setTextureThreshold(IO_textureTreshold);
				matcher_left->setNumDisparities(IO_numberOfDisparities);

				Ptr<DisparityWLSFilter> filter = cv::ximgproc::createDisparityWLSFilter(matcher_left); // creation of the filter
				Ptr<StereoMatcher> matcher_right= createRightMatcher(matcher_left);// creation of the right matcher

				cv::cvtColor(left_for_matching,left_for_matching,COLOR_BGR2GRAY);
				cv::cvtColor(right_for_matching, right_for_matching, COLOR_BGR2GRAY);
				matcher_left->compute(left_for_matching,right_for_matching,left_disparity);    // compute the left disparity map
				matcher_right->compute(right_for_matching,left_for_matching, right_disparity); // compute the right disparity map

				filter->setLambda(IO_lambda);// lambda defining regularization of the filter.  With a high value, the edge of the disparity map will "more" match with the source image
				filter->setSigmaColor(IO_sigma); // sigma represents the sensitivity of the filter

				filter->filter(left_disparity,imageL,filtered,right_disparity); // apply the filter

				cv::ximgproc::getDisparityVis(filtered,dst, 10.0);// permits to visualize the disparity map
				normalize(dst, dst,0,255,CV_MINMAX, CV_8U);
			}

			void CustomController::disparity_post_filtering(cv::Mat const&imageL, cv::Mat const&imageR, cv::Mat &dst,
				const size_t &IO_minDisparity, const size_t &IO_numberOfDisparities, const size_t &IO_SADWindowSize,
				const size_t &IO_P1, const size_t &IO_P2,
				const int &IO_disp12MaxDif, const size_t &IO_preFilterCap,
				const size_t &IO_uniquenessRatio, const size_t &IO_speckleWindowSize,
				const size_t &IO_speckleRange, const int &IO_full_scale,
				const size_t &IO_sigma, const size_t &IO_lambda)
				{
					using namespace cv::ximgproc;

					Mat left_disparity, right_disparity, filtered, left_for_matching, right_for_matching;
					left_for_matching= imageL.clone();
					right_for_matching = imageR.clone();
					Ptr <StereoSGBM> matcher_left = StereoSGBM::create(
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
					); // use SGBM to create a matcher


					Ptr<DisparityWLSFilter> filter = cv::ximgproc::createDisparityWLSFilter(matcher_left); // creates the WLSfilter
					Ptr<StereoMatcher> matcher_right= createRightMatcher(matcher_left);


					matcher_left->compute(left_for_matching,right_for_matching,left_disparity); // compute the left disparity map
					matcher_right->compute(right_for_matching,left_for_matching, right_disparity); // compute the right disparity map

					filter->setLambda(IO_lambda);
					filter->setSigmaColor(IO_sigma);

					filter->filter(left_disparity,imageL,filtered,right_disparity);
					cv::ximgproc::getDisparityVis(filtered,dst, 10.0);
					normalize(dst, dst,0,255,CV_MINMAX, CV_8U);
				}

				void CustomController::getDisparityMap(cv::Mat const&imageL, cv::Mat const&imageR, cv::Mat &dst, cv::Mat param ){
					/*if(!param){
						std::cout << "Calibration file is null, exiting prgm" << std::endl;
					}*/

					switch(param.at<int>(0)){
						case 0:
						std::cout << "[INFO] sbm entries" << std::endl;

						sbm(imageL, imageR, dst,
							param.at<int>(1), param.at<int>(2), param.at<int>(3),
							param.at<int>(4), param.at<int>(5), param.at<int>(6),
							param.at<int>(7), param.at<int>(8), param.at<int>(9), param.at<int>(10)
						);

						std::cout << "[INFO] sbm done" << std::endl;

						break;
						case 1:
							std::cout << "[INFO] sbm + PS entries" << std::endl;

							disparity_post_filtering(imageL, imageR, dst,
								param.at<int>(1), param.at<int>(2), param.at<int>(3),
								param.at<int>(4), param.at<int>(5), param.at<int>(6),
								param.at<int>(7), param.at<int>(8), param.at<int>(9),
								param.at<int>(10), param.at<int>(11), param.at<int>(12)
							);
							std::cout << "[INFO] sbm  +PS done" << std::endl;
							break;
						case 2:
							std::cout << "[INFO] SGBM entries" << std::endl;

							disparity_map_SGBM(imageL, imageR, dst,
								param.at<int>(1), param.at<int>(2), param.at<int>(3),
								param.at<int>(4), param.at<int>(5), param.at<int>(6),
								param.at<int>(7), param.at<int>(8), param.at<int>(9),
								param.at<int>(10),  param.at<int>(11)
							);
							std::cout << "[INFO] SGBM done" << std::endl;
							break;
						case 3:
							std::cout << "[INFO] SGBM + PS entries" << std::endl;

							disparity_post_filtering(imageL, imageR, dst,
								param.at<int>(1), param.at<int>(2), param.at<int>(3),
								param.at<int>(4), param.at<int>(5), param.at<int>(6),
								param.at<int>(7), param.at<int>(8), param.at<int>(9),
								param.at<int>(10), param.at<int>(11), param.at<int>(12)
							);
							std::cout << "[INFO] SGBM + PS done" << std::endl;

							break;
						default:
							std::cout << "[ERROR] can't match to any case" << std::endl;
							break;

					}

				}

				cv::Mat CustomController::depthMap(const cv::Mat &disparityMap, cv::Mat &dispToDepthMatrix)
				{
					cv::Mat depthMapImage;
					cv::Vec3f floatPoint;
					cv::reprojectImageTo3D(disparityMap, depthMapImage, dispToDepthMatrix, true);


					cv::Mat depthMapVal(depthMapImage.rows, depthMapImage.cols, CV_32F);
					for(size_t i(0); i < depthMapImage.rows; i++){
						for(size_t j(0); j < depthMapImage.cols; j++){
							/*
							if(depthMapImage.at<float>(i,j)<200){
								floatPoint = depthMapImage.at<Vec3f>(i,j);
								depthMapVal.at<float>(i, j) = floatPoint[2];
							}
							*/

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
