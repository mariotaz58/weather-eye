#ifndef _TCP_HANDLER_HPP_
#define _TCP_HANDLER_HPP_
#include "threadHandler.hpp"
#include <vector>

class tcpClientPrvate;
class tcpClient;

class tcpServer
{
public:
    tcpServer ();
    virtual ~tcpServer ();
    void start ();

    tcpClient* getClient (unsigned int id);

private:
    std::vector <tcpClient*> clientList;
    threadHandler connectThread;
    void connectHandler (void *p);
};

class tcpClient
{
public:
    tcpClient (int id, void *hndl);
    virtual ~tcpClient ();

    void start ();
    int receive (unsigned char *buff, const unsigned int buffSize, unsigned int &recvSize);
    int send (const unsigned char *buff, const unsigned int buffSize);

private:
    tcpClientPrvate *d;
    void rxHandler (void *p);
};

#endif
