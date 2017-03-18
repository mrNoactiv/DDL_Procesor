#include "cActiveObject.h"

// The constructor of the derived class
// should call
//    _thread.Resume ();
// at the end of construction

cActiveObject::cActiveObject (): _isDying (0),
#pragma warning(disable: 4355) // 'this' used before initialized
  _thread (ThreadEntry, this)
#pragma warning(default: 4355)
{
}

void cActiveObject::Kill ()
{
    _isDying++;
    FlushThread ();
    // Let's make sure it's gone
    _thread.WaitForDeath ();
}

DWORD WINAPI cActiveObject::ThreadEntry (void* pArg)
{
    cActiveObject * pActive = (cActiveObject*)pArg;
    pActive->InitThread ();
    pActive->Run ();
    return 0;
}

