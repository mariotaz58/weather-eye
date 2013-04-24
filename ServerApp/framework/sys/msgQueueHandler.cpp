#include "msgQueueHandler.hpp"
#include "mutexHandler.hpp"
#include "semaphoreHandler.hpp"

#define QUEUE_SIZE      (30)

struct msgItem
{
    void *data;
    int size;
};

class msgQueuePrivate
{
public:

    msgQueuePrivate ()
    {
        readIndex = 0;
        writeIndex = 0;
        count = 0;
        memset (items, 0, sizeof (items));
    }

    msgItem items [QUEUE_SIZE];
    semaphoreHandler sem;
    MutexHandler access;
    int readIndex;
    int writeIndex;
    int count;
};

msgQueueHandler* msgQueueHandler::create ()
{
    msgQueueHandler *q = new msgQueueHandler;
    return q;
}

msgQueueHandler::msgQueueHandler ()
{
    data = new msgQueuePrivate;
}

msgQueueHandler::~msgQueueHandler ()
{
    if (data)
    {
        for (int i = 0; i < data->count; i++)
            free (data->items[i].data);
        delete data;
    }
}

bool msgQueueHandler::read (void *buff, int size)
{
    bool loopMore = true;
    do
    {
        data->sem.wait ();
        data->access.lock ();
        if (data->count > 0)
        {
            msgItem *it = &(data->items[data->readIndex++]);
            data->count--;
            if (data->readIndex == QUEUE_SIZE)
                data->readIndex = 0;
            int copysize = (size > it->size)?(it->size):(size);
            if (it->data)
            {
                memcpy (buff, it->data, copysize);
                free (it->data);
            }
            loopMore = false;
        }
        data->access.unlock ();
    }while (loopMore);
    return true;
}

bool msgQueueHandler::write (const void *buff, int size)
{
    data->access.lock ();
    if (data->count < QUEUE_SIZE)
    {
        msgItem *it = &(data->items[data->writeIndex++]);
        if (data->writeIndex == QUEUE_SIZE)
            data->writeIndex = 0;
        it->data = malloc (size);
        it->size = size;
        memcpy (it->data, buff, size);
        data->count++;
        data->sem.signal ();
    }
    data->access.unlock ();
    return true;
}

