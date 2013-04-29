#include "controller.hpp"
#include <cstdio>

controller::controller ()
{
}

controller::~controller ()
{
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

    com->start ();
    server.start ();
    connectThread.create (fp, this);
}

void controller:: processUCData (const pkt_data *pkt)
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


void controller:: uc_cmdVal_Temp_Op_status(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller:: uc_cmdVal_Temp_Op_error(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller:: uc_cmdVal_Hum_Op_status(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller:: uc_cmdVal_Hum_Op_error(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller:: uc_cmdVal_hello_Op_status(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller:: uc_cmdVal_hello_Op_error(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller:: uc_cmdVal_ping_Op_status(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller:: uc_cmdVal_ping_Op_error(const pkt_data *pkt)
{
    printf ("%s\n", __FUNCTION__);
}

void controller:: processMobileData (const pkt_data *pkt)
{
}

