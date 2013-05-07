package com.suraily.weathereye;

public class debugMgr
{
    private static boolean printEnabled = true;
    private static boolean runningOnAVD = true;

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
    
    public static String getServerIP ()
    {
        String ip = null;
        if (debugMgr.isRunningOnAVD())
            ip = new String("10.0.2.2");
        else
            ip = new String ("192.168.1.100");
        return ip; 
    }
    
}
