package com.suraily.weathereye;

import java.lang.ref.WeakReference;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.view.Menu;

public class MainActivity extends Activity {
    private static IncomingHandler m_handler;
    static
    {
        m_handler = new IncomingHandler (); 
    }
    private SocketHandler sockMgr = null;
    private serverCommunicator comm = null;

    static class IncomingHandler extends Handler
    {
        private WeakReference<MainActivity> m_Layout; 

        public void setMgr(MainActivity mgr)
        {
            m_Layout = new WeakReference<MainActivity>(mgr);
        }
        
        @Override
        public void handleMessage(Message msg)
        {
        	MainActivity mgr = m_Layout.get();
             if (mgr != null)
             {
                 mgr.handleMessage(msg);
                 super.handleMessage(msg);
             }
        }
    }
    
    public void handleMessage(Message msg)
    {
        switch (msg.what)
        {
            case 0x1337:
            	SocketHandler.sockMsg ob = (SocketHandler.sockMsg)msg.obj;
                processCommandData (ob.data, ob.len);
                break;
            case 0x1234:
                terminate ();
                break;
            default:
                break;
        }
    }

    @Override
    public void onBackPressed()
    {
        sockMgr.stopReceive();
        comm.sendBye();
    }
    
    private void terminate ()
    {
        finish ();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        MainActivity.m_handler.setMgr(this);
        
        sockMgr = new SocketHandler (MainActivity.m_handler);
        comm = new serverCommunicator (sockMgr);
        sockMgr.startReceive ();
        comm.sendHello();
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    private void processCommandData (byte data[], int len)
    {
    	
    }
}
