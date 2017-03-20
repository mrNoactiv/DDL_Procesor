#include <Windows.h>

class cThread
{
public:
    cThread ( DWORD (WINAPI * pFun) (void* arg), void* pArg)
    {
        _handle = CreateThread (
            0, // Security attributes
            0, // Stack size
            pFun,
            pArg,
            CREATE_SUSPENDED,
            &_tid);
    }
    ~cThread () { CloseHandle (_handle); }
    void Resume () { ResumeThread (_handle); }
    void WaitForDeath ()
    {
        WaitForSingleObject (_handle, 2000);
    }
private:
    HANDLE _handle;
    DWORD  _tid;     // thread id
};
