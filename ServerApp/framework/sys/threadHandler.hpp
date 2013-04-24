#ifndef _THREAD_HANDLER_HPP_
#define _THREAD_HANDLER_HPP_

#include "callback.hpp"

class threadHandler
{
public:

    threadHandler ();

    virtual ~threadHandler ();

    const void* getHandle () const;

    void exec ();

    bool create (Slot<void, void*> fp, void *param = 0, unsigned int stackSize = 4096, unsigned int priority = 0);

private:
    void kill ();

    Slot<void, void*> callback;
    void *parameter;
    void *handle;
    bool created;
};

#endif
