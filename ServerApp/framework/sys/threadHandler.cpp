#include "threadHandler.hpp"
#include <Windows.h>

static DWORD runFunction (void *p);

threadHandler::threadHandler ()
{
    created = false;
    handle = 0;
}

threadHandler::~threadHandler ()
{
    if (created)
        kill ();
}

const void* threadHandler::getHandle () const
{
    return handle;
}

bool threadHandler::create (Slot<void, void*> fp, void *param, unsigned int stackSize, unsigned int priority)
{
    DWORD dwThreadID;

    callback = fp;
    parameter = param;

    HANDLE id = CreateThread (
                    (LPSECURITY_ATTRIBUTES)NULL,
                    (SIZE_T)stackSize,
                    (LPTHREAD_START_ROUTINE)(&runFunction),
                    (LPVOID)this,
                    (DWORD)0,
                    &dwThreadID);
    handle = id;
    SetThreadPriority (id, 0);

    return true;
}

void threadHandler::kill ()
{
    if (handle)
        TerminateThread (handle, 0);
    handle = 0;
    created = false;
}

void threadHandler::exec ()
{
    callback (parameter);
}

static DWORD runFunction (void *p)
{
    if (p)
    {
        threadHandler *hndl = static_cast<threadHandler*> (p);
        hndl->exec ();
    }
    return 0;
}

