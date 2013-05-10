#include "reg_c51.h"
#include "dataFormat.h"

#define ON                  (0)
#define OFF                 (1)

#define LOW                 (0)
#define HIGH                (1)

#define TRUE                (1)
#define FALSE               (0)

#define COOL_FAN_ON         (0)
#define COOL_FAN_OFF        (1)
#define HOT_FAN_ON          (2)
#define HOT_FAN_OFF         (3)

#define ADC_DATA P2 

sbit ADC_ADDA = P1^0;
sbit ADC_ALE = P1^1;
sbit ADC_START = P1^2;
sbit ADC_EOC = P1^3;
sbit ADC_OE = P1^4;
sbit c_fan = P1^5;
sbit h_fan = P1^6;

uartReadState   readState;

unsigned char uart_data;
unsigned char TxOK=0;

unsigned char adc_val;

unsigned char Temp=22;
unsigned char Humidity=45;

unsigned char rxBuffer[20];
unsigned char txBuffer[20];

unsigned char uartByteQueue[20];
unsigned char byteCount = 0;
unsigned char readIndex = 0;
unsigned char writeIndex = 0;

void data_recv(unsigned char dat);
void send_data(unsigned char *datav, unsigned char size);
void main_loop();
void processData (const pkt_data *pkt);
unsigned char formatPkt (pkt_data *pkt, unsigned char *buff);
void sendHello ();
void sendPingReply ();
void send_Temp();
void send_Humidity();
void output(unsigned char mode);
unsigned char ReadUart ();
unsigned char comPortReceive (unsigned char *buff);
void senseTemp ();
void senseHumidity ();
unsigned char checkADC();

const unsigned char code table_t[]=
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x01,0x02,0x02,0x02,0x03,0x03,0x04,0x04,0x05,0x05,0x05,0x06,
0x06,0x07,0x07,0x07,0x08,0x08,0x09,0x09,0x09,0x0a,0x0a,0x0b,0x0b,0x0b,0x0c,0x0c,
0x0c,0x0d,0x0d,0x0e,0x0e,0x0e,0x0f,0x0f,0x10,0x10,0x10,0x11,0x11,0x11,0x12,0x12,
0x13,0x13,0x13,0x14,0x14,0x15,0x15,0x15,0x16,0x16,0x16,0x17,0x17,0x17,0x18,0x18,
0x19,0x19,0x19,0x1a,0x1a,0x1b,0x1b,0x1b,0x1c,0x1c,0x1d,0x1d,0x1d,0x1e,0x1e,0x1f,
0x1f,0x1f,0x20,0x20,0x21,0x21,0x21,0x22,0x22,0x23,0x23,0x24,0x24,0x24,0x25,0x25,
0x26,0x26,0x27,0x27,0x28,0x28,0x29,0x29,0x29,0x2a,0x2a,0x2b,0x2b,0x2c,0x2c,0x2d,
0x2d,0x2e,0x2e,0x2f,0x2f,0x30,0x31,0x31,0x32,0x32,0x33,0x33,0x34,0x34,0x35,0x36,
0x36,0x37,0x37,0x38,0x39,0x39,0x3a,0x3b,0x3b,0x3c,0x3d,0x3d,0x3e,0x3f,0x40,0x41,
0x41,0x42,0x43,0x44,0x45,0x46,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x52,0x53,0x55,0x56,0x58,0x5a,0x5b,0x5c,0x5e,0x5f,0x61,0x63,0x63,0x63,0x63,
0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63};

const unsigned char code table_h[]=
{0x00,0x05,0x05,0x05,0x05,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,
0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
0x08,0x09,0x09,0x09,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x13,
0x14,0x14,0x15,0x15,0x16,0x16,0x17,0x17,0x18,0x18,0x19,0x19,0x1A,0x1A,0x1B,0x1B,
0x1C,0x1D,0x1E,0x1E,0x1F,0x1F,0x20,0x20,0x21,0x21,0x22,0x22,0x23,0x23,0x24,0x24,
0x25,0x25,0x26,0x26,0x27,0x28,0x29,0x2A,0x2A,0x2B,0x2C,0x2C,0x2D,0x2D,0x2E,0x2E,
0x2F,0x2F,0x30,0x30,0x31,0x31,0x31,0x32,0x33,0x33,0x34,0x35,0x35,0x36,0x37,0x37,
0x38,0x38,0x39,0x39,0x3A,0x3A,0x3B,0x3B,0x3C,0x3D,0x3D,0x3E,0x3E,0x3F,0x3F,0x40,
0x41,0x41,0x42,0x43,0x43,0x44,0x44,0x45,0x45,0x46,0x47,0x48,0x48,0x49,0x4A,0x4A,
0x4B,0x4C,0x4C,0x4D,0x4E,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,0x61,0x62,0x63,0x63,0x63,0x63,0x63,0x63,
0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63};

