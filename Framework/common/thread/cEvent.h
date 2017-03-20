#include <Windows.h>

class cEvent
{
public:
    cEvent ()
    {
        // start in non-signaled state (red light)
        // auto reset after every Wait
        _handle = CreateEvent (0, FALSE, FALSE, 0);
    }

    ~cEvent ()
    {
        CloseHandle (_handle);
    }

    // put into signaled state
    void Release () { SetEvent (_handle); }
    void Wait ()
    {
        // Wait until event is in signaled (green) state
        WaitForSingleObject (_handle, INFINITE);
    }
    operator HANDLE () { return _handle; }
private:
    HANDLE _handle;
};
