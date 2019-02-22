#include "tcp_thread.h"

TCP_Thread::TCP_Thread(int ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
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

    // make this thread a loop
    exec();
}

void TCP_Thread::readyRead()
{
    QByteArray data_received = socket->readAll();

    qDebug() << socketDescriptor << " Data in: " << data_received;

    //cv::Mat received_image = bytes_to_mat(data_received,)

    socket->write(data_received); //Send back the depth map generated
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
    exit(0);
}
