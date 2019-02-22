#ifndef TCP_THREAD_H
#define TCP_THREAD_H

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

#include <iostream>

typedef unsigned char byte;

class TCP_Thread : public QThread
{
    Q_OBJECT
public:
    explicit TCP_Thread(int iID, QObject *parent = 0);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

public slots:

private:
    QTcpSocket *socket;
    int socketDescriptor;

    byte* mat_to_bytes(cv::Mat image);
    cv::Mat bytes_to_mat(byte * img_data, int width, int height);

    void send_depth_map(cv::Mat depth_map);
};

#endif // TCP_THREAD_H
