#include "controller.hpp"
#include <Windows.h>
#include <cstdio>

controller::controller ()
{
    tempHigh = 25;
    tempLow = 16;
    humHigh = 50;
    humLow = 40;

    curTemp = 20;
    curHum = 45;

    isFanCold_On = false;
    isFanHot_On = false;

    isSensorStarted = false;
    tempStatusNeeded = true;
}

controller::~controller ()
{
}

void controller::timeout(void *p)
{
    if (static_cast<controller*>(p) != this)
    {
        return;
    }
    while (1)
    {
        Sleep (2000);
        if (isSensorStarted)
        {
            pkt_data pkt;
            pkt.header[0] = HEADER_BYTE_START;
            pkt.header[1] = HEADER_BYTE_END;
            pkt.sourceID = PC_SOURCE_ID;
            pkt.command = (unsigned char)commandVal_ping;
            pkt.operation = (unsigned char)Op_get;
            pkt.datalength = 2;
            pkt.footer = FOOTER_BYTE;
            pkt.checksum = 0;
            mqueuePtr->write (&pkt, sizeof (pkt_data));
        }
    }
}

void controller::process(void *p)
{
    if (static_cast<controller*>(p) != this)
    {
        return;
    }

    unsigned char buff[20];
    while(1)
    {
        mqueuePtr->read (buff,20);
        const pkt_data *pkt;
        pkt = (const pkt_data*)(buff);
        switch (pkt->sourceID)
        {
            case UC_SOURCE_ID:
                processUCData (pkt);
                break;

            case PC_SOURCE_ID:
                requestStatusFromUC ();
                break;

            case MOBILE_SOURCE_ID:
                processMobileData (pkt);
                break;

            default :
                break;
        }
    }
}

void controller::start ()
{
    mqueuePtr = msgQueueHandler::create();
    server.setParentMsgQueue (mqueuePtr);
    com = new comPortHandler (COM_PORT_NAME);
    com->setParentMsgQueue (mqueuePtr);

    Slot<void, void*> fp;
    fp = new Callback <controller, void, void*>(this, &controller::process);

    Slot<void, void*> tm;
    tm = new Callback <controller, void, void*>(this, &controller::timeout);

    com->start ();
    server.start ();
    connectThread.create (fp, this);
    timerThread.create (tm, this);
}

int controller::formatPkt (pkt_data *pkt, unsigned char *buff)
{
    int index = 0;
    pkt->header[0] = HEADER_BYTE_START;
    pkt->header[1] = HEADER_BYTE_END;
    pkt->sourceID = PC_SOURCE_ID;
    pkt->footer = FOOTER_BYTE;
    pkt->checksum = 0;

    memcpy (buff, (void*)pkt, (pkt->datalength + 6));
    index = (pkt->datalength + 6);
    buff[index++] = pkt->checksum;
    buff[(pkt->datalength + 7)] = pkt->footer;

    return (pkt->datalength + 8);
}

