#ifndef _SEMAPHORE_HANDLER_HPP_
#define _SEMAPHORE_HANDLER_HPP_
#include <Windows.h>

class semaphoreHandler
{
public:
    semaphoreHandler ();
    ~semaphoreHandler ();
    bool wait ();
    bool signal ();
private:
    HANDLE data;
};

#endif