/*-----------------------------------------------------------------------------------------*/
void main (void) 
{
    SCON = 0x50;
    TMOD = 0x20;
    TH1  = 0xFD;
    TL1  = 0xFD;
    ES = 1;
    EA = 1;
    TR1 = 1;
    ADC_START = LOW;
    ADC_EOC = HIGH;
    ADC_OE = LOW;
    ADC_ALE = LOW;

    output(COOL_FAN_OFF);
    output(HOT_FAN_OFF);
    sendHello ();

    while(1)
    {
        main_loop();
    }
}

void senseTemp ()
{
    ADC_ADDA = LOW;
    Temp = checkADC(); 
    Temp = table_t[Temp]; 
}

void senseHumidity ()
{
    ADC_ADDA = HIGH;
    Humidity = checkADC(); 
    Humidity = table_h[Humidity]; 
}

void serial(void) interrupt 4 
{

    if (TI == 1) 
    {
      TI=0;
      TxOK=0;
   }
    if (RI == 1) 
    {
      RI = 0;
      uart_data = SBUF;
      data_recv(uart_data);
    }
}

unsigned char checkADC()
{
    asm{0,0,0,0,0,0,0,0};
    ADC_ALE = 1;
    asm{0,0,0,0,0,0,0,0};
    ADC_START = 1;
    asm{0,0,0,0,0,0,0,0};
    ADC_ALE = 0;
    asm{0,0,0,0};
    ADC_START = 0;
    asm{0,0,0,0};
    ADC_DATA = 0xff;
    while(ADC_EOC == 0);
    ADC_OE = 1;
    asm{0,0,0,0,0,0,0,0};
    adc_val = ADC_DATA;
    ADC_OE = 0;
    return adc_val;
}

void data_recv(unsigned char dat)
{
    uartByteQueue [writeIndex++] = dat;
    byteCount++;
    if (writeIndex == 20)
        writeIndex = 0;
}

void send_data(unsigned char *dataval, unsigned char size)
{
    unsigned char index;
    for (index = 0; index < size; index++)
    {
        TxOK=1;
        SBUF = dataval[index];
        while(TxOK);
    }
}

void main_loop()
{
    pkt_data *pkt = (pkt_data*)rxBuffer;
    comPortReceive (rxBuffer);
    processData (pkt);
}

void processData (const pkt_data *pkt)
{
    switch (pkt->command)
    {
        case commandVal_ping:
            if (pkt->operation == Op_get)
                sendPingReply ();
            break;

        case commandVal_fan:
            if (pkt->operation == Op_set)
            {
                if ((pkt->parameter[0] == COLD_AIR_FAN) && (pkt->parameter[1] == TRUE))
                {
                    output (COOL_FAN_ON);
                }
                if ((pkt->parameter[0] == HOT_AIR_FAN) && (pkt->parameter[1] == TRUE))
                {
                    output (HOT_FAN_ON);
                }
                if ((pkt->parameter[0] == COLD_AIR_FAN) && (pkt->parameter[1] == FALSE))
                {
                    output (COOL_FAN_OFF);
                }
                if ((pkt->parameter[0] == HOT_AIR_FAN) && (pkt->parameter[1] == FALSE))
                {
                    output (HOT_FAN_OFF);
                }
            }
            break;

        case commandVal_Temp:
            if (pkt->operation == Op_get)
            {
                senseTemp ();
                send_Temp ();
            }
            break;

        case commandVal_Humidity:
            if (pkt->operation == Op_get)
            {
                senseHumidity ();
                send_Humidity ();
            }
            break;

        default:
            break;
    }
}

unsigned char formatPkt (pkt_data *pkt, unsigned char *buff)
{
    unsigned char index = 0;
    unsigned char pos = 0;
    unsigned char *ptr = (unsigned char*)pkt;

    pkt->header[0] = HEADER_BYTE_START;
    pkt->header[1] = HEADER_BYTE_END;
    pkt->sourceID = UC_SOURCE_ID;
    pkt->footer = FOOTER_BYTE;
    pkt->checksum = 0;

    for (pos = 0; pos < (pkt->datalength + 6); pos++)
        buff[pos] = ptr[pos];
    index = (pkt->datalength + 6);
    buff[index++] = pkt->checksum;
    buff[(pkt->datalength + 7)] = pkt->footer;

    return (pkt->datalength + 8);
}

