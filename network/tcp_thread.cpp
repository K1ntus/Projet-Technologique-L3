#include "tcp_thread.h"

TCP_Thread::TCP_Thread(int ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
    parametersWindow = new Disparity();
}

void TCP_Thread::run()
{
    // thread starts here
    qDebug() << socketDescriptor << " Starting thread";
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()),Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()),Qt::DirectConnection);

    qDebug() << socketDescriptor << " Client connected";

    parametersWindow->show();

    // make this thread a loop
    exec();
}

void TCP_Thread::readyRead()
{
    QByteArray data_received;
    while (socket->waitForReadyRead(1000))
        data_received += socket->readAll();

    //qDebug() << "\n\n" << socketDescriptor << " Data in: " << data_received;

    QImage received_image_qimg = QImage::fromData(data_received, "PNG");

    cv::Mat received_image = imagecv::qimage_to_mat(received_image_qimg);

    qDebug("Image well received.");
    /*
    if(!received_image.empty())
    {
       Display it on an OpenCV window
      imshow("Raw Received Image", received_image);
    }
    */

    cv::Mat left_view, right_view;
    ImgCv::split(received_image, left_view, right_view);
    ImgCv image_to_analyze = ImgCv(left_view, right_view);

    parametersWindow->set_img_mat(image_to_analyze);

    data_received.clear();
    qDebug("Disparity map loaded");

}

cv::Mat TCP_Thread::bytes_to_mat(byte * img_data, int width, int height){
    cv::Mat image = cv::Mat(height, width, CV_8UC3, img_data).clone(); // make a copy
    return image;
}

byte* TCP_Thread::mat_to_bytes(cv::Mat image){
    int size = image.total() * image.elemSize();
    byte * bytes = new byte[size];  // you will have to delete[] that later
    std::memcpy(bytes,image.data,size * sizeof(byte));
    return bytes;
}

void TCP_Thread::send_depth_map(cv::Mat depth_map){
    qDebug() << "Sending the depth map message to the client: " << socketDescriptor;

    socket->write("dpt_map");
    QByteArray data_ack = socket->readAll();
    if(data_ack == "ready"){
        qDebug() << socketDescriptor << "Ready to download the depth map ";

        //socket->write(mat_to_bytes(depth_map)); //Send back the depth map generated

    }

}

void TCP_Thread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    parametersWindow->hide();
    exit(0);
}
