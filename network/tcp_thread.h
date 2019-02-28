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
    explicit TCP_Thread(int iID, QObject *parent = 0x0);
    Disparity * parametersWindow;
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void send_raw_images(QImage img);

public slots:
    void readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    int socketDescriptor;

    QByteArray mat_to_qByteArray(cv::Mat image);

    bool receive_raw_stereo_image();
    void send_depth_map(cv::Mat depth_map);

};

#endif // TCP_THREAD_H
