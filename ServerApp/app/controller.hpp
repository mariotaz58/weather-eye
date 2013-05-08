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
    void timeout(void *p);

private:
    threadHandler   connectThread;
    threadHandler   timerThread;
    msgQueueHandler *mqueuePtr;
    tcpServer       server;
    comPortHandler  *com;

    int             tempHigh;
    int             tempLow;
    int             humHigh;
    int             humLow;

    int             curTemp;
    int             curHum;

    bool            isFanCold_On;
    bool            isFanHot_On;

    bool            isSensorStarted;
    bool            tempStatusNeeded;

    unsigned char   sendBuff[20];

    int formatPkt (pkt_data *pkt, unsigned char *buff);

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

    void uc_sendCommand_Fan (int fanID, bool isOn);
    void requestStatusFromUC ();

    void mob_cmdVal_TempRange_Op_set(const pkt_data *pkt);

    void mob_sendTemp_Status (int temp, bool isColdFan, bool isHotFan);
    void mob_sendHumidity_Status (int hum, bool warnOn);
    void mob_sendBye (int id);
    void mob_sendTempRange (int id, int high, int low);
};

#endif
