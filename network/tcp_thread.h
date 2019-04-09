#ifndef TCP_THREAD_H
#define TCP_THREAD_H

#include <iostream>

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>

#include "widgets/disparity.h"

#define IMG_WIDTH 1024
#define IMG_HEIGHT 562
typedef unsigned char byte;

class TCP_Thread : public QThread
{
    Q_OBJECT
public:
    explicit TCP_Thread(int iID, QObject *parent = nullptr);
    ~TCP_Thread();
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void send_raw_images(ImgCv img, int&order);

public slots:
    void readyRead();
    void disconnected();
    void send_order(int order);

private:
    QTcpSocket *socket;
    int socketDescriptor;
    QByteArray mat_to_qByteArray(cv::Mat image);

    bool receive_raw_stereo_image();

};

#endif // TCP_THREAD_H
