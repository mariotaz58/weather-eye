package com.suraily.weathereye;

import java.util.*;
import java.lang.Byte;

public class ReverseEnumMap<V extends Enum<V> & EnumConverter<?>>
{
    private Map<Byte, V> map = new HashMap<Byte, V>();
    public ReverseEnumMap(Class<V> valueType)
    {
        for (V v : valueType.getEnumConstants())
        {
            map.put(v.convert(), v);
        }
    }

    public V get(byte msgID)
    {
        return map.get(msgID);
    }
}
