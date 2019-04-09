#ifndef SERVER_H
#define SERVER_H

#include <QObject>

#include <QTcpServer>
#include <QDebug>
#include "tcp_thread.h"

#define PORT_NUMBER 25556

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();
    void StartServer(std::string const&calibFile);

signals:
    void order(int);

public slots:
    void received_pair_image(ImgCv received_image, int &order);

protected:
    //void incomingConnection(qintptr socketDescriptor);  //New version Qt's signature
    void incomingConnection(int socketDescriptor);     //Old Qt's signature

private:
    Disparity * parametersWindow;
    TCP_Thread *thread;
    std::string calibFile;


};
#endif // SERVER_H

