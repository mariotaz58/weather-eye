package com.suraily.weathereye;

import java.net.*;
import java.io.*;
import android.os.Handler;
import android.os.Message;

import java.util.Arrays;

class SocketHandler
{
    socketReceiver recv = null;
    private Socket recvSock = null;
    private Socket sendSock = null;
    private boolean rxRun = true;
    private boolean socketThreadExited = false;
    
    SocketHandler (Handler hndl)
    {
        recv = new socketReceiver ();
        recv.setHandler(hndl);
    }
    
    SocketHandler (String ip, Handler hndl)
    {
        debugMgr.setServerIP(ip);
        recv = new socketReceiver ();
        recv.setHandler(hndl);
    }

    public boolean isSocketThreadExited ()
    {
    	return socketThreadExited;
    }
    
    
    public void startReceive ()
    {
        recv.startRx ();
    }
    
    public void stopReceive ()
    {
    	rxRun = false;
    }
    
    public void sendMessage (byte data[], int len)
    {
        if (sendSock == null)
            return;
        try
        {
            DataOutputStream dos = new DataOutputStream(sendSock.getOutputStream());
            dos.write(data, 0, len);
            try
            {
                Thread.sleep(100);
            }
            catch (InterruptedException e)
            {
                e.printStackTrace();
            }            
        }
        catch (IOException e1)
        {
            e1.printStackTrace();
            return;
        }
    }
    
    class sockMsg
    {
        public byte data[];
        public int len;
    }

    class socketReceiver
    {
        private Thread commTh = null;
        private DataInputStream is = null;
        private String serverIpAddress = null;

        // Port 20250 is common for all
        private static final int TCP_CLIENT_SEND_PORT = 20249;
        private static final int TCP_CLIENT_RECV_PORT = 20250;
        
        Handler myUpdateHandler = null;
        
        public void setHandler (Handler hndl)
        {
            myUpdateHandler = hndl;
        }
        
        socketReceiver ()
        {
            serverIpAddress = debugMgr.getServerIP ();
        }
        
        public void startRx ()
        {
            commTh = new Thread (new CommsThread());
            this.commTh.start();
        }
        
        
        class CommsThread implements Runnable
        {
            private void sendReady ()
            {
                Message m = new Message();
                m.what = debugMgr.startID();
                m.obj = null;
                myUpdateHandler.sendMessage(m);             
            }
            public void run()
            {
            	while (rxRun)
            	{
	                try
	                {
	                    
	                    InetAddress serverAddr = InetAddress.getByName(serverIpAddress);
	                    recvSock = new Socket(serverAddr, TCP_CLIENT_RECV_PORT);
	                    try
	                    {
	                        Thread.sleep(500);
	                    }
	                    catch (InterruptedException e)
	                    {
	                        e.printStackTrace();
	                    }            
	                    is = new DataInputStream(recvSock.getInputStream());
	                    sendSock = new Socket(serverAddr, TCP_CLIENT_SEND_PORT);
	                }
	                catch (UnknownHostException e1)
	                {
	                    e1.printStackTrace();
	                    recvSock = null;
	                    sendSock =  null;
	                }
	                catch (IOException e1)
	                {
	                    e1.printStackTrace();
	                    recvSock = null;
	                    sendSock =  null;
	                }
	                if (recvSock != null && sendSock != null && is != null)
	                {
	                    try
	                    {
	                        byte response[] = new byte[32768];
	                        int len = 0;
	                        sendReady ();
	                        while (rxRun && (len != -1))
	                        {
	                            len = is.read(response);
	                            if (len < 0)
	                            {
	                                break;
	                            }
	                            Message m = new Message();
	                            sockMsg ob = new sockMsg();
	                            ob.data = Arrays.copyOf(response, 32768);
	                            ob.len = len;
	                            
	                            m.what = debugMgr.msgID();
	                            m.obj = ob;
	                            myUpdateHandler.sendMessage(m);
	                        }
	                        is.close();
	                        recvSock.close ();
	                        sendSock.close();
	                        recvSock = null;
	                        sendSock =  null;
	                    }
	                    catch (UnknownHostException e1) 
	                    {
	                        e1.printStackTrace();
	                        recvSock = null;
	                        sendSock =  null;
	                    }
	                    catch (IOException e1)
	                    {
	                        e1.printStackTrace();
	                        recvSock = null;
	                        sendSock =  null;
	                    }
	                }
                    try
                    {
                    	if (rxRun)
                    	{
	                    	debugMgr.println("Sleeping for 1s before doing a retry");
	                        Thread.sleep(1000);
                    	}
                    }
                    catch (InterruptedException e)
                    {
                        e.printStackTrace();
                    }     
            	}
            	socketThreadExited = true;
                Message m = new Message();
                m.what = debugMgr.finishID();
                m.obj = null;
                myUpdateHandler.sendMessage(m);            	
            }
        }
    }
}
