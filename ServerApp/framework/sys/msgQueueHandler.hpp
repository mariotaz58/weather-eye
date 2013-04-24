#ifndef _MSG_QUEUE_HANDLER_HPP_
#define _MSG_QUEUE_HANDLER_HPP_

class msgQueuePrivate;

class msgQueueHandler
{
public:
    static msgQueueHandler* create ();
    virtual ~msgQueueHandler ();

    bool read (void *buff, int size);
    bool write (const void *buff, int size);

private:
    msgQueueHandler ();
    msgQueuePrivate *data;
};

#endif
