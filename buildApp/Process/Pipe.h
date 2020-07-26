#pragma once
#include "Helpers/WinApiSmartHandle.h"

#include <memory>

namespace Process
{
    class Pipe
    {
    public:
        Pipe();

        HANDLE GetReadHandle() const;
        HANDLE GetWriteHandle() const;

        bool IsReadHandleInheritable() const;
        void SetReadHandleInheritable(bool inheritable);

        bool IsWriteHandleInheritable() const;
        void SetWriteHandleInheritable(bool inheritable);

    private:
        Helpers::UniqueCloseHandle readPipe;
        Helpers::UniqueCloseHandle writePipe;

        bool IsHandleInheritable(HANDLE handle) const;
        void SetHandleInheritable(HANDLE handle, bool inheritable);
    };
}
