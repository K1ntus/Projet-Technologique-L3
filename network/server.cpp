#include "server.h"

//doc: https://doc.qt.io/qt-5/qtnetwork-programming.html

//test connection with
// nc -v localhost PORT_NUMBER (see server.h)
//Or
// nc -v localhost PORT_NUMBER < path/to/file (could be an image!)

/**
 * @brief Server::Server
 * @param parent
 */
Server::Server(QObject *parent) :
    QTcpServer(parent),
    parametersWindow(nullptr),
    thread(nullptr),
    calibFile()
{
}

Server::~Server()
{
    delete parametersWindow;
    thread->disconnected();
    delete thread;
}

/**
 * @brief Server::StartServer Start the TCP server. Currently using localhost and the port define in the header
 */
void Server::StartServer(const std::string &calibFile)
{
    /*
    * Return false if no server can be created
    * Last parameters is the port
    */
    //if(!this->listen(QHostAddress::Any, PORT_NUMBER))
    if(!this->listen(QHostAddress::Any, PORT_NUMBER))
    {
        qDebug() << "Could not start server. Port maybe already used.";
    }
    else
    {
        qDebug() << "Adress is: " << this->serverAddress().toString() << "\nPort is: " << PORT_NUMBER;
        qDebug() << "\n *** Listening... ***";
        this->calibFile = calibFile;
    }
}


void Server::incomingConnection(int socketDescriptor)     //Old Qt's signature, if that crash in the cremin use this one
//void Server::incomingConnection(qintptr socketDescriptor)   //Use this for the newer version of QT
{
    parametersWindow = new Disparity(calibFile);
    qDebug() << socketDescriptor << " A Client is Connecting...";
    thread = new TCP_Thread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(send_raw_images(ImgCv, int&)), this, SLOT(received_pair_image(ImgCv, int&)));
    connect(this, SIGNAL(order(int)), thread, SLOT(send_order(int)));
    thread->run();
}

void Server::received_pair_image(ImgCv received_image, int &next_order){

    qDebug ("Received signal for new pair images !");

//    cv::Mat received_image_cv = imagecv::qimage_to_mat(received_image);
//    ImgCv image_to_analyze = ImgCv(received_image_cv, true);

    parametersWindow->set_img_mat(received_image);
    if(parametersWindow->isHidden())
        parametersWindow->show();

//    int next_order(0);

    float distance = ImgCv::calculateDistanceDepthMap(parametersWindow->get_depth_map(true));
    std::cout << "distance " << distance << std::endl;
    if(distance < 80)
        next_order = 4;
    else if(distance > 120)
        next_order = 1;

    std::cout << "[INFO] in received_pair_image ::order " << next_order << std::endl;

//    emit order(next_order);

}
