#include "MutexHandler.hpp"

MutexHandler::MutexHandler ()
{
    data = CreateMutex ((LPSECURITY_ATTRIBUTES)NULL, 0, "");
}

MutexHandler::~MutexHandler ()
{
    if (data)
        CloseHandle (data);
}

bool MutexHandler::lock ()
{
    if (data)
        WaitForSingleObject (data, INFINITE);
    else
        return false;
    return true;
}

bool MutexHandler::unlock ()
{
    if (data)
        ReleaseMutex (data);
    else
        return false;
    return true;
}

