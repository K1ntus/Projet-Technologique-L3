#include "server.h"

//https://www.bogotobogo.com/cplusplus/sockets_server_client_QT.php
//test conneciton with
//nc -v localhost 57575

/**
 * @brief Server::Server
 * @param parent
 */
Server::Server(QObject *parent) :
    QTcpServer(parent)
{

}

void Server::StartServer()
{
    /*
    * Return false if no server can be created
    * Last parameters is the port
    */
    if(!this->listen(QHostAddress::Any, PORT_NUMBER))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening...";
    }
}

void Server::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << " Connecting...";
    TCP_Thread *thread = new TCP_Thread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();

    //parametersWindow = new Disparity();
    //parametersWindow->show();
}
