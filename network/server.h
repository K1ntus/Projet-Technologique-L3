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
    void StartServer();

signals:

public slots:
    void received_pair_image(QImage received_image);

protected:
    //void incomingConnection(qintptr socketDescriptor);  //New version Qt's signature
    void incomingConnection(int socketDescriptor);     //Old Qt's signature

private:
    Disparity * parametersWindow;
    TCP_Thread *thread;

};
#endif // SERVER_H

