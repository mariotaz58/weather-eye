#include "semaphoreHandler.hpp"
#include <cstdio>

semaphoreHandler::semaphoreHandler ()
{
    data = CreateSemaphore ((LPSECURITY_ATTRIBUTES)NULL, (LONG)0, (LONG)100,(LPCTSTR)"");
}

semaphoreHandler::~semaphoreHandler ()
{
    CloseHandle (data);
}

bool semaphoreHandler::wait ()
{
    DWORD ret = WaitForSingleObject (data, INFINITE);
    if (ret == -1)
    {
        printf ("OSAL_SignalWait: %d\n", GetLastError());
        return false;
    }
    return true;
}

bool semaphoreHandler::signal ()
{
    ReleaseSemaphore (data, 1, NULL);
    return true;
}
