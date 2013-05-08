package com.suraily.weathereye;

public class debugMgr
{
    private static boolean printEnabled = true;
    private static boolean runningOnAVD = true;
    private static String ip = null;

    static
    {
        if (debugMgr.isRunningOnAVD())
            ip = new String("10.0.2.2");
        else
            ip = new String ("192.168.1.100");
    
    }

    public synchronized static void println (String str)
    {
        if (printEnabled)
        {
            System.out.println(str);
        }
    }
    
    public static boolean isRunningOnAVD ()
    {
        return runningOnAVD;
    }
    
    public static void setServerIP (String newip)
    {
        ip = newip;
    }
    public static String getServerIP ()
    {
        return ip; 
    }
    
    public static final int startID ()
    {
        return 0xAAAA;
    }
    public static final int finishID ()
    {
        return 0xBBBB;
    }
    public static final int msgID ()
    {
        return 0xCCCC;
    }
}
