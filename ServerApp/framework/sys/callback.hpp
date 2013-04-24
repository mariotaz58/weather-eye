#ifndef _CALLBACK_HPP_
#define _CALLBACK_HPP_

class RefCount
{
public:
    RefCount()
    {
        count = new int;
        (*count) = 1;
    }

    RefCount(const RefCount &rc)
    {
        count = rc.count;
        reffer();
    }

    virtual ~RefCount()
    {
        unRef();
    }

    RefCount &operator = (const RefCount &ref)
    {
        ref.reffer();
        unRef();
        count = ref.count;
        return *this;
    }

    bool noRef() const
    {
        return (*count) == 0;
    }

    bool isOne() const
    {
        return (*count) == 1;
    }

private:

    void reffer() const
    {
        ++ (*count);
    }
    void unRef() const
    {
        --(*count);

        if (noRef())
        {
            delete count;
        }
    }

private:
    int *count;
};

template <class T>
class RefPtr
{
public:
    RefPtr(T *ptr = 0)
        : objPtr(ptr)
    {}

    ~RefPtr()
    {
        if (count.isOne()) delete objPtr;
    }

    RefPtr &operator = (const RefPtr &ref)
    {
        if (this != &ref)
        {
            if (count.isOne())
                delete objPtr;

            objPtr = ref.objPtr;
            count = ref.count;
        }
        return *this;
    }

    T &operator *() const
    {
        return *objPtr;
    }

    T *operator ->() const
    {
        if (count.noRef())
            return 0;
        return objPtr;
    }

    T *get() const
    {
        if (count.noRef())
            return 0;
        return objPtr;
    }

private:
    T *objPtr;
    RefCount count;
};

template <class R, class P>
class callbackBase
{
public:
    virtual R call(P param) const = 0;
    virtual ~callbackBase(){}
};


template <class R, class P>
class Slot
{
public:
    Slot& operator=(callbackBase<R,P>* s)
    {
        callback = s;
        return *this;
    }

    R operator()(P param) const
    {
        return callback->call(param);
    }

    R call(P param) const
    {
        return callback->call(param);
    }

    bool empty()
    {
        return callback.get() == 0;
    }

private:
    RefPtr <callbackBase<R,P> > callback;
};

template <class C, class R, class P>
class Callback : public callbackBase<R,P>
{
public:
    typedef R (C::*M)(P);

    Callback(C *c, M m)
        : obj(c), method(m)
    {}

    virtual R call(P param) const
    {
        return (obj->*method)(param);
    }

private:
    C *obj;
    M method;
};

#endif
