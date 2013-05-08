package com.suraily.weathereye;

import java.lang.ref.WeakReference;
import java.util.regex.Pattern;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.content.Context;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Menu;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

public class MainActivity extends Activity {
    private static IncomingHandler m_handler;
    private Handler timeHandler = new Handler();
    static
    {
        m_handler = new IncomingHandler (); 
    }
    private SocketHandler sockMgr = null;
    private serverCommunicator comm = null;
    private int highTemp = 0;
    private int lowTemp = 0;

    private int highTemp_old = 0;
    private int lowTemp_old = 0;

    private boolean maxTempReady = false;
    
    private static final Pattern PARTIAl_IP_ADDRESS =
            Pattern.compile("^((25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[0-9])\\.){0,3}"+
                             "((25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[0-9])){0,1}$"); 


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
        if (msg.what == debugMgr.msgID())
        {
            	SocketHandler.sockMsg ob = (SocketHandler.sockMsg)msg.obj;
                processCommandData (ob.data, ob.len);
        }
        else if (msg.what == debugMgr.finishID())
        {
                terminate ();
        }
        else if (msg.what == debugMgr.startID())
        {
            comm.sendHello();
        }
    }

    @Override
    public void onBackPressed()
    {
        if (sockMgr != null)
        {
            sockMgr.stopReceive();
            comm.sendBye();
        }
        else
        {
            terminate ();
        }
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
        
        EditText ipEditText = (EditText)findViewById(R.id.ipEditText);
        ipEditText.addTextChangedListener(
                new TextWatcher()
                {
                    @Override
                    public void onTextChanged(CharSequence s, int start, int before, int count) {}            
                    
                    @Override
                    public void beforeTextChanged(CharSequence s,int start,int count,int after) {}            
        
                    private String mPreviousText = "";          
                    public void afterTextChanged(Editable s)
                    {          
                        if(PARTIAl_IP_ADDRESS.matcher(s).matches())
                        {
                            mPreviousText = s.toString();
                        }
                        else
                        {
                            s.replace(0, s.length(), mPreviousText);
                        }
                    }
                });
        ImageButton btnGo = (ImageButton)findViewById(R.id.btnGo);
        btnGo.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View arg0) 
            {
                  loadMain ();
            }
        });
    }

    private void loadMain ()
    {
        EditText ipEditText = (EditText)findViewById(R.id.ipEditText);
        InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.hideSoftInputFromWindow(ipEditText.getWindowToken(), 0);

        String ip_addr = ipEditText.getText().toString();

        setContentView(R.layout.activity_panel);
        sockMgr = new SocketHandler (ip_addr, MainActivity.m_handler);
        comm = new serverCommunicator (sockMgr);
        sockMgr.startReceive ();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    private void processCommandData (byte data[], int len)
    {
    	byte commandID = data [3];
    	msgEnums.commandValue cmd = msgEnums.commandValue.convert(commandID);
    	System.out.println("processCommandData: " + cmd);
    	switch (cmd)
    	{
        	case commandVal_TempRange:
        	    processCommand_TempRange (data);
        	    break;
        	case commandVal_Temp:
        	    processCommand_Temp (data);
        	    break;
        	case commandVal_Humidity:
        	    processCommand_Humidity (data);
        	    break;
    	    default:
    	        break;
    	}
    }
    
    private void processCommand_Humidity(byte[] data)
    {
        int humidity = (int)data[6];
        int hWarn = (int)data[7];

        TextView hum = (TextView)findViewById(R.id.curHumDisp);
        
        String text = Integer.toString(humidity);
        hum.setText(text);
        ImageView red = (ImageView)findViewById(R.id.hum_greenLight);
        ImageView green = (ImageView)findViewById(R.id.hum_redLight);
        if (hWarn == 1)
        {
            red.setImageResource(R.drawable.redlight_on);
            green.setImageResource(R.drawable.greenlight_off);
        }
        else
        {
            red.setImageResource(R.drawable.redlight_off);
            green.setImageResource(R.drawable.greenlight_on);
        }
    }

    private void processCommand_Temp(byte[] data)
    {
        int temp = (int)data[6];
        int cFan = (int)data[7];
        int hFan = (int)data[8];
        TextView tmp = (TextView)findViewById(R.id.curTempDisp);
        
        String text = Integer.toString(temp);
        tmp.setText(text);
        ImageView red = (ImageView)findViewById(R.id.temp_greenLight);
        ImageView green = (ImageView)findViewById(R.id.temp_redLight);
        if (cFan == 1 || hFan == 1)
        {
            red.setImageResource(R.drawable.redlight_on);
            green.setImageResource(R.drawable.greenlight_off);
        }
        else
        {
            red.setImageResource(R.drawable.redlight_off);
            green.setImageResource(R.drawable.greenlight_on);
        }
    }

    private void processCommand_TempRange (byte data[])
    {
        System.out.println("processCommand_TempRange");
        highTemp = (int)data[6];
        lowTemp = (int)data[7];
        
        highTemp_old = highTemp;
        lowTemp_old = lowTemp;

        if (!maxTempReady)
        {
            maxTempReady = true;
            ImageButton maxUp = (ImageButton)findViewById(R.id.btnUp_max);
            maxUp.setOnClickListener(new View.OnClickListener()
            {
                public void onClick(View arg0) 
                {
                    stopTimer ();
                    highTemp++;
                    if (highTemp > 45)
                        highTemp = 45;
                    updateTempRangeDisplay ();
                    startTimer ();
                }
            });
            
            ImageButton maxDown = (ImageButton)findViewById(R.id.btnDown_max);
            maxDown.setOnClickListener(new View.OnClickListener()
            {
                public void onClick(View arg0) 
                {
                    stopTimer ();
                    highTemp--;
                    if (highTemp == (lowTemp +2))
                        highTemp++;
                    updateTempRangeDisplay ();
                    startTimer ();
                }
            });

            ImageButton minUp = (ImageButton)findViewById(R.id.btnUp_min);
            minUp.setOnClickListener(new View.OnClickListener()
            {
                public void onClick(View arg0) 
                {
                    stopTimer ();
                    lowTemp++;
                    if (lowTemp > (highTemp - 2))
                        lowTemp--;
                    updateTempRangeDisplay ();
                    startTimer ();
                }
            });

            ImageButton minDown = (ImageButton)findViewById(R.id.btnDown_min);
            minDown.setOnClickListener(new View.OnClickListener()
            {
                public void onClick(View arg0) 
                {
                    stopTimer ();
                    lowTemp--;
                    if (lowTemp < 10)
                        lowTemp++;
                    updateTempRangeDisplay ();
                    startTimer ();
                }
            });

            ImageButton goBtn = (ImageButton)findViewById(R.id.btnGo);
            goBtn.setOnClickListener(new View.OnClickListener()
            {
                public void onClick(View arg0) 
                {
                    stopTimer ();
                    highTemp_old = highTemp;
                    lowTemp_old = lowTemp;
                    comm.sendTempRange(highTemp, lowTemp);
                }
            });
        }
        updateTempRangeDisplay ();
    }
    
    private void updateTempRangeDisplay ()
    {
        TextView high = (TextView)findViewById(R.id.textMaxTemp);
        TextView low = (TextView)findViewById(R.id.textMinTemp);
        
        String highText = Integer.toString(highTemp);
        high.setText(highText);

        String lowText = Integer.toString(lowTemp);
        low.setText(lowText);
    }
    
    private void stopTimer ()
    {
        timeHandler.removeCallbacks(mUpdateTimeTask);
    }
    
    private void startTimer ()
    {
        timeHandler.removeCallbacks(mUpdateTimeTask);
        timeHandler.postDelayed(mUpdateTimeTask, 3000);        
    }
    
    private Runnable mUpdateTimeTask = new Runnable()
    {
        public void run()
        {
            highTemp = highTemp_old;
            lowTemp = lowTemp_old;
            updateTempRangeDisplay ();
        }
    };
}

