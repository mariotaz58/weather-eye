#ifndef _CONTROLLER_HPP_
#define _CONTROLLER_HPP_

#include "threadHandler.hpp"
#include "msgQueueHandler.hpp"
#include "tcpHandler.hpp"
#include "comPortHandler.hpp"
#include "dataFormat.h"

#define COM_PORT_NAME   "COM3"

class controller
{
public:
    controller ();
    ~controller ();
    void start ();
    void process (void *p);

private:
    threadHandler   connectThread;
    msgQueueHandler *mqueuePtr;
    tcpServer       server;
    comPortHandler  *com;

    void processUCData (const pkt_data *pkt);
    void processMobileData (const pkt_data *pkt);
    void uc_cmdVal_Hum_Op_status(const pkt_data *pkt);
    void uc_cmdVal_Hum_Op_error(const pkt_data *pkt);
    void uc_cmdVal_Temp_Op_status(const pkt_data *pkt);
    void uc_cmdVal_Temp_Op_error(const pkt_data *pkt);
    void uc_cmdVal_hello_Op_status(const pkt_data *pkt);
    void uc_cmdVal_hello_Op_error(const pkt_data *pkt);
    void uc_cmdVal_ping_Op_status(const pkt_data *pkt);
    void uc_cmdVal_ping_Op_error(const pkt_data *pkt);
};

#endif