void sendHello ()
{
    pkt_data pkt;
    unsigned char size = 0;

    pkt.command = (unsigned char)commandVal_hello;
    pkt.operation = (unsigned char)Op_status;
    pkt.datalength = 0;

    size = formatPkt (&pkt, txBuffer);
    send_data (txBuffer, size);
}

void sendPingReply ()
{
    pkt_data pkt;
    unsigned char size = 0;

    pkt.command = (unsigned char)commandVal_ping;
    pkt.operation = (unsigned char)Op_status;
    pkt.datalength = 0;

    size = formatPkt (&pkt, txBuffer);
    send_data (txBuffer, size);
}

void send_Temp()
{
    pkt_data pkt;
    unsigned char size = 0;

    pkt.command = (unsigned char)commandVal_Temp;
    pkt.operation = (unsigned char)Op_status;
    pkt.datalength = 1;
    pkt.parameter[0] = Temp;

    size = formatPkt (&pkt, txBuffer);
    send_data (txBuffer, size);
}

void send_Humidity()
{
    pkt_data pkt;
    unsigned char size = 0;

    pkt.command = (unsigned char)commandVal_Humidity;
    pkt.operation = (unsigned char)Op_status;
    pkt.datalength = 1;
    pkt.parameter[0] = Humidity;

    size = formatPkt (&pkt, txBuffer);
    send_data (txBuffer, size);
}

void output(unsigned char mode)
{
    switch(mode)
        {
        case COOL_FAN_ON:
            c_fan = ON;
            break;
        case COOL_FAN_OFF:
            c_fan = OFF;
            break;
        case HOT_FAN_ON:
            h_fan = ON;
            break;
        case HOT_FAN_OFF:
            h_fan = OFF;
            break;
        }
}

unsigned char ReadUart ()
{
    unsigned char ret = 0;
    while (byteCount == 0);
    ret = uartByteQueue [readIndex++];
    byteCount--;
    if (readIndex == 20)
        readIndex = 0;
    return ret;
}

unsigned char comPortReceive (unsigned char *buff)
{
    unsigned char index = 0;
    unsigned char numberOfDataByte = 0;
    unsigned char readChar;

    readState = uartRead_headerByteStart;
    do
    {
        readChar = ReadUart ();
        if (readChar == HEADER_BYTE_START)
        {
            buff[0] = HEADER_BYTE_START;
            index = 1;
            readState = uartRead_headerByteEnd;
        }
        if ((readState != uartRead_headerByteEnd) && (readChar == HEADER_BYTE_END))
        {
            index = 0;
            readState = uartRead_headerByteStart;
        }
        if ((readState != uartRead_footer) && (readChar == FOOTER_BYTE))
        {
            index = 0;
            readState = uartRead_headerByteStart;
        }
        switch (readState)
        {
            case uartRead_headerByteEnd:
                if (readChar == HEADER_BYTE_END)
                {
                    buff[1] = HEADER_BYTE_END;
                    readState = uartRead_sourceID;
                    index = 2;
                }
                break;
            case uartRead_sourceID:
                buff[index] = readChar;
                readState = uartRead_command;
                index++;
                break;
            case uartRead_command:
                buff[index] = readChar;
                readState = uartRead_operation;
                index++;
                break;
            case uartRead_operation:
                buff[index] = readChar;
                readState = uartRead_datalength;
                index++;
                break;
            case uartRead_datalength:
                buff[index] = readChar;
                readState = uartRead_parameter;
                numberOfDataByte = readChar;
                if (numberOfDataByte == 0)
                    readState = uartRead_checksum;
                index++;
                break;
            case uartRead_parameter:
                buff[index] = readChar;
                numberOfDataByte--;
                if (numberOfDataByte <= 0)
                    readState = uartRead_checksum;
                index++;
                break;
            case uartRead_checksum:
                index = 18;
                buff[index] = readChar;
                readState = uartRead_footer;
                index++;
                break;
            case uartRead_footer:
                if (readChar == FOOTER_BYTE)
                {
                    buff[index] = readChar;
                    readState = uartRead_Done;
                    index++;
                }
                break;
            default:
                break;
        }
    } while (readState != uartRead_Done);
    return 0;
}
