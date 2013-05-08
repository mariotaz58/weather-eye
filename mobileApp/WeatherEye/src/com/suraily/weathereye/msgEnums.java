package com.suraily.weathereye;

public class msgEnums
{
    public enum commandValue implements EnumConverter<commandValue>
    {
        commandVal_Temp (0),
        commandVal_Humidity,
        commandVal_hello,
        commandVal_bye,
        commandVal_ping,
        commandVal_fan,
        commandVal_TempRange,
        commandVal_Max;
    
        private static ReverseEnumMap<commandValue> map =
                new ReverseEnumMap<commandValue>(commandValue.class);
    
        private static byte enumCount;
        private static byte getCountAndIncrement()
        {
            byte ret = enumCount;
            enumCount++;
            return ret;
        }
        private static void setEnumCount (byte val){enumCount = val;}

        private final byte value;

        commandValue(int val)
        {
            this.value = (byte)val;
            commandValue.setEnumCount ((byte)(val + 1));
            //System.out.println (this.value + "==> " + this);
        }

        commandValue() {this.value = commandValue.getCountAndIncrement();}
        
        public byte convert() {return value;}
        
        public static commandValue convert(byte msgID) {return map.get(msgID);}
    }

    public enum operations implements EnumConverter<operations>
    {
        Op_set (0),
        Op_get,
        Op_status,
        Op_error,
        Op_Max;
    
        private static ReverseEnumMap<operations> map =
                new ReverseEnumMap<operations>(operations.class);
    
        private static byte enumCount;
        private static byte getCountAndIncrement()
        {
            byte ret = enumCount;
            enumCount++;
            return ret;
        }
        private static void setEnumCount (byte val){enumCount = val;}

        private final byte value;

        operations(int val)
        {
            this.value = (byte)val;
            operations.setEnumCount ((byte)(val + 1));
            //System.out.println (this.value + "==> " + this);
        }

        operations() {this.value = operations.getCountAndIncrement();}
        
        public byte convert() {return value;}
        
        public static operations convert(byte msgID) {return map.get(msgID);}
    }

}
