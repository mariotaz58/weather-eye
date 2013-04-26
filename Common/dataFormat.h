#ifndef _DATA_FORMAT_H_
#define _DATA_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define UC_SOURCE_ID        (0)
#define PC_SOURCE_ID        (1)
#define MOBILE_SOURCE_ID    (2)

#define HEADER_BYTE_START   (0xAA)
#define HEADER_BYTE_END     (0xBB)

typedef struct _pkt_data
{
    unsigned char header[2];
    unsigned char sourceID;
    unsigned char command;
    unsigned char operation;
    unsigned char datalength;
    unsigned char parameter;
    unsigned char checksum;
}pkt_data;

typedef enum _uartReadState
{
    uartRead_headerByteStart = 0,
    uartRead_headerByteEnd,
    uartRead_sourceID,
    uartRead_command,
    uartRead_operation,
    uartRead_datalength,
    uartRead_parameter,
    uartRead_checksum,
    uartRead_Done
}uartReadState;

typedef enum _commandValue
{
    commandVal_Temp = 0,
    commandVal_Humidity,
    commandVal_hello,
    commandVal_ping,
    commandVal_TempWarn,
    commandVal_HumidityWarn,
    commandVal_Max
}commandValue;

typedef enum _operations
{
    Op_set = 0,
    Op_get,
    Op_status,
    Op_error,
    Op_Max
}operations;

#ifdef __cplusplus
}
#endif

#endif
