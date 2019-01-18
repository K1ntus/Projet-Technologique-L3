#include "intrinsicparameters.h"
using namespace std;
using namespace cv;

IntrinsicParameters::IntrinsicParameters() :
    camera_matrix(nullptr),
    dist_coeffs(nullptr)
{
    camera_matrix = new Mat(3, 3, CV_32FC1);
    camera_matrix->ptr<float>(0)[0] = 1;
    camera_matrix->ptr<float>(1)[1] = 1;
    dist_coeffs = new Mat;
}

IntrinsicParameters::IntrinsicParameters(cv::Mat& new_camera_matrix, cv::Mat& dist_coeffs_mat) :
    camera_matrix(nullptr),
    dist_coeffs(nullptr)
{
    camera_matrix = new Mat;
    dist_coeffs = new Mat;
    new_camera_matrix.copyTo(*camera_matrix);
    dist_coeffs_mat.copyTo(*dist_coeffs);

}

IntrinsicParameters::~IntrinsicParameters()
{
    delete camera_matrix;
    delete dist_coeffs;
}


Mat& IntrinsicParameters::getCameraMatrix() const{
    return *camera_matrix;
}

Mat& IntrinsicParameters::getDistCoeffs() const{
    return *dist_coeffs;
}

void IntrinsicParameters::setCameraMatrix(Mat &camera_matrix)
{
    camera_matrix.copyTo(*this->camera_matrix);
}

void IntrinsicParameters::setDistCoeffsMatrix(Mat &dist_coeffs_mat)
{
    dist_coeffs_mat.copyTo(*this->dist_coeffs);
}

bool IntrinsicParameters::empty()
{
    return dist_coeffs  == nullptr || dist_coeffs->empty() || camera_matrix == nullptr;
}

void IntrinsicParameters::readIntrCalibration(std::string& filename, IntrinsicParameters &outputIntrinsicParam)
{
    FileStorage in(filename, FileStorage::READ);
    if(in.isOpened()){

        Mat &cameraMat = outputIntrinsicParam.getCameraMatrix(),
                distCoeffs;

        in["cameraMatrix"] >> cameraMat;
        in["distCoefficientsMatrix"] >> distCoeffs;

        in.release();
        outputIntrinsicParam.setCameraMatrix(cameraMat);
        outputIntrinsicParam.setDistCoeffsMatrix(distCoeffs);

    }else
        cout << "error while opening file stream" << endl;

}

void IntrinsicParameters::printIntrCalibration(string& filename, IntrinsicParameters &intrinsicParamToPrint)
{
    FileStorage out(filename, FileStorage::WRITE);
    if(out.isOpened()){
        Mat &dist_coeffs = intrinsicParamToPrint.getDistCoeffs();
        Mat &camera_matrix = intrinsicParamToPrint.getCameraMatrix();

        out << "cameraMatrix" << camera_matrix;
        out << "distCoefficientsMatrix" << dist_coeffs;

        out.release();

    }
    else
        cout << "error while opening file stream" << endl;
}
