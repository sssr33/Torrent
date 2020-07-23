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

    private:
        Helpers::UniqueCloseHandle readPipe;
        Helpers::UniqueCloseHandle writePipe;
    };
}
