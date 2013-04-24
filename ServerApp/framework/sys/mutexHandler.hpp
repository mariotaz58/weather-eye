#ifndef _MUTEX_HANDLER_H_
#define _MUTEX_HANDLER_H_

#include <Windows.h>

class MutexHandler
{
public:
    MutexHandler ();
    virtual ~MutexHandler ();
    bool lock ();
    bool unlock ();
private:
    HANDLE data;
};

#endif
