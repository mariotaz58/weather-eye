#include "tcpHandler.hpp"
#include <Windows.h>

int main ()
{
    tcpServer server;
    server.start ();

    Sleep (INFINITE);
    return 0;
}
