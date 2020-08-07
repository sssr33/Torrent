#pragma once
#include "Helpers/WinApiSmartHandle.h"
#include "Helpers/Strong.h"

#include <memory>

namespace Arg
{
    using Async = Helpers::Strong<bool, struct AsyncParameter>;
}

namespace Process
{
    class Pipe
    {
    public:
        explicit Pipe(Arg::Async async);

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
