#ifndef SERVER_H
#define SERVER_H

#include <QObject>

#include <QTcpServer>
#include <QDebug>
#include "tcp_thread.h"
#include "widgets/disparity.h"

#define PORT_NUMBER 57575

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    void StartServer();

signals:

public slots:

protected:
    //Disparity * parametersWindow;
    void incomingConnection(int socketDescriptor);

};
#endif // SERVER_H

