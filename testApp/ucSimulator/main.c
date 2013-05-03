#include <Windows.h>
#include "dataFormat.h"

#define PORT_NAME "COM4"

struct data
{
    HANDLE          hndl;
    uartReadState   readState;
}gData;

//-----------------------------------------------------------------------------------------------
int comPortSend (const unsigned char *buff, const unsigned int buffSize);
int comPortReceive (unsigned char *buff, const unsigned int buffSize, unsigned int *recvSize);
void processData (const pkt_data *pkt);
int formatPkt (pkt_data *pkt, unsigned char *buff);
void sendHello ();
void sendPingReply ();
void sendTempStatus ();
void sendHumStatus ();
void sendSysStatus ();

int gTime = 0;
int gTemp = 20;
int gHum = 40;
unsigned char sendBuff[20];
//-----------------------------------------------------------------------------------------------

int main ()
{
    unsigned char buffer[20];
    struct data *d = &gData;
    d->hndl = CreateFile((LPCTSTR)PORT_NAME, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (d->hndl != INVALID_HANDLE_VALUE)
    {
        DCB dcb;
        COMMTIMEOUTS    UartTimeOut;
        memset(&dcb,0,sizeof(dcb));
        dcb.DCBlength = sizeof(dcb);
        dcb.BaudRate = BAUD_9600;
        dcb.StopBits = 1;
        dcb.Parity = 0;
        dcb.ByteSize = 8;

        SetCommState(d->hndl, &dcb);
        SetCommMask(d->hndl, EV_RXCHAR);

        UartTimeOut.ReadIntervalTimeout = 3;
        UartTimeOut.ReadTotalTimeoutMultiplier = 3;
        UartTimeOut.ReadTotalTimeoutConstant = 2;
        UartTimeOut.WriteTotalTimeoutMultiplier = 3;
        UartTimeOut.WriteTotalTimeoutConstant = 2;
        SetCommTimeouts(d->hndl, &UartTimeOut);
    }

    sendHello ();
    while (1)
    {
        pkt_data *pkt = (pkt_data*)buffer;
        unsigned int recv = 0;
        comPortReceive (buffer, 20, &recv);
        processData (pkt);
    }
}
//----------------------------------------------------------------------------------------------------
void processData (const pkt_data *pkt)
{
    switch (pkt->command)
    {
        case commandVal_ping:
            if (pkt->operation == Op_get)
                sendPingReply ();
            break;

        case commandVal_Temp:
            if (pkt->operation == Op_set)
            {
                gTemp = pkt->parameter[0];
                sendTempStatus ();
            }
            break;

        case commandVal_Humidity:
            if (pkt->operation == Op_set)
            {
                gHum = pkt->parameter[0];
                sendHumStatus ();
            }
            break;

        default:
            break;
    }
}

//----------------------------------------------------------------------------------------------------
int formatPkt (pkt_data *pkt, unsigned char *buff)
{
    int index = 0;
    pkt->header[0] = HEADER_BYTE_START;
    pkt->header[1] = HEADER_BYTE_END;
    pkt->sourceID = UC_SOURCE_ID;
    pkt->footer = FOOTER_BYTE;
    pkt->checksum = 0;

    memcpy (buff, (void*)pkt, (pkt->datalength + 6));
    index = (pkt->datalength + 6);
    buff[index++] = pkt->checksum;
    buff[(pkt->datalength + 7)] = pkt->footer;

    return (pkt->datalength + 8);
}

void sendHello ()
{
    pkt_data pkt;
    int size = 0;

    pkt.command = (unsigned char)commandVal_hello;
    pkt.operation = (unsigned char)Op_status;
    pkt.datalength = 0;

    size = formatPkt (&pkt, sendBuff);
    comPortSend (sendBuff, size);
}

void sendPingReply ()
{
    pkt_data pkt;
    int size = 0;

    pkt.command = (unsigned char)commandVal_ping;
    pkt.operation = (unsigned char)Op_status;
    pkt.datalength = 0;

    size = formatPkt (&pkt, sendBuff);
    comPortSend (sendBuff, size);
}

void sendTempStatus ()
{
    pkt_data pkt;
    int size = 0;

    pkt.command = (unsigned char)commandVal_Temp;
    pkt.operation = (unsigned char)Op_status;
    pkt.datalength = 1;
    pkt.parameter[0] = gTemp;

    size = formatPkt (&pkt, sendBuff);
    comPortSend (sendBuff, size);
}

void sendHumStatus ()
{
    pkt_data pkt;
    int size = 0;

    pkt.command = (unsigned char)commandVal_Humidity;
    pkt.operation = (unsigned char)Op_status;
    pkt.datalength = 1;
    pkt.parameter[0] = gHum;

    size = formatPkt (&pkt, sendBuff);
    comPortSend (sendBuff, size);
}

void sendSysStatus ()
{
    gTime++;
    if (gTime == 300)
    {
        gTime = 0;
        sendTempStatus ();
        sendHumStatus ();
    }
}

//----------------------------------------------------------------------------------------------------
int comPortReceive (unsigned char *buff, const unsigned int buffSize, unsigned int *recvSize)
{
    int index = 0;
    DWORD read = 0;
    int numberOfDataByte = 0;
    struct data *d = &gData;

    if (d->hndl == INVALID_HANDLE_VALUE)
        Sleep (INFINITE);

    d->readState = uartRead_headerByteStart;
    memset (buff, 0, buffSize);
    do
    {
        unsigned char readChar;
        ReadFile (d->hndl, &readChar, 1, &read, NULL);
        Sleep (10);
        sendSysStatus ();
        if (read > 0)
        {
            if (readChar == HEADER_BYTE_START)
            {
                memset (buff, 0, buffSize);
                buff[0] = HEADER_BYTE_START;
                index = 1;
                d->readState = uartRead_headerByteEnd;
            }
            if ((d->readState != uartRead_headerByteEnd) && (readChar == HEADER_BYTE_END))
            {
                memset (buff, 0, buffSize);
                index = 0;
                d->readState = uartRead_headerByteStart;
            }
            if ((d->readState != uartRead_footer) && (readChar == FOOTER_BYTE))
            {
                memset (buff, 0, buffSize);
                index = 0;
                d->readState = uartRead_headerByteStart;
            }
            switch (d->readState)
            {
                case uartRead_headerByteEnd:
                    if (readChar == HEADER_BYTE_END)
                    {
                        buff[1] = HEADER_BYTE_END;
                        d->readState = uartRead_sourceID;
                        index = 2;
                    }
                    break;
                case uartRead_sourceID:
                    buff[index] = readChar;
                    d->readState = uartRead_command;
                    index++;
                    break;
                case uartRead_command:
                    buff[index] = readChar;
                    d->readState = uartRead_operation;
                    index++;
                    break;
                case uartRead_operation:
                    buff[index] = readChar;
                    d->readState = uartRead_datalength;
                    index++;
                    break;
                case uartRead_datalength:
                    buff[index] = readChar;
                    d->readState = uartRead_parameter;
                    numberOfDataByte = readChar;
                    if (numberOfDataByte == 0)
                        d->readState = uartRead_checksum;
                    index++;
                    break;
                case uartRead_parameter:
                    buff[index] = readChar;
                    numberOfDataByte--;
                    if (numberOfDataByte <= 0)
                        d->readState = uartRead_checksum;
                    index++;
                    break;
                case uartRead_checksum:
                    index = 18;
                    buff[index] = readChar;
                    d->readState = uartRead_footer;
                    index++;
                    break;
                case uartRead_footer:
                    if (readChar == FOOTER_BYTE)
                    {
                        buff[index] = readChar;
                        d->readState = uartRead_Done;
                        index++;
                        *recvSize = index;
                    }
                    break;
                default:
                    break;
            }
        }
    } while (d->readState != uartRead_Done);
    return 0;
}

int comPortSend (const unsigned char *buff, const unsigned int buffSize)
{
    struct data *d = &gData;
    DWORD ret = 0;
    unsigned int i = 0;

    if (d->hndl == INVALID_HANDLE_VALUE)
        return -1;
    for (i = 0; i < buffSize; i++)
    {
        WriteFile(d->hndl, &buff[i], 1, &ret, NULL);
        Sleep (5);
    }
    return (int)ret;
}

