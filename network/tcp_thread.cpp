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
    if(receive_raw_stereo_image())
        send_depth_map(parametersWindow->get_img_mat()->getImg());

}

QByteArray TCP_Thread::mat_to_qByteArray(cv::Mat image){
    std::vector<uint8_t> buffer;
    cv::imencode(".png", image, buffer);
    QByteArray byteArray = QByteArray::fromRawData((const char*)buffer.data(), buffer.size());
    QString base64Image(byteArray.toBase64());

    return byteArray;
}

void TCP_Thread::send_depth_map(cv::Mat depth_map){
    socket->write("DEPTH_MAP");
    qDebug() << "Sending the depth map message to the client: " << socketDescriptor;

    //QByteArray data_ack = socket->readAll();
    //if(data_ack == "ready"){
        qDebug() << socketDescriptor << "Ready to upload the depth map ";

        socket->write(mat_to_qByteArray(depth_map)); //Send back the depth map generated

        socket->write("DEPTH MAP SENT");
    //}

}

bool TCP_Thread::receive_raw_stereo_image(){
    QByteArray data_received;
    while (socket->waitForReadyRead(3000))
        data_received += socket->readAll();

    //qDebug() << "\n\n" << socketDescriptor << " Data in: " << data_received;

    QImage received_image_qimg = QImage::fromData(data_received, "JPG");    //Crash si l'image est dans un autre format (ie. la fille en rouge qui est en png)

    if(received_image_qimg.isNull()){
        qDebug("Image is null");
        return false;
    }

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

    socket->write(data_received);
    data_received.clear();
    qDebug("Disparity map loaded");

    return true;
}


void TCP_Thread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    parametersWindow->hide();
    exit(0);
}