void controller::processUCData (const pkt_data *pkt)
{
    switch ((commandValue)pkt->command)
    {
        case commandVal_Temp:
            switch ((operations)pkt->operation)
            {
                case Op_status: 
                    uc_cmdVal_Temp_Op_status(pkt);
                    break;

                case Op_error: 
                    uc_cmdVal_Temp_Op_error(pkt);
                    break;

                default:
                    break;
            }
            break;

        case commandVal_Humidity: 
            switch ((operations)pkt->operation)
            {
                case Op_status: 
                    uc_cmdVal_Hum_Op_status(pkt);
                    break;

                case Op_error: 
                    uc_cmdVal_Hum_Op_error(pkt);
                    break;

                default:
                    break;
            }
            break;

        case commandVal_hello: 
            switch ((operations)pkt->operation)
            {
                case Op_status: 
                    uc_cmdVal_hello_Op_status(pkt);
                    break;

                case Op_error:
                    uc_cmdVal_hello_Op_error(pkt);
                    break;

                default:
                    break;
            }
            break;

        case commandVal_ping:
            switch ((operations)pkt->operation)
            {
                case Op_status:
                    uc_cmdVal_ping_Op_status(pkt);
                    break;

                case Op_error:
                    uc_cmdVal_ping_Op_error(pkt);
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

void controller::processMobileData (const pkt_data *pkt)
{
}

void controller::uc_cmdVal_Temp_Op_status(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
    curTemp = pkt->parameter[0];
    if (curTemp > tempHigh)
    {
        uc_sendCommand_Fan (COLD_AIR_FAN, true);
        isFanCold_On = true;
        isFanHot_On = false;
    }
    else if (curTemp < tempLow)
    {
        uc_sendCommand_Fan (HOT_AIR_FAN, true);
        isFanHot_On = true;
        isFanCold_On = false;
    }
    else if (curTemp > MAX_TEMP_VAL)
    {
        //TODO: Send command to shutdown system
    }
    else if ((curTemp > tempLow) && (curTemp < tempHigh))
    {
        if (isFanCold_On)
        {
            isFanCold_On = false;
            uc_sendCommand_Fan (COLD_AIR_FAN, false);
        }
        if (isFanHot_On)
        {
            isFanHot_On = false;
            uc_sendCommand_Fan (HOT_AIR_FAN, false);
        }
    }
    mob_sendTemp_Status (curTemp, isFanCold_On, isFanHot_On);
}

void controller::uc_cmdVal_Temp_Op_error(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller::uc_cmdVal_Hum_Op_status(const pkt_data *pkt)
{
    curHum = pkt->parameter[0];
    printf ("Current Humidity: %d\n", curHum);
    if ((curHum > humHigh) || (curHum < humLow))
    {
        mob_sendHumidity_Warning (true);
    }
    else
    {
        mob_sendHumidity_Warning (false);
    }

    printf ("%s\n", __FUNCTION__);
}

void controller::uc_cmdVal_Hum_Op_error(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller::uc_cmdVal_hello_Op_status(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
    isSensorStarted = true;
}

void controller::uc_cmdVal_hello_Op_error(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller::uc_cmdVal_ping_Op_status(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller::uc_cmdVal_ping_Op_error(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller::uc_sendCommand_Fan (int fanID, bool isOn)
{
    pkt_data pkt;
    int size = 0;

    pkt.command = (unsigned char)commandVal_fan;
    pkt.operation = (unsigned char)Op_set;
    pkt.datalength = 2;
    pkt.parameter[0] = fanID;
    pkt.parameter[1] = (isOn)?(1):(0);

    size = formatPkt (&pkt, sendBuff);
    com->send (sendBuff, size);
}

void controller::requestStatusFromUC ()
{
    pkt_data pkt;
    int size = 0;

    if (tempStatusNeeded)
        pkt.command = (unsigned char)commandVal_Temp;
    else
        pkt.command = (unsigned char)commandVal_Humidity;
    tempStatusNeeded = !tempStatusNeeded;

    pkt.operation = (unsigned char)Op_get;
    pkt.datalength = 0;

    size = formatPkt (&pkt, sendBuff);
    com->send (sendBuff, size);
}

void controller::mob_sendTemp_Status (int temp, bool isColdFan, bool isHotFan)
{
    pkt_data pkt;
    int size = 0;

    printf ("Current Temperature: %d\n", temp);
    pkt.command = (unsigned char)commandVal_Temp;
    pkt.operation = (unsigned char)Op_status;
    pkt.datalength = 3;
    pkt.parameter[0] = temp;
    pkt.parameter[1] = (isColdFan)?(1):(0);
    pkt.parameter[2] = (isHotFan)?(1):(0);

    size = formatPkt (&pkt, sendBuff);
    //TODO: Send status to mobile
}

void controller::mob_sendHumidity_Warning (bool warnOn)
{
    pkt_data pkt;
    int size = 0;

    pkt.command = (unsigned char)commandVal_HumidityWarn;
    pkt.operation = (unsigned char)Op_status;
    pkt.datalength = 1;
    pkt.parameter[0] = (warnOn)?(1):(0);

    size = formatPkt (&pkt, sendBuff);
    //TODO: Send status to mobile
}
