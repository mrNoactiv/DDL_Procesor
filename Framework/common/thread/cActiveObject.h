#pragma once

#include <Windows.h>
#include "cThread.h"

class cActiveObject
{
public:
    cActiveObject ();
    virtual ~cActiveObject () {}
    void Kill ();

protected:
    virtual void InitThread () = 0;
    virtual void Run () = 0;
    virtual void FlushThread () = 0;

    static DWORD WINAPI ThreadEntry (void *pArg);

    int             _isDying;
    cThread          _thread;
};
