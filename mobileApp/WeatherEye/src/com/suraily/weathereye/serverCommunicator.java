package com.suraily.weathereye;

public class serverCommunicator
{
    private byte data[] = null;
    private SocketHandler sockMgr = null;

    serverCommunicator (SocketHandler sock)
    {
    	sockMgr = sock;
    	data = new byte[20];
    }

    private int setHeader (byte cmdID, byte op, int len)
    {
        data [0] = (byte) 0xAA;	    // Header Start
        data [1] = (byte) 0xBB;	    // Header End
        
        data[2] = (byte)2;	        // SourceID Mobile
        data[3] = (byte)cmdID;      // Command
        data[4] = (byte)op;         // Operation
        
        data[5] = (byte)len;        // data length
        data[len+6] = (byte)0;      //Checksum
        data[len+7] = (byte)0xCC;   //Checksum
        
        return (len+8);
    }

    public void sendHello ()
    {
        int size = setHeader(msgEnums.commandValue.commandVal_hello.convert(), msgEnums.operations.Op_status.convert(), 0);
        sockMgr.sendMessage(data, size);
    }

    public void sendBye ()
    {
        int size = setHeader(msgEnums.commandValue.commandVal_bye.convert(), msgEnums.operations.Op_status.convert(), 0);
        sockMgr.sendMessage(data, size);
    }

    public void sendTempRange (int highTemp, int lowTemp)
    {
        int size = setHeader(msgEnums.commandValue.commandVal_TempRange.convert(), msgEnums.operations.Op_set.convert(), 2);
        data[6] = (byte)highTemp;
        data[7] = (byte)lowTemp;
        sockMgr.sendMessage(data, size);
    }

}
