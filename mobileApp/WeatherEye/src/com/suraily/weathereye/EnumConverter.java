package com.suraily.weathereye;

public interface EnumConverter <E extends Enum<E> & EnumConverter<E>>
{
    public byte convert();
}
