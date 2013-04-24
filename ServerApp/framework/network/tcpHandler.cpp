#include "tcpHandler.hpp"
#include <iostream>
#include <winsock2.h>

class channelHandle
{
public:
    channelHandle ()
    {
        recvSocket = INVALID_SOCKET;
        sendSocket = INVALID_SOCKET;
    }
    SOCKET recvSocket;
    SOCKET sendSocket;
};

class tcpClientPrvate
{
public:
    channelHandle *handle;
    threadHandler rxThread;

    int id;
    bool runMore;
};


//-----------------------------------------------------------------
tcpServer::tcpServer ()
{
}

tcpServer::~tcpServer ()
{
}

void tcpServer::start ()
{
    Slot<void, void*> fp;
    fp = new Callback <tcpServer, void, void*>(this, &tcpServer::connectHandler);
    connectThread.create (fp, this);
}

tcpClient* tcpServer::getClient (unsigned int id)
{
    if (id < clientList.size())
        return clientList.at (id);
    else
        return 0;
}

void tcpServer::connectHandler (void *p)
{
    if (static_cast<tcpServer*>(p) != this)
    {
        std::cout << "FATAL ERROR:: Pointer mismatch for TCP Connect handler" << std::endl;
        return;
    }

    SOCKET RxServer;
    SOCKET TxServer;
    SOCKADDR_IN local;
    WSADATA wsaData;
    SOCKADDR_IN from;
    int fromlen=sizeof(from);

    WSAStartup(MAKEWORD(2,2),&wsaData);

    local.sin_family=AF_INET; //Address family
    local.sin_addr.s_addr=INADDR_ANY; //Wild card IP address
    local.sin_port=htons((u_short)20249); //port to use
    RxServer=socket(AF_INET,SOCK_STREAM, 0);
    bind(RxServer,(struct sockaddr*)&local,sizeof(local));

    local.sin_family=AF_INET; //Address family
    local.sin_addr.s_addr=INADDR_ANY; //Wild card IP address
    local.sin_port=htons((u_short)20250); //port to use
    TxServer=socket(AF_INET,SOCK_STREAM, 0);
    bind(TxServer,(struct sockaddr*)&local,sizeof(local));

    while (1)
    {
        channelHandle *client = new channelHandle;
        listen(TxServer,10);
        printf ("TCP Server: Waiting for connection of Send Socket\n");
        client->sendSocket = accept(TxServer, (struct sockaddr*)&from,&fromlen);
        printf ("TCP Server: Send Socket connection, socket ID: %d\n", client->sendSocket);

        listen(RxServer,10);
        printf ("TCP Server: Waiting for connection of Recv Socket\n");
        client->recvSocket = accept(RxServer, (struct sockaddr*)&from,&fromlen);
        printf ("TCP Server: Recv Socket connection, socket ID: %d\n", client->recvSocket);

        tcpClient *handler = new tcpClient (clientList.size(), client);
        clientList.push_back (handler);
        handler->start ();
    }
}

//-----------------------------------------------------------------

tcpClient::tcpClient (int id, void *hndl)
{
    d = new tcpClientPrvate;
    d->handle = static_cast<channelHandle*>(hndl);
    d->runMore = true;
    d->id = id;
}

tcpClient::~tcpClient ()
{
    delete d;
}

void tcpClient::start ()
{
    Slot<void, void*> fp;
    fp = new Callback <tcpClient, void, void*>(this, &tcpClient::rxHandler);
    d->rxThread.create (fp, this);
}

int tcpClient::receive (unsigned char *buff, const unsigned int buffSize, unsigned int &recvSize)
{
    if (d->handle->recvSocket != INVALID_SOCKET)
        return ::recv(d->handle->recvSocket, (char*)buff, buffSize, 0);
    else
        Sleep (INFINITE);
    return 0;
}

int tcpClient::send (const unsigned char *buff, const unsigned int buffSize)
{
    int ret = 0;
    if (d->handle->sendSocket != INVALID_SOCKET)
    {
        ret = ::send(d->handle->sendSocket, (char*)buff, buffSize, 0);
    }
    return ret;
}

void tcpClient::rxHandler (void *p)
{
    if (static_cast<tcpClient*>(p) != this)
    {
        std::cout << "FATAL ERROR:: Pointer mismatch for TCP Rx handler" << std::endl;
        return;
    }

    unsigned char *buffer = new unsigned char[20];
    unsigned int recvSize = 0;
    while (d->runMore)
    {
        receive (buffer, 20, recvSize);
    }
    delete [] buffer;
}
